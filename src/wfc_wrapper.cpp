#include "godot_cpp/core/object.hpp"
#include <overlapping_patterns.h>
#include <wfc_core.h>
#include <wfc_wrapper.h>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void WFC::_bind_methods() {
	// Public methods
	ClassDB::bind_method(D_METHOD("getPatternTextures"), &WFC::getPatternTextures);
	ClassDB::bind_method(
			D_METHOD("validCellsForPattern", "pattern_id"), &WFC::validCellsForPattern);

	// WFC settings
	ClassDB::bind_method(D_METHOD("getConfig"), &WFC::getConfig);
	ClassDB::bind_method(D_METHOD("setConfig", "p_config"), &WFC::setConfig);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "config", PROPERTY_HINT_RESOURCE_TYPE, "WFCConfig"),
			"setConfig", "getConfig");

	// Input sprite
	ClassDB::bind_method(D_METHOD("getInputSprite"), &WFC::getInputSprite);
	ClassDB::bind_method(D_METHOD("setInputSprite", "p_input_sprite"), &WFC::setInputSprite);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "input_sprite", PROPERTY_HINT_NODE_TYPE, "Sprite2D"),
			"setInputSprite", "getInputSprite");
}

void WFC::_ready() {
	if (input_sprite) {
		convertInputSpriteToPixels();
		computeAdjacencyData();
		initializeWFCCore();
	}
};

Ref<WFCConfig> WFC::getConfig() const { return config; }

void WFC::setConfig(const Ref<WFCConfig> &p_config) { config = p_config; }

TypedArray<Texture2D> WFC::getPatternTextures() {
	TypedArray<Texture2D> texture_list;

	const auto &patterns = overlapping_patterns->getInputPixelPatterns();
	size_t N = config->get_pattern_size();
	size_t pattern_size = N * N * 4; // Assuming RGBA
	size_t num_patterns = patterns.size() / pattern_size;

	for (size_t i = 0; i < num_patterns; ++i) {
		PackedByteArray buffer;
		buffer.resize(pattern_size);
		memcpy(buffer.ptrw(), &patterns[i * pattern_size], pattern_size);

		// We need to convert to image before creating a texture
		Ref<Image> image = Image::create_from_data(N, N, false, Image::FORMAT_RGBA8, buffer);

		Ref<Texture2D> texture = ImageTexture::create_from_image(image);

		texture_list.append(texture);
	}

	return texture_list;
}

Ref<Texture2D> WFC::validCellsForPattern(pattern_id_t pattern_id) {
	if (!wfc_core) {
		ERR_PRINT("WFCCore is null. Please call initializeWFCCore() first.");
		return Ref<Texture2D>();
	}

	std::vector<uint8_t> valid_cells = wfc_core->getValidCellsForPattern(
			pattern_id, config->get_width(), config->get_height());
	PackedByteArray valid_cells_array;
	valid_cells_array.resize(valid_cells.size());
	memcpy(valid_cells_array.ptrw(), valid_cells.data(), valid_cells.size());

	// Create a Godot Image from the valid cells data
	Ref<Image> valid_cells_image = Image::create_from_data(
			config->get_width(), config->get_height(), false, Image::FORMAT_L8, valid_cells_array);

	// Create a new Texture2D based on the image
	Ref<Texture2D> valid_cells_texture = ImageTexture::create_from_image(valid_cells_image);

	return valid_cells_texture;
}

void WFC::convertInputSpriteToPixels() {
	if (!input_sprite) {
		ERR_PRINT("Input sprite is null.");
		return;
	}

	// Check if texture is valid
	Ref<Texture2D> texture = input_sprite->get_texture();
	if (texture.is_null()) {
		UtilityFunctions::print("Sprite has no texture!");
		return;
	}

	Ref<Image> img = texture->get_image();
	if (img.is_null())
		return;

	int width = img->get_width();
	int height = img->get_height();

	// Converto to RGBAB format for consistency
	img->convert(Image::FORMAT_RGBA8);
	PackedByteArray pixel_data = img->get_data();
	const uint8_t *pixels = pixel_data.ptr();

	// Create a vector to hold the pixel data
	std::vector<uint8_t> pixel_vector(pixels, pixels + pixel_data.size());
	sprite_holder = std::make_unique<SpriteHolder>(width, height, 4, std::move(pixel_vector));
}

void WFC::computeAdjacencyData() {
	if (!sprite_holder) {
		ERR_PRINT("SpriteHolder is null. Please call convertInputSpriteToPixels() first.");
		return;
	}

	// Create an instance of OverlappingPatterns
	overlapping_patterns = std::make_unique<OverlappingPatterns>(
			*sprite_holder, config->get_pattern_size(), config->get_boundary_condition());
}

void WFC::initializeWFCCore() {
	if (!overlapping_patterns) {
		ERR_PRINT("OverlappingPatterns is null. Please call computeAdjacencyData() first.");
		return;
	}

	// If seed is negative use a random seed
	int seed = config->get_seed();
	if (seed < 0) {
		seed = std::chrono::system_clock::now().time_since_epoch().count();
	}

	size_t pattern_size = config->get_pattern_size();
	wfc_core = std::make_unique<WFCCore>(overlapping_patterns->getAdjacencyData(), seed);
	wfc_core->prepareWFCSolver(config->get_width() - pattern_size + 1,
			config->get_height() - pattern_size + 1, config->get_force_boundary_patterns());
}

std::vector<uint8_t> WFC::computeOutputPixels() {
	if (!wfc_core) {
		ERR_PRINT("WFCCore is null. Please call initializeWFCCore() first.");
		return {};
	}

	size_t adj_width = config->get_width() - config->get_pattern_size() + 1;
	size_t adj_height = config->get_height() - config->get_pattern_size() + 1;

	std::span<const pattern_id_t> collapsed_patterns = wfc_core->solve(adj_width, adj_height, 0,
			config->get_force_boundary_patterns(), config->get_cell_selection_strategy());

	// Convert the collapsed patterns back to pixel data
	return overlapping_patterns->convertIdsToPixels(collapsed_patterns, adj_width, adj_height);
}

void WFC::mapPixelsToTexture() {
	std::vector<uint8_t> output_pixels = computeOutputPixels();
	if (output_pixels.empty()) {
		ERR_PRINT("Output pixels are empty. Please ensure that the WFC process completed "
				  "successfully.");
		return;
	}

	int output_width = config->get_width();
	int output_height = config->get_height();

	// Transform pixel data to Godot packed array
	PackedByteArray pixel_data;
	pixel_data.resize(output_pixels.size());
	std::copy(output_pixels.begin(), output_pixels.end(), pixel_data.ptrw());

	// Create a Godot Image from the packed data
	Ref<Image> output_image = Image::create_from_data(
			output_width, output_height, false, Image::FORMAT_RGBA8, pixel_data);

	// Create a new Texture2D based on the image
	Ref<Texture2D> output_texture = ImageTexture::create_from_image(output_image);

	// Set the texture to the Sprite2D
	this->set_texture(output_texture);
}