extends Area2D

class_name CaveDoor

@export var target_scene: String
@export var entrance_id: int = 0

func _ready() -> void:
	body_entered.connect(_on_body_entered)

func _on_body_entered(body: Node) -> void:
	if body is Player:
		GlobalVariables.entrance_id = entrance_id
		get_tree().call_deferred("change_scene_to_file", target_scene)
