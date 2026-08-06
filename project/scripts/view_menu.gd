extends MenuButton

class_name ViewMenu

# NOTE(Luis): The order of the buttons in this enum must match the order of the buttons in the menu. Otherwise, the signals will be emitted for the wrong button.
enum ViewButtons{
	HIGHLIGHT_GRID = 0,
	ORIGINAL_SAMPLE = 1,
	PIXEL_COORDINATES = 2
}

func _ready():
	get_popup().index_pressed.connect(on_index_pressed)
	var shortcut_keys: Array[int] = [Key.KEY_H, Key.KEY_O, Key.KEY_C]
	for view_index in ViewButtons.values():
		set_shortcut_for_index(view_index, shortcut_keys[view_index])

func on_index_pressed(index: int) -> void:
	get_popup().toggle_item_checked(index)


func set_shortcut_for_index(index: int, keycode: int) -> void:
	var event: InputEventKey = InputEventKey.new()
	event.keycode = keycode as Key
	event.ctrl_pressed = true
	event.shift_pressed = true
	var action_shortcut = Shortcut.new()
	action_shortcut.events = [event]
	get_popup().set_item_shortcut(index, action_shortcut, true)