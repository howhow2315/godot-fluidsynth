# godot-fluidsynth

a godot gdextension for fluidsynth with sf2 support

# tip

useful alongside [howhow2315/godot-midi](https://github.com/howhow2315/godot-midi) to read and play midi files !

## notes

- fluidsynth should work with sf3 but windows requires a library to be bundled with it and thats out of my personal scope


## todo

- export to all platforms fluidsynth and godot support; currently implemented: linux.x86_64, windows.x86_64 (basic macos stuff is laid out)
- add pitch_bend/chorus/reverb midi fluidsynth controls (basic functions are present, they do not work)
- add editor annotations via godots .xml feature
- cleanup `presets` to be sorted by bank and program number instead of a big array/dictionary
- figure out how to handle buffer size better as well as reduce latency (its already pretty low but lower the better)
- add FluidSynth license on build from SConstruct instead of it being static manually
- properly handle "program" changes not make it global and instead per note

#### platforms

- linux: builds, tested: success!
- windows: builds, tested: success!
- ios: builds, untested
- macos: builds, untested
- android: builds, tested: failed
- web: fails to build OR fails to link


on android version
```
2026-10-12 09:06:40.221 11694-11694 godot                   com.example.godotfluidsynth          E  ERROR: Can't open dynamic library: addons/godot-fluidsynth/binaries/android.arm64/libfluidsynth.android.template_debug.arm64.so. Error: dlopen failed: cannot locate symbol "__android_log_print" referenced by "/data/app/~~hNUxFACGjZqdihuIV_LgwQ==/com.example.godotfluidsynth-frEFf0VUpsHwe0Zif9_TBQ==/lib/arm64/libfluidsynth.android.template_debug.arm64.so"....
2026-10-12 09:06:40.221 11694-11694 godot                   com.example.godotfluidsynth          E     at: open_dynamic_library (platform/android/os_android.cpp:263)
2026-10-12 09:06:40.221 11694-11694 godot                   com.example.godotfluidsynth          E  ERROR: Can't open GDExtension dynamic library: 'res://addons/godot-fluidsynth/fluidsynth.gdextension'.
2026-10-12 09:06:40.222 11694-11694 godot                   com.example.godotfluidsynth          E     at: open_library (core/extension/gdextension.cpp:811)
2026-10-12 09:06:40.222 11694-11694 godot                   com.example.godotfluidsynth          E  ERROR: Error loading extension: 'res://addons/godot-fluidsynth/fluidsynth.gdextension'.
2026-10-12 09:06:40.222 11694-11694 godot                   com.example.godotfluidsynth          E     at: load_extensions (core/extension/gdextension_manager.cpp:333)
```
