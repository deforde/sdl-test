# sdl-test
Playing with [SDL](https://www.libsdl.org/).

---------------------------------------------------

## Building and Running

The current build configuration is intended to target Linux only (for now).
```
git clone https://github.com/deforde/sdl-test
cd sdl-test && mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --target sdl-test
```

---------------------------------------------------

### Dependencies

- [SDL2](https://www.libsdl.org)
- [SDL2 Image](https://www.libsdl.org/projects/SDL_image)


```
apt install libsdl2-dev libsdl2-image-dev
```
