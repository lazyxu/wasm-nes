//
// Created by meteor on 2018/11/4.
//

#ifndef WASM_NES_INES_H
#define WASM_NES_INES_H

#include "port.h"

#define INES_FILE_MAGIC 0x1a53454e

typedef struct
{
    uint32_t magic;     // iNES magic number
    uint8_t num_prg;    // number of PRG-ROM banks (16KB each)
    uint8_t num_chr;    // number of CHR-ROM banks (8KB each)
    uint8_t control_1;  // control bits
    uint8_t control_2;  // control bits
    uint8_t num_ram;    // PRG-RAM size (x 8KB)
    uint8_t padding[7]; // unused padding
} ines_header_t;

#define TRAINER_SIZE 0x200
#define PRG_ROM_SIZE 0x4000
#define CHR_ROM_SIZE 0x2000
#define CHR_RAM_SIZE 0x2000

typedef struct
{
    uint8_t *trainer;
    uint8_t num_prg; // number of PRG-ROM banks (16KB each)
    uint8_t *prg_rom;
    uint8_t num_chr; // number of CHR-ROM banks (8KB each)
    uint8_t *chr_rom;
    uint8_t *chr_ram;
} ines_t;

void ines_free(ines_t *ines);

int32_t ines_init(ines_t *ines, uint8_t *data, uint32_t data_len);

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

#endif //WASM_NES_INES_H
