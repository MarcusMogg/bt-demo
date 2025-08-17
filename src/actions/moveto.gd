extends BeehaveNode

var goal_2d: Vector2

func string2vector2(data:Variant)->Vector2:
	if typeof(data) != TYPE_STRING:
		return Vector2.ZERO
	var goal: String = data
	var arr = goal.split(',')
	if arr.size() != 2 :
		push_error("data format error")
		return Vector2.ZERO
	return Vector2(float(arr[0]),float(arr[1]))

func _start(actor: Node) -> NodeStatus:
	if not actor is Node2D:
		return NodeStatus.FAILURE

	var goal = get_input("goal")
	match typeof(goal):
		TYPE_VECTOR2:
			goal_2d = goal
		TYPE_STRING:
			goal_2d = string2vector2(goal)
		_:
			push_error("goal type error")
			return NodeStatus.FAILURE
	return NodeStatus.RUNNING

func _tick(actor: Node) -> NodeStatus:
	if not actor is Node2D:
		return NodeStatus.FAILURE
	var actor_2d = actor as Node2D
	var cur_pos = actor_2d.position
	
	if cur_pos.distance_to(goal_2d) < 1:
		return NodeStatus.SUCCESS
	var target = goal_2d - cur_pos
	actor_2d.translate(target.normalized())
	
	return NodeStatus.RUNNING

func _halt(actor: Node) -> void:
	print("halt")
