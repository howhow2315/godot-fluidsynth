#!/usr/bin/env python3

from __future__ import annotations

import shutil
import subprocess
from pathlib import Path


# https://www.fluidsynth.org/wiki/BuildingWithCMake/
# https://www.fluidsynth.org/wiki/BuildingWithCMake/#output-library-naming-conventions
# we dont need fluidsynth to have access to any audio servers, midi devices, or anything system level like networking or d-bus/systemd
BUILD_OPTIONS = {
    # Linux
    "enable-alsa": False,          # Linux ALSA audio device backend.
    "enable-jack": False,          # JACK audio server backend.
    "enable-pipewire": False,      # Linux PipeWire audio server backend.
    "enable-pulseaudio": False,    # PulseAudio audio server backend.
    "enable-oss": False,           # OSS audio device backend.
    "enable-systemd": False,       # Linux systemd integration; unnecessary for library use.
    "enable-dbus": False,          # D-Bus integration; unnecessary for library use.

    # Windows
    "enable-dsound": False,        # Windows DirectSound audio device backend.
    "enable-wasapi": False,        # Windows WASAPI audio device backend.
    "enable-waveout": False,       # Windows WaveOut audio device backend.
    "enable-winmidi": False,       # Windows MIDI device backend.

    # macOS
    "enable-coreaudio": False,     # macOS/iOS CoreAudio audio device backend.
    "enable-coremidi": False,      # macOS/iOS CoreMIDI MIDI device backend.
    "enable-framework": False,     # Build a normal dylib instead of FluidSynth.framework.

    # Cross-platform
    "enable-portaudio": False,     # PortAudio audio device backend.
    "enable-sdl3": False,          # SDL3 audio device backend.
    "enable-aufile": False,        # Audio-file output driver.
    "enable-libsndfile": False,    # libsndfile audio-file output support.
    "enable-midishare": False,     # MidiShare MIDI backend.
    "enable-network": False,       # Network/MIDI networking support.

    # Android
    "enable-oboe": False,          # Android Oboe audio backend.
    "enable-opensles": False,       # Android OpenSL ES audio backend.

    # Core synthesis
    "enable-threads": True,        # Multithreading, including parallel voice synthesis.
    "enable-openmp": True,         # OpenMP acceleration for synthesis/SoundFont processing.
    "enable-unicode": True,        # Unicode support, particularly relevant on Windows.
    "enable-native-dls": False,    # Native DLS support; unrelated to audio device access.
    "enable-signalsmith": True,    # Signalsmith DSP limiter/reverb; useful for normal DSP.

    # Optional integrations
    "enable-ladspa": False,        # LADSPA plugin/effect support; enable only if needed.
    "enable-readline": False,      # Readline support for the interactive CLI; unnecessary for library use.
    "enable-ipv6": False,          # IPv6 support; only relevant if network functionality is enabled.
}


def run(*args: str) -> None:
    print("+", " ".join(args))
    subprocess.run(args, check=True)


def cmake_bool(value: bool) -> str:
    return "ON" if value else "OFF"


def build_cmake(source_path, build, install, architecture=None) -> None:
    configure = ["cmake", "-S", str(source_path), "-B", str(build), "-DCMAKE_BUILD_TYPE=Release", "-DBUILD_SHARED_LIBS=ON", f"-DCMAKE_INSTALL_PREFIX={install}"]

    if architecture is not None:
        configure.append(f"-DCMAKE_OSX_ARCHITECTURES={architecture}")

    configure.extend(
        f"-D{name}={cmake_bool(value)}"
        for name, value in BUILD_OPTIONS.items()
    )

    run(*configure)
    run("cmake", "--build", str(build), "--parallel", "--config", "Release")
    run("cmake", "--install", str(build), "--config", "Release")


# build and install fluidsynth with cmake
# every file in `target` is an installed artifact produced by the `cmake --install` step
# scons therefore knows that those files are outputs of this action
def build_fluidsynth(target, source, env) -> None:
    source_path = Path(str(source[0]))

    target_paths = [
        Path(str(node))
        for node in target
    ]

    install = Path(env["FLUIDSYNTH_INSTALL"])
    build = install.parent
    platform = env["platform"]

    # this will automatically build fluidsynth for the platform its running on
    # no idea how building for web will work

    # for macos to build 'universal' (x86 and arm64, aka intel and silicon) we need to build both and "lipo" them together
    if platform == "macos":
        arm64_build = build / "fluidsynth-arm64"
        arm64_install = build / "fluidsynth-arm64-install"
        x86_64_build = build / "fluidsynth-x86_64"
        x86_64_install = build / "fluidsynth-x86_64-install"

        build_cmake(source_path, arm64_build, arm64_install, "arm64")
        build_cmake(source_path, x86_64_build, x86_64_install, "x86_64")

        if install.exists():
            shutil.rmtree(install)

        shutil.copytree(arm64_install, install)

        arm64_library = arm64_install / "lib/libfluidsynth.3.6.1.dylib"
        x86_64_library = x86_64_install / "lib/libfluidsynth.3.6.1.dylib"
        universal_library = install / "lib/libfluidsynth.3.6.1.dylib"

        run("lipo", "-create", str(arm64_library), str(x86_64_library), "-output", str(universal_library))

        print("FluidSynth architectures:")
        run("lipo", "-archs", str(install / "lib/libfluidsynth.dylib"))
        run("lipo", "-archs", str(install / "lib/libfluidsynth.3.dylib"))
        run("lipo", "-archs", str(universal_library))

    else:
        build_cmake(source_path, build, install)

    # debug to ensure the platform artifact names are right
    # print("FluidSynth installation tree:")
    # for path in sorted(install.rglob("*")):
    #     if path.is_file():
    #         print("  ", path)

    # scons expects every declared target to exist when the action finishes, cmake should have created them
    # fail explicitly here if the installed tree does not match the platform-specific artifact list
    missing = [
        path
        for path in target_paths
        if not path.exists() and not path.is_symlink()
    ]

    if missing:
        names = "\n".join(f"  {path}" for path in missing)
        raise RuntimeError(f"FluidSynth installation completed, but the following declared SCons targets were not produced:\n {names}")


def fluidsynth(env, target, source):
    return env.Command(target, source, build_fluidsynth)

def exists(env) -> bool:
    return True

def generate(env) -> None:
    env["FLUIDSYNTH_INSTALL"] = "build/fluidsynth"
    env.AddMethod(fluidsynth, "fluidsynth")
