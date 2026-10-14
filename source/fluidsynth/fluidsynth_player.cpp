#include "fluidsynth_player.hpp"

FluidSynthPlayer::FluidSynthPlayer() {
	set_process(true);

	settings = new_fluid_settings();
	if (!settings) {
		return;
	}

	fluid_settings_setnum(settings, "synth.sample-rate", 44100.0);
	synth = new_fluid_synth(settings);
	if (!synth) {
		return;
	}

	sfloader = SoundFont::create_memory_loader(settings);
	if (!sfloader) {
		delete_fluid_synth(synth);
		synth = nullptr;
		return;
	}

	// fluidsynth takes ownership of the sound_font loader do not delete sfloader ourselves
	fluid_synth_add_sfloader(synth, sfloader);
	apply_synth_settings();
}

FluidSynthPlayer::~FluidSynthPlayer() {
	if (synth) {
		delete_fluid_synth(synth);
		synth = nullptr;
		sfloader = nullptr;
	}

	if (settings) {
		delete_fluid_settings(settings);
		settings = nullptr;
	}
}

void FluidSynthPlayer::_ready() {
	_configure_audio_player();
}

void FluidSynthPlayer::_configure_audio_player() {
	if (audio_stream.is_null()) {
		audio_stream.instantiate();
	}

	audio_stream->set_mix_rate(44100.0);
	audio_stream->set_buffer_length(buffer_length);

	if (!audio_player) {
		audio_player = memnew(AudioStreamPlayer);
	}
	if (!audio_player->is_inside_tree()) {
		add_child(audio_player);
	}

	audio_player->set_stream(audio_stream);
	_start_stream_playback();
}

void FluidSynthPlayer::_start_stream_playback() {
	if (!audio_stream.is_valid() || !audio_player) {
		return;
	}

	audio_player->play();
	audio_playback = Ref<AudioStreamGeneratorPlayback>(Object::cast_to<AudioStreamGeneratorPlayback>(audio_player->get_stream_playback().ptr()));
}

void FluidSynthPlayer::render_audio() {
	if (!synth || !audio_playback.is_valid()) {
		return;
	}

	const int frames_available = audio_playback->get_frames_available();
	if (frames_available <= 0) {
		return;
	}

	// limit the amount rendered in a single godot frame
	// this prevents a large underrun from causing one enormous fluidsynth render
	constexpr int MAX_FRAMES = 2048;
	const int frames = MIN(frames_available, MAX_FRAMES);

	PackedFloat32Array left;
	left.resize(frames);

	PackedFloat32Array right;
	right.resize(frames);

	float *left_ptr = left.ptrw();
	float *right_ptr = right.ptrw();

	// fluid_synth_process() adds samples to the buffers, so clear them before rendering
	std::fill_n(left_ptr, frames, 0.0f);
	std::fill_n(right_ptr, frames, 0.0f);

	// stereo output buffers: left and right
	float *outputs[2] = { left_ptr, right_ptr };

	// Render dry audio and mix in reverb/chorus.
	// The effect buffers alias the output buffers intentionally.
	const int result = fluid_synth_process(
			synth,
			frames,
			2, outputs, // Effect output: stereo
			2, outputs // Dry output: stereo
	);

	if (result != FLUID_OK) {
		UtilityFunctions::push_error("FluidSynth audio rendering failed.");
		return;
	}

	for (int i = 0; i < frames; ++i) {
		audio_playback->push_frame(Vector2(left_ptr[i], right_ptr[i]));
	}
}

void FluidSynthPlayer::set_buffer_length(double seconds) {
	buffer_length = seconds;

	if (!audio_stream.is_valid() || !audio_player) {
		return;
	}

	const bool was_playing = audio_player->is_playing();
	if (was_playing) {
		audio_player->stop();
	}

	audio_stream->set_buffer_length(buffer_length);

	if (was_playing) {
		_start_stream_playback();
	}
}

// sound_font

int FluidSynthPlayer::load_sound_font(const Ref<SoundFont> &sound_font) {
	if (!synth || sound_font.is_null()) {
		return -1;
	}

	const int new_sound_font_id = sound_font->load_into_synth(synth);
	if (new_sound_font_id < 0) {
		UtilityFunctions::push_error("FluidSynth failed to load SoundFont from memory.");
		return -1;
	}

	if (sound_font_id >= 0) {
		fluid_synth_sfunload(synth, sound_font_id, 1);
	}
	sound_font_id = new_sound_font_id;

	return sound_font_id;
}

TypedArray<SoundFontPreset> FluidSynthPlayer::get_sound_font_presets() {
	TypedArray<SoundFontPreset> presets;
	if (!synth || sound_font_id < 0) {
		return presets;
	}

	fluid_sfont_t *sfont = fluid_synth_get_sfont_by_id(synth, sound_font_id);
	if (!sfont) {
		return presets;
	}

	fluid_preset_t *preset = nullptr;

	fluid_sfont_iteration_start(sfont);

	while ((preset = fluid_sfont_iteration_next(sfont)) != nullptr) {
		Ref<SoundFontPreset> info;
		info.instantiate();

		info->set_bank(fluid_preset_get_banknum(preset));
		info->set_program(fluid_preset_get_num(preset));
		info->set_name(String(fluid_preset_get_name(preset)));

		presets.push_back(info);
	}

	return presets;
}

// MIDI

void FluidSynthPlayer::note_on(int channel, int key, int velocity) {
	if (!synth) {
		return;
	}

	channel = CLAMP(channel, 0, 15);
	key = CLAMP(key, 0, 127);
	velocity = CLAMP(velocity, 0, 127);

	fluid_synth_noteon(synth, channel, key, velocity);
}

void FluidSynthPlayer::note_off(int channel, int key) {
	if (!synth) {
		return;
	}

	channel = CLAMP(channel, 0, 15);
	key = CLAMP(key, 0, 127);

	fluid_synth_noteoff(synth, channel, key);
}

void FluidSynthPlayer::key_pressure(int channel, int key, int pressure) {
	if (!synth) {
		return;
	}

	channel = CLAMP(channel, 0, 15);
	key = CLAMP(key, 0, 127);
	pressure = CLAMP(pressure, 0, 127);

	fluid_synth_key_pressure(synth, channel, key, pressure);
}

void FluidSynthPlayer::all_notes_off(int channel) {
	if (!synth) {
		return;
	}

	channel = CLAMP(channel, 0, 15);

	fluid_synth_cc(synth, channel, 123, 0);
}

void FluidSynthPlayer::all_sounds_off(int channel) {
	if (!synth) {
		return;
	}

	channel = CLAMP(channel, 0, 15);

	fluid_synth_cc(synth, channel, 120, 0);
}

void FluidSynthPlayer::program_change(int channel, int bank, int program) {
	if (!synth) {
		return;
	}

	channel = CLAMP(channel, 0, 15);
	bank = MAX(bank, 0);
	program = CLAMP(program, 0, 127);

	fluid_synth_bank_select(synth, channel, bank);
	fluid_synth_program_change(synth, channel, program);
}

void FluidSynthPlayer::control_change(int channel, int controller, int value) {
	if (!synth) {
		return;
	}

	channel = CLAMP(channel, 0, 15);
	controller = CLAMP(controller, 0, 127);
	value = CLAMP(value, 0, 127);

	fluid_synth_cc(synth, channel, controller, value);
}

void FluidSynthPlayer::channel_pressure(int channel, int pressure) {
	if (!synth) {
		return;
	}

	channel = CLAMP(channel, 0, 15);
	pressure = CLAMP(pressure, 0, 127);

	fluid_synth_channel_pressure(synth, channel, pressure);
}

void FluidSynthPlayer::pitch_bend(int channel, int value) {
	if (!synth) {
		return;
	}

	channel = CLAMP(channel, 0, 15);
	value = CLAMP(value, 0, 16383);

	fluid_synth_pitch_bend(synth, channel, value);
}

void FluidSynthPlayer::reset_controllers(int channel) {
	if (!synth) {
		return;
	}

	channel = CLAMP(channel, 0, 15);

	fluid_synth_cc(synth, channel, 121, 0);
}

void FluidSynthPlayer::handle_midi_event(const Ref<InputEventMIDI> &event) {
	if (event.is_null()) {
		return;
	}

	const int channel = CLAMP(event->get_channel(), 0, 15);

	switch (event->get_message()) {
		case MIDI_MESSAGE_NOTE_OFF:
			note_off(channel, event->get_pitch());
			break;

		case MIDI_MESSAGE_NOTE_ON: {
			const int key = event->get_pitch();
			const int velocity = event->get_velocity();

			// MIDI defines NOTE_ON with velocity 0 as equivalent to NOTE_OFF
			if (velocity == 0) {
				note_off(channel, key);
			} else {
				note_on(channel, key, velocity);
			}

			break;
		}

		case MIDI_MESSAGE_AFTERTOUCH:
			key_pressure(channel, event->get_pitch(), event->get_pressure());
			break;

		case MIDI_MESSAGE_CONTROL_CHANGE:
			control_change(channel, event->get_controller_number(), event->get_controller_value());
			break;

		case MIDI_MESSAGE_PROGRAM_CHANGE:
			program_change(channel, 0, event->get_instrument());
			break;

		case MIDI_MESSAGE_CHANNEL_PRESSURE:
			channel_pressure(channel, event->get_pressure());
			break;

		case MIDI_MESSAGE_PITCH_BEND:
			pitch_bend(channel, event->get_pitch());
			break;

		case MIDI_MESSAGE_SYSTEM_RESET:
			for (int ch = 0; ch < 16; ++ch) {
				all_sounds_off(ch);
				reset_controllers(ch);
			}
			break;

		case MIDI_MESSAGE_STOP:
			for (int ch = 0; ch < 16; ++ch) {
				all_sounds_off(ch);
			}
			break;

		// these messages are meaningful to sequencers or MIDI synchronization systems rather than the synthesizer itself
		case MIDI_MESSAGE_SYSTEM_EXCLUSIVE:
		case MIDI_MESSAGE_START:
		case MIDI_MESSAGE_CONTINUE:
		case MIDI_MESSAGE_TIMING_CLOCK:
		case MIDI_MESSAGE_ACTIVE_SENSING:
		case MIDI_MESSAGE_TUNE_REQUEST:
		case MIDI_MESSAGE_QUARTER_FRAME:
		case MIDI_MESSAGE_SONG_POSITION_POINTER:
		case MIDI_MESSAGE_SONG_SELECT:
		default:
			break;
	}
}

void FluidSynthPlayer::apply_synth_settings() {
	if (!synth) {
		return;
	}

	const double linear_gain = pow(10.0, gain / 20.0) * volume;

	fluid_synth_set_gain(synth, static_cast<float>(linear_gain));

	if (fluid_synth_set_polyphony(synth, polyphony) != FLUID_OK) {
		UtilityFunctions::push_warning("Failed to set FluidSynth polyphony.");
	}

	// Reverb.
	fluid_synth_reverb_on(synth, -1, reverb_enabled ? 1 : 0);
	fluid_synth_set_reverb_group_roomsize(synth, -1, reverb_room_size);
	fluid_synth_set_reverb_group_damp(synth, -1, reverb_damping);
	fluid_synth_set_reverb_group_width(synth, -1, reverb_width);
	fluid_synth_set_reverb_group_level(synth, -1, reverb_level);

#ifdef ENABLE_FLUID_EFFECTS

	// Chorus.
	fluid_synth_chorus_on(synth, -1, chorus_enabled ? 1 : 0);
	fluid_synth_set_chorus_group_nr(synth, -1, chorus_number);
	fluid_synth_set_chorus_group_level(synth, -1, chorus_level);
	fluid_synth_set_chorus_group_speed(synth, -1, chorus_speed);
	fluid_synth_set_chorus_group_depth(synth, -1, chorus_depth);
	fluid_synth_set_chorus_group_type(synth, -1, FLUID_CHORUS_MOD_SINE);

#endif // ENABLE_FLUID_EFFECTS
}