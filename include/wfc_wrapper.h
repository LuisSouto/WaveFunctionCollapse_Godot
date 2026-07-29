#pragma once

#include "godot_cpp/classes/image_texture.hpp"
#include "godot_cpp/classes/sprite2d.hpp"
#include "overlapping_patterns.h"
#include "sprite_holder.h"
#include "wfc_typedefs.h"
#include <sprite_reader.h>
#include <wfc_config.h>
#include <wfc_core.h>
#include <cstddef>
#include <memory>
#include <unordered_map>

using namespace godot;

class WFC : public Sprite2D {
	GDCLASS(WFC, Sprite2D)

protected:
	std::unordered_map<size_t, pattern_id_t> fixed_cells = {};
	PackedByteArray pixel_data;
	std::vector<uint8_t> input_patterns_as_pixels;
	Ref<WFCConfig> config;
	Sprite2D *input_sprite = nullptr;
	Ref<Image> output_image;
	Ref<ImageTexture> output_texture;
	std::unique_ptr<SpriteHolder> sprite_holder = nullptr;
	std::unique_ptr<WFCCore> wfc_core = nullptr;
	std::unique_ptr<OverlappingPatterns> overlapping_patterns = nullptr;
	size_t grid_width;
	size_t grid_height;

	static void _bind_methods();
	void convertInputSpriteToPixels();
	void generateOverlappingPatterns();
	void initializeWFCCore();
	void initializeOutputTexture();
	std::vector<uint8_t> generateOutputPixelImage();
	void mapPixelsToTexture(const std::vector<uint8_t> &pixels);

public:
	WFC() = default;
	~WFC() override = default;

	void autocompleteImage();

	bool erasePatternAtPosition(const Vector2i &cell_pos);

	void _ready() override;

	Ref<WFCConfig> getConfig() const { return config; };
	void setConfig(const Ref<WFCConfig> &p_config) { config = p_config; };

	Sprite2D *getInputSprite() const { return input_sprite; }
	void setInputSprite(Sprite2D *p_input_sprite) { input_sprite = p_input_sprite; }

	TypedArray<Texture2D> getPatternTextures();

	void resetImage();

	bool setPatternAtPosition(const Vector2i &cell_pos, pattern_id_t pattern_id);

	Ref<Texture2D> validCellsForPattern(pattern_id_t pattern_id);
};
