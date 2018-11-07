//
// Created by meteor on 2018/11/8.
//

#ifndef WASM_NES_MAPPER1_H
#define WASM_NES_MAPPER1_H

#include "port.h"

void init1();

void free1();

uint8_t read1(uint16_t addr);

void write1(uint16_t addr, uint8_t value);

#endif //WASM_NES_MAPPER1_H
