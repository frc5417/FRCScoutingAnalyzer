# FRCScoutingAnalyzer
A Desktop Application that pairs with frc5417/FRCScoutingApp to make a comprehensive scouting system.

## Configurable
This app is completely configurable for your team. Simply change your [databreakdown](https://github.com/frc5417/FRCScoutingAnalyzer/blob/main/build/databreakdown) file to match your FRCScoutingApp's DataModel.

## Data Transfer
Currently the system requires you to have a physical QR Code Scanner, but in the future, we will be adding camera support.

## How to Install/Run
1. Download the latest executable's zip that corresponds to your operating system from github releases (or generate your own - see below) with the support files.
2. Extract all files into a folder.
3. Run the executable (`./ScoutingAnalyzer-x86_64.AppImage` on Linux and `ScoutingAnalyzer.exe` on Windows)

## How to Build (Developer Mode)
1. Clone the repository
### Linux (Debian):
2. Run Once: `sudo apt install qtbase5-dev`
3. Run Once: `cmake ..` inside the `build/` folder.
3. Run: `make && ./src/./ScoutingAnalyzer` in the `build/` folder.
### Windows:
2. Once: Download QT Creator
3. Open in QT Creator (clicking on the .pro file)
4. Bottom left to run.

## How to Build (Release)
1. Clone the repository
### Linux (Debian):
2. Run Once: `sudo apt install qtbase5-dev`
3. `cd` into the `build/` directory
4. Once: Download [linuxdeploy/linuxdeploy](https://github.com/linuxdeploy/linuxdeploy/) and [linuxdeploy/linuxdeploy-plugin-qt](https://github.com/linuxdeploy/linuxdeploy-plugin-qt/) from their respective repositories on Github.
5. Run `qmake ..`, `make -j$(nproc)`, and `make install INSTALL_ROOT=AppDir`
6. Run `./linuxdeploy-x86_64.AppImage --appdir AppDir --plugin qt --output appimage`
### Windows:
2. Once: Download QT Creator for Qt5
3. Open in QT Creator (clicking on the .pro file)
4. Bottom left to build.
5. Open the QT CLI for your version (e.g., Qt 5.12.12 (MSVC 2015 64-bit))
6. Run `windeploy --quick --no-translations FILEPATH_TO_BUILT_EXE`

## Features
### Data Summary
Provides a data summary based on all recorded matches for a team.

### Custom Sort
Allows for custom sorting based on user specifications (`sortoptions`)

### Custom Graphs
Allows the user to visualize their data in graphs with customizable y-axis data and range (`plotoptions`).

## Filter Data
* Filter out the Worst # of Matches the Teams (incase of breakdowns)
* Filter out matches recorded before # (only recent data)

## License Info:
QCustomPlot (a dependency of this project) has a GPL-3.0 license.
