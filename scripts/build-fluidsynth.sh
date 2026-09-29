#!/usr/bin/env bash

set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

SOURCE="$ROOT/library/fluidsynth"
BUILD="$ROOT/build/fluidsynth-linux"
INSTALL="$ROOT/build/fluidsynth"

rm -rf "$BUILD" "$INSTALL"

cmake \
    -S "$SOURCE" \
    -B "$BUILD" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$INSTALL" \
    -DBUILD_SHARED_LIBS=ON \
    -Denable-libsndfile=OFF \
    -Denable-jack=OFF \
    -Denable-pulseaudio=OFF \
    -Denable-alsa=OFF \
    -Denable-pipewire=OFF \
    -Denable-sdl3=OFF \
    -Denable-dbus=OFF \
    -Denable-readline=OFF \
    -Denable-ladspa=OFF \
    -Denable-network=OFF \
    -Denable-midishare=OFF \
    -Denable-opensles=OFF \
    -Denable-oboe=OFF \
    -Denable-portaudio=OFF \
    -Denable-coreaudio=OFF \
    -Denable-coremidi=OFF \
    -Denable-dsound=OFF \
    -Denable-wasapi=OFF \
    -Denable-waveout=OFF \
    -Denable-winmidi=OFF \
    -Denable-oss=OFF

cmake \
    --build "$BUILD" \
    --parallel

cmake \
    --install "$BUILD"
