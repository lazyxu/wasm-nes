//
// Created by meteor on 2018/11/8.
//

#include "ppu.h"
#include "nes.h"
#include "port.h"

static uint32_t g_palette[] = {
    0xFF525252, 0xFFB40000, 0xFFA00000, 0xFFB1003D, 0xFF740069, 0xFF00005B, 0xFF00005F, 0xFF001840,
    0xFF002F10, 0xFF084A08, 0xFF006700, 0xFF124200, 0xFF6D2800, 0xFF000000, 0xFF000000, 0xFF000000,
    0xFFC4D5E7, 0xFFFF4000, 0xFFDC0E22, 0xFFFF476B, 0xFFD7009F, 0xFF680AD7, 0xFF0019BC, 0xFF0054B1,
    0xFF006A5B, 0xFF008C03, 0xFF00AB00, 0xFF2C8800, 0xFFA47200, 0xFF000000, 0xFF000000, 0xFF000000,
    0xFFF8F8F8, 0xFFFFAB3C, 0xFFFF7981, 0xFFFF5BC5, 0xFFFF48F2, 0xFFDF49FF, 0xFF476DFF, 0xFF00B4F7,
    0xFF00E0FF, 0xFF00E375, 0xFF03F42B, 0xFF78B82E, 0xFFE5E218, 0xFF787878, 0xFF000000, 0xFF000000,
    0xFFFFFFFF, 0xFFFFF2BE, 0xFFF8B8B8, 0xFFF8B8D8, 0xFFFFB6FF, 0xFFFFC3FF, 0xFFC7D1FF, 0xFF9ADAFF,
    0xFF88EDF8, 0xFF83FFDD, 0xFFB8F8B8, 0xFFF5F8AC, 0xFFFFFFB0, 0xFFF8D8F8, 0xFF000000, 0xFF000000};

#define CYCLE ppu->cycle
#define SCANLINE ppu->scanline
#define FRAME ppu->frame
#define OAM ppu->oam
#define ODD_FRAME_FLAG ppu->odd_frame_flag
#define SHIFT_REGISTER8 ppu->shift_register8
#define SHIFT_REGISTER16 ppu->shift_register16
/* ----------------------------------------------------- mirror ----------------------------------------------------- */
static mirror_t mirror_lookup[][4] = {
    {0, 0, 1, 1}, {0, 1, 0, 1}, {0, 0, 0, 0}, {1, 1, 1, 1}, {0, 1, 2, 3}}; // mirror_t table_index

uint16_t mirror_address(nes_t *nes, uint16_t addr) {
    addr &= 0x1FFF;
    return 0x2000 + (mirror_lookup[nes->cart->mirror][addr >> 10] << 10) + (addr & 0x3FF);
}

/* ----------------------------------------------------- memory ----------------------------------------------------- */

uint8_t ppu_read(ppu_t *ppu, uint16_t addr) {
    TRACE_MSG("ppu read: %x\n", addr);
    nes_t *nes = ppu->nes;
    ASSERT(nes != NULL);
    ASSERT(nes->ppu != NULL);
    if (addr < 0x2000) {
        ASSERT(nes->mmc != NULL);
        ASSERT(nes->mmc->mapper != NULL);
        ASSERT(nes->mmc->mapper->mapper_ppu_read != NULL);
        return nes->mmc->mapper->mapper_ppu_read(nes->mmc, addr);
    } else if (addr < 0x3F00) {
        return ppu->name_table[mirror_address(nes, addr)];
    } else if (addr < 0x4000) {
        return ppu->palette_index[addr & 0x31];
    }
    ASSERT(false);
    return 0;
}

void ppu_write(ppu_t *ppu, uint16_t addr, uint8_t val) {
    TRACE_MSG("ppu write: %x <- %x\n", addr, val);
    nes_t *nes = ppu->nes;
    ASSERT(nes != NULL);
    ASSERT(nes->ppu != NULL);
    if (addr < 0x2000) {
        ASSERT(nes->mmc != NULL);
        ASSERT(nes->mmc->mapper != NULL);
        ASSERT(nes->mmc->mapper->mapper_ppu_write != NULL);
        nes->mmc->mapper->mapper_ppu_write(nes->mmc, addr, val);
        return;
    } else if (addr < 0x3F00) {
        ppu->name_table[mirror_address(nes, addr)] = val;
        return;
    } else if (addr < 0x4000) {
        ppu->palette_index[addr & 0x31] = val;
        return;
    }
    ASSERT(false);
    return;
}
#define READ(addr) ppu_read(ppu, addr)
#define WRITE(addr, val) ppu_write(ppu, addr, val)

/* ---------------------------------------------------- register ---------------------------------------------------- */
#define CTRL_REG ppu->regs[0]
/**
 * 7  bit  0
 * ---- ----
 * VPHB SINN
 * |||| ||||
 * |||| ||++- Base nametable address
 * |||| ||    (0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
 * |||| |+--- VRAM address increment per CPU read/write of PPUDATA
 * |||| |     (0: add 1, going across; 1: add 32, going down)
 * |||| +---- Sprite pattern table address for 8x8 sprites
 * ||||       (0: $0000; 1: $1000; ignored in 8x16 mode)
 * |||+------ Background pattern table address (0: $0000; 1: $1000)
 * ||+------- Sprite size (0: 8x8; 1: 8x16)
 * |+-------- PPU master/slave select
 * |          (0: read backdrop from EXT pins; 1: output color on EXT pins)
 * +--------- Generate an NMI at the start of the vertical blanking interval (0: off; 1: on)
 */
#define FLAG_NAMT_TABLE (CTRL_REG & 0B11)
#define FLAG_INCREMENT ((CTRL_REG >> 2) & 1)
#define FLAG_SPRITE_TABLE ((CTRL_REG >> 3) & 1)
#define FLAG_BACKGROUND_TABLE ((CTRL_REG >> 4) & 1)
#define FLAG_SPRITE_SIZE ((CTRL_REG >> 5) & 1)
#define FLAG_MASTER_SLAVE ((CTRL_REG >> 6) & 1)
#define FLAG_NMI_OUTPUT ((CTRL_REG >> 7) & 1)

#define MASK_REG ppu->regs[1]
/**
 * 7  bit  0
 * ---- ----
 * BGRs bMmG
 * |||| ||||
 * |||| |||+- Greyscale (0: normal color, 1: produce a greyscale display)
 * |||| ||+-- 1: Show background in leftmost 8 pixels of screen, 0: Hide
 * |||| |+--- 1: Show sprites in leftmost 8 pixels of screen, 0: Hide
 * |||| +---- 1: Show background
 * |||+------ 1: Show sprites
 * ||+------- Emphasize red*
 * |+-------- Emphasize green*
 * +--------- Emphasize blue*
 */
#define FLAG_GRAY_SCALE (MASK_REG & 1)
#define FLAG_SHOW_LEFT_BACKGROUND ((MASK_REG >> 1) & 1)
#define FLAG_SHOW_LEFT_SPRITES ((MASK_REG >> 2) & 1)
#define FLAG_SHOW_BACKGROUND ((MASK_REG >> 3) & 1)
#define FLAG_SHOW_SPRITES ((MASK_REG >> 4) & 1)
#define FLAG_RED_EMPHASIZE ((MASK_REG >> 5) & 1)
#define FLAG_GREEN_EMPHASIZE ((MASK_REG >> 6) & 1)
#define FLAG_BLUE_EMPHASIZE ((MASK_REG >> 7) & 1)

#define RENDER_ENABLED (FLAG_SHOW_BACKGROUND || FLAG_SHOW_SPRITES)

#define STATUS_REG ppu->regs[2]
/**
 * 7  bit  0
 * ---- ----
 * VSO. ....
 * |||| ||||
 * |||+-++++- Least significant bits previously written into a PPU register
 * |||        (due to register not being updated for this address)
 * ||+------- Sprite overflow. The intent was for this flag to be set
 * ||         whenever more than eight sprites appear on a scanline, but a
 * ||         hardware bug causes the actual behavior to be more complicated
 * ||         and generate false positives as well as false negatives; see
 * ||         PPU sprite evaluation. This flag is set during sprite
 * ||         evaluation and cleared at dot 1 (the second dot) of the
 * ||         pre-render line.
 * |+-------- Sprite 0 Hit.  Set when a nonzero pixel of sprite 0 overlaps
 * |          a nonzero background pixel; cleared at dot 1 of the pre-render
 * |          line.  Used for raster timing.
 * +--------- Vertical blank has started (0: not in vblank; 1: in vblank).
 *            Set at dot 1 of line 241 (the line *after* the post-render
 *            line); cleared after reading $2002 and at dot 1 of the
 *            pre-render line.
 */
#define FLAG_SPRITE_OVERFLOW (MASK_REG >> 5) & 1
#define FLAG_SPRITE_ZERO_HIT (MASK_REG >> 6) & 1
#define FLAG_NMI_OCCURRED (MASK_REG >> 7) & 1

#define SET_FLAG_SPRITE_OVERFLOW()                                                                                     \
    do {                                                                                                               \
        MASK_REG |= (1 << 5);                                                                                          \
    } while (0)
#define SET_FLAG_SPRITE_ZERO_HIT()                                                                                     \
    do {                                                                                                               \
        MASK_REG |= (1 << 6);                                                                                          \
    } while (0)
#define SET_FLAG_NMI_OCCURRED()                                                                                        \
    do {                                                                                                               \
        MASK_REG |= (1 << 7);                                                                                          \
    } while (0)

#define CLEAR_FLAG_SPRITE_OVERFLOW()                                                                                   \
    do {                                                                                                               \
        MASK_REG &= ~(1 << 5);                                                                                         \
    } while (0)
#define CLEAR_FLAG_SPRITE_ZERO_HIT()                                                                                   \
    do {                                                                                                               \
        MASK_REG &= ~(1 << 6);                                                                                         \
    } while (0)
#define CLEAR_FLAG_NMI_OCCURRED()                                                                                      \
    do {                                                                                                               \
        MASK_REG &= ~(1 << 7);                                                                                         \
    } while (0)

#define OAM_ADDR_REG ppu->regs[3]
#define OAM_IO_REG ppu->regs[4]
#define VRAM_ADDR1_REG ppu->regs[5]
#define VRAM_ADDR2_REG ppu->regs[6]
#define VRAM_IO_REG ppu->regs[7]

#define INC_VRAM_ADDR()                                                                                                \
    do {                                                                                                               \
        ppu->v += FLAG_INCREMENT ? 32 : 1;                                                                             \
    } while (0)

bool ppu_nmi(ppu_t *ppu) {
    return (STATUS_REG & ppu->regs[0x0000]) & (1 << 7) ? 0 : 1;
}

uint8_t ppu_read_register(ppu_t *ppu, uint16_t addr) {
    uint8_t ret, buffered;
    switch (addr) {
    case 0x2002:
        // Read PPUSTATUS: Return old status of NMI_occurred in bit 7, then set NMI_occurred to false.
        ret = STATUS_REG;
        STATUS_REG &= ~(1 << 7);
        // w:                   = 0
        ppu->write_toggle = 0;
        return ret;
    case 0x2004: return OAM[OAM_ADDR_REG];
    case 0x2007:
        ret = READ(ppu->v);
        // emulate buffered reads
        if (ppu->v % 0x4000 < 0x3F00) {
            buffered = ppu->buffered_data;
            ppu->buffered_data = ret;
            ret = buffered;
        } else {
            ppu->buffered_data = READ(ppu->v - 0x1000);
        }
        INC_VRAM_ADDR();
        return ret;
    default: break;
    }
    ASSERT(false);
    return 0;
}

void ppu_write_register(ppu_t *ppu, uint16_t addr, uint8_t val) {
    uint16_t cpu_addr;
    switch (addr) {
    case 0x2000:
        CTRL_REG = val;
        // In the following, d refers to the data written to the port, and A through H to individual bits of a val.
        // update name table
        // t: ....BA.. ........ = d: ......BA
        ppu->t = (ppu->t & 0xF3FF) | (CTRL_REG << 10);
        return;
    case 0x2001: MASK_REG = val; return;
    case 0x2003: OAM_ADDR_REG = val; return;
    case 0x2004: OAM[OAM_ADDR_REG++] = val; return;
    case 0x2005:
        if (ppu->write_toggle == 0) {
            // $2005 first write (w is 0): update coarse X scroll
            // t: ........ ...HGFED = d: HGFED...
            // x:               CBA = d: .....CBA
            // w:                   = 1
            ppu->t = (ppu->t & 0xFFE0) | ((val >> 3) & 0x1F);
            ppu->x = val & 0x07;
            ppu->write_toggle = 1;
        } else {
            // $2005 second write (w is 1): update coarse Y scroll
            // t: .CBA..HG FED..... = d: HGFEDCBA
            // w:                   = 0
            ppu->t = (ppu->t & 0x8FFF) | ((val & 0x07) << 12);
            ppu->t = (ppu->t & 0xFC1F) | ((val & 0xF8) << 2);
            ppu->write_toggle = 0;
        }
        return;
    case 0x2006:
        if (ppu->write_toggle == 0) {
            // $2006 first write (w is 0): update vram higher address
            // t: ..FEDCBA ........ = d: ..FEDCBA
            // t: .X...... ........ = 0
            // w:                   = 1
            ppu->t = (ppu->t & 0x80FF) | ((val & 0x3F) << 8);
            ppu->write_toggle = 1;
        } else {
            // $2006 second write (w is 1): update vram lower address
            // t: ........ HGFEDCBA = d: HGFEDCBA
            // v                    = t
            // w:                   = 0
            ppu->t = (ppu->t & 0xFF00) | val;
            ppu->v = ppu->t;
            ppu->write_toggle = 0;
        }
        return;
    case 0x2007:
        WRITE(ppu->v, val);
        INC_VRAM_ADDR();
        return;
    case 0x4014:
        cpu_addr = ((uint16_t)val) << 8;
        for (uint16_t i = 0; i < 0x100; i++) {
            OAM[OAM_ADDR_REG] = cpu_read(ppu->nes, cpu_addr);
            OAM_ADDR_REG++;
            cpu_addr++;
        }
        cpu_t *cpu = ppu->nes->cpu;
        cpu->stall += 513;
        if (cpu->cycles & 1) {
            cpu->stall++;
        }
        return;
    default: break;
    }
    ASSERT(false);
    return;
}

void increment_x(ppu_t *ppu) {
    // increment hori(v)
    // if coarse X == 31
    if ((ppu->v & 0x001F) == 31) {
        // coarse X = 0
        ppu->v &= 0xFFE0;
        // switch horizontal nametable
        ppu->v ^= 0x0400;
    } else {
        // increment coarse X
        ppu->v++;
    }
}

void increment_y(ppu_t *ppu) {
    // increment vert(v)
    // if fine Y < 7
    if ((ppu->v & 0x7000) != 0x7000) {
        // increment fine Y
        ppu->v += 0x1000;
    } else {
        // fine Y = 0
        ppu->v &= 0x8FFF;
        // let y = coarse Y
        uint8_t y = (ppu->v & 0x03E0) >> 5;
        if (y == 29) {
            // coarse Y = 0
            y = 0;
            // switch vertical nametable
            ppu->v ^= 0x0800;
        } else if (y == 31) {
            // coarse Y = 0, nametable not switched
            y = 0;
        } else {
            // increment coarse Y
            y++;
        }
        // put coarse Y back into v
        ppu->v = (ppu->v & 0xFC1F) | (y << 5);
    }
}

void copy_x(ppu_t *ppu) {
    // hori(v) = hori(t)
    // v: .....F.. ...EDCBA = t: .....F.. ...EDCBA
    ppu->v = (ppu->v & 0xFBE0) | (ppu->t & 0x041F);
}

void copy_y(ppu_t *ppu) {
    // vert(v) = vert(t)
    // v: .IHGF.ED CBA..... = t: .IHGF.ED CBA.....
    ppu->v = (ppu->v & 0x841F) | (ppu->t & 0x7BE0);
}

/* ------------------------------------------------- main functions ------------------------------------------------- */

void ppu_reset(ppu_t *ppu) {
    DEBUG_MSG("ppu reset\n");
    ASSERT(ppu != NULL);
    CYCLE = 340;
    SCANLINE = 0;
    FRAME = 0;
    CTRL_REG = 0;
    MASK_REG = 0;
    OAM_ADDR_REG = 0;
}

void tick(ppu_t *ppu) {
    if (CYCLE == 339 && SCANLINE == 261 && ODD_FRAME_FLAG) {
        if (RENDER_ENABLED) {
            CYCLE = 0;
            SCANLINE = 0;
            FRAME++;
            ODD_FRAME_FLAG = !ODD_FRAME_FLAG;
            return;
        }
    }
    CYCLE++;
    if (CYCLE > 340) {
        CYCLE = 0;
        SCANLINE++;
        if (SCANLINE > 261) {
            SCANLINE = 0;
            FRAME++;
            ODD_FRAME_FLAG = !ODD_FRAME_FLAG;
        }
    }
}

uint32_t *g_screen = NULL;

void set_screen(uintptr_t address) {
    g_screen = (uint32_t *)address;
    // DEBUG_MSG("set_screen: %X\n", g_screen);
}

void render_pixel(ppu_t *ppu) {
    // DEBUG_MSG("render_pixel: %X\n", g_screen);
    uint16_t x = CYCLE - 1;
    uint16_t y = SCANLINE;
    uint8_t color;

    uint8_t background_color;
    if (!FLAG_SHOW_BACKGROUND) {
        background_color = 0;
    } else {
        uint8_t high2bit = (SHIFT_REGISTER8 >> ((14 - ppu->x) * 2)) & 0b1100;
        uint8_t low2bit = (SHIFT_REGISTER16 >> ((15 - ppu->x) * 2)) & 0b11;
        background_color = high2bit | low2bit;
    }

    uint8_t sprite_index = 0;
    uint8_t sprite_color = 0;
    uint8_t i;
    if (!FLAG_SHOW_SPRITES) {
        for (i = 0; i < ppu->sprite_count; i++) {
            uint16_t offset = x - ppu->sprite_positions[i];
            if (offset < 0 || offset > 7) {
                continue;
            }
            color = (ppu->sprite_patterns[i] >> ((7 - offset) * 4)) & 0x0F;
            if (!(color & 3)) {
                continue;
            }
            sprite_index = i;
            sprite_color = color;
        }
    }

    if (x < 8) {
        if (FLAG_SHOW_LEFT_BACKGROUND) {
            background_color = 0;
        }
        if (FLAG_SHOW_LEFT_SPRITES) {
            sprite_color = 0;
        }
    }

    /**
     * Priority multiplexer decision table
     * BG pixel    Sprite pixel    Priority    Output
     *   0             0              X        BG ($3F00)
     *   0             1-3            X        Sprite
     *   1-3           0              X        BG
     *   1-3           1-3            0        Sprite
     *   1-3           1-3            1        BG
     */
    bool b = background_color & 3;
    bool s = sprite_color & 3;
    if (!b && !s) {
        color = 0;
    } else if (!b && s) {
        color = sprite_color | 0x10;
    } else if (b && !s) {
        color = background_color;
    } else {
        if (ppu->sprite_indexes[i] == 0 && x < 255) {
            SET_FLAG_SPRITE_ZERO_HIT();
        }
        if (ppu->sprite_priorities[i] == 0) {
            color = sprite_color | 0x10;
        } else {
            color = background_color;
        }
    }
    if (g_screen != NULL) {
        DEBUG_MSG("screen: %X, val: %X\n", (uint32_t)(g_screen + (y << 8) + x), g_palette[ppu->palette_index[color]]);
        *(g_screen + ((y << 8) + x)) = g_palette[ppu->palette_index[color]];
    }
}

uint32_t fetch_sprite_pattern(ppu_t *ppu, uint16_t i, uint16_t row) {
    uint8_t tile_index = OAM[i * 4 + 1];
    /** attributes
     76543210
     ||||||||
     ||||||++- Palette (4 to 7) of sprite
     |||+++--- Unimplemented
     ||+------ Priority (0: in front of background; 1: behind background)
     |+------- Flip sprite horizontally
     +-------- Flip sprite vertically
     */
    uint8_t attributes = OAM[i * 4 + 2];
    uint8_t palette_base = (attributes * 3) << 2;
    uint8_t flip_horizontally = attributes & 0x40;
    uint8_t flip_vertically = attributes & 0x80;
    uint16_t address;
    uint8_t table;
    if (!FLAG_SPRITE_SIZE) {
        if (flip_vertically) {
            row = 7 - row;
        }
        table = FLAG_SPRITE_TABLE;
    } else {
        if (flip_vertically) {
            row = 15 - row;
        }
        table = tile_index & 1;
        tile_index &= 0xFE;
        if (row > 7) {
            tile_index++;
            row -= 8;
        }
    }
    address = (table << 12) | tile_index << 4 | row;

    uint8_t low_tile_byte = READ(address);
    uint8_t high_tile_byte = READ(address + 8);
    uint32_t data;

    for (uint8_t i = 0; i < 8; i++) {
        uint8_t p1, p2;
        if (flip_horizontally) {
            p1 = (low_tile_byte & 1) << 0;
            p1 = (high_tile_byte & 1) << 0;
            low_tile_byte >>= 1;
            high_tile_byte >>= 1;
        } else {
            p1 = (low_tile_byte & 1) >> 7;
            p1 = (high_tile_byte & 1) >> 6;
            low_tile_byte <<= 1;
            high_tile_byte <<= 1;
        }
        data <<= 4;
        data |= palette_base | p1 | p2;
    }
    return data;
}

void fetch_sprites(ppu_t *ppu) {
    uint8_t h = FLAG_SPRITE_SIZE ? 8 : 16;
    uint8_t count = 0;

    for (uint8_t i = 0; i < 64; i++) {
        uint8_t y = OAM[i * 4];
        uint8_t a = OAM[i * 4 + 2];
        uint8_t x = OAM[i * 4 + 3];
        uint16_t row = SCANLINE - y;
        if (row < 0 || row >= h) {
            continue;
        }
        if (count < 8) {
            ppu->sprite_patterns[count] = fetch_sprite_pattern(ppu, i, row);
            ppu->sprite_positions[count] = x;
            ppu->sprite_priorities[count] = (a >> 5) & 1;
            ppu->sprite_indexes[count] = i;
        }
        count++;
    }
    if (count > 8) {
        count = 8;
        SET_FLAG_SPRITE_OVERFLOW();
    }
    ppu->sprite_count = count;
}

void ppu_step(ppu_t *ppu) {
    // DEBUG_MSG("ppu_step\n");
    tick(ppu);
    bool pre_line = SCANLINE == 261;
    bool visible_line = SCANLINE < 240;
    bool render_line = pre_line || visible_line;
    bool pre_fetch_cycle = CYCLE >= 321 && CYCLE <= 336;
    bool visible_cycle = CYCLE >= 1 && CYCLE <= 256;
    bool fetch_cycle = pre_fetch_cycle || visible_cycle;

    if (RENDER_ENABLED) {
        DEBUG_MSG("ppu_step: %d, %d\n", visible_line, visible_cycle);
        if (visible_line && visible_cycle) {
            render_pixel(ppu);
        }

        if (render_line && fetch_cycle) {
            uint8_t v = ppu->v;
            uint16_t data;
            SHIFT_REGISTER8 <<= 2;
            SHIFT_REGISTER16 <<= 2;
            uint8_t shift, i, p1, p2;
            switch (CYCLE & 7) {
            case 1: ppu->name_table_byte = READ(0x2000 | (v & 0x0FFF)); break;
            case 3:
                shift = ((v >> 4) & 4) | (v & 2);
                ppu->attribute = (READ(0x23C0 | (v & 0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07)) >> shift) & 3;
                break;
            case 5:
                ppu->low_tile_byte = READ((FLAG_BACKGROUND_TABLE << 12) | (FLAG_NAMT_TABLE << 4) | ((v >> 12) & 7));
                break;
            case 7:
                ppu->high_tile_byte =
                    READ((FLAG_BACKGROUND_TABLE << 12) | (FLAG_NAMT_TABLE << 4) | ((v >> 12) & 7) + 8);
                break;

            case 0:
                data = 0;
                for (i = 0; i < 8; i++) {
                    data <<= 2;
                    data |= ppu->attribute;
                }
                SHIFT_REGISTER8 |= data;

                data = 0;
                for (i = 0; i < 16; i++) {
                    p1 = (ppu->low_tile_byte & 0x80) >> 7;
                    p2 = (ppu->high_tile_byte & 0x80) >> 6;
                    ppu->low_tile_byte <<= 1;
                    ppu->high_tile_byte <<= 1;
                    data <<= 2;
                    data |= p1 | p2;
                }
                SHIFT_REGISTER16 |= data;
                break;
            default: break;
            }
        }

        if (pre_line) {
            if (CYCLE > 279 && CYCLE < 305) {
                ppu->v = (ppu->v & 0x841F) | (ppu->t & 0x7BE0);
            }
        }
        if (render_line) {
            if (fetch_cycle && (CYCLE & 7) == 0) {
                increment_x(ppu);
            }
            if (CYCLE == 256) {
                increment_y(ppu);
            }
            if (CYCLE == 257) {
                ppu->v = (ppu->v & 0xFBE0) | (ppu->t & 0x041F);
            }
        }
        if (CYCLE == 256) {
            if (visible_line) {
                fetch_sprites(ppu);
            } else {
                ppu->sprite_count = 0;
            }
        }
    }

    if (SCANLINE == 241 && CYCLE == 1) {
        SET_FLAG_NMI_OCCURRED();
    }

    if (pre_line && CYCLE == 1) {
        CLEAR_FLAG_NMI_OCCURRED();
        CLEAR_FLAG_SPRITE_ZERO_HIT();
        CLEAR_FLAG_SPRITE_OVERFLOW();
    }
}