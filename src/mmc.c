//
// Created by meteor on 2018/11/8.
//

#include "mmc.h"

extern mapper_t m0;
extern mapper_t m1;
extern mapper_t m2;

// mapper list
static mapper_t *g_mapper_list[256] =
    {
        NULL,
        &m1,
        &m2,
};

void mmc_init(mmc_t *mmc, cartridge_t *cart, cpu_t *cpu, ppu_t *ppu)
{
    mmc->cpu = cpu;
    mmc->ppu = ppu;
    mmc->cart = cart;
    mmc->mapper = g_mapper_list[cart->mapper_no];
    LOG("using mapper: %u\n", cart->mapper_no);
    ASSERT(mmc->mapper != NULL);
    LOG("mmc->mapper: %u\n", (uint8_t)mmc->mapper);
    ASSERT(mmc->mapper->mapper_init != NULL);
    LOG("mmc->mapper->mapper_init: %u\n", (uint8_t)mmc->mapper->mapper_init);
    mmc->mapper->mapper_init(mmc);
    LOG("mapper_init done\n");
}

void mmc_free(mmc_t *mmc)
{
    mmc->mapper->mapper_free();
}