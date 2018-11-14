# wasm-nes
wasm-nes is a NES emulator written in C that runs in the browser with WebAssembly.

## Quick Start

### Setup Emscripten Environment
Get the Emscripten SDK, using these instructions: https://kripken.github.io/emscripten-site/docs/getting_started/downloads.html

It is recommended to put the `emsdk` folder in the `HOME` directory.

### Getting the sources
```bash
git clone https://github.com/MeteorKL/wasm-nes
cd wasm-nes
```

### Create build directory and active environment variables
```sh
mkdir -p build && cd build
source ~/emsdk/emsdk_env.sh
```

### Build wasm-nes
you can execute the script(`run.sh`) to compile and run in one click.

#### Compiling to a NATIVE executable file
```sh
cmake .. && make
./nes
```

#### Compiling to wasm and just creating the JavaScript

Build RELEASE version:
```sh
cmake -DCMAKE_TOOLCHAIN_FILE=~/emsdk/emscripten/1.38.16/cmake/Modules/Platform/Emscripten.cmake .. && make
```

Build DEBUG version:
```sh
cmake -DCMAKE_TOOLCHAIN_FILE=~/emsdk/emscripten/1.38.16/cmake/Modules/Platform/Emscripten.cmake -DVERSION=DEBUG .. && make
```

Build TRACE version:
```sh
cmake -DCMAKE_TOOLCHAIN_FILE=~/emsdk/emscripten/1.38.16/cmake/Modules/Platform/Emscripten.cmake -DVERSION=TRACE .. && make
```

## Examples
see [example/static/index.html](example/static/index.html)

## Docs
- [Compiling a New C/C++ Module to WebAssembly](https://developer.mozilla.org/en-US/docs/WebAssembly/C_to_wasm)
- [Nesdev#6502](http://wiki.nesdev.com/w/index.php/Nesdev#6502)
- [Everynes - Nocash NES Specs](http://problemkaputt.de/everynes.htm)
- [envelope](https://www.zhihu.com/question/37731612/answer/73399028)

## Emulators
- [another nes emulator written in JS](https://github.com/MeteorKL/nes)
- [fogleman's nes](https://github.com/fogleman/nes)
- [rockcarry's ffnes](https://github.com/rockcarry/ffnes)
- [bfirsh's jsnes](https://github.com/bfirsh/jsnes)