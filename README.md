# Chess

This is a simple two player chess game written in c++ and using SFML 3.0 for the graphics library.

### Current roadmap (not in order): 
- add proper pawn promotion menu
- add pause menu with forfiet, draw, and quit buttons
- add audio
- add cmake to compile and build code
- improve code for readibility
- improve the look of the menus
- rewrite some code for more efficient approach

To compile and run, first make sure SFML is installed, on mac compile/build with:
```bash
clang++ src/*.cpp -I/opt/homebrew/include -L/opt/homebrew/lib -lsfml-graphics -lsfml-window -lsfml-system -std=c++17 -o main
```
Then to run it:
```bash
./main
```