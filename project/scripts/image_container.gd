extends PanelContainer

class_name ImageContainer

signal pixel_clicked_draw(pixel_pos: Vector2i)
signal pixel_clicked_erase(pixel_pos: Vector2i)


@onready var empty_texture: ColorRect = $"EmptyTexture"
@onready var highlight_texture: ColorRect = $"HighlightTexture"
@onready var image_texture: TextureRect = $"ImageTexture"
@onready var pixel_pos_label: Label = $"PixelPositionLabel"
var texture_size: Vector2

func resize_textures(new_size: Vector2) -> void:
	texture_size = Vector2(new_size)
	empty_texture.material.set_shader_parameter("grid_size", new_size)
	mouse_exited.connect(on_mouse_exited)

func on_mouse_exited() -> void:
	pixel_pos_label.text = "[  ,  ]"

func _gui_input(event: InputEvent) -> void:
	if event is InputEventMouseMotion:
		var pixel_pos: Vector2i = get_pixel_position(image_texture.get_local_mouse_position())
		pixel_pos_label.text = "[" + str(pixel_pos.x) + ", " + str(pixel_pos.y) + "]"


	if event is InputEventMouseButton and event.pressed and event.button_index == MOUSE_BUTTON_LEFT:
		var pixel_pos: Vector2i = get_pixel_position(image_texture.get_local_mouse_position())
		notify_pixel_clicked(pixel_pos)

	if event is InputEventMouseMotion and event.button_mask & MOUSE_BUTTON_MASK_LEFT:
		var pixel_pos: Vector2i = get_pixel_position(image_texture.get_local_mouse_position())
		notify_pixel_clicked(pixel_pos)

	if event is InputEventMouseButton and event.pressed and event.button_index == MOUSE_BUTTON_RIGHT:
		var pixel_pos: Vector2i = get_pixel_position(image_texture.get_local_mouse_position())	
		notify_pixel_clicked_erase(pixel_pos)

	if event is InputEventMouseMotion and event.button_mask & MOUSE_BUTTON_MASK_RIGHT:
		var pixel_pos: Vector2i = get_pixel_position(image_texture.get_local_mouse_position())
		notify_pixel_clicked_erase(pixel_pos)

func notify_pixel_clicked(grid_pos: Vector2i) -> void:
	pixel_clicked_draw.emit(grid_pos)

func notify_pixel_clicked_erase(grid_pos: Vector2i) -> void:
	pixel_clicked_erase.emit(grid_pos)

func get_pixel_position(local_pos: Vector2) -> Vector2i:
	var normalized_pos: Vector2 = local_pos / self.size
	return Vector2i(normalized_pos * texture_size)
