//
// Created by meteor on 2018/11/5.
//

#ifndef WASM_NES_NES_H
#define WASM_NES_NES_H

#include "port.h"
#include "ines.h"

ines_t ines = {0};

// int32_t nes_init();
int32_t nes_load(uint8_t *data_p, uint32_t data_len);
void nes_free();
// int32_t nes_reset();

#endif //WASM_NES_NES_H
