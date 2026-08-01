extends Control

class_name WFCPanel

@onready var wfc_solver: WFC = $"WFC"
@onready var list_pattern_textures: GridContainer = $"MainWindow/WindowMargins/HBoxContainer/VBoxContainer/ScrollContainer/PatternTextures"
@onready var image_container: ImageContainer = $"MainWindow/WindowMargins/HBoxContainer/ImageContainer"
@onready var actions_button: ActionsMenu = $"MainWindow/WindowMargins/HBoxContainer/VBoxContainer/MenusBox/ActionsMenu"
@onready var view_button: ViewMenu = $"MainWindow/WindowMargins/HBoxContainer/VBoxContainer/MenusBox/ViewMenu"
@onready var original_sample: TextureRect = $"MainWindow/WindowMargins/HBoxContainer/VBoxContainer/PanelContainer/OriginalSample"
var save_counter: int = 1
var input_file_name: String

@export var input_pattern_scene: PackedScene 
var selected_panel: InputPattern
var output_texture_width: int
var output_texture_height: int

func _ready():
	# Initialize textures
	output_texture_width = wfc_solver.config.width
	output_texture_height = wfc_solver.config.height
	image_container.resize_textures(Vector2(output_texture_width, output_texture_height))
	image_container.image_texture.set_texture(wfc_solver.texture)

	# Draw and erase pattern signals		
	image_container.pixel_clicked_draw.connect(on_pixel_clicked_draw)
	image_container.pixel_clicked_erase.connect(on_pixel_clicked_erase)

	# Menus
	actions_button.get_popup().id_pressed.connect(on_actions_button_id_pressed)
	view_button.get_popup().index_pressed.connect(on_view_button_index_pressed)

	original_sample.texture = wfc_solver.input_sprite.texture

	# Visibility
	image_container.highlight_texture.visible = view_button.get_popup().is_item_checked(ViewMenu.ViewButtons.HIGHLIGHT_GRID)
	original_sample.get_parent().visible = view_button.get_popup().is_item_checked(ViewMenu.ViewButtons.ORIGINAL_SAMPLE)

	# Input file name (for saving outputs)
	input_file_name = wfc_solver.input_sprite.texture.resource_path.get_file().get_basename()

	display_input_patterns()

func display_input_patterns() -> void:
	var pattern_textures: Array[Texture2D] = wfc_solver.getPatternTextures()

	for pattern_id in range(pattern_textures.size()):
		var texture_panel = input_pattern_scene.instantiate()
		texture_panel.pattern_id = pattern_id
		texture_panel.pattern_selected.connect(on_pattern_selected)
		list_pattern_textures.add_child(texture_panel)		
		texture_panel.set_texture(pattern_textures[pattern_id])

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
		# image_container.highlight_texture.visible = true

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
			wfc_solver.autocompleteImage()

		ActionsMenu.ActionButtons.RESET:
			wfc_solver.resetImage()

		ActionsMenu.ActionButtons.SAVE_IMAGE:
			save_output_texture()

	if selected_panel:
		generate_highlight_map()

func on_view_button_index_pressed(index: int) -> void:
	match index:
		ViewMenu.ViewButtons.HIGHLIGHT_GRID:
			image_container.highlight_texture.visible = view_button.get_popup().is_item_checked(ViewMenu.ViewButtons.HIGHLIGHT_GRID)
				
		ViewMenu.ViewButtons.ORIGINAL_SAMPLE:
			original_sample.get_parent().visible = view_button.get_popup().is_item_checked(ViewMenu.ViewButtons.ORIGINAL_SAMPLE)

func save_output_texture() -> void:
	var image: Image = wfc_solver.texture.get_image()
	var file_path: String = "res://Outputs/" + input_file_name + "_output_" + str(save_counter) + ".png"
	image.save_png(file_path)
	save_counter += 1