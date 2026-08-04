extends Node2D

class_name ProcGenCave

@onready var wfc_generator: WFC = $WFC
@onready var player: Player = $Player

func _ready() -> void:
  var pattern_id: int = 6
  var path_start: Vector2i = Vector2i(0, 9)
  var path_end: Vector2i = Vector2i(62, 35)
  var path: Array[Vector2i] = PathFinder2D.pathfinder(path_start, path_end, 0.5)
  for tile in path:
    wfc_generator.setPatternAtPosition(tile, pattern_id)

  wfc_generator.autocompleteImage()
  wfc_generator.scale = Vector2(get_viewport().size) / wfc_generator.texture.get_size()
  player.position = GlobalVariables.player_spawn_pos
