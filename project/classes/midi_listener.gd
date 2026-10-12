@icon("res://assets/icons/midi-port.svg")
class_name MIDIListener
extends Node

signal midi_event(event: InputEventMIDI)

static var current: MIDIListener = null
static var listener_count: int = 0


func _enter_tree() -> void:
	if listener_count == 0:
		make_current()

	listener_count += 1


func _exit_tree() -> void:
	if current == self:
		current = null

	listener_count -= 1

	if listener_count <= 0:
		listener_count = 0


func _input(event: InputEvent) -> void:
	if current != self:
		return

	if event is InputEventMIDI:
		midi_event.emit(event)


func make_current() -> void:
	current = self


func is_current() -> bool:
	return current == self

static func get_current() -> MIDIListener:
	return current
