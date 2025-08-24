extends Control

signal on_message_submit(input:String)

@onready var chatLog = $VBoxContainer/RichTextLabel
@onready var inputLabel = get_node("VBoxContainer/HBoxContainer/Label")
@onready var inputField = $VBoxContainer/HBoxContainer/LineEdit

var groups = [
	{'name': 'Player', 'color': '#00abc7'},
	{'name': 'LLM', 'color': '#ffdd8b'},
]
var group_index = 0
var user_name = 'Player'

func _ready():
	inputField.connect("text_submitted", text_entered)
	
func add_message(username, text, group = 0, color = ''):
	chatLog.text += '\n' 
	if color == '':
		chatLog.text += '[color=' + groups[group]['color'] + ']'
	else:
		chatLog.text += '[color=' + color + ']'
	if username != '':
		chatLog.text += '[' + username + ']: '
	chatLog.text += text
	chatLog.text += '[/color]'

func text_entered(text):
	if text =='/h':
		add_message('', 'There is no help message yet!', 0, '#ff5757')
		inputField.text = ''		
		return
	if text != '':
		add_message(user_name, text, group_index)
		# Here you have to send the message to the server
		on_message_submit.emit(text)
		inputField.text = ''

func _on_python_exec_llm_output(output: String) -> void:
	add_message("LLM",output, 1)
	
