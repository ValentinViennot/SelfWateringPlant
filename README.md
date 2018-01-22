# SelfWateringPlant
A small Arduino program to automatically water a plant and maintain a humidity threshold.

## Usage
### Configuration mode
If the red light is blinking you're in configuration mode.
1. Press button to memorize low water level.
2. Watering process starts.
3. When the plant has enough water, press button to memorize "enough water level"
### Sleep and control water level
Every 10 seconds, water level will be checked and if under low level it will start watering process until water level is over "enough water" memorized state.
Any time in this mode you can press config button (for at most 10s) to enter config mode.
