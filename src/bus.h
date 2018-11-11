//
// Created by meteor on 2018/11/11.
//

#ifndef WASM_NES_BUS_H
#define WASM_NES_BUS_H

#include "controller.h"
#include "cpu.h"
#include "port.h"
#include "ppu.h"
uint8_t cpu_read(void *_nes, uint16_t addr);
void cpu_write(void *_nes, uint16_t addr, uint8_t val);

#endif // WASM_NES_BUS_H
