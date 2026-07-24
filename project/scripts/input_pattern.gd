extends PanelContainer

class_name InputPattern

signal pattern_selected(pattern_id: int)

@onready var texture_rect: TextureRect = $"PatternTexture"
var pattern_id: int

func set_texture(texture: Texture) -> void:
	texture_rect.set_texture(texture)

func _gui_input(event: InputEvent) -> void:
	if event is InputEventMouseButton and event.pressed and event.button_index == MOUSE_BUTTON_LEFT:
		pattern_selected.emit(pattern_id)
