# godot-fluidsynth

a godot gdextension for fluidsynth with sf2 support

# tip

useful alongside [howhow2315/godot-midi](https://github.com/howhow2315/godot-midi) to read and play midi files !

## notes

fluidsynth should work with sf3 but windows requires a library to be bundled with it and thats out of my personal scope

frankly theres a lot of work left to do to make this a proper, and good plugin, but this proof of concept has basic functionality without much compromise (openmp enraging me)

if you decide to use this or help at all, i deeply apologize for my lack of commits, i needed to pump this out as fast as i could and that meant mass local hotfixing (especially with ci...), id be happy to explain anything thats needed, feel free to contact me at [howhow.stream](https://howhow.stream)

## todo

- export to all platforms that fluidsynth and godot support; currently implemented and maintained: linux, windows
- add chorus midi fluidsynth controls (basic functions are present, they do not work)
- look into handling "program"/"instruments" better
- finish editor annotations via godots .xml feature
- cleanup `presets` to be sorted by bank and program number instead of a big array/dictionary
- figure out how to reduce latency and handle buffer size better (its already pretty low but lower the better)
- cache fluidsynth build in ci

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
