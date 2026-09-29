#include "fluidsynth_player.hpp"

#include <cmath>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void FluidSynthPlayer::_bind_methods() {
    // Audio

    ClassDB::bind_method(D_METHOD("render_audio"), &FluidSynthPlayer::render_audio);
    
    ClassDB::bind_method(D_METHOD("set_buffer_length", "seconds"), &FluidSynthPlayer::set_buffer_length);
    ClassDB::bind_method(D_METHOD("get_buffer_length"), &FluidSynthPlayer::get_buffer_length);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "buffer_length", PROPERTY_HINT_RANGE, "0.005,0.5,0.005"), "set_buffer_length", "get_buffer_length"
    );

    // SoundFont

    ClassDB::bind_method(D_METHOD("load_soundfont", "soundfont"), &FluidSynthPlayer::load_soundfont);
    ClassDB::bind_method(D_METHOD("get_soundfont_presets"), &FluidSynthPlayer::get_soundfont_presets);

    // MIDI

    ClassDB::bind_method(D_METHOD("note_on", "channel", "key", "velocity"), &FluidSynthPlayer::note_on);
    ClassDB::bind_method(D_METHOD("note_off", "channel", "key"), &FluidSynthPlayer::note_off);
    ClassDB::bind_method(D_METHOD("program_change", "channel", "bank", "program"), &FluidSynthPlayer::program_change);
    ClassDB::bind_method(D_METHOD("control_change", "channel", "controller", "value"), &FluidSynthPlayer::control_change);
    ClassDB::bind_method(D_METHOD("all_notes_off", "channel"), &FluidSynthPlayer::all_notes_off);
    ClassDB::bind_method(D_METHOD("all_sounds_off", "channel"), &FluidSynthPlayer::all_sounds_off);
    ClassDB::bind_method(D_METHOD("key_pressure", "channel", "key", "pressure"), &FluidSynthPlayer::key_pressure);
    ClassDB::bind_method(D_METHOD("channel_pressure", "channel", "pressure"), &FluidSynthPlayer::channel_pressure);
    ClassDB::bind_method(D_METHOD("reset_controllers", "channel"), &FluidSynthPlayer::reset_controllers);
    ClassDB::bind_method(D_METHOD("handle_midi_event", "event"), &FluidSynthPlayer::handle_midi_event);

    // Volume

    ClassDB::bind_method(D_METHOD("set_volume", "value"), &FluidSynthPlayer::set_volume);
    ClassDB::bind_method(D_METHOD("get_volume"), &FluidSynthPlayer::get_volume);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "volume", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_volume", "get_volume"
    );
    
    // Gain

    ClassDB::bind_method(D_METHOD("set_gain", "value"), &FluidSynthPlayer::set_gain);
    ClassDB::bind_method(D_METHOD("get_gain"), &FluidSynthPlayer::get_gain);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "gain", PROPERTY_HINT_RANGE, "-20.0,20.0,0.1"), "set_gain", "get_gain"
    );

    // Polyphony

    ClassDB::bind_method(D_METHOD("set_polyphony", "value"), &FluidSynthPlayer::set_polyphony);
    ClassDB::bind_method(D_METHOD("get_polyphony"), &FluidSynthPlayer::get_polyphony);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "polyphony", PROPERTY_HINT_RANGE, "1,4096,1"), "set_polyphony", "get_polyphony"
    );
    
    // // Reverb

    // ClassDB::bind_method(D_METHOD("set_reverb_enabled", "value"), &FluidSynthPlayer::set_reverb_enabled);
    // ClassDB::bind_method(D_METHOD("is_reverb_enabled"), &FluidSynthPlayer::is_reverb_enabled);
    // ADD_PROPERTY(PropertyInfo(Variant::BOOL, "reverb_enabled"), "set_reverb_enabled", "is_reverb_enabled");

    // ClassDB::bind_method(D_METHOD("set_reverb_room_size", "value"), &FluidSynthPlayer::set_reverb_room_size);
    // ClassDB::bind_method(D_METHOD("get_reverb_room_size"), &FluidSynthPlayer::get_reverb_room_size);
    // ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "reverb_room_size", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_reverb_room_size", "get_reverb_room_size");

    // ClassDB::bind_method(D_METHOD("set_reverb_damping", "value"), &FluidSynthPlayer::set_reverb_damping);
    // ClassDB::bind_method(D_METHOD("get_reverb_damping"), &FluidSynthPlayer::get_reverb_damping);
    // ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "reverb_damping", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_reverb_damping", "get_reverb_damping");

    // ClassDB::bind_method(D_METHOD("set_reverb_width", "value"), &FluidSynthPlayer::set_reverb_width);
    // ClassDB::bind_method(D_METHOD("get_reverb_width"), &FluidSynthPlayer::get_reverb_width);
    // ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "reverb_width", PROPERTY_HINT_RANGE, "0.0,100.0,0.1"), "set_reverb_width", "get_reverb_width");

    // ClassDB::bind_method(D_METHOD("set_reverb_level", "value"), &FluidSynthPlayer::set_reverb_level);
    // ClassDB::bind_method(D_METHOD("get_reverb_level"), &FluidSynthPlayer::get_reverb_level);
    // ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "reverb_level", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_reverb_level", "get_reverb_level");

    // // Chorus
    // ClassDB::bind_method(D_METHOD("set_chorus_enabled", "value"), &FluidSynthPlayer::set_chorus_enabled);
    // ClassDB::bind_method(D_METHOD("is_chorus_enabled"), &FluidSynthPlayer::is_chorus_enabled);
    // ADD_PROPERTY(PropertyInfo(Variant::BOOL, "chorus_enabled"), "set_chorus_enabled", "is_chorus_enabled");

    // ClassDB::bind_method(D_METHOD("set_chorus_number", "value"), &FluidSynthPlayer::set_chorus_number);
    // ClassDB::bind_method(D_METHOD("get_chorus_number"), &FluidSynthPlayer::get_chorus_number);
    // ADD_PROPERTY(PropertyInfo(Variant::INT, "chorus_number", PROPERTY_HINT_RANGE, "0,99,1"), "set_chorus_number", "get_chorus_number");

    // ClassDB::bind_method(D_METHOD("set_chorus_level", "value"), &FluidSynthPlayer::set_chorus_level);
    // ClassDB::bind_method(D_METHOD("get_chorus_level"), &FluidSynthPlayer::get_chorus_level);
    // ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "chorus_level", PROPERTY_HINT_RANGE, "0.0,10.0,0.01"), "set_chorus_level", "get_chorus_level");

    // ClassDB::bind_method(D_METHOD("set_chorus_speed", "value"), &FluidSynthPlayer::set_chorus_speed);
    // ClassDB::bind_method(D_METHOD("get_chorus_speed"), &FluidSynthPlayer::get_chorus_speed);
    // ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "chorus_speed", PROPERTY_HINT_RANGE, "0.1,5.0,0.01"), "set_chorus_speed", "get_chorus_speed");

    // ClassDB::bind_method(D_METHOD("set_chorus_depth", "value"), &FluidSynthPlayer::set_chorus_depth);
    // ClassDB::bind_method(D_METHOD("get_chorus_depth"), &FluidSynthPlayer::get_chorus_depth);
    // ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "chorus_depth", PROPERTY_HINT_RANGE, "0.0,256.0,0.1"), "set_chorus_depth", "get_chorus_depth");
}

FluidSynthPlayer::FluidSynthPlayer() {
    set_process(true);

    settings = new_fluid_settings();
    if (!settings) { return; }

    fluid_settings_setnum( settings, "synth.sample-rate", 44100.0);
    synth = new_fluid_synth(settings);
    if (!synth) { return; }

    sfloader = SoundFont::create_memory_loader(settings);
    if (!sfloader) {
        delete_fluid_synth(synth);
        synth = nullptr;
        return;
    }

    // fluidsynth takes ownership of the soundfont loader
    // do not delete sfloader ourselves
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
    audio_stream.instantiate();

    audio_stream->set_mix_rate(44100.0);
    audio_stream->set_buffer_length(buffer_length);

    audio_player = memnew(AudioStreamPlayer);

    add_child(audio_player);

    audio_player->set_stream(audio_stream);
    _start_stream_playback();
}

void FluidSynthPlayer::_process(double) {
    render_audio();
}

void FluidSynthPlayer::_start_stream_playback() {
    if (!audio_stream.is_valid() || !audio_player) { return; }

    audio_player->play();
    audio_playback = Ref<AudioStreamGeneratorPlayback>(
        Object::cast_to<AudioStreamGeneratorPlayback>(
            audio_player->get_stream_playback().ptr()
        ));}

void FluidSynthPlayer::render_audio() {
    if (!synth || !audio_playback.is_valid()) { return; }

    const int frames_available = audio_playback->get_frames_available();
    if (frames_available <= 0) { return; }

    
    // limit the amount rendered in a single godot frame
    // this prevents a large underrun from causing one enormous fluidsynth render operation
    // there is likely a smarter way to handle this
    constexpr int MAX_FRAMES = 2048;
    const int frames = MIN(frames_available, MAX_FRAMES);

    // something feels very redundant here it might be there vector creation or pushing each frame the way we are...
    PackedFloat32Array left; left.resize(frames);
    PackedFloat32Array right; right.resize(frames);
 
    float *left_ptr = left.ptrw();
    float *right_ptr = right.ptrw();
    float *outputs[2] = {left_ptr, right_ptr};

    const int result = fluid_synth_process(synth, frames, 0, nullptr, 2, outputs);
    if (result != FLUID_OK) { return; }

    for (int i = 0; i < frames; ++i) {
        audio_playback->push_frame(Vector2(left_ptr[i], right_ptr[i]));
    }
}

void FluidSynthPlayer::set_buffer_length(double seconds) {
    buffer_length = seconds;

    if (!audio_stream.is_valid() || !audio_player) { return; }

    const bool was_playing = audio_player->is_playing();
    if (was_playing) { audio_player->stop(); }

    audio_stream->set_buffer_length(buffer_length);

    if (was_playing) { _start_stream_playback(); }
}

double FluidSynthPlayer::get_buffer_length() const {
    return buffer_length;
}

// soundfont

// note: going by id here is a little sketch but it was quick and simple
int FluidSynthPlayer::load_soundfont(const Ref<SoundFont> &soundfont) {
    if (!synth || soundfont.is_null()) { return -1; }

    const int new_soundfont_id = soundfont->load_into_synth(synth);

    if (new_soundfont_id < 0) {
        UtilityFunctions::push_error("FluidSynth failed to load SoundFont from memory.");
        return -1;
    }

    if (soundfont_id >= 0) {
        fluid_synth_sfunload(synth, soundfont_id, 1);
    }

    soundfont_id = new_soundfont_id;

    return soundfont_id;
}

TypedArray<Dictionary> FluidSynthPlayer::get_soundfont_presets() {
    TypedArray<Dictionary> presets;
    if (!synth || soundfont_id < 0) { return presets; }

    fluid_sfont_t *sfont = fluid_synth_get_sfont_by_id(synth, soundfont_id);
    if (!sfont) { return presets; }

    fluid_preset_t *preset = nullptr;

    fluid_sfont_iteration_start(sfont);

    while ((preset = fluid_sfont_iteration_next(sfont)) != nullptr) {
        Dictionary info;
        info["bank"] = fluid_preset_get_banknum(preset);
        info["program"] = fluid_preset_get_num(preset);
        info["name"] = String(fluid_preset_get_name(preset));

        presets.push_back(info);
    }

    return presets;
}

// MIDI

void FluidSynthPlayer::note_on(int channel, int key, int velocity) {
    if (!synth) { return; }

    channel = CLAMP(channel, 0, 15);
    key = CLAMP(key, 0, 127);
    velocity = CLAMP(velocity, 0, 127);

    fluid_synth_noteon(synth, channel, key, velocity);
}

void FluidSynthPlayer::note_off(int channel, int key) {
    if (!synth) {return;}

    channel = CLAMP(channel, 0, 15);
    key = CLAMP(key, 0, 127);

    fluid_synth_noteoff(synth, channel, key);
}

void FluidSynthPlayer::program_change(int channel, int bank, int program) {
    if (!synth) { return; }

    channel = CLAMP(channel, 0, 15);
    bank = MAX(bank, 0);
    program = CLAMP(program, 0, 127);

    fluid_synth_bank_select(synth, channel, bank);
    fluid_synth_program_change(synth, channel, program);
}

void FluidSynthPlayer::control_change(int channel, int controller, int value) {
    if (!synth) { return; }

    channel = CLAMP(channel, 0, 15);
    controller = CLAMP(controller, 0, 127);
    value = CLAMP(value, 0, 127);

    fluid_synth_cc(synth, channel, controller, value);
}

void FluidSynthPlayer::all_notes_off(int channel) {
    if (!synth) { return; }

    channel = CLAMP(channel, 0, 15);

    fluid_synth_cc(synth, channel, 123, 0);
}

void FluidSynthPlayer::all_sounds_off(int channel) {
    if (!synth) { return; }

    channel = CLAMP(channel, 0, 15);

    fluid_synth_cc(synth, channel, 120, 0);
}

void FluidSynthPlayer::key_pressure(int channel, int key, int pressure) {
    if (!synth) { return; }

    channel = CLAMP(channel, 0, 15);
    key = CLAMP(key, 0, 127);
    pressure = CLAMP(pressure, 0, 127);

    fluid_synth_key_pressure(synth, channel, key, pressure);
}

void FluidSynthPlayer::channel_pressure(int channel, int pressure ) {
    if (!synth) { return; }

    channel = CLAMP(channel, 0, 15);
    pressure = CLAMP(pressure, 0, 127);

    fluid_synth_channel_pressure(synth, channel, pressure);
}

void FluidSynthPlayer::reset_controllers(int channel) {
    if (!synth) { return; }

    channel = CLAMP(channel, 0, 15);

    fluid_synth_cc(synth, channel, 121, 0);
}

void FluidSynthPlayer::handle_midi_event(const Ref<InputEventMIDI> &event) {
    if (event.is_null()) { return; }

    const int channel = CLAMP(event->get_channel(), 0, 15);

    switch (event->get_message()) {
        case MIDI_MESSAGE_NOTE_ON: {
            const int key = event->get_pitch();
            const int velocity = event->get_velocity();

            // MIDI defines NOTE_ON with velocity 0 as equivalent to NOTE_OFF
            if (velocity == 0) { note_off(channel, key);
            } else { note_on(channel, key, velocity);
            }

            break;
        }

        case MIDI_MESSAGE_NOTE_OFF:
            note_off( channel, event->get_pitch());
            break;

        case MIDI_MESSAGE_CONTROL_CHANGE:
            control_change( channel, event->get_controller_number(), event->get_controller_value());
            break;

        case MIDI_MESSAGE_PROGRAM_CHANGE:
            program_change( channel, 0, event->get_instrument());
            break;

        case MIDI_MESSAGE_AFTERTOUCH:
            key_pressure(channel, event->get_pitch(), event->get_pressure());
            break;

        case MIDI_MESSAGE_CHANNEL_PRESSURE:
            channel_pressure(channel, event->get_pressure());
            break;

        case MIDI_MESSAGE_SYSTEM_RESET:
            for (int ch = 0; ch < 16; ++ch) { all_sounds_off(ch); reset_controllers(ch); }
            break;

        case MIDI_MESSAGE_STOP:
            for (int ch = 0; ch < 16; ++ch) { all_sounds_off(ch); }
            break;

        // these messages are meaningful to sequencers or MIDI synchronization systems rather than the synthesizer itself.
        case MIDI_MESSAGE_START:
        case MIDI_MESSAGE_CONTINUE:
        case MIDI_MESSAGE_TIMING_CLOCK:
        case MIDI_MESSAGE_ACTIVE_SENSING:
        case MIDI_MESSAGE_TUNE_REQUEST:
        case MIDI_MESSAGE_QUARTER_FRAME:
        case MIDI_MESSAGE_SONG_POSITION_POINTER:
        case MIDI_MESSAGE_SONG_SELECT:
        case MIDI_MESSAGE_SYSTEM_EXCLUSIVE:
        default:
            break;
    }
}

void FluidSynthPlayer::apply_synth_settings() {
    if (!settings || !synth) { return; }

    const double linear_gain = std::pow(10.0, gain / 20.0) * volume;

    fluid_settings_setnum(settings, "synth.gain", linear_gain);
    fluid_settings_setint(settings, "synth.polyphony", polyphony);

    // fluid_settings_setint(settings, "synth.reverb.active", reverb_enabled ? 1 : 0);
    // fluid_settings_setnum(settings, "synth.reverb.room-size", reverb_room_size);
    // fluid_settings_setnum(settings, "synth.reverb.damp", reverb_damping);
    // fluid_settings_setnum(settings, "synth.reverb.width", reverb_width);
    // fluid_settings_setnum(settings, "synth.reverb.level", reverb_level);

    // fluid_settings_setint(settings, "synth.chorus.active", chorus_enabled ? 1 : 0);
    // fluid_settings_setint(settings, "synth.chorus.nr", chorus_number);
    // fluid_settings_setnum(settings, "synth.chorus.level", chorus_level);
    // fluid_settings_setnum(settings, "synth.chorus.speed", chorus_speed);
    // fluid_settings_setnum(settings, "synth.chorus.depth", chorus_depth);
}

// Volume / Gain / Polyphony

void FluidSynthPlayer::set_volume(double value) {
    volume = CLAMP(value, 0.0, 1.0);
    apply_synth_settings();
}

double FluidSynthPlayer::get_volume() const {
    return volume;
}

void FluidSynthPlayer::set_gain(double value) {
    gain = CLAMP(value, -20.0, 20.0);
    apply_synth_settings();
}

double FluidSynthPlayer::get_gain() const {
    return gain;
}

void FluidSynthPlayer::set_polyphony(int value) {
    polyphony = CLAMP(value, 1, 4096);

    if (synth) {
        fluid_synth_set_polyphony(synth, polyphony);
    }
}

int FluidSynthPlayer::get_polyphony() const {
    return polyphony;
}

// Reverb

// void FluidSynthPlayer::set_reverb_enabled(bool value) {
//     reverb_enabled = value;

//     if (settings) {
//         fluid_settings_setint(settings, "synth.reverb.active", reverb_enabled ? 1 : 0);
//     }
// }

// bool FluidSynthPlayer::is_reverb_enabled() const {
//     return reverb_enabled;
// }

// void FluidSynthPlayer::set_reverb_room_size(double value) {
//     reverb_room_size = CLAMP(value, 0.0, 1.0);

//     if (settings) {
//         fluid_settings_setnum(settings, "synth.reverb.room-size", reverb_room_size);
//     }
// }

// double FluidSynthPlayer::get_reverb_room_size() const {
//     return reverb_room_size;
// }

// void FluidSynthPlayer::set_reverb_damping(double value) {
//     reverb_damping = CLAMP(value, 0.0, 1.0);

//     if (settings) {
//         fluid_settings_setnum(settings, "synth.reverb.damp", reverb_damping);
//     }
// }

// double FluidSynthPlayer::get_reverb_damping() const {
//     return reverb_damping;
// }

// void FluidSynthPlayer::set_reverb_width(double value) {
//     reverb_width = CLAMP(value, 0.0, 100.0);

//     if (settings) {
//         fluid_settings_setnum(
//             settings,
//             "synth.reverb.width",
//             reverb_width);
//     }
// }

// double FluidSynthPlayer::get_reverb_width() const {
//     return reverb_width;
// }

// void FluidSynthPlayer::set_reverb_level(double value) {
//     reverb_level = CLAMP(value, 0.0, 1.0);

//     if (settings) {
//         fluid_settings_setnum(settings, "synth.reverb.level", reverb_level);
//     }
// }

// double FluidSynthPlayer::get_reverb_level() const {
//     return reverb_level;
// }

// // Chorus

// void FluidSynthPlayer::set_chorus_enabled(bool value) {
//     chorus_enabled = value;

//     if (settings) {
//         fluid_settings_setint(settings, "synth.chorus.active", chorus_enabled ? 1 : 0);
//     }
// }

// bool FluidSynthPlayer::is_chorus_enabled() const {
//     return chorus_enabled;
// }

// void FluidSynthPlayer::set_chorus_number(int value) {
//     chorus_number = CLAMP(value, 0, 99);

//     if (settings) {
//         fluid_settings_setint(settings, "synth.chorus.nr", chorus_number);
//     }
// }

// int FluidSynthPlayer::get_chorus_number() const {
//     return chorus_number;
// }

// void FluidSynthPlayer::set_chorus_level(double value) {
//     chorus_level = CLAMP(value, 0.0, 10.0);

//     if (settings) {
//         fluid_settings_setnum(settings, "synth.chorus.level", chorus_level);
//     }
// }

// double FluidSynthPlayer::get_chorus_level() const {
//     return chorus_level;
// }

// void FluidSynthPlayer::set_chorus_speed(double value) {
//     chorus_speed = CLAMP(value, 0.1, 5.0);

//     if (settings) {
//         fluid_settings_setnum(settings, "synth.chorus.speed", chorus_speed);
//     }
// }

// double FluidSynthPlayer::get_chorus_speed() const {
//     return chorus_speed;
// }

// void FluidSynthPlayer::set_chorus_depth(double value) {
//     chorus_depth = CLAMP(value, 0.0, 256.0);

//     if (settings) {
//         fluid_settings_setnum(settings, "synth.chorus.depth", chorus_depth);
//     }
// }

// double FluidSynthPlayer::get_chorus_depth() const {
//     return chorus_depth;
// }

