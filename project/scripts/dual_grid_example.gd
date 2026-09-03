extends Node2D

class_name DualGridExample

@onready var wfc: WFC = $WFC
@onready var visualLayer: TileMapLayer = $VisualLayer
@onready var levelEntrance: CaveDoor = $Entrance
@onready var levelExit: CaveDoor = $Exit
@onready var player: Player = $Player
@onready var player_sprite: Sprite2D = $Player/Sprite2D
# This mapping is due to how I define my tilesets in Aseprite
static var pattern_map: Array[int] = [6, 5, 2, 3, 10, 1, 4, 13, 7, 14, 11, 0, 9, 8, 15, 12]

func _ready() -> void:
	# Fix a path from a start tile to an end tile. This could be used to ensure there is always a path between the entrance and exit	
	var fixed_pattern_id: int = wfc.findPathPattern()

	# Start and end can be either hardcoded or obtained from other procedural generation methods.
	var path_max_val: int = wfc.config.height - wfc.config.pattern_length
	var path_start: Vector2i = Vector2i(0, 6)
	var path_end: Vector2i = Vector2i(wfc.config.width - wfc.config.pattern_length, 9)
	var path: Array[Vector2i] = PathFinder2D.pathfinder(path_start, path_end, 1.5, path_max_val)
	var fixed_pattern_list: Array[int]
	fixed_pattern_list.resize(path.size())
	fixed_pattern_list.fill(fixed_pattern_id)
	wfc.fixPatternsAtCells(path, fixed_pattern_list)			
	wfc.solve()

	fillVisualLayer()

	levelEntrance.position = visualLayer.to_global(visualLayer.map_to_local(path_start))
	levelExit.position = visualLayer.to_global(visualLayer.map_to_local(path_end))
	if (GlobalVariables.entrance_id == 0):
		player.position = visualLayer.to_global(visualLayer.map_to_local(path_start) + 1.5 * visualLayer.tile_set.tile_size * Vector2(1, 0))
	else:
		player.position = visualLayer.to_global(visualLayer.map_to_local(path_end) - 1.5 * visualLayer.tile_set.tile_size * Vector2(1, 0))

	var min_axis: int = player_sprite.texture.get_size().min_axis_index()
	player.scale = visualLayer.scale * visualLayer.tile_set.tile_size[min_axis] / player_sprite.texture.get_size()[min_axis]


# Map the pattern ids from the simplified texture to the atlas of the visual layer
func fillVisualLayer() -> void:
	visualLayer.position = -get_viewport().size * 0.5	
	var pattern_ids: PackedInt32Array = wfc.getDualGridPatterns()
	var tilemap_size: int = wfc.config.width - wfc.config.pattern_length + 1

	var tile_pos: Vector2i = Vector2i.ZERO
	for y in range(tilemap_size):
		for x in range(tilemap_size):
			tile_pos = Vector2i(x, y)
			var pattern_id: int = pattern_ids[y * tilemap_size + x]
			visualLayer.set_cell(tile_pos, 0, Vector2i(pattern_map[pattern_id], 0))	
	
	# Scale the visual layer to occupy the entire viewport
	visualLayer.scale = Vector2(get_viewport().size) / (tilemap_size * Vector2(visualLayer.tile_set.tile_size))
