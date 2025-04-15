# Game Launcher Prototype & Diff/Patch Utility

This project is a **prototyping build** for a game launcher and a separate diff/patch utility tool, developed in C. The game launcher uses **raylib** and **raygui** to create a graphical interface for handling game updates and patches. The diff/patch utility is a standalone tool for testing and generating file differences and patches, not integrated into the launcher itself.
## Table of Contents
- [License](#license)
- [Overview](#overview)
- [Features](#features)
- [Tools and Libraries](#tools-and-libraries)
- [Setup and Build](#setup-and-build)
- [Usage](#usage)
- [Credits](#credits)
# License
This project is licensed under the MIT License (LICENSE).
## Overview
The project consists of two main components:
1. **Game Launcher**: A prototype GUI application built with raylib and raygui, designed to manage game updates and apply patches using **libcurl** for network operations.
2. **Diff/Patch Utility**: A standalone tool for generating and applying file diffs and patches, using **bsdiff**. This utility is intended for testing and development purposes, separate from the launcher.

The diff/patch tool is **not** used within the launcher but serves as a utility for handling file comparison and patch testing workflows.
## Features
- **Game Launcher**:
  - Graphical interface powered by raylib and raygui.
  - Network support for downloading updates/patches via libcurl.
  - Prototype for managing game updates and patching workflows.
- **Diff/Patch Utility**:
  - Generates file differences using bsdiff.
  - Applies patches for testing purposes.
  - Standalone tool for developers to validate diff and patch processes.

## Tools and Libraries
- **Development Tools**:
  - Visual Studio 2022 (current configuration)
  - CMake (build system)
- **Libraries**:
  - [raylib 5.5](https://www.raylib.com/) - For rendering and game launcher GUI.
  - [raygui 4.0](https://github.com/raysan5/raygui) - For GUI components.
  - [bsdiff](https://github.com/zhuyie/bsdiff) - For diff and patch generation (Windows build supported).
  - [libcurl 8.13.0](https://curl.se/libcurl/) - For network operations in the launcher.

## Setup and Build
1. **Prerequisites**:
   - Install Visual Studio 2022 with C++ support.
   - Install CMake.
   - Download and configure the required libraries: raylib, raygui, bsdiff, and libcurl.
2. **Build Instructions**:
```bash
   mkdir build
   cd build
   cmake -DCMAKE_BUILD_TYPE=Debug ..
   cmake --build . --config Release
```

3. Notes:
    - Ensure libraries are correctly linked in your CMake configuration.
    - The diff/patch utility requires separate setup for file input/output testing.

Usage

- Game Launcher:
    - Run the compiled launcher executable.
    - Use the GUI to initiate game updates or apply patches (network connectivity required for libcurl).
        
- Diff/Patch Utility:
    - Run the utility with appropriate command-line arguments to generate or apply patches.
    - Example (placeholder, adjust based on implementation):

Example not added command line.
```text
./diff_tool old_file new_file patch_file
./patch_tool old_file patch_file output_file
```
 - Configure input/output files for testing diff and patch operations.

- Server:
  - server script download bandwidth limited test.

# Server:
  This is just a debug test server for download test. Work in progress.

# Credits:

- [bsdiff](https://github.com/zhuyie/bsdiff) - For diff and patch functionality.
- [raylib](https://www.raylib.com/) - For rendering and GUI framework.
- [raygui](https://github.com/raysan5/raygui) - For GUI components.
- [libcurl](https://curl.se/libcurl/) - For network operations.

- https://raylibtech.itch.io/rguilayout
    - raygui design tool