//
// Created by meteor on 2018/11/8.
//

#include "mmc.h"

void mmc_init(mmc_t *mmc, cartridge_t *cart, cpu_t *cpu, ppu_t *ppu)
{
    mmc->cpu = cpu;
    mmc->ppu = ppu;
    mmc->cart = cart;
    mmc->mapper = g_mapper_list[cart->mapper_no];
    mmc->mapper->init();
}

void mmc_free(mmc_t *mmc)
{
    mmc->mapper->free();
}