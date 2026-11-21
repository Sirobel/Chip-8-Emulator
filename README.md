# Chip-8-Emulator

A Chip-8 Emulator written in C++ with SDL3. This project was mostly for learning how to emulate.

## Features

- All 35 original CHIP-8 opcodes
- 64x32 display, scaled to a resizable window
- Square-wave beep driven by th sound timer
- Native file dialog for loading ROMs
- COSMAC VIP quirks

## Building

### Requirements
- C++17 compiler
- CMAKE 3.5 or newer
- SDL3 (Development package)

### Building the Emulator
```bash
git clone https://github.com/Sirobel/Chip-8-Emulator.git
cd Chip-8-Emulator
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Controls

| CHIP-8 | Key | CHIP-8 | Key | CHIP-8 | Key | CHIP-8 | Key |
|--------|-----|--------|-----|--------|-----|--------|-----|
| 1 | KP 7 | 2 | KP 8 | 3 | KP 9 | C | KP / |
| 4 | KP 4 | 5 | KP 5 | 6 | KP 6 | D | KP * |
| 7 | KP 1 | 8 | KP 2 | 9 | KP 3 | E | KP - |
| A | KP 0 | 0 | KP . | B | KP + | F | KP Enter |

|Key|Action|
|---|------|
|'E'| Pause/resume|
|'ESC'| Quit|

If you want to change the Keybinds for the CHIP-8 Keyboard look at *mapKey* in [main.c](src/main.cpp)

## Resources
- [Timendus CHIP-8 test suite](https://github.com/Timendus/chip8-test-suite)
- [Cowgod's CHIP-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)


## License
[MIT](LICENSE)

## Acknowledgements

Built with [SDL3](https://libsdl.org), licensed under the zlib license.