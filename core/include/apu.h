//
// Created by meteor on 2018/11/8.
//

#ifndef WASM_NES_APU_H
#define WASM_NES_APU_H

#include "port.h"

typedef struct {
    void *nes;
} apu_t;

uint8_t apu_read_register(void *nes, uint16_t addr);
void apu_write_register(void *nes, uint16_t addr, uint8_t val);

void apu_step(apu_t *apu);
#endif // WASM_NES_APU_H
