extends BeehaveNode

var distance: float
var direction: Vector2

func _start(actor: Node) -> NodeStatus:
	if not actor is Node2D:
		return NodeStatus.FAILURE
	
	var direction_input = get_input("dir")
	if typeof(direction_input) != TYPE_STRING:
		push_error("direction type error")
		return NodeStatus.FAILURE
	match String(direction_input).to_upper():
		"RIGHT":
			direction = Vector2.RIGHT
		"LEFT":
			direction = Vector2.LEFT
		"UP":
			direction = Vector2.UP
		"DOWN":
			direction = Vector2.DOWN
		_:
			push_error("direction type error")
			return NodeStatus.FAILURE
	
	var distance_input = get_input("dis")
	if typeof(distance_input) != TYPE_STRING:
		push_error("distance_input type error")
		return NodeStatus.FAILURE
	distance = float(distance_input)
	if distance <= 0:
		push_error("distance_input type error",distance_input)
		return NodeStatus.FAILURE
	return NodeStatus.RUNNING

func _tick(actor: Node) -> NodeStatus:
	if not actor is Node2D:
		return NodeStatus.FAILURE
	var actor_2d = actor as Node2D
	
	if distance <= 0:
		return NodeStatus.SUCCESS

	actor_2d.translate(direction)
	distance -= 1
	
	return NodeStatus.RUNNING

func _halt(_actor: Node) -> void:
	print("halt")
