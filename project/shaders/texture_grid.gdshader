shader_type canvas_item;

uniform vec2 grid_size; // Size of the grid in pixels


void fragment() {
  vec2 check_pos = floor(UV * grid_size);
  float pattern = mod(check_pos.x + check_pos.y, 2.0);	

	// Use the classic checkered colors
  vec4 color1 = vec4(0.2, 0.2, 0.2, 1.0);
  vec4 color2 = vec4(0.3, 0.3, 0.3, 1.0);

	COLOR = mix(color1, color2, pattern);	
}

