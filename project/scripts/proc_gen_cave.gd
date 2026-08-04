extends Node2D

class_name ProcGenCave

@onready var wfc_generator: WFC = $WFC
@onready var player: Player = $Player

func _ready() -> void:
  wfc_generator.autocompleteImage()
  wfc_generator.scale = Vector2(get_viewport().size) / wfc_generator.texture.get_size()
  player.position = GlobalVariables.player_spawn_pos
