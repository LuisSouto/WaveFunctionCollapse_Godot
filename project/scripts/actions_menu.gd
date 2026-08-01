extends MenuButton

class_name ActionsMenu

# NOTE(Luis): The order of the buttons in this enum must match the order of the buttons in the menu. Otherwise, the signals will be emitted for the wrong button.
enum ActionButtons{
  AUTOCOMPLETE = 0,
  RESET = 1,
  UNDO = 2,
	SAVE_IMAGE = 3
}

func _ready():
	var shortcut_keys: Array[int] = [Key.KEY_A, Key.KEY_R, Key.KEY_Z, Key.KEY_S]
	for action_index in ActionButtons.values():
		set_shortcut_for_action(action_index, shortcut_keys[action_index])


func set_shortcut_for_action(action_index: int, keycode: int) -> void:
	var event: InputEventKey = InputEventKey.new()
	event.keycode = keycode as Key
	event.ctrl_pressed = true
	var action_shortcut = Shortcut.new()
	action_shortcut.events = [event]
	get_popup().set_item_shortcut(action_index, action_shortcut, true)
		