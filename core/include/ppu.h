//
// Created by meteor on 2018/11/8.
//

#ifndef WASM_NES_PPU_H
#define WASM_NES_PPU_H

#include "port.h"

typedef struct {
    struct _nes_t *nes;
    uint16_t cycle;    // 0-340
    uint16_t scanline; // 0-261, 0-239=visible, 240=post, 241-260=vblank, 261=pre
    uint32_t frame;    // frame counter

    uint8_t palette_index[0x20]; // 32
    uint8_t name_table[0x800];   // 1024
    uint8_t oam[0x100];          // 256

    uint16_t v; // current vram address (15 bit)
    uint16_t t; // temporary vram address (15 bit); can also be thought of as the address of the top left onscreen tile.
    uint8_t x;  // fine x scroll (3 bit)
    bool write_toggle;   // write toggle (1 bit)
    bool odd_frame_flag; // even/odd frame flag (1 bit), true if odd

    uint8_t regs[8];

    // sprite temporary variables
    uint8_t sprite_count;
    uint32_t sprite_patterns[8];
    uint8_t sprite_positions[8];
    uint8_t sprite_priorities[8];
    uint8_t sprite_indexes[8];

    uint8_t buffered_data;
    uint8_t shift_register8;
    uint16_t shift_register16;

    uint8_t name_table_byte;
    uint8_t attribute;
    uint8_t low_tile_byte;
    uint8_t high_tile_byte;

} ppu_t;

uint8_t ppu_read_register(ppu_t *ppu, uint16_t addr);
void ppu_write_register(ppu_t *ppu, uint16_t addr, uint8_t val);
bool ppu_nmi(ppu_t *ppu);
void set_screen(uint32_t address);

void ppu_reset(ppu_t *ppu);
void ppu_step(ppu_t *ppu);
#endif // WASM_NES_PPU_H
