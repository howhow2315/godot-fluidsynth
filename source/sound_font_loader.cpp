#include "sound_font_loader.hpp"

PackedStringArray SoundFontLoader::_get_recognized_extensions() const {
	PackedStringArray extensions;
	extensions.push_back("sf2");
	// extensions.push_back("sf3");
	return extensions;
}

bool SoundFontLoader::_handles_type(const StringName &type) const {
	return type == StringName("SoundFont");
}

String SoundFontLoader::_get_resource_type(const String &path) const {
	const String extension = path.get_extension().to_lower();

	if (
			extension == "sf2"
			// || extension == "sf3"
	) {
		return "SoundFont";
	}

	return String();
}

Variant SoundFontLoader::_load(const String &path, const String &original_path,
		bool use_sub_threads, int32_t cache_mode) const {
	Ref<SoundFont> sound_font;
	sound_font.instantiate();

	if (!sound_font->scan(path)) {
		UtilityFunctions::push_error("Failed to load SoundFont: " + path);

		return Variant();
	}

	return sound_font;
}
