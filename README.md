CSC 581 Homework 3
====
This assignment is to add more functionality to the game engine, including object model and networking designs.

Prerequisites
----
* Ubuntu 20.04
    + Desktop system installed
    + Additional packages
        - `x11-apps`
        - `build-essential`
        - `libsfml-dev`
        - `libzmq3-dev`
        - `nlohmann-json3-dev`

Getting Started
----
1. In the root directory of this project, run `make` in command line.
    * If succeed, four executables: `server`, `client`, `server_rr` and `client_rr` should appear in the directory.
2. run `./server` in any terminal and then run `./client` in a terminal which is opened from the Ubuntu desktop.
    * If succeed, a 800 x 600 window should appear and there will be a white square and several blue platforms.
    * Use left-right keys to move and space key to jump
    * The goal is to move the square to the red platform
3. run `./server_rr` and `./client_rr` instead to run the request-reply version of the game