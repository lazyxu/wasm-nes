//
// Created by meteor on 2018/11/5.
//

#ifndef WASM_NES_MIRROR_H
#define WASM_NES_MIRROR_H

typedef enum { MIRROR_HORIZONTAL, MIRROR_VERTICAL, MIRROR_SINGLE0, MIRROR_SINGLE1, MIRROR_FOUR } mirror_t;

#ifdef DEBUG
const char * const get_mirror_name(mirror_t mirror);
#endif // DEBUG

#endif // WASM_NES_MIRROR_H
