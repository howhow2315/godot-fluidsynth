# godot-fluidsynth

a godot gdextension for fluidsynth with sf2 support


## notes

- fluidsynth should work with sf3 but windows requires a library to be bundled with it and thats out of my personal scope


## todo

- export to all platforms fluidsynth and godot support; currently implemented: linux.x86_64, windows.x86_64 (basic macos stuff is laid out)
- add pitch_bend/chorus/reverb midi fluidsynth controls (basic functions are present, they do not work)
- add editor annotations via godots .xml feature
- cleanup `presets` to be sorted by bank and program number instead of a big array/dictionary
- figure out how to handle buffer size better as well as reduce latency (its already pretty low but lower the better)
- add FluidSynth license on build from SConstruct instead of it being static manually