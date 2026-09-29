extends Node

const SF := preload("res://assets/soundfonts/TimGM6mb/TimGM6mb.sf2")

func _ready():
	var synth = $FluidSynthPlayer
	synth.load_soundfont(SF)
	
	OS.close_midi_inputs()
	OS.open_midi_inputs()
	
	var midi_listener = $MIDIListener
	midi_listener.midi_event.connect(synth.handle_midi_event)
