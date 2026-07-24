extends PanelContainer

class_name InputPattern

signal pattern_selected(panel: InputPattern)

@onready var texture_rect: TextureRect = $"PatternTexture"
var pattern_id: int

func set_texture(texture: Texture) -> void:
	texture_rect.set_texture(texture)

func _gui_input(event: InputEvent) -> void:
	if event is InputEventMouseButton and event.pressed and event.button_index == MOUSE_BUTTON_LEFT:
		highlight()
		pattern_selected.emit(self)

func change_style_border_color(color: Color) -> void:
	self.self_modulate = color

func highlight() -> void:
	change_style_border_color(Color.GREEN)

func unhighlight() -> void:
	change_style_border_color(Color.YELLOW)
