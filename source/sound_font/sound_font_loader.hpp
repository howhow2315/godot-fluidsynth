#pragma once

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "sound_font.hpp"

using namespace godot;

class SoundFontLoader : public ResourceFormatLoader {
	GDCLASS(SoundFontLoader, ResourceFormatLoader);

protected:
	static void _bind_methods() {}

public:
	SoundFontLoader() = default;
	~SoundFontLoader() = default;

	PackedStringArray _get_recognized_extensions() const override;

	bool _handles_type(const StringName &type) const override;

	String _get_resource_type(const String &path) const override;

	Variant _load(const String &path, const String &original_path, bool use_sub_threads, int32_t cache_mode) const override;
};