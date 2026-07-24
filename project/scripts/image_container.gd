extends PanelContainer

class_name ImageContainer

signal pixel_clicked(pixel_pos: Vector2i)

@onready var empty_texture: ColorRect = $"EmptyTexture"
@onready var highlight_texture: ColorRect = $"HighlightTexture"
@onready var image_texture: TextureRect = $"ImageTexture"
var texture_size: Vector2

func resize_textures(new_size: Vector2) -> void:
	texture_size = Vector2(new_size)
	empty_texture.material.set_shader_parameter("grid_size", new_size)

func _gui_input(event: InputEvent) -> void:
	if event is InputEventMouseButton and event.pressed and event.button_index == MOUSE_BUTTON_LEFT:
		var local_mouse_pos: Vector2 = image_texture.get_local_mouse_position()
		var pixel_pos: Vector2i = get_pixel_position(local_mouse_pos)
		emit_signal("pixel_clicked", pixel_pos)

func get_pixel_position(local_pos: Vector2) -> Vector2i:
	var normalized_pos: Vector2 = local_pos / self.size
	return Vector2i(normalized_pos * texture_size)
