#pragma once

#include "godot_cpp/classes/resource.hpp"
#include <wfc_settings.h>

using namespace godot;

class WFCConfig : public Resource {
	GDCLASS(WFCConfig, Resource)

protected:
	uint8_t transform_flags;
	uint32_t width;
	uint32_t height;
	int seed;
	uint8_t dimensions;
	uint8_t pattern_size;
	BoundaryCondition boundary_condition;
	CellSelectionStrategy cell_selection_strategy;
	bool force_boundary_patterns;

	static void _bind_methods();

public:
	WFCConfig() {
		width = 128;
		height = 128;
		seed = -1;
		dimensions = 2;
		boundary_condition = BoundaryCondition::NONE;
		cell_selection_strategy = CellSelectionStrategy::SCANLINE;
		pattern_size = 3;
		force_boundary_patterns = false;
	}
	~WFCConfig() override = default;

	int get_seed() const { return seed; }
	void set_seed(int p_seed) { seed = p_seed; }
	uint8_t get_dimensions() const { return dimensions; }
	void set_dimensions(uint8_t p_dimensions) { dimensions = p_dimensions; }

	BoundaryCondition get_boundary_condition() const { return boundary_condition; }
	void set_boundary_condition(BoundaryCondition p_boundary_condition) {
		printf("Setting condition to: %d\n", (int)p_boundary_condition);
		boundary_condition = p_boundary_condition;
	}

	CellSelectionStrategy get_cell_selection_strategy() const { return cell_selection_strategy; }
	void set_cell_selection_strategy(CellSelectionStrategy p_cell_selection_strategy) {
		cell_selection_strategy = p_cell_selection_strategy;
	}

	uint8_t get_pattern_size() const { return pattern_size; }
	void set_pattern_size(uint8_t p_pattern_size) { pattern_size = p_pattern_size; }

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
};

VARIANT_ENUM_CAST(BoundaryCondition);
VARIANT_ENUM_CAST(CellSelectionStrategy);