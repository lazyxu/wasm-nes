//
// Created by meteor on 2018/11/4.
//

#ifndef WASM_NES_CARTRIDGE_H
#define WASM_NES_CARTRIDGE_H

#include "port.h"

#define INES_FILE_MAGIC 0x1a53454e

typedef struct
{
    uint32_t magic;           // iNES magic number
    uint8_t num_prg_rom_bank; // number of PRG-ROM banks (16KB each)
    uint8_t num_chr_rom_bank; // number of CHR-ROM banks (8KB each)
    uint8_t control_1;        // control bits
    uint8_t control_2;        // control bits
    uint8_t padding[8];       // unused padding
} ines_header_t;

#define TRAINER_SIZE 0x200
#define PRG_ROM_SIZE 0x4000
#define CHR_ROM_SIZE 0x2000
#define CHR_RAM_SIZE 0x2000

typedef struct
{
    uint8_t *trainer;
    uint8_t mapper_no;
    uint8_t num_prg_rom_bank; // number of PRG-ROM banks (16KB each)
    uint8_t *prg_rom;
    uint8_t num_chr_rom_bank; // number of CHR-ROM banks (8KB each)
    uint8_t *chr_rom;
    uint8_t *chr_ram;
} cartridge_t;

int32_t cartridge_load(cartridge_t *cart, uint8_t *data, uint32_t data_len);
void cartridge_free(cartridge_t *cart);

#define COPY_DATA(dst, size)              \
    do                                    \
    {                                     \
        if (data + size < data_end)       \
        {                                 \
            return EINVALID_INES_CONTENT; \
        }                                 \
        dst = malloc(size);               \
        memcpy(dst, data, size);          \
        data += size;                     \
    } while (0)

#endif //WASM_NES_CARTRIDGE_H
