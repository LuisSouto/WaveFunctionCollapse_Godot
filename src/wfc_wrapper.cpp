#include "wfc_wrapper.h"
#include "godot_cpp/classes/image.hpp"
#include "godot_cpp/classes/image_texture.hpp"
#include "godot_cpp/classes/texture2d.hpp"
#include "godot_cpp/core/object.hpp"
#include "godot_cpp/variant/packed_int32_array.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/vector2i.hpp"
#include "overlapping_patterns.h"
#include "wfc_core.h"
#include "wfc_typedefs.h"
#include <cstdint>
#include <vector>

using namespace godot;

void WFC::_ready() {
	if (config.is_null()) {
		ERR_PRINT("Config resource is not assigned!");
		return;
	}
	if (!input_sprite) {
		ERR_PRINT("Input sprite is not assigned!");
		return;
	}

	convertInputSpriteToPixels();
	generateOverlappingPatterns();
	initializeWFCCore();
	initializeOutputTexture();
};

void WFC::initializeOutputTexture() {
	if (!wfc_core) {
		ERR_PRINT("WFCCore is null. Please call initializeWFCCore() first.");
		return;
	}

	int output_width = config->get_width();
	int output_height = config->get_height();
	output_pixel_data = PackedByteArray();
	output_pixel_data.resize(output_width * output_height * sprite_holder->getChannels());
	output_pixel_data.fill(0); // Start with a blank image

	// Map pixels into texture
	output_image = Image::create_from_data(
			output_width, output_height, false, Image::FORMAT_RGBA8, output_pixel_data);

	output_texture = ImageTexture::create_from_image(output_image);

	this->set_texture(output_texture);
}

void WFC::solve() {
	if (!wfc_core) {
		ERR_PRINT("WFCCore is null. Please call initializeWFCCore() first.");
		return;
	}

	std::span<const pattern_id_t> generated_patterns = wfc_core->solve(grid_width, grid_height,
			config->get_start_index(), config->get_force_boundary_patterns(),
			config->get_cell_selection_strategy(), fixed_cells);

	// Here we don't throw error to let user handle it however they want
	if (generated_patterns.empty()) {
		UtilityFunctions::print("Failed to generate image. Please check the input "
								"sprite and configuration.");
		return;
	}

	mapPixelsToTexture(
			overlapping_patterns->convertIdsToPixels(generated_patterns, grid_width, grid_height));
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

	size_t pattern_length = config->get_pattern_length();
	grid_width = config->get_width() - pattern_length + 1;
	grid_height = config->get_height() - pattern_length + 1;
	wfc_core = std::make_unique<WFCCore>(overlapping_patterns->generateAdjacencyData(), seed);
	wfc_core->startSolver(grid_width, grid_height, config->get_force_boundary_patterns(),
			config->get_cell_selection_strategy(), fixed_cells);
}

void WFC::generateOverlappingPatterns() {
	if (!sprite_holder) {
		ERR_PRINT("SpriteHolder is null. Please call convertInputSpriteToPixels() first.");
		return;
	}

	overlapping_patterns =
			std::make_unique<OverlappingPatterns>(*sprite_holder, config->get_pattern_length(),
					config->get_boundary_condition(), config->get_transform_flags());
}

void WFC::convertInputSpriteToPixels() {
	if (!input_sprite) {
		ERR_PRINT("Input sprite is null.");
		return;
	}

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

	// Convert to RGBA format for consistency
	img->convert(Image::FORMAT_RGBA8);
	PackedByteArray pixel_data = img->get_data();

	std::vector<uint8_t> input_pixels(pixel_data.ptr(), pixel_data.ptr() + pixel_data.size());
	sprite_holder = std::make_unique<SpriteHolder>(width, height, 4, input_pixels);
}

void WFC::_bind_methods() {
	// Public methods
	ClassDB::bind_method(D_METHOD("getPatternTextures"), &WFC::getPatternTextures);
	ClassDB::bind_method(
			D_METHOD("validCellsForPattern", "pattern_id"), &WFC::validCellsForPattern);
	ClassDB::bind_method(D_METHOD("fixPatternsAtCells", "cell_positions", "pattern_ids"),
			&WFC::fixPatternsAtCells);
	ClassDB::bind_method(D_METHOD("solve"), &WFC::solve);
	ClassDB::bind_method(D_METHOD("resetImage"), &WFC::resetImage);
	ClassDB::bind_method(
			D_METHOD("erasePatternAtPosition", "cell_pos"), &WFC::erasePatternAtPosition);
	ClassDB::bind_method(D_METHOD("getDualGridPatterns"), &WFC::getDualGridPatterns);
	ClassDB::bind_method(D_METHOD("findPathPattern"), &WFC::findPathPattern);

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
		return true; // nothing to delete
	}

	// It's easier to just reset the entire grid and add the remaining fixed cells one by one
	fixed_cells.erase(cell_index);
	output_pixel_data.fill(0);

	// Do not add the fixed cells yet or the texture won't update
	wfc_core->startSolver(grid_width, grid_height, config->get_force_boundary_patterns(),
			config->get_cell_selection_strategy(), {});

	for (auto &[cell_index, pattern_id] : fixed_cells) {
		Vector2i cell_pos(cell_index % grid_width, cell_index / grid_width);
		setPatternAtCell(cell_pos, pattern_id);
	}
	updateTexture();

	return true;
}

void WFC::mapPixelsToTexture(const std::vector<uint8_t> &pixels) {
	if (pixels.empty()) {
		ERR_PRINT("Pixel data is empty. Cannot map to texture.");
		return;
	}
	memcpy(output_pixel_data.ptrw(), pixels.data(), pixels.size());

	output_image->set_data(config->get_width(), config->get_height(), false, Image::FORMAT_RGBA8,
			output_pixel_data);

	output_texture->update(output_image);

	this->set_texture(output_texture);
}

void WFC::resetImage() {
	if (!wfc_core) {
		ERR_PRINT("WFCCore is null. Please call initializeWFCCore() first.");
		return;
	}

	fixed_cells.clear();
	output_pixel_data.fill(0);
	updateTexture();

	wfc_core->startSolver(grid_width, grid_height, config->get_force_boundary_patterns(),
			config->get_cell_selection_strategy(), {});
}

/* Return all the patterns used by WFC */
TypedArray<Texture2D> WFC::getPatternTextures() {
	TypedArray<Texture2D> texture_list;

	const auto &patterns = overlapping_patterns->getInputPixelPatterns();
	size_t N = config->get_pattern_length();
	size_t pattern_size = N * N * sprite_holder->getChannels() * sizeof(uint8_t);
	size_t num_patterns = patterns.size() / pattern_size;

	for (size_t i = 0; i < num_patterns; ++i) {
		PackedByteArray buffer;
		buffer.resize(pattern_size);
		memcpy(buffer.ptrw(), &patterns[i * pattern_size], pattern_size);

		Ref<Image> image = Image::create_from_data(N, N, false, Image::FORMAT_RGBA8, buffer);

		Ref<Texture2D> texture = ImageTexture::create_from_image(image);

		texture_list.append(texture);
	}

	return texture_list;
}

bool WFC::fixPatternsAtCells(
		const TypedArray<Vector2i> &cell_positions, TypedArray<pattern_id_t> pattern_ids) {
	if (!wfc_core) {
		ERR_PRINT("WFCCore is null. Please call initializeWFCCore() first.");
		return false;
	}

	size_t num_cells = cell_positions.size();
	if (num_cells != pattern_ids.size()) {
		ERR_PRINT("Sizes of list of cell positions and list of patterns do not match.");
		return false;
	}

	for (size_t i = 0; i < num_cells; ++i) {
		Vector2i cell_pos = cell_positions[i];
		if (cell_pos.x >= static_cast<int>(grid_width) ||
				cell_pos.y >= static_cast<int>(grid_height)) {
			ERR_PRINT("Cell index is out of bounds.");
			return false;
		}
	}

	bool success = true;
	for (size_t i = 0; i < num_cells; ++i) {
		Vector2i cell_pos = cell_positions[i];
		pattern_id_t pattern_id = pattern_ids[i];
		success &= setPatternAtCell(cell_pos, pattern_id);
	}
	updateTexture();

	return success;
}

void WFC::updateTexture() {
	output_image->set_data(config->get_width(), config->get_height(), false, Image::FORMAT_RGBA8,
			output_pixel_data);
	output_texture->update(output_image);
	this->set_texture(output_texture);
}

bool WFC::setPatternAtCell(const Vector2i cell_pos, pattern_id_t pattern_id) {
	if (!wfc_core) {
		ERR_PRINT("WFCCore is null. Please call initializeWFCCore() first.");
		return false;
	}

	bool success = true;
	size_t cell_index = cell_pos.x + cell_pos.y * grid_width;
	size_t num_channels = sprite_holder->getChannels();
	size_t pattern_length = config->get_pattern_length();
	size_t pixels_per_row = pattern_length * num_channels * sizeof(uint8_t);
	size_t width = config->get_width();

	success &= wfc_core->collapseSelectedCell(cell_index, pattern_id);
	if (success) {
		fixed_cells[cell_index] = pattern_id;
		std::vector<uint8_t> pattern_pixels =
				overlapping_patterns->convertIdsToPixels({ &pattern_id, 1 }, 1, 1);
		size_t start_index = (cell_pos.y * config->get_width() + cell_pos.x) * num_channels;
		for (size_t dy = 0; dy < pattern_length; ++dy) {
			size_t pixel_index = (dy * pattern_length) * num_channels;
			size_t output_index = start_index + (dy * width) * num_channels;
			memcpy(output_pixel_data.ptrw() + output_index, &pattern_pixels[pixel_index],
					pixels_per_row);
		}
	}

	return success;
}

/* Returns a pixel map with the size of the WFC grid such that positions where the pattern can be
 * located are mapped to white pixels (255) and positions where it cannot be located to black pixels
 * (0).*/
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

	// It's only one channel so we use L format
	Ref<Image> valid_cells_image = Image::create_from_data(
			config->get_width(), config->get_height(), false, Image::FORMAT_L8, valid_cells_array);

	Ref<Texture2D> valid_cells_texture = ImageTexture::create_from_image(valid_cells_image);

	return valid_cells_texture;
}

// Note(Luis): the algorithm assumes that these are black and white images, such that white pixels
// get marked with a 1 and black pixels with a 0. Every NxN pixels this info is compiled into a
// number from 0 (all pixels black) to 2^N-1 (all white).
PackedInt32Array WFC::getDualGridPatterns() {
	size_t pattern_length = config->get_pattern_length();
	size_t num_channels = sprite_holder->getChannels();
	size_t pattern_stride = pattern_length * num_channels;
	size_t total_width = config->get_width();
	PackedInt32Array pattern_ids;
	pattern_ids.resize(grid_width * grid_height);
	for (size_t y = 0; y < grid_height; ++y) {
		for (size_t x = 0; x < grid_width; ++x) {
			size_t pixel_index = (x + y * total_width) * num_channels;
			int32_t pattern_id = 0;
			for (size_t dy = 0; dy < pattern_length; ++dy) {
				for (size_t dx = 0; dx < pattern_length; ++dx) {
					uint8_t pixel_value =
							output_pixel_data[pixel_index + (dx + dy * total_width) * num_channels];
					pattern_id |= (static_cast<int32_t>(pixel_value >> 7)
							<< ((dy * pattern_length + dx)));
				}
			}
			pattern_ids[x + y * grid_width] = pattern_id;
		}
	}

	return pattern_ids;
}

/* Finds a pattern whose pixels are all white. The assumption is that white means there's ground
 * while black means there's not. Thus a solid path tile could be used to manually draw paths and
 * later let WFC fill in the rest of the image. This implies that the input image is black and
 * white. Otherwise it will give wrong results.
 */
int WFC::findPathPattern() {
	const auto &patterns = overlapping_patterns->getInputPixelPatterns();
	size_t pattern_length = config->get_pattern_length();
	size_t num_patterns = overlapping_patterns->getNumPatterns();
	size_t pattern_size = pattern_length * pattern_length * sprite_holder->getChannels();
	uint8_t path_pattern = ~0; // 255

	for (size_t i = 0; i < num_patterns; ++i) {
		uint8_t pattern = ~0;
		for (size_t j = 0; j < pattern_size; ++j) {
			pattern &= patterns[i * pattern_size + j]; // Actually it just checks the R component
		}
		if (pattern == path_pattern)
			return i;
	}

	return -1; // Path pattern not present
}