#include <wfc_config.h>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void WFCConfig::_bind_methods() {
	// Bind properties
	ClassDB::bind_method(D_METHOD("get_seed"), &WFCConfig::get_seed);
	ClassDB::bind_method(D_METHOD("set_seed", "seed"), &WFCConfig::set_seed);
	ClassDB::bind_method(D_METHOD("get_dimensions"), &WFCConfig::get_dimensions);
	ClassDB::bind_method(D_METHOD("set_dimensions", "dimensions"), &WFCConfig::set_dimensions);
	ClassDB::bind_method(D_METHOD("get_boundary_condition"), &WFCConfig::get_boundary_condition);
	ClassDB::bind_method(D_METHOD("set_boundary_condition", "boundary_condition"),
			&WFCConfig::set_boundary_condition);
	ClassDB::bind_method(
			D_METHOD("get_cell_selection_strategy"), &WFCConfig::get_cell_selection_strategy);
	ClassDB::bind_method(D_METHOD("set_cell_selection_strategy", "cell_selection_strategy"),
			&WFCConfig::set_cell_selection_strategy);
	ClassDB::bind_method(D_METHOD("get_pattern_size"), &WFCConfig::get_pattern_size);
	ClassDB::bind_method(
			D_METHOD("set_pattern_size", "pattern_size"), &WFCConfig::set_pattern_size);
	ClassDB::bind_method(
			D_METHOD("get_force_boundary_patterns"), &WFCConfig::get_force_boundary_patterns);
	ClassDB::bind_method(D_METHOD("set_force_boundary_patterns", "force_boundary_patterns"),
			&WFCConfig::set_force_boundary_patterns);
	ClassDB::bind_method(D_METHOD("get_width"), &WFCConfig::get_width);
	ClassDB::bind_method(D_METHOD("set_width", "width"), &WFCConfig::set_width);
	ClassDB::bind_method(D_METHOD("get_height"), &WFCConfig::get_height);
	ClassDB::bind_method(D_METHOD("set_height", "height"), &WFCConfig::set_height);
	ClassDB::bind_method(D_METHOD("get_transform_flags"), &WFCConfig::get_transform_flags);
	ClassDB::bind_method(
			D_METHOD("set_transform_flags", "transform_flags"), &WFCConfig::set_transform_flags);

	// Bind enums
	BIND_ENUM_CONSTANT(BoundaryCondition::NONE);
	BIND_ENUM_CONSTANT(BoundaryCondition::PERIODIC_X);
	BIND_ENUM_CONSTANT(BoundaryCondition::PERIODIC_Y);
	BIND_ENUM_CONSTANT(BoundaryCondition::PERIODIC);
	BIND_ENUM_CONSTANT(CellSelectionStrategy::SCANLINE);
	BIND_ENUM_CONSTANT(CellSelectionStrategy::ENTROPY);

	// Add properties to the class
	ADD_PROPERTY(PropertyInfo(Variant::INT, "seed"), "set_seed", "get_seed");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "dimensions"), "set_dimensions", "get_dimensions");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "boundary_condition", PROPERTY_HINT_ENUM,
						 "None,Periodic X,Periodic Y,Periodic", PROPERTY_USAGE_DEFAULT,
						 "Boundary Condition"),
			"set_boundary_condition", "get_boundary_condition");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "cell_selection_strategy", PROPERTY_HINT_ENUM,
						 "Scanline,Entropy", PROPERTY_USAGE_DEFAULT, "Cell Selection Strategy"),
			"set_cell_selection_strategy", "get_cell_selection_strategy");
	ADD_PROPERTY(
			PropertyInfo(Variant::INT, "pattern_size"), "set_pattern_size", "get_pattern_size");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "force_boundary_patterns"),
			"set_force_boundary_patterns", "get_force_boundary_patterns");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "width", PROPERTY_HINT_RANGE, "1,1024,1"), "set_width",
			"get_width");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "height", PROPERTY_HINT_RANGE, "1,1024,1"),
			"set_height", "get_height");
	ADD_PROPERTY(
			PropertyInfo(Variant::INT, "transform_flags", PROPERTY_HINT_FLAGS,
					"Identity, Rotate 90, Rotate 180, Rotate 270, Flip Horizontal, Flip Vertical",
					PROPERTY_USAGE_DEFAULT, "Transforms"),
			"set_transform_flags", "get_transform_flags");
}