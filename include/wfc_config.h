#pragma once

#include "godot_cpp/classes/resource.hpp"
#include "sprite_transforms.h"
#include <wfc_settings.h>

using namespace godot;

class WFCConfig : public Resource {
	GDCLASS(WFCConfig, Resource)

protected:
	uint32_t width;
	uint32_t height;
	int seed;
	int start_index;
	BoundaryCondition boundary_condition;
	CellSelectionStrategy cell_selection_strategy;
	uint8_t pattern_length;
	uint8_t transform_flags;
	bool force_boundary_patterns;

	static void _bind_methods();

public:
	WFCConfig() {
		width = 64;
		height = 64;
		seed = -1;
		start_index = -1;
		boundary_condition = BoundaryCondition::NONE;
		cell_selection_strategy = CellSelectionStrategy::SCANLINE;
		pattern_length = 3;
		force_boundary_patterns = false;
		transform_flags = SpriteTransforms::IDENTITY;
	}
	~WFCConfig() override = default;

	int get_seed() const { return seed; }
	void set_seed(int p_seed) { seed = p_seed; }

	BoundaryCondition get_boundary_condition() const { return boundary_condition; }
	void set_boundary_condition(BoundaryCondition p_boundary_condition) {
		boundary_condition = p_boundary_condition;
	}

	CellSelectionStrategy get_cell_selection_strategy() const { return cell_selection_strategy; }
	void set_cell_selection_strategy(CellSelectionStrategy p_cell_selection_strategy) {
		cell_selection_strategy = p_cell_selection_strategy;
	}

	uint8_t get_pattern_length() const { return pattern_length; }
	void set_pattern_length(uint8_t p_pattern_length) { pattern_length = p_pattern_length; }

	uint32_t get_width() const { return width; }
	void set_width(uint32_t p_width) { width = p_width; }

	uint32_t get_height() const { return height; }
	void set_height(uint32_t p_height) { height = p_height; }

	bool get_force_boundary_patterns() const { return force_boundary_patterns; }
	void set_force_boundary_patterns(bool p_force_boundary_patterns) {
		force_boundary_patterns = p_force_boundary_patterns;
	}

	uint8_t get_transform_flags() const { return transform_flags; }
	void set_transform_flags(uint8_t p_transform_flags) { transform_flags = p_transform_flags; }

	int get_start_index() const { return start_index; }
	void set_start_index(int p_start_index) { start_index = p_start_index; }
};

VARIANT_ENUM_CAST(BoundaryCondition);
VARIANT_ENUM_CAST(CellSelectionStrategy);