//
// Created by meteor on 2018/11/8.
//

#include "apu.h"

static uint8_t status_4004 = 0xFF;
static uint8_t status_4005 = 0xFF;
static uint8_t status_4006 = 0xFF;
static uint8_t status_4007 = 0xFF;
static uint8_t status_4015 = 0xFF;

uint8_t apu_read_register(void *nes, uint16_t addr) {
    switch (addr) {
    case 0x4004: return status_4004;
    case 0x4005: return status_4005;
    case 0x4006: return status_4006;
    case 0x4007: return status_4007;
    case 0x4015: return status_4015;
    default: DEBUG_MSG("unhandled I/O Registers II apu read at addr: %X\n", addr); break;
    }
    ASSERT(false);
}

void apu_write_register(void *nes, uint16_t addr, uint8_t val) {
    switch (addr) {
    case 0x4004: status_4004 = val; break;
    case 0x4005: status_4005 = val; break;
    case 0x4006: status_4006 = val; break;
    case 0x4007: status_4007 = val; break;
    case 0x4015: status_4015 = val; break;
    default: DEBUG_MSG("unhandled I/O Registers II apu write at addr: %X <- %X\n", addr, val); break;
    }
}

void apu_step(apu_t *apu) {
}