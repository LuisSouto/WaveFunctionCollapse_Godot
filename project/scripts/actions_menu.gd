extends MenuButton

class_name ActionsMenu

enum ActionButtons{
  AUTOCOMPLETE = 0,
  RESET = 1,
  UNDO = 2,
}

func _ready():
	var shortcut_keys: Array[int] = [Key.KEY_A, Key.KEY_R, Key.KEY_Z]
	for action_id in ActionButtons.values():
		set_shortcut_for_action(action_id, shortcut_keys[action_id])


func set_shortcut_for_action(action_id: int, keycode: int) -> void:
	var event: InputEventKey = InputEventKey.new()
	event.keycode = keycode as Key
	event.ctrl_pressed = true
	var action_shortcut = Shortcut.new()
	action_shortcut.events = [event]
	get_popup().set_item_shortcut(action_id, action_shortcut, true)
		