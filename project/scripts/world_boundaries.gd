extends Node2D

class_name WorldBoundaries

@onready var downBoundary: StaticBody2D = $WorldBoundaryDown
@onready var upBoundary: StaticBody2D = $WorldBoundaryUp
@onready var leftBoundary: StaticBody2D = $WorldBoundaryLeft
@onready var rightBoundary: StaticBody2D = $WorldBoundaryRight

func _ready() -> void:
  var viewport_offsets: Vector2 = get_viewport().size * 0.5
  downBoundary.global_position = Vector2(0, -viewport_offsets.y)
  upBoundary.global_position = Vector2(0, viewport_offsets.y)
  leftBoundary.global_position = Vector2(-viewport_offsets.x, 0)
  rightBoundary.global_position = Vector2(viewport_offsets.x, 0)

