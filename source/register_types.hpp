#pragma once

#include <godot_cpp/core/class_db.hpp>

void initialize_fluidsynth_module(
    godot::ModuleInitializationLevel p_level
);

void uninitialize_fluidsynth_module(
    godot::ModuleInitializationLevel p_level
);
