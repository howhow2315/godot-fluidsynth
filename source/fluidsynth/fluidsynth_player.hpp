#pragma once

#include "godot_cpp/classes/global_constants.hpp"
#include <godot_cpp/classes/audio_stream_generator.hpp>
#include <godot_cpp/classes/audio_stream_generator_playback.hpp>
#include <godot_cpp/classes/audio_stream_player.hpp>
#include <godot_cpp/classes/audio_stream_player2d.hpp>
#include <godot_cpp/classes/audio_stream_player3d.hpp>
#include <godot_cpp/classes/input_event_midi.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <fluidsynth.h>

#include "../sound_font/sound_font.hpp"

using namespace godot;

class FluidSynthPlayer : public Node {
	GDCLASS(FluidSynthPlayer, Node);

private:
	// fluidsynth
	fluid_settings_t *settings = nullptr;
	fluid_synth_t *synth = nullptr;
	fluid_sfloader_t *sfloader = nullptr;

	int sound_font_id = -1;

	// godot audio output
	bool is_valid_audio_player(Node *_audio_player);
	void set_player_stream(const Ref<AudioStream> &stream);
	void play_audio_player();
	void stop_audio_player();
	bool is_audio_player_playing() const;

	Ref<AudioStreamGeneratorPlayback> get_audio_player_playback() const;

    NodePath audio_player_path;
    Node *audio_player = nullptr;
	bool owns_audio_player = false;

	void _configure_audio_player();
	void _start_stream_playback();

	Ref<AudioStreamGenerator> audio_stream;
	Ref<AudioStreamGeneratorPlayback> audio_playback;

	// Audio configuration.
	double buffer_length = 0.05;
	double volume = 1.0;
	double gain = 0.0;
	int polyphony = 256;

	// Reverb.
	bool reverb_enabled = false;
	double reverb_room_size = 0.61;
	double reverb_damping = 0.23;
	double reverb_width = 0.76;
	double reverb_level = 0.61;

#ifdef ENABLE_FLUID_EFFECTS

	// Chorus.
	bool chorus_enabled = false;
	int chorus_number = 3;
	double chorus_level = 1.2;
	double chorus_speed = 0.3;
	double chorus_depth = 8.0;

#endif // ENABLE_FLUID_EFFECTS

	void apply_synth_settings();

protected:
	static void _bind_methods() {
		// Audio

		ClassDB::bind_method(D_METHOD("render_audio"), &FluidSynthPlayer::render_audio);

		ClassDB::bind_method(D_METHOD("set_audio_player", "audio_player"), &FluidSynthPlayer::set_audio_player);
		ClassDB::bind_method(D_METHOD("get_audio_player"), &FluidSynthPlayer::get_audio_player);
		ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "audio_player"), "set_audio_player", "get_audio_player");

		ClassDB::bind_method(D_METHOD("set_buffer_length", "seconds"), &FluidSynthPlayer::set_buffer_length);
		ClassDB::bind_method(D_METHOD("get_buffer_length"), &FluidSynthPlayer::get_buffer_length);
		ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "buffer_length", PROPERTY_HINT_RANGE, "0.005,0.5,0.005"), "set_buffer_length", "get_buffer_length");

		// SoundFont

		ClassDB::bind_method(D_METHOD("load_sound_font", "sound_font"), &FluidSynthPlayer::load_sound_font);
		ClassDB::bind_method(D_METHOD("get_sound_font_presets"), &FluidSynthPlayer::get_sound_font_presets);

		// MIDI

		ClassDB::bind_method(D_METHOD("note_on", "channel", "key", "velocity"), &FluidSynthPlayer::note_on);
		ClassDB::bind_method(D_METHOD("note_off", "channel", "key"), &FluidSynthPlayer::note_off);
		ClassDB::bind_method(D_METHOD("program_change", "channel", "bank", "program"), &FluidSynthPlayer::program_change);
		ClassDB::bind_method(D_METHOD("control_change", "channel", "controller", "value"), &FluidSynthPlayer::control_change);
		ClassDB::bind_method(D_METHOD("all_notes_off", "channel"), &FluidSynthPlayer::all_notes_off);
		ClassDB::bind_method(D_METHOD("all_sounds_off", "channel"), &FluidSynthPlayer::all_sounds_off);
		ClassDB::bind_method(D_METHOD("key_pressure", "channel", "key", "pressure"), &FluidSynthPlayer::key_pressure);
		ClassDB::bind_method(D_METHOD("channel_pressure", "channel", "pressure"), &FluidSynthPlayer::channel_pressure);
		ClassDB::bind_method(D_METHOD("pitch_bend", "channel", "value"), &FluidSynthPlayer::pitch_bend);

		ClassDB::bind_method(D_METHOD("reset_controllers", "channel"), &FluidSynthPlayer::reset_controllers);
		ClassDB::bind_method(D_METHOD("handle_midi_event", "event"), &FluidSynthPlayer::handle_midi_event);

		// Volume

		ClassDB::bind_method(D_METHOD("set_volume", "value"), &FluidSynthPlayer::set_volume);
		ClassDB::bind_method(D_METHOD("get_volume"), &FluidSynthPlayer::get_volume);
		ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "volume", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_volume", "get_volume");

		// Gain

		ClassDB::bind_method(D_METHOD("set_gain", "value"), &FluidSynthPlayer::set_gain);
		ClassDB::bind_method(D_METHOD("get_gain"), &FluidSynthPlayer::get_gain);
		ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "gain", PROPERTY_HINT_RANGE, "-20.0,20.0,0.1"), "set_gain", "get_gain");

		// Polyphony

		ClassDB::bind_method(D_METHOD("set_polyphony", "value"), &FluidSynthPlayer::set_polyphony);
		ClassDB::bind_method(D_METHOD("get_polyphony"), &FluidSynthPlayer::get_polyphony);
		ADD_PROPERTY(PropertyInfo(Variant::INT, "polyphony", PROPERTY_HINT_RANGE, "1,4096,1"), "set_polyphony", "get_polyphony");

		// Reverb
		ClassDB::bind_method(D_METHOD("set_reverb_enabled", "value"), &FluidSynthPlayer::set_reverb_enabled);
		ClassDB::bind_method(D_METHOD("is_reverb_enabled"), &FluidSynthPlayer::is_reverb_enabled);
		ADD_PROPERTY(PropertyInfo(Variant::BOOL, "reverb_enabled"), "set_reverb_enabled", "is_reverb_enabled");

		ClassDB::bind_method(D_METHOD("set_reverb_room_size", "value"), &FluidSynthPlayer::set_reverb_room_size);
		ClassDB::bind_method(D_METHOD("get_reverb_room_size"), &FluidSynthPlayer::get_reverb_room_size);
		ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "reverb_room_size", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_reverb_room_size", "get_reverb_room_size");

		ClassDB::bind_method(D_METHOD("set_reverb_damping", "value"), &FluidSynthPlayer::set_reverb_damping);
		ClassDB::bind_method(D_METHOD("get_reverb_damping"), &FluidSynthPlayer::get_reverb_damping);
		ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "reverb_damping", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_reverb_damping", "get_reverb_damping");

		ClassDB::bind_method(D_METHOD("set_reverb_width", "value"), &FluidSynthPlayer::set_reverb_width);
		ClassDB::bind_method(D_METHOD("get_reverb_width"), &FluidSynthPlayer::get_reverb_width);
		ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "reverb_width", PROPERTY_HINT_RANGE, "0.0,100.0,0.1"), "set_reverb_width", "get_reverb_width");

		ClassDB::bind_method(D_METHOD("set_reverb_level", "value"), &FluidSynthPlayer::set_reverb_level);
		ClassDB::bind_method(D_METHOD("get_reverb_level"), &FluidSynthPlayer::get_reverb_level);
		ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "reverb_level", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_reverb_level", "get_reverb_level");

#ifdef ENABLE_FLUID_EFFECTS

		// Chorus
		ClassDB::bind_method(D_METHOD("set_chorus_enabled", "value"), &FluidSynthPlayer::set_chorus_enabled);
		ClassDB::bind_method(D_METHOD("is_chorus_enabled"), &FluidSynthPlayer::is_chorus_enabled);
		ADD_PROPERTY(PropertyInfo(Variant::BOOL, "chorus_enabled"), "set_chorus_enabled", "is_chorus_enabled");

		ClassDB::bind_method(D_METHOD("set_chorus_number", "value"), &FluidSynthPlayer::set_chorus_number);
		ClassDB::bind_method(D_METHOD("get_chorus_number"), &FluidSynthPlayer::get_chorus_number);
		ADD_PROPERTY(PropertyInfo(Variant::INT, "chorus_number", PROPERTY_HINT_RANGE, "0,99,1"), "set_chorus_number", "get_chorus_number");

		ClassDB::bind_method(D_METHOD("set_chorus_level", "value"), &FluidSynthPlayer::set_chorus_level);
		ClassDB::bind_method(D_METHOD("get_chorus_level"), &FluidSynthPlayer::get_chorus_level);
		ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "chorus_level", PROPERTY_HINT_RANGE, "0.0,10.0,0.01"), "set_chorus_level", "get_chorus_level");

		ClassDB::bind_method(D_METHOD("set_chorus_speed", "value"), &FluidSynthPlayer::set_chorus_speed);
		ClassDB::bind_method(D_METHOD("get_chorus_speed"), &FluidSynthPlayer::get_chorus_speed);
		ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "chorus_speed", PROPERTY_HINT_RANGE, "0.1,5.0,0.01"), "set_chorus_speed", "get_chorus_speed");

		ClassDB::bind_method(D_METHOD("set_chorus_depth", "value"), &FluidSynthPlayer::set_chorus_depth);
		ClassDB::bind_method(D_METHOD("get_chorus_depth"), &FluidSynthPlayer::get_chorus_depth);
		ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "chorus_depth", PROPERTY_HINT_RANGE, "0.0,256.0,0.1"), "set_chorus_depth", "get_chorus_depth");

#endif
	}

public:
	FluidSynthPlayer();
	~FluidSynthPlayer();

	void _ready() override;
	void _process(double delta) override { render_audio(); };

	// Audio.

    void set_audio_player(const NodePath &path);
	NodePath get_audio_player() { return audio_player_path; };

	void render_audio();

	// SoundFont.
	int load_sound_font(const Ref<SoundFont> &sound_font);
	TypedArray<SoundFontPreset> get_sound_font_presets();

	// MIDI.
	void note_on(int channel, int key, int velocity);
	void note_off(int channel, int key);
	void program_change(int channel, int bank, int program);
	void control_change(int channel, int controller, int value);
	void key_pressure(int channel, int key, int pressure);
	void channel_pressure(int channel, int pressure);

	void pitch_bend(int channel, int value);

	void all_notes_off(int channel);
	void all_sounds_off(int channel);
	void reset_controllers(int channel);

	void handle_midi_event(const Ref<InputEventMIDI> &event);

	// Buffer.
	void set_buffer_length(double seconds);
	double get_buffer_length() const { return buffer_length; }

	// Volume / Gain / Polyphony

	void set_volume(double value) {
		volume = CLAMP(value, 0.0, 1.0);
		apply_synth_settings();
	}
	double get_volume() const { return volume; }

	void set_gain(double value) {
		gain = CLAMP(value, -20.0, 20.0);
		apply_synth_settings();
	}
	double get_gain() const { return gain; }

	void set_polyphony(int value) {
		polyphony = CLAMP(value, 1, 4096);
		if (synth) {
			fluid_synth_set_polyphony(synth, polyphony);
		}
	}
	int get_polyphony() const { return polyphony; }

	// the way fluidsynth handles effects in the latest verions is by "fx groups", meaning
	// we should probably add a configurable array with an "effect settrings" resource
	// for full configuration

	// Reverb.

	void set_reverb_enabled(bool value) {
		reverb_enabled = value;
		if (synth) {
			fluid_synth_reverb_on(synth, -1, value ? 1 : 0);
		}
	}
	bool is_reverb_enabled() const { return reverb_enabled; }

	void set_reverb_room_size(double value) {
		reverb_room_size = CLAMP(value, 0.0, 1.0);
		if (synth) {
			fluid_synth_set_reverb_group_roomsize(synth, -1, reverb_room_size);
		}
	}
	double get_reverb_room_size() const { return reverb_room_size; }

	void set_reverb_damping(double value) {
		reverb_damping = CLAMP(value, 0.0, 1.0);
		if (synth) {
			fluid_synth_set_reverb_group_damp(synth, -1, reverb_damping);
		}
	}
	double get_reverb_damping() const { return reverb_damping; }

	void set_reverb_width(double value) {
		reverb_width = CLAMP(value, 0.0, 100.0);
		if (synth) {
			fluid_synth_set_reverb_group_width(synth, -1, reverb_width);
		}
	}
	double get_reverb_width() const { return reverb_width; }

	void set_reverb_level(double value) {
		reverb_level = CLAMP(value, 0.0, 1.0);
		if (synth) {
			fluid_synth_set_reverb_group_level(synth, -1, reverb_level);
		}
	}
	double get_reverb_level() const { return reverb_level; }

#ifdef ENABLE_FLUID_EFFECTS

	// Chorus.

	void set_chorus_enabled(bool value) {
		chorus_enabled = value;
		if (synth) {
			fluid_synth_chorus_on(synth, -1, value ? 1 : 0);
		}
	}
	bool is_chorus_enabled() const { return chorus_enabled; }

	void set_chorus_number(int value) {
		chorus_number = CLAMP(value, 0, 99);
		if (synth) {
			fluid_synth_set_chorus_group_nr(synth, -1, chorus_number);
		}
	}
	int get_chorus_number() const { return chorus_number; }

	void set_chorus_level(double value) {
		chorus_level = CLAMP(value, 0.0, 10.0);
		if (synth) {
			fluid_synth_set_chorus_group_level(synth, -1, chorus_level);
		}
	}
	double get_chorus_level() const { return chorus_level; }

	void set_chorus_speed(double value) {
		chorus_speed = CLAMP(value, 0.1, 5.0);
		if (synth) {
			fluid_synth_set_chorus_group_speed(synth, -1, chorus_speed);
		}
	}
	double get_chorus_speed() const { return chorus_speed; }

	void set_chorus_depth(double value) {
		chorus_depth = CLAMP(value, 0.0, 256.0);
		if (synth) {
			fluid_synth_set_chorus_group_depth(synth, -1, chorus_depth);
		}
	}
	double get_chorus_depth() const { return chorus_depth; }

#endif // ENABLE_FLUID_EFFECTS
};