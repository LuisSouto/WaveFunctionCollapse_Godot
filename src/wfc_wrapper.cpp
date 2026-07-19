#include "godot_cpp/core/object.hpp"
#include <wfc_wrapper.h>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void WFC::_bind_methods() {
	// WFC settings
	ClassDB::bind_method(D_METHOD("get_settings"), &WFC::getSettings);
	ClassDB::bind_method(D_METHOD("set_settings", "p_settings"),
			&WFC::setSettings);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "settings", PROPERTY_HINT_RESOURCE_TYPE, "WFCSettings"), "set_settings",
			"get_settings");
}

Ref<WFCConfig> WFC::getSettings() const {
	return settings;
}

void WFC::setSettings(const Ref<WFCConfig> &p_settings) {
	settings = p_settings;
}