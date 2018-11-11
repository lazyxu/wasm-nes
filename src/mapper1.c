//
// Created by meteor on 2018/11/8.
//

#include "mapper1.h"

void mapper_init(mmc_t *mmc) { DEBUG_MSG("mapper_1 init\n"); }

void mapper_free() { DEBUG_MSG("mapper_1 free\n"); }

uint8_t mapper_cpu_read(mmc_t *mmc, uint16_t addr) { return 0; }

void mapper_cpu_write(mmc_t *mmc, uint16_t addr, uint8_t val) {}

uint8_t mapper_ppu_read(mmc_t *mmc, uint16_t addr) { return 0; }

void mapper_ppu_write(mmc_t *mmc, uint16_t addr, uint8_t val) {}
