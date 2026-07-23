extends PanelContainer

class_name InputPattern

@onready var texture_rect: TextureRect = $"PatternTexture"

func set_texture(texture: Texture) -> void:
	texture_rect.set_texture(texture)
