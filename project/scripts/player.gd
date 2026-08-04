extends CharacterBody2D

class_name Player

@export var speed: float = 100.0

func _physics_process(delta: float) -> void:
  velocity = Vector2.ZERO
  if Input.is_action_pressed("move_up"):
    velocity.y = -speed
  elif Input.is_action_pressed("move_down"):
    velocity.y = speed

  if Input.is_action_pressed("move_left"):
    velocity.x = -speed
  elif Input.is_action_pressed("move_right"):
    velocity.x = speed

  move_and_slide()