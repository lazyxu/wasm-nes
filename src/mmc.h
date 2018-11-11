//
// Created by meteor on 2018/11/8.
//

#ifndef WASM_NES_MMC_H
#define WASM_NES_MMC_H

#include "cartridge.h"
#include "cpu.h"
#include "mmc.h"
#include "port.h"
#include "ppu.h"

typedef struct _mapper_t mapper_t;

// Memory management controller
typedef struct {
    cpu_t *cpu;
    ppu_t *ppu;
    cartridge_t *cart;
    uint8_t prg_rom_bank_size;
    uint8_t prg_rom_bank_8000;
    uint8_t prg_rom_bank_c000;
    uint8_t chr_rom_bank_size;
    uint8_t chr_rom_bank_0000;
    uint8_t chr_rom_bank_1000;
    mapper_t *mapper;
} mmc_t;

struct _mapper_t {
    void (*mapper_init)();
    void (*mapper_free)();
    uint8_t (*cpu_read)(uint16_t addr);
    void (*cpu_write)(uint16_t addr, uint8_t value);
    uint8_t (*ppu_read)(uint16_t addr);
    void (*ppu_write)(uint16_t addr, uint8_t value);
};

void mmc_init(mmc_t *mmc, cartridge_t *cart, cpu_t *cpu, ppu_t *ppu);
void mmc_free(mmc_t *mmc);

static void mapper_init(mmc_t *mmc);
static void mapper_free();
static uint8_t cpu_read(mmc_t *mmc, uint16_t addr);
static void cpu_write(mmc_t *mmc, uint16_t addr, uint8_t value);
static uint8_t ppu_read(mmc_t *mmc, uint16_t addr);
static void ppu_write(mmc_t *mmc, uint16_t addr, uint8_t value);

#endif // WASM_NES_MMC_H
