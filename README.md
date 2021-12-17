# sdl-test
Playing with [SDL](https://www.libsdl.org/).

---------------------------------------------------

## Building and Running

The current build configuration is intended to target Windows only (for now).
```
git clone https://github.com/deforde/sdl-test
cd sdl-test
mkdir build
cd build
cmake ..
cmake --build . --config Release --target ALL_BUILD
```

Expects the following environment variables to be defined:
- `SDL_INCLUDE`: The path to the SDL2 include directory.
- `SDL_LIB`: The path to the SDL2 lib directory.
- `SDL_IMAGE_INCLUDE`: The path to the SDL Image extension include directory.
- `SDL_LIB`: The path to the SDL Image extension lib directory.

The executable will require access to the various `SDL2` and `SDL2_image` dlls at runtime.

---------------------------------------------------

### Dependencies

- [SDL Development Libs](https://www.libsdl.org/release/SDL2-devel-2.0.18-VC.zip) (for building from source)
- [SDL Runtime Binaries](https://www.libsdl.org/release/SDL2-2.0.18-win32-x64.zip) (for distribution of pre-compiled binary)
- [SDL Image Development Libs](https://www.libsdl.org/projects/SDL_image/release/SDL2_image-devel-2.0.5-VC.zip) (for building from source)
- [SDL Image Runtime Binaries](https://www.libsdl.org/projects/SDL_image/release/SDL2_image-2.0.5-win32-x64.zip) (for distribution of pre-compiled binary)
