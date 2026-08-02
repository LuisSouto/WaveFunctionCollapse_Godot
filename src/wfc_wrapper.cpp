#include "godot_cpp/core/object.hpp"
#include <overlapping_patterns.h>
#include <sprite_transforms.h>
#include <wfc_core.h>
#include <wfc_wrapper.h>
#include <cstdint>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <random>
#include <vector>

using namespace godot;

void WFC::_ready() {
	if (config.is_null()) {
		ERR_PRINT("WFC: Config resource is not assigned!");
		return;
	}
	if (input_sprite != nullptr) {
		convertInputSpriteToPixels();
		generateOverlappingPatterns();
		initializeWFCCore();
		initializeOutputTexture();
	}
};

void WFC::initializeOutputTexture() {
	if (!wfc_core) {
		ERR_PRINT("WFCCore is null. Please call initializeWFCCore() first.");
		return;
	}

	int output_width = config->get_width();
	int output_height = config->get_height();
	pixel_data = PackedByteArray();
	pixel_data.resize(output_width * output_height * 4);
	pixel_data.fill(0); // Start with a blank image

	// Map pixels into texture
	output_image = Image::create_from_data(
			output_width, output_height, false, Image::FORMAT_RGBA8, pixel_data);

	output_texture = ImageTexture::create_from_image(output_image);

	this->set_texture(output_texture);
}

void WFC::autocompleteImage() {
	std::vector<uint8_t> output_pixels = generateOutputPixelImage();
	if (output_pixels.empty()) {
		UtilityFunctions::print("WFC: Failed to generate output pixels. Please check the input "
								"sprite and configuration.");
		return;
	}

	mapPixelsToTexture(output_pixels);
}

std::vector<uint8_t> WFC::generateOutputPixelImage() {
	if (!wfc_core) {
		ERR_PRINT("WFCCore is null. Please call initializeWFCCore() first.");
		return {};
	}

	size_t start_index;
	if (config->get_start_index() < 0 || config->get_start_index() >= grid_width * grid_height) {
		std::uniform_int_distribution<size_t> dist(0, grid_width * grid_height - 1);
		std::mt19937 rng(std::random_device{}());
		start_index = dist(rng);
	} else {
		start_index = static_cast<size_t>(config->get_start_index());
	}

	std::span<const pattern_id_t> generated_patterns = wfc_core->solve(grid_width, grid_height,
			start_index, config->get_force_boundary_patterns(),
			config->get_cell_selection_strategy(), fixed_cells);

	// Convert the collapsed patterns back to pixel data
	return overlapping_patterns->convertIdsToPixels(generated_patterns, grid_width, grid_height);
}

void WFC::initializeWFCCore() {
	if (!overlapping_patterns) {
		ERR_PRINT("OverlappingPatterns is null. Please call generateOverlappingPatterns() first.");
		return;
	}

	// If config seed is negative use a random seed
	uint64_t seed;
	if (config->get_seed() < 0) {
		seed = std::chrono::system_clock::now().time_since_epoch().count();
	} else {
		seed = static_cast<uint64_t>(config->get_seed());
	}

	size_t pattern_size = config->get_pattern_size();
	grid_width = config->get_width() - pattern_size + 1;
	grid_height = config->get_height() - pattern_size + 1;
	wfc_core = std::make_unique<WFCCore>(overlapping_patterns->getAdjacencyData(), seed);
	wfc_core->startSolver(
			grid_width, grid_height, config->get_force_boundary_patterns(), fixed_cells);
}

void WFC::generateOverlappingPatterns() {
	if (!sprite_holder) {
		ERR_PRINT("SpriteHolder is null. Please call convertInputSpriteToPixels() first.");
		return;
	}

	overlapping_patterns =
			std::make_unique<OverlappingPatterns>(*sprite_holder, config->get_pattern_size(),
					config->get_boundary_condition(), config->get_transform_flags());
}

void WFC::convertInputSpriteToPixels() {
	if (!input_sprite) {
		ERR_PRINT("Input sprite is null.");
		return;
	}

	// Check if texture is valid
	Ref<Texture2D> texture = input_sprite->get_texture();
	if (texture.is_null()) {
		ERR_PRINT("Sprite has no texture!");
		return;
	}

	Ref<Image> img = texture->get_image();
	if (img.is_null()) {
		ERR_PRINT("Failed to get image from texture!");
		return;
	}
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

void WFC::_bind_methods() {
	// Public methods
	ClassDB::bind_method(D_METHOD("getPatternTextures"), &WFC::getPatternTextures);
	ClassDB::bind_method(
			D_METHOD("validCellsForPattern", "pattern_id"), &WFC::validCellsForPattern);
	ClassDB::bind_method(D_METHOD("setPatternAtPosition", "cell_index", "pattern_id"),
			&WFC::setPatternAtPosition);
	ClassDB::bind_method(D_METHOD("autocompleteImage"), &WFC::autocompleteImage);
	ClassDB::bind_method(D_METHOD("resetImage"), &WFC::resetImage);
	ClassDB::bind_method(
			D_METHOD("erasePatternAtPosition", "cell_pos"), &WFC::erasePatternAtPosition);

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

bool WFC::erasePatternAtPosition(const Vector2i &cell_pos) {
	if (!wfc_core) {
		ERR_PRINT("WFCCore is null. Please call initializeWFCCore() first.");
		return false;
	}
	if (cell_pos.x >= static_cast<int>(grid_width) || cell_pos.y >= static_cast<int>(grid_height)) {
		return false; // out of bounds
	}

	size_t cell_index = cell_pos.x + cell_pos.y * grid_width;
	if (!wfc_core->checkIfCellCollapsed(cell_index)) {
		return false;
	}

	// It's easier to just reset the entire grid and add the remaining fixed cells one by one
	fixed_cells.erase(cell_index);
	pixel_data.fill(0);
	output_image->set_data(
			config->get_width(), config->get_height(), false, Image::FORMAT_RGBA8, pixel_data);
	output_texture->update(output_image);

	// Do not add the fixed cells yet or the texture won't be updated
	wfc_core->startSolver(grid_width, grid_height, config->get_force_boundary_patterns(), {});

	for (auto &[cell_index, pattern_id] : fixed_cells) {
		Vector2i cell_pos(cell_index % grid_width, cell_index / grid_width);
		setPatternAtPosition(cell_pos, pattern_id);
	}

	return true;
}

void WFC::mapPixelsToTexture(const std::vector<uint8_t> &pixels) {
	if (pixels.empty()) {
		ERR_PRINT("Pixel data is empty. Cannot map to texture.");
		return;
	}
	memcpy(pixel_data.ptrw(), pixels.data(), pixels.size());

	output_image->set_data(
			config->get_width(), config->get_height(), false, Image::FORMAT_RGBA8, pixel_data);

	output_texture->update(output_image);

	this->set_texture(output_texture);
}

void WFC::resetImage() {
	if (!wfc_core) {
		ERR_PRINT("WFCCore is null. Please call initializeWFCCore() first.");
		return;
	}

	fixed_cells.clear();
	pixel_data.fill(0);
	output_image->set_data(
			config->get_width(), config->get_height(), false, Image::FORMAT_RGBA8, pixel_data);
	output_texture->update(output_image);
	this->set_texture(output_texture);

	wfc_core->startSolver(config->get_width() - config->get_pattern_size() + 1,
			config->get_height() - config->get_pattern_size() + 1,
			config->get_force_boundary_patterns(), fixed_cells);
}

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

bool WFC::setPatternAtPosition(const Vector2i &cell_pos, pattern_id_t pattern_id) {
	if (!wfc_core) {
		ERR_PRINT("WFCCore is null. Please call initializeWFCCore() first.");
		return false;
	}

	if (cell_pos.x >= static_cast<int>(grid_width) || cell_pos.y >= static_cast<int>(grid_height)) {
		return false;
	}

	size_t cell_index = cell_pos.x + cell_pos.y * grid_width;

	bool success = wfc_core->collapseSelectedCell(cell_index, pattern_id);
	if (success) {
		fixed_cells[cell_index] = pattern_id;
		std::vector<uint8_t> pattern_pixels =
				overlapping_patterns->convertIdsToPixels({ &pattern_id, 1 }, 1, 1);
		size_t start_index = (cell_pos.y * config->get_width() + cell_pos.x) * 4;
		for (size_t dy = 0; dy < config->get_pattern_size(); ++dy) {
			for (size_t dx = 0; dx < config->get_pattern_size(); ++dx) {
				size_t pixel_index = (dy * config->get_pattern_size() + dx) * 4;
				size_t output_index = start_index + (dy * config->get_width() + dx) * 4;
				memcpy(pixel_data.ptrw() + output_index, &pattern_pixels[pixel_index], 4);
			}
		}
		// Update the output texture with the new pixel data
		output_image->set_data(
				config->get_width(), config->get_height(), false, Image::FORMAT_RGBA8, pixel_data);
		output_texture->update(output_image);
		this->set_texture(output_texture);
	}

	return success;
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