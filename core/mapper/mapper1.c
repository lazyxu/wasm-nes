//
// Created by meteor on 2018/11/8.
//

#include "mapper1.h"

void mapper_init(mmc_t *mmc) {
    DEBUG_MSG("mapper_1 init\n");
    mmc->prgRam = malloc(0x2000); // PRG RAM: 8 KB PRG RAM bank, (optional)
}

void mapper_free() {
    DEBUG_MSG("mapper_1 free\n");
}

uint8_t mapper_cpu_read(mmc_t *mmc, uint16_t addr) {
    TRACE_MSG("mapper_cpu_read: %x\n", addr);
    if (addr >= 0xc000) {
        TRACE_MSG("prg_rom offset: %u, %x\n", mmc->prg_rom_bank_c000, mmc->prg_rom_bank_c000 * 0x4000 + addr - 0xc000);
        return mmc->cart->prg_rom[mmc->prg_rom_bank_c000 * 0x4000 + addr - 0xc000];
    } else if (addr >= 0x8000) {
        TRACE_MSG("prg_rom offset: %u, %x\n", mmc->prg_rom_bank_8000, mmc->prg_rom_bank_8000 * 0x4000 + addr - 0x8000);
        return mmc->cart->prg_rom[mmc->prg_rom_bank_8000 * 0x4000 + addr - 0x8000];
    } else if (addr >= 0x6000) {
        return mmc->prgRam[addr - 0x6000];
    }
    ASSERT(false);
    return 0;
}

static uint8_t prg_mode, chr_mode;
static uint8_t prg_bank, chr_bank0, chr_bank1;

// PRG ROM bank mode (0, 1: switch 32 KB at $8000, ignoring low bit of bank number;
//                    2: fix first bank at $8000 and switch 16 KB bank at $C000;
//                    3: fix last bank at $C000 and switch 16 KB bank at $8000)
// CHR ROM bank mode (0: switch 8 KB at a time; 1: switch two separate 4 KB banks)
static void updateOffsets(mmc_t *mmc) {
    switch (prg_mode) {
    case 0:
    case 1:
        mmc->prg_rom_bank_8000 = prg_bank & 0xFE;
        mmc->prg_rom_bank_c000 = prg_bank | 0x01;
        break;
    case 2:
        mmc->prg_rom_bank_8000 = 0;
        mmc->prg_rom_bank_c000 = prg_bank;
        break;
    case 3:
        mmc->prg_rom_bank_8000 = prg_bank;
        mmc->prg_rom_bank_c000 = mmc->cart->num_prg_rom_bank - 1;
        break;
    }
    switch (chr_mode) {
    case 0:
        mmc->chr_rom_bank_0000 = chr_bank0 & 0xFE;
        mmc->chr_rom_bank_1000 = chr_bank0 | 0x01;
        break;
    case 1:
        mmc->chr_rom_bank_0000 = chr_bank0;
        mmc->chr_rom_bank_1000 = chr_bank1;
        break;
    }
}

static uint8_t control;

// Control (internal, $8000-$9FFF)
static void writeControl(mmc_t *mmc, uint8_t val) {
    control = val;
    chr_mode = (val >> 4) & 1;
    prg_mode = (val >> 2) & 3;
    mirror_t mirror = val & 3;
    switch (mirror) {
    case MIRROR_HORIZONTAL: break;
    }
    updateOffsets(mmc);
}

// Control (internal, $8000-$9FFF)
static void writeRegister(mmc_t *mmc, uint16_t addr, uint8_t val) {
    // PRG bank (internal, $E000-$FFFF)
    if (addr >= 0xE000) {
        prg_bank = val & 0x0F;
    }
    // CHR bank 1 (internal, $C000-$DFFF)
    if (addr >= 0xC000) {
        chr_bank1 = val & 0x0F;
    }
    // CHR bank 0 (internal, $A000-$BFFF)
    if (addr >= 0xA000) {
        chr_bank0 = val & 0x0F;
    }
    updateOffsets(mmc);
}

static uint8_t shift_register;
void mapper_cpu_write(mmc_t *mmc, uint16_t addr, uint8_t val) {
    if (addr >= 0x8000) {
        if (val & 0x80 == 0x80) {
            shift_register = 0x10;
            writeControl(mmc, control | 0x0C);
        } else {
            bool complete = (shift_register & 1) == 1;
            shift_register >>= 1;
            shift_register |= (val & 1) << 4;
            if (complete) {
                writeRegister(mmc, addr, shift_register);
                shift_register = 0x10;
            }
        }
        return;
    } else if (addr >= 0x6000) {
        mmc->prgRam[addr - 0x6000] = val;
        return;
    }
    ASSERT(false);
}

uint8_t mapper_ppu_read(mmc_t *mmc, uint16_t addr) {
    if (addr < 0x1000) {
        return mmc->cart->chr_rom[mmc->chr_rom_bank_0000 * 0x1000 + addr];
    } else if (addr < 0x2000) {
        return mmc->cart->chr_rom[mmc->chr_rom_bank_1000 * 0x1000 + addr - 0x1000];
    }
    ASSERT(false);
    return 0;
}

void mapper_ppu_write(mmc_t *mmc, uint16_t addr, uint8_t val) {
    if (addr < 0x1000) {
        mmc->cart->chr_rom[mmc->chr_rom_bank_0000 * 0x1000 + addr] = val;
        return;
    } else if (addr < 0x2000) {
        mmc->cart->chr_rom[mmc->chr_rom_bank_1000 * 0x1000 + addr - 0x1000] = val;
        return;
    }
    ASSERT(false);
}