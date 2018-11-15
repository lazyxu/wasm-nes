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

### Build and run
you can execute the scripts(`tools/run_xxx.sh`) to build and run in one click.

#### NATIVE executable file
```sh
bash ./tools/run_native.sh
```

#### WASM and JavaScript "glue" code
```sh
bash ./tools/run_wasm.sh
```

open http://localhost:1111/.

## Examples
see [targets/wasm/example/static/index.html](targets/wasm/example/static/index.html)

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