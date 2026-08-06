extends Node2D

class_name ProcGenCave

@onready var wfc_generator: WFC = $WFC
@onready var player: Player = $Player

func _ready() -> void:
  var pattern_id: int = 6 # One could also make a list of "path" patterns and randomly sample from it. Although it may be simpler to apply nicer textures on a second pass and only use WFC for the general layout.

  # Start and end can be either hardcoded or obtained from other procedural generation methods. For example, binary partition could divide the level into rooms, including the entrances and exits for each room.
  var path_start: Vector2i = Vector2i(0, 9)
  var path_end: Vector2i = Vector2i(62, 35)
  var path: Array[Vector2i] = PathFinder2D.pathfinder(path_start, path_end, 0.5)
  for tile in path:
    # TODO(Luis): it may be more efficient to pass the entire array. Otherwise we are updating the texture for every single update.
    wfc_generator.setPatternAtPosition(tile, pattern_id)

  wfc_generator.autocompleteImage()
  wfc_generator.scale = Vector2(get_viewport().size) / wfc_generator.texture.get_size()
  player.position = GlobalVariables.player_spawn_pos
