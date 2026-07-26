extends Control

class_name WFCPanel

@onready var wfc_solver: WFC = $"WFC"
@onready var list_pattern_textures: GridContainer = $"MainWindow/WindowMargins/HBoxContainer/VBoxContainer/ScrollContainer/PatternTextures"
@onready var image_container: ImageContainer = $"MainWindow/WindowMargins/HBoxContainer/ImageContainer"
@onready var actions_button: ActionsMenu = $"MainWindow/WindowMargins/HBoxContainer/VBoxContainer/ActionsMenu"

@export var input_pattern_scene: PackedScene 
var selected_panel: InputPattern
var width: int
var height: int

func _ready():
	# Initialize textures
	width = wfc_solver.config.width
	height = wfc_solver.config.height
	image_container.resize_textures(Vector2(width, height))
	image_container.pixel_clicked_draw.connect(on_pixel_clicked_draw)
	image_container.pixel_clicked_erase.connect(on_pixel_clicked_erase)
	image_container.image_texture.set_texture(wfc_solver.texture)

	# Connect action button and add shortcuts
	actions_button.get_popup().id_pressed.connect(on_actions_button_id_pressed)

	display_input_patterns()

func display_input_patterns() -> void:
	var pattern_textures: Array[Texture2D] = wfc_solver.getPatternTextures()
	var pattern_id: int = 0
	for texture in pattern_textures:
		var texture_panel = input_pattern_scene.instantiate()
		texture_panel.pattern_id = pattern_id
		texture_panel.pattern_selected.connect(on_pattern_selected)
		list_pattern_textures.add_child(texture_panel)		
		texture_panel.set_texture(texture)
		pattern_id += 1

func on_pattern_selected(panel: InputPattern) -> void:
	if selected_panel:
		selected_panel.unhighlight()

	selected_panel = panel
	selected_panel.highlight()

	generate_highlight_map()

func generate_highlight_map() -> void:
	if selected_panel:
		var highlight_map: Texture2D = wfc_solver.validCellsForPattern(selected_panel.pattern_id)
		image_container.highlight_texture.material.set_shader_parameter("highlight_map", highlight_map)
		image_container.highlight_texture.visible = true

func on_pixel_clicked_draw(pixel_pos: Vector2i) -> void:
	if selected_panel:			
		var success: bool = wfc_solver.setPatternAtPosition(pixel_pos, selected_panel.pattern_id)
		generate_highlight_map()

func on_pixel_clicked_erase(pixel_pos: Vector2i) -> void:
	var success: bool = wfc_solver.erasePatternAtPosition(pixel_pos)
	generate_highlight_map()

func on_actions_button_id_pressed(id: int) -> void:
	match id:
		ActionsMenu.ActionButtons.AUTOCOMPLETE:
			image_container.highlight_texture.visible = false
			wfc_solver.autocompleteImage()

		ActionsMenu.ActionButtons.RESET:
			image_container.highlight_texture.visible = false
			wfc_solver.resetImage()
