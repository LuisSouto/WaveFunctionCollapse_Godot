extends Control

class_name WFCPanel

@onready var wfc_solver: WFC = $"WFC"
@onready var list_pattern_textures: GridContainer = $"MainWindow/WindowMargins/HBoxContainer/ScrollContainer/PatternTextures"
@export var input_pattern_scene: PackedScene 

func _ready():
	var pattern_textures: Array[Texture2D] = wfc_solver.getPatternTextures()
	for texture in pattern_textures:
		var texture_panel = input_pattern_scene.instantiate()
		list_pattern_textures.add_child(texture_panel)		
		texture_panel.set_texture(texture)
