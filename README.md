# Chess

This is a simple two player chess game written in c++ and using SFML 3.0 for the graphics library.

### Currently it is mosty functional, the only things missing are:
- En Passant
- Threefold repitition stalemate
- 50 move rule stalemate
- insuficient material stalemate

### Current roadmap: 
- add en passant
- add different stalemates
- add settings menu
- improve code for readibility
- improve the look of the menus
- rewrite code for more efficient approach
- add other features such as sound and piece highlighting

To compile and run, first make sure SFML is installed, on mac run:
```
clang++ src/*.cpp -I/opt/homebrew/include -L/opt/homebrew/lib -lsfml-graphics -lsfml-window -lsfml-system -std=c++17 -o main
```
Then to run it:
```
./main
```