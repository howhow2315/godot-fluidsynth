#!/usr/bin/env python3
"""Build and install FluidSynth for a godot-cpp target using CMake."""

from __future__ import annotations

import os
import shlex
import shutil
import subprocess
from pathlib import Path


# https://www.fluidsynth.org/wiki/BuildingWithCMake/#output-library-naming-conventions
# we dont need fluidsynth to have access to any audio servers, midi devices, or anything system level like networking or d-bus/systemd
BUILD_OPTIONS = {
    # Linux
    "alsa": False,  # Linux ALSA audio device backend.
    "jack": False,  # JACK audio server backend.
    "pipewire": False,  # Linux PipeWire audio server backend.
    "pulseaudio": False,  # PulseAudio audio server backend.
    "oss": False,  # OSS audio device backend.
    "systemd": False,  # Linux systemd integration; unnecessary for library use.
    "dbus": False,  # D-Bus integration; unnecessary for library use.
    # Windows
    "dsound": False,  # Windows DirectSound audio device backend.
    "wasapi": False,  # Windows WASAPI audio device backend.
    "waveout": False,  # Windows WaveOut audio device backend.
    "winmidi": False,  # Windows MIDI device backend.
    # Cross-platform
    "portaudio": False,  # PortAudio audio device backend.
    "sdl3": False,  # SDL3 audio device backend.
    "aufile": False,  # Audio-file output driver.
    "libsndfile": False,  # libsndfile audio-file output support.
    "midishare": False,  # MidiShare MIDI backend.
    "network": False,  # Network/MIDI networking support.
    # Android
    "oboe": False,  # Android Oboe audio backend.
    "opensles": False,  # Android OpenSL ES audio backend.
    # Core synthesis
    "threads": True,  # Multithreading, including parallel voice synthesis.
    "openmp": True,  # OpenMP acceleration for synthesis/SoundFont processing.
    "unicode": True,  # Unicode support, particularly relevant on Windows.
    "native-dls": False,  # Native DLS support; unrelated to audio device access.
    "signalsmith": True,  # Signalsmith DSP limiter/reverb; useful for normal DSP.
    # Optional integrations
    "ladspa": False,  # LADSPA plugin/effect support; enable only if needed.
    "readline": False,  # Readline support for the interactive CLI; unnecessary for library use.
    "ipv6": False,  # IPv6 support; only relevant if network functionality is enabled.
}

PLATFORM_BUILD_OPTIONS = {
    "linux": {
        "openmp": False,
    },
    "windows": {
        "openmp": False,
    },
    "macos": {
        "coreaudio": False,  # macOS/iOS CoreAudio audio device backend.
        "coremidi": False,  # macOS/iOS CoreMIDI MIDI device backend.
        "framework": False,  # Build a normal dylib instead of FluidSynth.framework.
    },
    "ios": {
        "coreaudio": False,
        "coremidi": False,
        "framework": False,
    },
    "web": {
        "threads": False,
        "openmp": False,
        "signalsmith": False,
    },
}


def run(*args: str) -> None:
    print("+", shlex.join(args), flush=True)
    subprocess.run(args, check=True)


def cmake_bool(value: bool) -> str:
    return "ON" if value else "OFF"


def target_cmake_args(platform: str, arch: str) -> list[str]:
    """Return CMake target settings, not host settings."""
    args: list[str] = []

    if platform == "macos":
        if arch == "universal":
            args.append("-DCMAKE_OSX_ARCHITECTURES=arm64;x86_64")
        elif arch in ("arm64", "x86_64"):
            args.append(f"-DCMAKE_OSX_ARCHITECTURES={arch}")
        else:
            raise ValueError(f"Unsupported macOS architecture: {arch}")

    elif platform == "ios":
        if arch != "arm64":
            raise ValueError(
                f"This iOS configuration supports device arm64 only, not {arch}"
            )

        args.extend(
            [
                "-DCMAKE_SYSTEM_NAME=iOS",
                "-DCMAKE_OSX_ARCHITECTURES=arm64",
                "-DCMAKE_MACOSX_BUNDLE=OFF",
            ]
        )

    elif platform == "android":
        ndk = os.environ.get("ANDROID_NDK_ROOT") or os.environ.get("ANDROID_NDK_HOME")
        if not ndk:
            raise RuntimeError("Set ANDROID_NDK_ROOT to your Android NDK installation.")

        args.extend(
            [
                f"-DCMAKE_TOOLCHAIN_FILE={Path(ndk) / 'build/cmake/android.toolchain.cmake'}",
                "-DANDROID_PLATFORM=android-24",
            ]
        )

        android_abis = {
            "x86_64": "x86_64",
            "x86_32": "x86",
            "arm64": "arm64-v8a",
            "arm32": "armeabi-v7a",
        }

        try:
            args.append(f"-DANDROID_ABI={android_abis[arch]}")
        except KeyError:
            raise ValueError(f"Unsupported Android architecture: {arch}") from None

    elif platform == "web":
        if arch != "wasm32":
            raise ValueError(f"Unsupported Web architecture: {arch}")

        emsdk = os.environ.get("EMSDK")
        if not emsdk:
            raise RuntimeError(
                "Set EMSDK to the Emscripten SDK installation directory."
            )

        emsdk_path = Path(emsdk)

        # Official emsdk and distro-packaged Emscripten use different layouts.
        candidates = [
            emsdk_path / "upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake",
            emsdk_path / "cmake/Modules/Platform/Emscripten.cmake",
        ]

        toolchain = next(
            (path for path in candidates if path.is_file()),
            None,
        )

        if toolchain is None:
            expected = "\n".join(f"  {path}" for path in candidates)
            raise RuntimeError(
                f"Could not find Emscripten.cmake under EMSDK={emsdk}.\n"
                f"Checked:\n{expected}"
            )

        args.extend(
            [
                f"-DCMAKE_TOOLCHAIN_FILE={toolchain}",
                "-DCMAKE_SYSTEM_NAME=Emscripten",
                "-DCMAKE_SYSTEM_PROCESSOR=wasm32",
                "-Dosal=embedded",
            ]
        )

    elif platform == "windows":
        args.extend(
            [
                "-G",
                "MinGW Makefiles",
                "-DCMAKE_C_COMPILER=gcc",
                "-DCMAKE_CXX_COMPILER=g++",
                # if this doesnt work consider MinGW-w64, im resigning to disable openmp instead for now
                "-DCMAKE_C_FLAGS=-foffload=disable",
                "-DCMAKE_CXX_FLAGS=-foffload=disable",
            ]
        )

    elif platform == "linux":
        linux_arches = {
            "x86_64",
            "x86_32",
            "arm64",
            "arm32",
            "rv64",
            "ppc32",
            "ppc64",
        }
        if arch not in linux_arches:
            raise ValueError(f"Unsupported Linux architecture: {arch}")

        if arch == "x86_32":
            args.extend(
                [
                    "-DCMAKE_C_FLAGS=-m32",
                    "-DCMAKE_CXX_FLAGS=-m32",
                    "-DCMAKE_EXE_LINKER_FLAGS=-m32",
                    "-DCMAKE_SHARED_LINKER_FLAGS=-m32",
                    "-DCMAKE_MODULE_LINKER_FLAGS=-m32",
                    "-DCMAKE_SYSTEM_PROCESSOR=i686",
                ]
            )

    else:
        raise ValueError(f"Unsupported platform: {platform}")

    # Optional toolchain override for Linux, Windows, or custom setups.
    # For Android/Web/iOS, set this only if you intentionally want to
    # override the toolchain selected above.
    override = os.environ.get("FLUIDSYNTH_TOOLCHAIN_FILE")
    if override:
        args = [item for item in args if not item.startswith("-DCMAKE_TOOLCHAIN_FILE=")]
        args.append(f"-DCMAKE_TOOLCHAIN_FILE={override}")

    # Additional CMake arguments, supplied by CI or the caller.
    args.extend(shlex.split(os.environ.get("FLUIDSYNTH_CMAKE_ARGS", "")))

    return args


def build_cmake(
    source: Path, build: Path, install: Path, platform: str, arch: str
) -> None:
    build.mkdir(parents=True, exist_ok=True)

    # Avoid accidentally packaging artifacts from an earlier target.
    if install.exists():
        shutil.rmtree(install)

    configure = [
        "cmake",
        "-S",
        str(source),
        "-B",
        str(build),
        "-DCMAKE_BUILD_TYPE=Release",
        "-DBUILD_SHARED_LIBS=OFF",
        "-DCMAKE_POSITION_INDEPENDENT_CODE=ON",
        f"-DCMAKE_INSTALL_PREFIX={install}",
        "-DCMAKE_WARN_DEPRECATED=OFF",
        # web seems like it needs this 'error: no member named 'clamp' in namespace 'std'', but it just wont build with EMCC_FORCE_STDLIBS=1 or link without it
        # "-DCMAKE_CXX_STANDARD=17",
        # "-DCMAKE_CXX_STANDARD_REQUIRED=ON",
        # "-DCMAKE_CXX_EXTENSIONS=OFF",
        # "-DCMAKE_CXX_FLAGS=-std=c++17 -include algorithm",
    ]

    build_options = {
        **BUILD_OPTIONS,
        **PLATFORM_BUILD_OPTIONS.get(platform, {}),
    }

    configure.extend(
        f"-Denable-{name}={cmake_bool(value)}" for name, value in build_options.items()
    )

    # Pass the target architecture and toolchain settings to CMake.
    configure.extend(target_cmake_args(platform, arch))

    run(*configure)
    run(
        "cmake",
        "--build",
        str(build),
        "--target",
        "libfluidsynth",
        "--parallel",
        "--config",
        "Release",
    )
    run(
        "cmake",
        "--install",
        str(build),
        "--config",
        "Release",
        "--component",
        "fluidsynth_development",
    )


def build_fluidsynth(target, source, env) -> None:
    source_path = Path(str(source[0])).resolve()
    install = Path(env["FLUIDSYNTH_INSTALL"]).resolve()

    platform = str(env["platform"])
    arch = str(env.get("arch", ""))

    # Include the target in the build directory to prevent incompatible
    # CMake caches from being reused across platforms and architectures.
    build_root = install.parent / "fluidsynth-build"
    build = build_root / f"{platform}-{arch}"

    if platform == "ios":
        ios_simulator = str(env.get("ios_simulator", ""))
        if ios_simulator:
            build = Path(f"{build}-{ios_simulator}")

    build_cmake(source_path, build, install, platform, arch)

    # Validate the actual artifacts declared by the SCons caller.
    target_paths = [Path(str(node)) for node in target]

    missing = [
        path for path in target_paths if not path.exists() and not path.is_symlink()
    ]

    if missing:
        names = "\n".join(f"  {path}" for path in missing)
        raise RuntimeError(
            f"FluidSynth installation completed, but these declared SCons targets were not produced:\n{names}"
        )


def fluidsynth(env, target, source):
    return env.Command(target, source, build_fluidsynth)


def exists(env) -> bool:
    return True


def generate(env) -> None:
    env.AddMethod(fluidsynth, "fluidsynth")
