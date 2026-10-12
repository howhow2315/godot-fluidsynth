#!/usr/bin/env python
import os, sys
from methods import print_error


libname = "fluidsynth"
ADDON_NAME = f"godot-{libname}"
API_VERSION = "4.7"

# Build profiles can be used to decrease compile times.
# You can either specify "disabled_classes", OR
# explicitly specify "enabled_classes" which disables all other classes.
# Modify the example file as needed and uncomment the line below or
# manually specify the build_profile parameter when running SCons.
# see build_profile.json
USE_BUILD_PROFILE = True

PROJECT_DIRECTORY = "project"
LIBRARY_DIRECTORY = "library"
SOURCE_DIRECTORY = "source"
GENERATED_DIRECTORY = f"generated"
ADDON_DIRECTORY = f"{PROJECT_DIRECTORY}/addons/{ADDON_NAME}"
BINARY_DIRECTORY = f"{ADDON_DIRECTORY}/binaries"
GENERATED_DOCUMENTS_DIRECTORY = f"{GENERATED_DIRECTORY}/documents"
LIBRARY_INSTALL = f"{GENERATED_DIRECTORY}/{LIBRARY_DIRECTORY}"

FLUIDSYNTH_SOURCE = f"{LIBRARY_DIRECTORY}/fluidsynth"
FLUIDSYNTH_INSTALL = f"{GENERATED_DIRECTORY}/fluidsynth"

localEnv = Environment(tools=["default"], PLATFORM="")
if USE_BUILD_PROFILE:
    localEnv["build_profile"] = "build_profile.json"

customs = ["custom.py"]
customs = [os.path.abspath(path) for path in customs]

opts = Variables(customs, ARGUMENTS)
opts.Update(localEnv)

Help(opts.GenerateHelpText(localEnv))

env = localEnv.Clone()

if not (
    os.path.isdir(f"{LIBRARY_DIRECTORY}/godot-cpp")
    and os.listdir(f"{LIBRARY_DIRECTORY}/godot-cpp")
):
    print_error(
        """godot-cpp is not available within this folder, as Git submodules haven't been initialized.
Run the following command to download godot-cpp:

    git submodule update --init --recursive"""
    )
    sys.exit(1)

env = SConscript(
    f"{LIBRARY_DIRECTORY}/godot-cpp/SConstruct",
    {"api_version": API_VERSION, "env": env, "customs": customs},
)
sources = Glob(f"{SOURCE_DIRECTORY}/*.cpp") + Glob(
    f"{SOURCE_DIRECTORY}/*/*.cpp"
)  # also include subfolders

env["FLUIDSYNTH_INSTALL"] = FLUIDSYNTH_INSTALL
env.Tool("fluidsynth", toolpath=["tools"])

platform = env["platform"]

# BIN_SUBDIRECTORY = platform
BIN_SUBDIRECTORY = f"{env["platform"]}.{env['arch']}"  # if you use this make sure to update the .gdextension

FLUIDSYNTH_LINK_LIBRARY = f"lib/libfluidsynth{'-3' if platform == 'windows' else ''}.a"

fluidsynth_targets = [f"{FLUIDSYNTH_INSTALL}/{FLUIDSYNTH_LINK_LIBRARY}"]
fluidsynth_build = env.fluidsynth(fluidsynth_targets, FLUIDSYNTH_SOURCE)

env.Append(
    CPPPATH=[
        "source",
        f"{FLUIDSYNTH_INSTALL}/include",
    ],
    LIBPATH=[
        f"{FLUIDSYNTH_INSTALL}/lib",
        f"{LIBRARY_DIRECTORY}/godot-cpp/bin",
    ],
)



env.Append(CPPPATH=[f"{SOURCE_DIRECTORY}/"])

objects = env.SharedObject(sources)
env.Depends(objects, fluidsynth_build)

suffix = env["suffix"]

godot_cpp = File(
    f"{LIBRARY_DIRECTORY}/godot-cpp/bin/libgodot-cpp{suffix}{env['LIBSUFFIX']}"
)
fluidsynth = File(f"{FLUIDSYNTH_INSTALL}/{FLUIDSYNTH_LINK_LIBRARY}")

# .dev doesn't inhibit compatibility, so we don't need to key it.
# .universal just means "compatible with all relevant arches" so we don't need to key it.
suffix = env["suffix"].replace(".dev", "").replace(".universal", "")
lib_filename = (
    f"{env.subst('$SHLIBPREFIX')}{libname}{suffix}{env.subst('$SHLIBSUFFIX')}"
)

link_libs = [godot_cpp, fluidsynth]
# if platform == "windows": link_libs.append("gomp")
if platform == "android": link_libs.append("log")
# link the OpenMP runtime; windows needs this too if were adding openmp
if platform == "linux": 
    link_libs.append("gomp")
    env.Append(LINKFLAGS=["-fopenmp"])

if platform == "ios":
    # iOS GDExtensions are packaged as static archives.
    library = env.StaticLibrary(
        f"{LIBRARY_INSTALL}/{BIN_SUBDIRECTORY}/lib{libname}",
        source=objects,
        LIBS=link_libs,
    )
else:
    library = env.SharedLibrary(
        f"{LIBRARY_INSTALL}/{BIN_SUBDIRECTORY}/{lib_filename}",
        source=objects,
        LIBS=link_libs,
    )

env.Depends(library, fluidsynth_build)
default_targets = [library]

# project files
addons_source = ["license.md", "readme.md"]
addons_files = env.Command(
    [f"{ADDON_DIRECTORY}/{file}" for file in addons_source],
    addons_source,
    [Copy(f"{ADDON_DIRECTORY}/{file}", file) for file in addons_source],
)
default_targets.append(addons_files)
Alias("addons_files", addons_files)

# external licenses
license_file = "license"
licenses = {"fluidsynth": f"{LIBRARY_DIRECTORY}/fluidsynth/LICENSE"}
license_targets = [
    f"{ADDON_DIRECTORY}/licenses/{name}.{license_file}" for name in licenses
]
license_sources = list(licenses.values())
licenses_files = env.Command(
    license_targets,
    license_sources,
    [
        Copy(f"{ADDON_DIRECTORY}/licenses/{name}.{license_file}", source)
        for name, source in licenses.items()
    ],
)
default_targets.append(licenses_files)
Alias("licenses_files", licenses_files)

# include document data
if env["target"] in ["editor", "template_debug"]:
    try:
        doc_data = env.GodotCPPDocData(
            f"{GENERATED_DOCUMENTS_DIRECTORY}/doc_data.gen.cpp",
            source=Glob("doc_classes/*.xml"),
        )
        sources.append(doc_data)
    except AttributeError:
        print("Not including class reference as we're targeting a pre-4.3 baseline.")

copy = env.Install(f"{BINARY_DIRECTORY}/{BIN_SUBDIRECTORY}/", library)

default_args = [library, copy]
Default(*default_args, *default_targets)
