extends Node2D

class_name DualGridExample

@onready var wfc: WFC = $WFC
@onready var visualLayer: TileMapLayer = $VisualLayer

func _ready() -> void:
	visualLayer.position = -get_viewport().size/2	
	wfc.autocompleteImage()
	var pattern_ids: PackedInt32Array = wfc.getDualGridPatterns()
	var tilemap_size: int = wfc.config.width - 1
	var pattern_map: Array[int] = [6, 5, 2, 3, 10, 1, 4, 13, 7, 14, 11, 0, 9, 8, 15, 12]

	var tile_pos: Vector2i = Vector2i.ZERO
	for y in range(tilemap_size):
		for x in range(tilemap_size):
			tile_pos = Vector2i(x, y)
			var pattern_id: int = pattern_ids[y * tilemap_size + x]
			visualLayer.set_cell(tile_pos, 0, Vector2i(pattern_map[pattern_id], 0))
	visualLayer.scale = Vector2(get_viewport().size) / (tilemap_size * Vector2(visualLayer.tile_set.tile_size))
