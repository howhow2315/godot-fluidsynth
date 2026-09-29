#!/usr/bin/env bash

set -euo pipefail


ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

rm -rf "$ROOT/build/fluidsynth" # rm fluidsynth build files
rm -rf "$ROOT/addons/godot-fluidsynth/bin/" # rm built addon bin 
rm -rf "$ROOT/.godot/" # rm godot cache 

scons -c # clean out c++ build files 

scons