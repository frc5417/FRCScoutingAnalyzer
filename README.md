# FRCScoutingAnalyzer
A Desktop Application that pairs with frc5417/FRCScoutingApp to make a comprehensive scouting system.

## Configurable
This app is completely configurable for your team. Simply change [assets/databreakdown](https://github.com/frc5417/FRCScoutingAnalyzer/blob/main/assets/databreakdown) to match your FRCScoutingApp's DataModel.

## How to Run
1. Clone the repository | Optional: Delete `*.cpp, *.h, qcustomplot/` (If you are not developing)
### Linux (Debian):
2. Run Once: `sudo apt install qtbase5-dev`

3. Run: `./ScoutingAnalyzer` in the `build/` folder.

(More OS's to Come)

## Data Transfer
Currently the system requires you to have a physical QR Code Scanner, but in the future, we will be adding camera support.

## Features
### Data Summary
Provides a data summary based on all recorded matches for a team.

### Custom Sort
Allows for custom sorting based on user specifications (`assets/sortoptions`)

### Custom Graphs
Allows the user to visualize their data in graphs with customizable y-axis data and range (`assets/plotoptions`).

## Filter Data
* Filter out the Worst # of Matches the Teams (incase of breakdowns)
* Filter out matches recorded before # (only recent data)