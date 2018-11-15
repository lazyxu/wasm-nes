//
// Created by meteor on 2018/11/8.
//

#ifndef WASM_NES_PPU_H
#define WASM_NES_PPU_H

#include "port.h"

typedef struct {
    void *nes;
} ppu_t;

uint8_t ppu_read_register(void *nes, uint16_t addr);
void ppu_write_register(void *nes, uint16_t addr, uint8_t val);

#endif // WASM_NES_PPU_H
