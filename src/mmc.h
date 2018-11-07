//
// Created by meteor on 2018/11/8.
//

#ifndef WASM_NES_MMC_H
#define WASM_NES_MMC_H

#include "port.h"
#include "mapper.h"
#include "cartridge.h"
#include "cpu.h"
#include "ppu.h"

// Memory management controller
typedef struct
{
    cpu_t *cpu;
    ppu_t *ppu;
    cartridge_t *cart;
    uint8_t prg_bank_size;
    uint8_t prg_bank_8000;
    uint8_t prg_bank_c000;
    uint8_t chr_bank_size;
    uint8_t chr_bank_0000;
    uint8_t chr_bank_1000;
    mapper_t *mapper;
} mmc_t;

void mmc_init(mmc_t *mmc, cartridge_t *cart, cpu_t *cpu, ppu_t *ppu);
void mmc_free(mmc_t *mmc);

#endif //WASM_NES_MMC_H
