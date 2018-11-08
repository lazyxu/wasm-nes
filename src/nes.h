//
// Created by meteor on 2018/11/5.
//

#ifndef WASM_NES_NES_H
#define WASM_NES_NES_H

#include "port.h"
#include "cartridge.h"
#include "mmc.h"
#include "cpu.h"
#include "ppu.h"

typedef struct
{
    cartridge_t *cart;
    mmc_t *mmc;
    cpu_t *cpu;
    ppu_t *ppu;
} nes_t;

nes_t g_nes;

// int32_t nes_init();
int32_t nes_load(uint8_t *data, uint32_t data_len);
void nes_free();
// int32_t nes_reset();

#endif //WASM_NES_NES_H
