//
// Created by meteor on 2018/11/5.
//

#ifndef WASM_NES_MIRROR_H
#define WASM_NES_MIRROR_H

typedef enum {
    HORIZONTAL_MIRRORING,
    VERTICAL_MIRRORING,
    FOURSCREEN_MIRRORING,
    INVALID_MIRROTING
} mirror_t;

mirror_t mirror = INVALID_MIRROTING;

#endif // WASM_NES_MIRROR_H
