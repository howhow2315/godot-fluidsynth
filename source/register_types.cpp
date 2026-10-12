#include "register_types.hpp"

#include "fluidsynth_player.hpp"
#include "sound_font.hpp"
#include "sound_font_loader.hpp"

using namespace godot;

static Ref<SoundFontLoader> sound_font_loader;

void initialize_fluidsynth_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	GDREGISTER_CLASS(FluidSynthPlayer);
	GDREGISTER_CLASS(SoundFont);
	GDREGISTER_CLASS(SoundFontLoader);

	sound_font_loader.instantiate();
	ResourceLoader::get_singleton()->add_resource_format_loader(sound_font_loader);
}

void uninitialize_fluidsynth_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	if (sound_font_loader.is_valid()) {
		ResourceLoader::get_singleton()->remove_resource_format_loader(
				sound_font_loader);
		sound_font_loader.unref();
	}
}

extern "C" {
GDExtensionBool GDE_EXPORT
fluidsynth_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address,
		const GDExtensionClassLibraryPtr p_library,
		GDExtensionInitialization *r_initialization) {
	GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library,
			r_initialization);

	init_obj.register_initializer(initialize_fluidsynth_module);

	init_obj.register_terminator(uninitialize_fluidsynth_module);

	init_obj.set_minimum_library_initialization_level(
			MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}
}
