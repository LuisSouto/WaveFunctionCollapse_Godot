#pragma once

#include "godot_cpp/classes/sprite2d.hpp"
#include <wfc_config.h>

using namespace godot;

class WFC : public Sprite2D {
	GDCLASS(WFC, Sprite2D)

protected:
	Ref<WFCConfig> settings;
	static void _bind_methods();
	void computeOutputPixels();
	void mapPixelsToImage();

public:
	WFC() = default;
	~WFC() override = default;

	Ref<WFCConfig> getSettings() const;
	void setSettings(const Ref<WFCConfig> &p_settings);
};
