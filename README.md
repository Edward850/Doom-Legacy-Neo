# Doom Legacy Neo

A modern branch/continuation of the Doom Legacy source port.

---

## Downloading

You don't need to do compile/build the project if you are an average user, simply go to the releases section in the github page and grab the latest preferred release for your platform. Extract these files to a location on your computer (i.e C:/Games/Doom/Doom Legacy Neo/)
For now, you must provide the relevant game files yourself (i.e doom.wad or doom2.wad) from your installed copy of Doom and place them in the directory along side the program itself.

## Building on Windows (CMake + vcpkg)

This guide covers building Doom Legacy Neo on Windows (64-bit) using Visual Studio, CMake, and Microsoft's `vcpkg` package manager. 

### 1. Prerequisites

Ensure you have the following installed:

* **Visual Studio 2022** (or Visual Studio 2019)
  * Make sure the **"Desktop development with C++"** workload is selected in the Visual Studio Installer.
* **Git for Windows** ([Download Git](https://git-scm.com/download/win))
* **CMake** (v3.25 or higher) – included with Visual Studio or available at [cmake.org](https://cmake.org/download/).

---

### 2. Set Up `vcpkg`

If you do not already have `vcpkg` installed, open a Command Prompt or PowerShell and clone it to a location such as `C:\vcpkg`:

```cmd
git clone [https://github.com/microsoft/vcpkg.git](https://github.com/microsoft/vcpkg.git) C:\vcpkg
cd C:\vcpkg
.\bootstrap-vcpkg.bat
```

### 3. Construct project files while targeting vcpkg

When configuring cmake, you need to point to the vcpkg toolchain for any packages to be found (otherwise you have to configure their locations manually, which can be very tedious and lead to versioning conflicts). The easiest way to do this is actually from the command line, like so:
```cmd
cmake -B build -S . -A x64 -DCMAKE_TOOLCHAIN_FILE="C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
```

### 4. Building the project

You can either build the project from the command line, or use use Visual Studio directly.
From the command line, run
```cmd
cmake --build build --config Release
```

Otherwise simply open the DoomLegacyNeo.sln/DoomLegacyNeo.slnx file from the target build directory generated from cmake and compile/run.

The necessary libraries and program assets are automatically copied as part of the build process.
