extends Node

@onready var synth := $FluidSynthPlayer
@onready var midi_listener := $MIDIListener
@onready var midi_visualizer := $MIDIVisualizer

func _ready() -> void:
	OS.close_midi_inputs()
	OS.open_midi_inputs()
	
	synth.load_sound_font(preload("res://assets/soundfonts/TimGM6mb.sf2"))
	
	midi_listener.midi_event.connect(synth.handle_midi_event)
	midi_listener.midi_event.connect(func(input_event: InputEventMIDI): midi_visualizer.on_midi_event(input_event, 1))
	
	await get_tree().create_timer(0.5).timeout
	print("playing scale")
	play_scale()
	
	#while true:
		#await get_tree().create_timer(3.5).timeout
		#print("playing scale")
		#play_scale()

func play_scale(delay: float = 0.15, transpose: int = 0) -> void:
	var notes := [60, 62, 64, 65, 67, 69, 71, 72]

	# ascending scale
	for note in notes:
		_play_note(note + transpose)
		await get_tree().create_timer(delay).timeout

	# descending scale
	for i in range(notes.size() - 2, -1, -1):
		_play_note(notes[i] + transpose)
		await get_tree().create_timer(delay).timeout

	# ascending arpeggio: 3, 5, 8
	for i in [2, 4, 7]:
		_play_note(notes[i] + transpose)
		await get_tree().create_timer(delay).timeout
	
	# descending arpeggio: 5, 3, 1
	for i in [4, 2, 0]:
		_play_note(notes[i] + transpose)
		await get_tree().create_timer(delay).timeout



func _play_note(pitch: int = 60, hold_length := 0.15) -> void:
	var event := InputEventMIDI.new()
	event.message = MIDI_MESSAGE_NOTE_ON
	event.pitch = pitch
	event.velocity = int(127.0 / 2.0)
	event.channel = 0
	Input.parse_input_event(event)
	await get_tree().create_timer(hold_length).timeout
	
	event.message = MIDI_MESSAGE_NOTE_OFF
	Input.parse_input_event(event)
