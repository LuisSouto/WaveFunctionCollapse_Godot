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
		std::vector<uint8_t> output_pixels = computeOutputPixels();
		mapPixelsToTexture();
	}
	this->set_visible(true);
};

Ref<WFCConfig> WFC::getConfig() const { return config; }

void WFC::setConfig(const Ref<WFCConfig> &p_config) { config = p_config; }

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
	sprite_holder = new SpriteHolder(width, height, 4, std::move(pixel_vector));
}

std::vector<uint8_t> WFC::computeOutputPixels() {
	if (!sprite_holder) {
		ERR_PRINT("SpriteHolder is null. Please call convertInputSpriteToPixels() first.");
		return std::vector<uint8_t>();
	}

	// Create an instance of OverlappingPatterns
	OverlappingPatterns overlapping_patterns(*sprite_holder, config->get_pattern_size());

	// Solve the WFC problem
	size_t adj_width = config->get_width() - config->get_pattern_size() + 1;
	size_t adj_height = config->get_height() - config->get_pattern_size() + 1;
	WFCCore wfc_core(overlapping_patterns.getAdjacencyData(), config->get_seed());
	std::span<const pattern_id_t> collapsed_patterns =
			wfc_core.solve(adj_width, adj_height, 0, config->get_force_boundary_patterns(),
					config->get_cell_selection_strategy(), config->get_boundary_condition());

	// Convert the collapsed patterns back to pixel data
	return overlapping_patterns.convertIdsToPixels(collapsed_patterns, adj_width, adj_height);
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