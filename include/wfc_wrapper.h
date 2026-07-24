#pragma once

#include "godot_cpp/classes/sprite2d.hpp"
#include "overlapping_patterns.h"
#include "sprite_holder.h"
#include <sprite_reader.h>
#include <wfc_config.h>
#include <wfc_core.h>
#include <godot_cpp/classes/image_texture.hpp>
#include <memory>

using namespace godot;

class WFC : public Sprite2D {
	GDCLASS(WFC, Sprite2D)

protected:
	std::vector<uint8_t> input_patterns_as_pixels;
	Ref<WFCConfig> config;
	std::unique_ptr<Sprite2D> input_sprite = nullptr;
	std::unique_ptr<SpriteHolder> sprite_holder = nullptr;
	std::unique_ptr<WFCCore> wfc_core = nullptr;
	std::unique_ptr<OverlappingPatterns> overlapping_patterns = nullptr;
	static void _bind_methods();
	void convertInputSpriteToPixels();
	void computeAdjacencyData();
	void initializeWFCCore();
	std::vector<uint8_t> computeOutputPixels();
	void mapPixelsToTexture();

public:
	WFC() = default;
	~WFC() override = default;

	void _ready() override;

	Ref<WFCConfig> getConfig() const;
	void setConfig(const Ref<WFCConfig> &p_config);

	Sprite2D *getInputSprite() const { return input_sprite.get(); }
	void setInputSprite(Sprite2D *p_input_sprite) { input_sprite.reset(p_input_sprite); }

	TypedArray<Texture2D> getPatternTextures();

	Ref<Texture2D> validCellsForPattern(pattern_id_t pattern_id);
};
