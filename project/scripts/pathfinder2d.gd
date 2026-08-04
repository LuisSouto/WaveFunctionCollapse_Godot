class_name PathFinder2D

static func generateBrownianBridgePath(start: float, end: float, steps: int, vol: float) -> Array[int]:
	var brownian_path: Array[float] = []
	brownian_path.resize(steps + 1)
	brownian_path[0] = 0

	# First calculate standard brownian motion
	for i in range(steps):
		brownian_path[i + 1] = brownian_path[i] + randfn(0, vol)
	var bridge_adjustment: float = brownian_path[steps] / steps

	# Calculate brownian bridge based on standard BM
	var bridge_path: Array[int] = []
	bridge_path.resize(steps + 1)
	for i in range(steps + 1):
		bridge_path[i] = roundi(start + (end - start) * i / steps +  brownian_path[i] - bridge_adjustment * i)

	return bridge_path

# Connects the nodes of a discrete path such that the path is continuous
static func connectPath(nodes: Array[int], start_x: int, end_x: int) -> Array[Vector2i]:
	var sign_x: int = sign(end_x - start_x)
	var path: Array[Vector2i] = []
	var current: Vector2i = Vector2i(start_x, nodes[0])
	path.append(current)

	var current_x: int = start_x
	for i in range(1, nodes.size()):
		current_x += sign_x
		path.append(Vector2i(current_x, nodes[i - 1]))
		var sign_y: int = signi(nodes[i] - nodes[i - 1])
		if sign_y != 0:
			for j in range(nodes[i - 1], nodes[i], sign_y):
				path.append(Vector2i(current_x, j + sign_y))

	return path

# Find a path between two points using Brownian bridge
static func pathfinder(start: Vector2i, end: Vector2i, vol: float) -> Array[Vector2i]:
	var steps: int = abs(end.x - start.x)
	var bridge_path: Array[int] = generateBrownianBridgePath(start.y, end.y, steps, vol)

	return connectPath(bridge_path, start.x, end.x)
