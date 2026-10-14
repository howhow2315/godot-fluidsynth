#pragma once

#include "godot_cpp/classes/resource.hpp"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

/// @brief SoundFontPreset
class SoundFontPreset : public Resource {
	GDCLASS(SoundFontPreset, Resource);

protected:
	static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("get_bank"), &SoundFontPreset::get_bank);
		ClassDB::bind_method(D_METHOD("set_bank", "value"), &SoundFontPreset::set_bank);
		ADD_PROPERTY(PropertyInfo(Variant::INT, "bank", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_READ_ONLY), "set_bank", "get_bank");

		ClassDB::bind_method(D_METHOD("get_program"), &SoundFontPreset::get_program);
		ClassDB::bind_method(D_METHOD("set_program", "value"), &SoundFontPreset::set_program);
		ADD_PROPERTY(PropertyInfo(Variant::INT, "program", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_READ_ONLY), "set_program", "get_program");

		ClassDB::bind_method(D_METHOD("get_name"), &SoundFontPreset::get_name);
		ClassDB::bind_method(D_METHOD("set_name", "value"), &SoundFontPreset::set_name);
		ADD_PROPERTY(PropertyInfo(Variant::STRING, "name", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_READ_ONLY), "set_name", "get_name");
	}

public:
	SoundFontPreset() = default;
	~SoundFontPreset() = default;

	int bank;
	int get_bank() { return bank; };
	void set_bank(int value) { bank = value; };

	int program;
	int get_program() { return program; };
	void set_program(int value) { program = value; };

	String name; // there would be no issue making this a StringName right?
	String get_name() { return name; };
	void set_name(String value) { name = value; };
};
