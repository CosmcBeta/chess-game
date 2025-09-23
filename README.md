# Chess

This is a simple two player chess game written in c++ and using SFML 3.0 for the graphics library. While in game you can access the pause menu by pressing ESC.

## Current roadmap (not in order): 
- add proper pawn promotion menu
- improve code for readibility
- improve the look of the menus
- rewrite some code for more efficient approach

## Build and run

#### CMake
This is the prefered way of building
- Create and enter the build directory
```bash
mkdir build
cd build
```
- Generate build files and build the project with CMake
```bash
cmake ..
cmake --build .
```
- Run the program from the project root
```bash
cd ..
./build/bin/chess
```

If you would like to use g++/clang++ you would run this command:
```bash
clang++ src/*.cpp -I/opt/homebrew/include -L/opt/homebrew/lib -lsfml-graphics -lsfml-window -lsfml-system -std=c++17 -o chess
```
- Then you would execute the file with this command:
```bash
./chess
```


- If a change is made to the code rebuild and run with this command:
```bash
cmake --build build && ./build/bin/chess
```