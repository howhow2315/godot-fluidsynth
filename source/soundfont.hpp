#pragma once

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

#include <fluidsynth.h>

namespace godot {

class SoundFont : public Resource {
    GDCLASS(SoundFont, Resource);

private:
    PackedByteArray data;
    TypedArray<Dictionary> presets;

    bool scan_data();

protected:
    static void _bind_methods();

public:
    SoundFont();
    ~SoundFont();

    void set_data(const PackedByteArray &value);
    PackedByteArray get_data() const;

    TypedArray<Dictionary> get_presets() const;
    void set_presets(const TypedArray<Dictionary> &value);

    // Convenience API for loading an SF2 directly from a path.
    bool scan(const String &path);

    // Primary API used by ResourceFormatLoader.
    bool load_data(const PackedByteArray &value);

    int load_into_synth(fluid_synth_t *synth);

    static fluid_sfloader_t *create_memory_loader(fluid_settings_t *settings);
};

} // namespace godot
