#pragma once

#include <fluidsynth.h>

#include <godot_cpp/classes/node.hpp>

#include <godot_cpp/classes/audio_stream_generator.hpp>
#include <godot_cpp/classes/audio_stream_generator_playback.hpp>
#include <godot_cpp/classes/audio_stream_player.hpp>

#include <godot_cpp/classes/input_event_midi.hpp>

#include "soundfont.hpp"

namespace godot {

class FluidSynthPlayer : public Node {
    GDCLASS(FluidSynthPlayer, Node);

private:
    // fluidsynth
    fluid_settings_t *settings = nullptr;
    fluid_synth_t *synth = nullptr;
    fluid_sfloader_t *sfloader = nullptr;

    // Godot audio output.
    AudioStreamPlayer *audio_player = nullptr;
    Ref<AudioStreamGenerator> audio_stream;
    Ref<AudioStreamGeneratorPlayback> audio_playback;

    // SoundFont.
    int soundfont_id = -1;

    // Audio configuration.
    double buffer_length = 0.05;
    double volume = 1.0;
    double gain = 0.0;
    int polyphony = 256;

    // // Reverb.
    // bool reverb_enabled = false;
    // double reverb_room_size = 0.61;
    // double reverb_damping = 0.23;
    // double reverb_width = 0.76;
    // double reverb_level = 0.61;


    // // Chorus.
    // bool chorus_enabled = false;
    // int chorus_number = 3;
    // double chorus_level = 1.2;
    // double chorus_speed = 0.3;
    // double chorus_depth = 8.0;

    void apply_synth_settings();

protected:
    static void _bind_methods();

public:
    FluidSynthPlayer();
    ~FluidSynthPlayer();

    void _ready();
    void _process(double delta);

    void _start_stream_playback();

    // Audio.
    void render_audio();

    // SoundFont.
    int load_soundfont(const Ref<SoundFont> &soundfont);
    TypedArray<Dictionary> get_soundfont_presets();

    // MIDI.
    void note_on(int channel, int key, int velocity);
    void note_off(int channel, int key);
    void program_change(int channel, int bank, int program);
    void control_change(int channel, int controller, int value);
    void key_pressure(int channel, int key, int pressure);
    void channel_pressure(int channel, int pressure);

    void all_notes_off(int channel);
    void all_sounds_off(int channel);
    void reset_controllers(int channel);

    void handle_midi_event(const Ref<InputEventMIDI> &event);

    // Buffer.
    void set_buffer_length(double seconds);
    double get_buffer_length() const;

    // Volume.
    void set_volume(double value);
    double get_volume() const;

    // Gain.
    void set_gain(double value);
    double get_gain() const;

    // Polyphony.
    void set_polyphony(int value);
    int get_polyphony() const;

    // // Reverb.
    // void set_reverb_enabled(bool value);
    // bool is_reverb_enabled() const;

    // void set_reverb_room_size(double value);
    // double get_reverb_room_size() const;

    // void set_reverb_damping(double value);
    // double get_reverb_damping() const;

    // void set_reverb_width(double value);
    // double get_reverb_width() const;

    // void set_reverb_level(double value);
    // double get_reverb_level() const;

    // // Chorus.
    // void set_chorus_enabled(bool value);
    // bool is_chorus_enabled() const;

    // void set_chorus_number(int value);
    // int get_chorus_number() const;

    // void set_chorus_level(double value);
    // double get_chorus_level() const;

    // void set_chorus_speed(double value);
    // double get_chorus_speed() const;

    // void set_chorus_depth(double value);
    // double get_chorus_depth() const;
};

} // namespace godot
