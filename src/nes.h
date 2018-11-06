//
// Created by meteor on 2018/11/5.
//

#ifndef WASM_NES_NES_H
#define WASM_NES_NES_H

#include "port.h"
#include "ines.h"

ines_t ines;

int32_t load(uint8_t *data_p, uint32_t data_len);

#endif //WASM_NES_NES_H
