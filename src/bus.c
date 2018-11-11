//
// Created by meteor on 2018/11/11.
//

#include "bus.h"
#include "nes.h"

uint8_t cpu_read(void *_nes, uint16_t addr) {
    nes_t *nes = (nes_t *)_nes;
    if (addr < 0x2000) {
        return nes->cpu->ram[addr & 0x7ff];
    } else if (addr < 0x4000) {
        return ppu_read_register(nes->ppu, 0x2000 + ((addr - 0x2000) & 7));
    } else if (addr < 0x4020) {
        switch (addr) {
        case 0x4014:
            return ppu_read_register(nes->ppu, addr);
        case 0x4015:
            return apu_read_register(nes->apu, addr);
        case 0x4016:
            return controller_read(&nes->controller[0]);
        case 0x4017:
            return controller_read(&nes->controller[1]);
        default:
            DEBUG_MSG("unhandled I/O Registers II read at addr: %X", addr);
            return 0;
        }
    } else if (addr < 0x6000) {
        DEBUG_MSG("TODO: I/O registers");
        return 0;
    } else if (addr >= 0x6000) {
        return nes->mmc->mapper->mapper_cpu_read(addr);
    }
    ASSERT(false);
}

void cpu_write(void *_nes, uint16_t addr, uint8_t val) {
    nes_t *nes = (nes_t *)_nes;
    if (addr < 0x2000) {
        nes->cpu->ram[addr & 0x7ff] = val;
    } else if (addr < 0x4000) {
        ppu_write_register(nes->ppu, 0x2000 + ((addr - 0x2000) & 7), val);
    } else if (addr < 0x4020) {
        switch (addr) {
        case 0x4014:
            ppu_write_register(nes->ppu, addr, val);
        case 0x4015:
            apu_write_register(nes->apu, addr, val);
        case 0x4016:
            controller_write(&nes->controller[0], val);
        case 0x4017:
            controller_write(&nes->controller[1], val);
        default:
            DEBUG_MSG("unhandled I/O Registers II write at addr: %X", addr);
        }
    } else if (addr < 0x6000) {
        DEBUG_MSG("TODO: I/O registers");
    } else if (addr >= 0x6000) {
        nes->mmc->mapper->mapper_cpu_write(addr, val);
    }
}