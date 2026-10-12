#!/usr/bin/env bash

set -euo pipefail


ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PROJECT_DIRETORY="${ROOT}/project"

rm -rf "$ROOT/build/fluidsynth" # rm fluidsynth build files
rm -rf "$PROJECT_DIRETORY/addons/godot-fluidsynth/binaries" # rm built addon bin files 
rm -rf "$PROJECT_DIRETORY/.godot/" # rm godot cache 
rm -rf "$ROOT/generated" # rm generated files

BUILD_PROFILE_FILE="${ROOT}/build_profile.json"
if [ ! -e "$BUILD_PROFILE_FILE" ]; then
    scons compiledb=yes compile_commands.json
fi

scons -c # clean out c++ build files 

# build for host platform
scons

# build for linux
# scons platform=linux arch=x86_64
# scons platform=linux arch=x86_32

# build for webasm
# export EMCC_FORCE_STDLIBS=1
# scons platform=web arch=wasm32 threads=false
# scons platform=web arch=wasm32
