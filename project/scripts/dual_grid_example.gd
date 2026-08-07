extends Node2D

class_name DualGridExample

@onready var wfc: WFC = $WFC
@onready var visualLayer: TileMapLayer = $VisualLayer
@onready var levelEntrance: CaveDoor = $CaveDoor
@onready var player: Player = $Player
@onready var player_sprite: Sprite2D = $Player/Sprite2D
# This mapping is due to how I define my tilesets in Aseprite
static var pattern_map: Array[int] = [6, 5, 2, 3, 10, 1, 4, 13, 7, 14, 11, 0, 9, 8, 15, 12]

func _ready() -> void:
	visualLayer.position = -get_viewport().size * 0.5

	# Fix a path from a start tile to an end tile	
	var fixed_pattern_id: int = 10 # One could also make a list of "path" patterns and randomly sample from it. Either way the pattern ids that can be used for paths need to be known in advance.

	# Start and end can be either hardcoded or obtained from other procedural generation methods. For example, binary partition could divide the level into rooms, including the entrances and exits for each room.
	var path_start: Vector2i = Vector2i(0, 6)
	var path_end: Vector2i = Vector2i(wfc.config.width - wfc.config.pattern_size, 9)
	var path: Array[Vector2i] = PathFinder2D.pathfinder(path_start, path_end, 1)
	var fixed_pattern_list: Array[int]
	fixed_pattern_list.resize(path.size())
	fixed_pattern_list.fill(fixed_pattern_id)
	wfc.fixPatternsAtCells(path, fixed_pattern_list)			

	wfc.autocompleteImage()
	fillVisualLayer()

	levelEntrance.position = visualLayer.to_global(visualLayer.map_to_local(path_start))
	player.position = visualLayer.to_global(visualLayer.map_to_local(path_start + Vector2i(1, 0)))


# Map the pattern ids from the simplified texture to the atlas of the visual layer
func fillVisualLayer() -> void:
	var pattern_ids: PackedInt32Array = wfc.getDualGridPatterns()
	var tilemap_size: int = wfc.config.width - wfc.config.pattern_size + 1

	var tile_pos: Vector2i = Vector2i.ZERO
	for y in range(tilemap_size):
		for x in range(tilemap_size):
			tile_pos = Vector2i(x, y)
			var pattern_id: int = pattern_ids[y * tilemap_size + x]
			visualLayer.set_cell(tile_pos, 0, Vector2i(pattern_map[pattern_id], 0))	
	
	# Scale the visual layer to occupy the entire viewport
	visualLayer.scale = Vector2(get_viewport().size) / (tilemap_size * Vector2(visualLayer.tile_set.tile_size))
