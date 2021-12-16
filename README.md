# sdl-test
Playing with [SDL](https://www.libsdl.org/).

---------------------------------------------------

## Building

The current build configuration is intended to target Windows only (for now).
```
git clone https://github.com/deforde/sdl-test
cd sdl_test && mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --target sdl_test
```

Expects environment variables: `SDL_INCLUDE` and `SDL_LIB` to be defined, and point to the include and lib directories for SDL respectively.
The produced executable will need access to `SDL2.dll` in order to run as well.

---------------------------------------------------

### Dependencies

- [SDL Development Libs](https://www.libsdl.org/release/SDL2-devel-2.0.18-VC.zip) (for building from source)
- [SDL Runtime Binaries](https://www.libsdl.org/release/SDL2-2.0.18-win32-x64.zip) (for distribution of pre-compiled binary)
