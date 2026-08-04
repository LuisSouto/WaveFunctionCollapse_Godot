extends Node2D

@onready var player: Player = $Player

func _ready() -> void:
  player.position = GlobalVariables.player_spawn_pos
