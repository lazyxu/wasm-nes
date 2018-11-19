//
// Created by meteor on 2018/11/8.
//

#ifndef WASM_NES_MMC_H
#define WASM_NES_MMC_H

#include "cartridge.h"
#include "cpu.h"
#include "port.h"
#include "ppu.h"

typedef struct _mapper_t mapper_t;

// Memory management controller
typedef struct {
    cpu_t *cpu;
    ppu_t *ppu;
    cartridge_t *cart;
    uint8_t prg_rom_bank_8000;
    uint8_t prg_rom_bank_c000;
    uint8_t chr_rom_bank_0000;
    uint8_t chr_rom_bank_1000;
    uint8_t *prgRam;
    mapper_t *mapper;
} mmc_t;

struct _mapper_t {
    void (*mapper_init)();
    void (*mapper_free)();
    uint8_t (*mapper_cpu_read)(mmc_t *mmc, uint16_t addr);
    void (*mapper_cpu_write)(mmc_t *mmc, uint16_t addr, uint8_t val);
    uint8_t (*mapper_ppu_read)(mmc_t *mmc, uint16_t addr);
    void (*mapper_ppu_write)(mmc_t *mmc, uint16_t addr, uint8_t val);
};

void mmc_init(mmc_t *mmc, cartridge_t *cart, cpu_t *cpu, ppu_t *ppu);
void mmc_free(mmc_t *mmc);

static void mapper_init(mmc_t *mmc);
static void mapper_free();
static uint8_t mapper_cpu_read(mmc_t *mmc, uint16_t addr);
static void mapper_cpu_write(mmc_t *mmc, uint16_t addr, uint8_t val);
static uint8_t mapper_ppu_read(mmc_t *mmc, uint16_t addr);
static void mapper_ppu_write(mmc_t *mmc, uint16_t addr, uint8_t val);

#endif // WASM_NES_MMC_H
