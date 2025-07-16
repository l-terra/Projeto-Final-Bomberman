# Bomberman in C

This is a Bomberman-style game developed as a final project for the Computer Programming II course at the Federal University of Rio de Janeiro (UFRJ). The game is built from scratch in C using the Raylib library for graphics, audio, and input handling.

The main objective of the game is to navigate through each level, using bombs to destroy obstacles and enemies, and find 5 hidden keys to unlock the exit to the next stage.

## Contributors

This project was developed by a team of students for the Computer Programming II course at the Federal University of Rio de Janeiro (UFRJ).

* Lucas Terra
* Caio Ogawa
* Davi Iecin
* Daniel Piccoli
* Leonardo Evangelista

## Core Features
* **Modular Game Engine:** Built with a central `GameState` struct to manage all data, adhering to the requirement of code structuration in modules and avoiding global variables.
* **Dynamic Map Loading:** Levels are loaded from simple `.txt` files (`mapa1.txt`, `mapa2.txt`, etc.) at runtime, with the map structure being dynamically allocated in memory.
* **Bomb Mechanics:** The player can plant bombs with a 3-second timer that explode in a cross-shaped area of approximately 100x100 pixels, destroying enemies, boxes, and destructible walls.
* **Intelligent Enemy AI:** Features enemies that actively pursue the player, an optional feature implemented for a greater challenge.
* **Save & Load System:** Game progress can be saved and loaded at any time via the menu. The save state is stored in a single binary file, as required.
* **Audio:** Includes sound effects for key game events, an optional feature to enhance gameplay.

## How to Compile and Run

### Prerequisites
* The **Raylib** library must be installed and configured in your environment.
* A C compiler (like **GCC**) and the **make** utility. For Windows, this is typically handled by the **MinGW** toolchain.

### File Structure
For the game to run correctly, the executable must be in the root directory along with the map files and the `assets` folder:

/project_directory/\
|-- executable (e.g., main or main.exe)\
|-- assets/\
|   |-- (all .mp3 and .wav sound files)\
|-- mapa1.txt\
|-- mapa2.txt\
|-- ...

### Compilation
The project is built using a standard Raylib `Makefile`. Open a terminal in the project's root directory and run the appropriate command for your system:

**On Linux/macOS (bash):**
```bash
make
```

**On Windows (with MinGW, PowerShell):**
```PowerShell
mingw32-make
```

### Execution
After a successful compilation, run the generated executable:

**On Linux/macOS (bash):**
```bash
./game
```

**On Windows (with MinGW, PowerShell):**
```PowerShell
./game
```

## Map Flexibility and Extensibility

A key design decision was to ensure the game engine was not restricted to a fixed number of maps. The game is designed to work with any number of maps placed in the executable's directory, loading and interpreting any level file that adheres to the defined specifications.

The level progression system automatically searches for the next map in the sequence. If the next map is not found, the game considers all levels to be completed.

For a custom map to be valid, it must follow these rules:

* **Naming Convention:** Map files must be named sequentially with increasing numbers, starting from 1 (e.g., `mapa1.txt`, `mapa2.txt`, `mapa3.txt`, and so on) for the level progression to work correctly.

* **Dimensions:** The text file must be exactly 25 lines high and 60 columns wide.

* **Content Requirements:**
    * The map must contain exactly 5 keys (represented by the character `'K'`).
    * The map must contain exactly 5 enemies (represented by the character `'E'`).

* **Valid Characters:** The characters used in the file must correspond to the game's specification, where each character maps to a specific game element:
    * `' '` (space): Empty tile
    * `'J'`: Player's starting position
    * `'W'`: Indestructible wall
    * `'D'`: Destructible wall
    * `'K'`: Box containing a key
    * `'B'`: Empty box (no key)
    * `'E'`: Enemy

This approach makes the game easily extensible. Anyone can create and add new levels simply by creating a new sequentially numbered `.txt` file that follows these rules and placing it in the executable's directory, with no need to recompile the source code.
