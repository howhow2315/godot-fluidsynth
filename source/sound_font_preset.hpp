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
		ADD_PROPERTY(PropertyInfo(Variant::INT, "bank"), "get_bank", "set_bank");

		ClassDB::bind_method(D_METHOD("get_program"), &SoundFontPreset::get_program);
		ClassDB::bind_method(D_METHOD("set_program", "value"), &SoundFontPreset::set_program);
		ADD_PROPERTY(PropertyInfo(Variant::INT, "program"), "get_program", "set_program");

		ClassDB::bind_method(D_METHOD("get_name"), &SoundFontPreset::get_name);
		ClassDB::bind_method(D_METHOD("set_name", "value"), &SoundFontPreset::set_name);
		ADD_PROPERTY(PropertyInfo(Variant::STRING, "name"), "get_name", "set_name");
	}

public:
	int bank;
	int get_bank() { return bank; };
	void set_bank(int value) { bank = value; };

	int program;
	int get_program() { return program; };
	void set_program(int value) { program = value; };

	String name; // there would be no issue making this a StringName right?
	String get_name() { return name; };
	void set_name(String value) { name = value; };

	SoundFontPreset();
	~SoundFontPreset();
};
