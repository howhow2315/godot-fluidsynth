#pragma once

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <fluidsynth.h>

#include "sound_font_preset.hpp"

using namespace godot;

// i dont think editing sf2s is even remotely in this scope

class SoundFont : public Resource {
	GDCLASS(SoundFont, Resource);

private:
	PackedByteArray data;
	TypedArray<SoundFontPreset> presets;

	bool scan_data();

protected:
	static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("set_data", "data"), &SoundFont::set_data);
		ClassDB::bind_method(D_METHOD("get_data"), &SoundFont::get_data);

		ClassDB::bind_method(D_METHOD("set_presets", "presets"), &SoundFont::set_presets);
		ClassDB::bind_method(D_METHOD("get_presets"), &SoundFont::get_presets);
		ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "presets", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_READ_ONLY), "set_presets","get_presets");

		ClassDB::bind_method(D_METHOD("scan", "path"), &SoundFont::scan);

		ClassDB::bind_method(D_METHOD("load_data", "data"), &SoundFont::load_data);
	}

public:
	SoundFont() = default;
	~SoundFont() = default;

	void set_data(const PackedByteArray &value);
	PackedByteArray get_data() const;

	TypedArray<SoundFontPreset> get_presets() const;
	void set_presets(const TypedArray<SoundFontPreset> &value);

	// Convenience API for loading an SF2 directly from a path.
	bool scan(const String &path);

	// Primary API used by ResourceFormatLoader.
	bool load_data(const PackedByteArray &value);

	int load_into_synth(fluid_synth_t *synth);

	static fluid_sfloader_t *create_memory_loader(fluid_settings_t *settings);
};