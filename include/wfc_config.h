#pragma once

#include "godot_cpp/classes/resource.hpp"
#include <wfc_settings.h>

using namespace godot;

class WFCConfig : public Resource {
	GDCLASS(WFCConfig, Resource)

protected:
	int seed;
	uint8_t dimensions;
	BoundaryCondition boundary_condition;
	CellSelectionStrategy cell_selection_strategy;
	uint8_t pattern_size;
	bool force_boundary_patterns;

	static void _bind_methods();

public:
	WFCConfig() {
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

	bool get_force_boundary_patterns() const { return force_boundary_patterns; }
	void set_force_boundary_patterns(bool p_force_boundary_patterns) {
		force_boundary_patterns = p_force_boundary_patterns;
	}
};

VARIANT_ENUM_CAST(BoundaryCondition);
VARIANT_ENUM_CAST(CellSelectionStrategy);