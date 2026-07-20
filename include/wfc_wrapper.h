#pragma once

#include "godot_cpp/classes/sprite2d.hpp"
#include <sprite_reader.h>
#include <wfc_config.h>
#include <memory>

using namespace godot;

class WFC : public Sprite2D {
	GDCLASS(WFC, Sprite2D)

protected:
	Ref<WFCConfig> config;
	std::unique_ptr<Sprite2D> input_sprite = nullptr;
	SpriteHolder *sprite_holder = nullptr;
	static void _bind_methods();
	void convertInputSpriteToPixels();
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
};
