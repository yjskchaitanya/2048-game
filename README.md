# 2048_Game
Download the repo and save it as a directory.
In the linux CLI,
```
   sudo apt install libsdl2-dev
   sudo apt install libsdl2-ttf-dev
   sudo apt install pkg-config
   gcc -o 2048 2048.c $(pkg-config --cflags --libs sdl2 SDL2_ttf)
   ./2048
```
