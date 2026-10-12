#pragma once

#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_fluidsynth_module(godot::ModuleInitializationLevel p_level);
void uninitialize_fluidsynth_module(godot::ModuleInitializationLevel p_level);
