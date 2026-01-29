# MyFirstGame
MyFirstGame is a fast paced multiplayer only pvp shooter

While jumping around and shooting is definitely possiple solo, actual gameplay requires a second person for fun or a second client.
# Requirements
## general
- cmake v3.28 or higher
- c++17 compiler
- window libraries(more details in the linux section)
## Windows
- Microsoft C++ Build Tools
## Linux
- cmake, gcc, g++

Additional packages(as used in the ci on debian trixie):
- libx11-dev libxrandr-dev libxcursor-dev libxinerama-dev libxi-dev
- libglvnd-dev libglu1-mesa-dev libglx-dev
- libudev-dev
# Compilation
1. `cmake -B build`
2. `cd build && cmake --build .`

# Execution
make sure to change your current working directory to `<repo_path>/build`. \
There are several options available to execute:
- Main: starts up the main menu, recommended
- Server [port]: server only
- Client [target] [port]: client only
- LocalGame: start server and connect to it locally

# Responsibilities

While having seperation in responsibilities, 
a lot of debugging and minor bug improvements have been done in cooperation.

## (0,5) Main menu - Alan Gallo
- Host game
- Join server
- Exit
## (0,5) Audio - Alan Gallo
- Background music
- ~~Sound effects~~
## (2) Movement - Alan Gallo
- Running left / right
- Jumping
- ~~Special move of some sort, potentially: (dashing, wall-jump,  double-jump, etc.)~~
## (2) Stages - Alan Gallo
- Design different stages
- ~~Moving parts of stages~~
## (1) Voting for Stages - Alan Gallo
- Players vote for stage before game
## (3) Multiplayer - Fabian Pfaff
- Ensure players can host and join game
- Ensure Game State Synchronization
## (2) Main combat - Fabian Pfaff
- Real time combat
- Attack, ~~Block~~