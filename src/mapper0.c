//
// Created by meteor on 2018/11/8.
//

#include "mapper0.h"

void mapper_init(mmc_t *mmc)
{
    DEBUG_MSG("mapper_0 init\n");
    // mmc->prg_rom_bank_size = mmc->cart->num_prg_rom_bank;
    // mmc->prg_rom_bank_c000 = mmc->prg_rom_bank_size - 1;
    // mmc->prg_rom_bank_8000 = 0;
    // mmc->chr_rom_bank_1000 = 0;
    // mmc->chr_rom_bank_0000 = 0;
}

void mapper_free()
{
    DEBUG_MSG("mapper_0 free\n");
}

uint8_t cpu_read(uint16_t addr)
{
    return 0;
}

void cpu_write(uint16_t addr, uint8_t value)
{
}

uint8_t ppu_read(uint16_t addr)
{
    return 0;
}

void ppu_write(uint16_t addr, uint8_t value)
{
}