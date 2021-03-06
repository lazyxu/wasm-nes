//
// Created by meteor on 2018/11/8.
//

#include "mmc.h"

extern mapper_t m0;
extern mapper_t m1;
extern mapper_t m2;

// mapper list
static mapper_t *g_mapper_list[256] = {
    &m0,
    &m1,
    &m2,
};

void mmc_init(mmc_t *mmc, cartridge_t *cart, cpu_t *cpu, ppu_t *ppu) {
    DEBUG_MSG("mmc_init\n");
    ASSERT(mmc != NULL);
    mmc->cpu = cpu;
    mmc->ppu = ppu;
    mmc->cart = cart;
    mmc->mapper = g_mapper_list[cart->mapper_no];

    ASSERT(mmc->mapper != NULL);
    TRACE_MSG("mmc->mapper: %u\n", (uint8_t)mmc->mapper);

    ASSERT(mmc->mapper->mapper_init != NULL);
    TRACE_MSG("mmc->mapper->mapper_init: %u\n", (uint8_t)mmc->mapper->mapper_init);

    mmc->prg_rom_bank_c000 = mmc->cart->num_prg_rom_bank - 1;
    mmc->prg_rom_bank_8000 = 0;
    mmc->chr_rom_bank_1000 = 0;
    mmc->chr_rom_bank_0000 = 0;
    mmc->mapper->mapper_init(mmc);

    DEBUG_MSG("mapper_init done\n");

    DEBUG_MSG("mmc_init done\n");
}

void mmc_free(mmc_t *mmc) {
    mmc->mapper->mapper_free();
}