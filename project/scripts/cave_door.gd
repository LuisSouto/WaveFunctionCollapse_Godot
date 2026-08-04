extends Area2D

class_name CaveDoor

@export var target_scene: String
@export var player_spawn_pos: Vector2

func _ready() -> void:
	body_entered.connect(_on_body_entered)

func _on_body_entered(body: Node) -> void:
	if body is Player:
		GlobalVariables.player_spawn_pos = player_spawn_pos
		get_tree().call_deferred("change_scene_to_file", target_scene)
