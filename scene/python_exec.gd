extends Node

var task_id:int = 0
var json_out = JSON.new()
var default_out = {
			"str_output": "parse llm output fail, xml to default",
			"xml_output": 
"""<root BTCPP_format="4">
	 <BehaviorTree>
		<Sequence>
			<MoveTo  goal="15,15" />
			<MoveTo  goal="15,500" />
			<Move  dir="right" dis="500" />
		</Sequence>
	 </BehaviorTree>
 </root>"""
		}

signal llm_output(output:String)
signal llm_xml_output(output:String)

func get_script_path() ->String:
	if OS.has_feature("editor"):
		return ProjectSettings.globalize_path("res://gdext/test.py")
	else:
		return OS.get_executable_path().get_base_dir().path_join("gdext/test.py")

func call_llm(input:String, json_output) :
	var script = get_script_path()
	if not FileAccess.file_exists(script):
		json_output.data = default_out
		json_output.data["str_output"] = "execute python fail, please check script exist"
		return
	var output = []
	var ret = OS.execute("python", [script, input] , output, true)
	if ret!= OK:
		json_output.data = default_out
		json_output.data["str_output"] = "execute python fail, please check python"
		return
	var out_str = "\n".join(output)
	var err = json_output.parse(out_str)
	if err != OK or typeof(json_output.data) != TYPE_DICTIONARY:
		json_output.data = default_out

func _process(_delta: float) -> void:
	if task_id != 0 and  WorkerThreadPool.is_task_completed(task_id):
		llm_output.emit(json_out.data["str_output"])
		llm_xml_output.emit(json_out.data["xml_output"])
		task_id = 0

func _on_chat_box_on_message_submit(input: String) -> void:
	if(task_id != 0):
		printerr("last task not complete")
		return
	json_out = JSON.new()
	var task = func (): 
		call_llm(input,json_out)
	task_id = WorkerThreadPool.add_task(task)
