shader_type canvas_item;

uniform sampler2D highlight_map;

void fragment() {
	vec2 grid_shape = vec2(textureSize(highlight_map, 0));
	ivec2 grid_pos = ivec2(floor(UV * grid_shape));
  float is_highlighted = texelFetch(highlight_map, grid_pos, 0).r;

  if (is_highlighted > 0.0) {
			COLOR = vec4(1.0, 1.0, 0.0, 0.6); // Apply highlight
	}
}


