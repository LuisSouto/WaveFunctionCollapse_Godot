extends Control

class_name WFCPanel

@onready var wfc_solver: WFC = $"WFC"
@onready var list_pattern_textures: GridContainer = $"MainWindow/WindowMargins/HBoxContainer/ScrollContainer/PatternTextures"
@onready var highlight_texture: ColorRect = $"MainWindow/WindowMargins/HBoxContainer/PanelContainer/HighlightTexture"
@export var input_pattern_scene: PackedScene 

func _ready():
	var pattern_textures: Array[Texture2D] = wfc_solver.getPatternTextures()
	var pattern_id: int = 0
	for texture in pattern_textures:
		var texture_panel = input_pattern_scene.instantiate()
		texture_panel.pattern_id = pattern_id
		texture_panel.pattern_selected.connect(on_pattern_selected)
		list_pattern_textures.add_child(texture_panel)		
		texture_panel.set_texture(texture)
		pattern_id += 1

func on_pattern_selected(pattern_id: int) -> void:
	var highlight_map: Texture2D = wfc_solver.validCellsForPattern(pattern_id)

	highlight_texture.material.set_shader_parameter("highlight_map", highlight_map)
	highlight_texture.visible = true
