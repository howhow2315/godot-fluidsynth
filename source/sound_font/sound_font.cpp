#include "sound_font.hpp"
#include "sound_font_preset.hpp"

namespace {

struct MemoryFile {
	const uint8_t *data = nullptr;
	size_t size = 0;
	size_t position = 0;
};

void *memory_open(const char *filename) {
	if (!filename) {
		return nullptr;
	}
	if (strncmp(filename, "godot-sf2://", 12) != 0) {
		return nullptr;
	}

	void *ptr = nullptr;
	if (sscanf(filename + 12, "%p", &ptr) != 1) {
		return nullptr;
	}

	return ptr;
}

int memory_read(void *buffer, fluid_long_long_t count, void *handle) {
	if (!buffer || !handle || count < 0) {
		return FLUID_FAILED;
	}

	MemoryFile *file = static_cast<MemoryFile *>(handle);
	const size_t amount = static_cast<size_t>(count);

	if (file->position > file->size || amount > file->size - file->position) {
		return FLUID_FAILED;
	}

	memcpy(buffer, file->data + file->position, amount);

	file->position += amount;

	return FLUID_OK;
}

int memory_seek(void *handle, fluid_long_long_t offset, int origin) {
	if (!handle) {
		return FLUID_FAILED;
	}

	MemoryFile *file = static_cast<MemoryFile *>(handle);
	fluid_long_long_t position = 0;

	switch (origin) {
		case SEEK_SET:
			position = offset;
			break;

		case SEEK_CUR:
			position = static_cast<fluid_long_long_t>(file->position) + offset;
			break;

		case SEEK_END:
			position = static_cast<fluid_long_long_t>(file->size) + offset;
			break;

		default:
			return FLUID_FAILED;
	}

	if (position < 0) {
		return FLUID_FAILED;
	}
	if (static_cast<uint64_t>(position) > file->size) {
		return FLUID_FAILED;
	}

	file->position = static_cast<size_t>(position);

	return FLUID_OK;
}

fluid_long_long_t memory_tell(void *handle) {
	if (!handle) {
		return FLUID_FAILED;
	}

	MemoryFile *file = static_cast<MemoryFile *>(handle);

	return static_cast<fluid_long_long_t>(file->position);
}

int memory_close(void *) { return FLUID_OK; }

} // namespace

void SoundFont::set_data(const PackedByteArray &value) { data = value; }

PackedByteArray SoundFont::get_data() const { return data; }

void SoundFont::set_presets(const TypedArray<SoundFontPreset> &value) {
	presets = value;
}

TypedArray<SoundFontPreset> SoundFont::get_presets() const { return presets; }

bool SoundFont::scan(const String &path) {
	Ref<FileAccess> file = FileAccess::open(path, FileAccess::READ);
	if (file.is_null()) {
		UtilityFunctions::push_error("Failed to open SoundFont: " + path);
		return false;
	}

	PackedByteArray file_data = file->get_buffer(file->get_length());
	if (file_data.is_empty()) {
		UtilityFunctions::push_error("SoundFont is empty: " + path);
		return false;
	}

	return load_data(file_data);
}

bool SoundFont::load_data(const PackedByteArray &value) {
	if (value.is_empty()) {
		return false;
	}

	data = value;

	return scan_data();
}

bool SoundFont::scan_data() {
	presets.clear();

	if (data.is_empty()) {
		return false;
	}
	fluid_settings_t *settings = new_fluid_settings();
	if (!settings) {
		return false;
	}

	fluid_settings_setnum(settings, "synth.sample-rate", 44100.0);
	fluid_synth_t *synth = new_fluid_synth(settings);
	if (!synth) {
		delete_fluid_settings(settings);
		return false;
	}

	fluid_sfloader_t *loader = create_memory_loader(settings);
	if (!loader) {
		delete_fluid_synth(synth);
		delete_fluid_settings(settings);
		return false;
	}

	fluid_synth_add_sfloader(synth, loader);

	MemoryFile memory;
	memory.data = data.ptr();
	memory.size = data.size();
	memory.position = 0;

	char virtual_filename[128];
	snprintf(virtual_filename, sizeof(virtual_filename), "godot-sf2://%p", static_cast<void *>(&memory));

	int sfid = fluid_synth_sfload(synth, virtual_filename, 0);
	if (sfid < 0) {
		delete_fluid_synth(synth);
		delete_fluid_settings(settings);
		return false;
	}

	fluid_sfont_t *sfont = fluid_synth_get_sfont_by_id(synth, sfid);
	if (!sfont) {
		fluid_synth_sfunload(synth, sfid, 0);

		delete_fluid_synth(synth);
		delete_fluid_settings(settings);

		return false;
	}

	fluid_preset_t *preset = nullptr;

	fluid_sfont_iteration_start(sfont);

	while ((preset = fluid_sfont_iteration_next(sfont)) != nullptr) {
		Ref<SoundFontPreset> info;
		info.instantiate();

		info->set_bank(fluid_preset_get_banknum(preset));
		info->set_program(fluid_preset_get_num(preset));
		info->set_name(String(fluid_preset_get_name(preset)));

		presets.push_back(info);
	}

	fluid_synth_sfunload(synth, sfid, 0);

	delete_fluid_synth(synth);
	delete_fluid_settings(settings);

	return true;
}

fluid_sfloader_t *SoundFont::create_memory_loader(fluid_settings_t *settings) {
	if (!settings) {
		return nullptr;
	}

	fluid_sfloader_t *loader = new_fluid_defsfloader(settings);

	if (!loader) {
		return nullptr;
	}

	const int result = fluid_sfloader_set_callbacks(loader, memory_open, memory_read, memory_seek, memory_tell, memory_close);

	if (result != FLUID_OK) {
		delete_fluid_sfloader(loader);
		return nullptr;
	}

	return loader;
}

int SoundFont::load_into_synth(fluid_synth_t *synth) {
	if (!synth || data.is_empty()) {
		return FLUID_FAILED;
	}

	MemoryFile memory;

	memory.data = data.ptr();
	memory.size = data.size();
	memory.position = 0;

	char virtual_filename[128];

	snprintf(virtual_filename, sizeof(virtual_filename), "godot-sf2://%p", static_cast<void *>(&memory));
	return fluid_synth_sfload(synth, virtual_filename, 1);
}
