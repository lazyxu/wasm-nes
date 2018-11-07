//
// Created by meteor on 2018/11/4.
//

#include "ines.h"
#include "mirror.h"

void ines_free(ines_t *ines)
{
    ASSERT(ines != NULL);
    FREE(ines->trainer);
    FREE(ines->prg_rom);
    FREE(ines->chr_rom);
    FREE(ines->chr_ram);
}

int32_t ines_init(ines_t *ines, uint8_t *data, uint32_t data_len)
{
    ASSERT(ines != NULL);
    ASSERT(ines->trainer == NULL);
    ASSERT(ines->prg_rom == NULL);
    ASSERT(ines->chr_rom == NULL);
    ASSERT(ines->chr_ram == NULL);
    if (data == NULL || data_len == 0)
    {
        return EINVALID_ARGUMENT;
    }
    uint8_t *data_end = data + data_len;

    ines_header_t *header = (ines_header_t *)data;

    // Identify the rom as an iNES file: NES\x1a.
    if (header->magic != INES_FILE_MAGIC)
    {
        return EINVALID_INES_HEADER;
    }
    // Reserved for future usage and should all be 0.
    for (uint8_t i = 0; i < sizeof(header->padding); ++i)
    {
        if (header->padding[i] != 0)
        {
            return EINVALID_INES_HEADER;
        }
    }

    // ROM Control Byte 1:
    // • Bit 0 - Indicates the type of mirroring used by the game
    //     where 0 indicates horizontal mirroring, 1 indicates
    //     vertical mirroring.
    // • Bit 1 - Indicates the presence of battery-backed RAM at
    //     memory locations $6000-$7FFF.
    // • Bit 2 - Indicates the presence of a 512-byte trainer at
    //     memory locations $7000-$71FF.
    // • Bit 3 - If this bit is set it overrides bit 0 to indicate fourscreen
    //     mirroring should be used.
    // • Bits 4-7 - Four lower bits of the mapper number.

    // ROM Control Byte 2:
    // • Bits 0-3 - Reserved for future usage and should all be 0.
    // • Bits 4-7 - Four upper bits of the mapper number.
    uint8_t mapper = (header->control_2 & 0xf0) | (header->control_1 >> 4);
    mirror = ((header->control_1 >> 3) & 1) ? FOURSCREEN_MIRRORING : (header->control_1 & 1);

    // battery-backed RAM
    bool battery = (header->control_1 >> 1) & 1;
    data = data + sizeof(ines_header_t);

    // Following the header is the 512-byte trainer, if one is present,
    // otherwise the ROM banks begin here, starting with PRG-ROM then CHR-ROM.
    if ((header->control_1 & 0b100) == 0b100)
    {
        COPY_DATA(ines->trainer, TRAINER_SIZE);
    }

    // Load PRG-ROM banks:
    COPY_DATA(ines->prg_rom, header->num_prg * PRG_ROM_SIZE);
    if (header->num_chr != 0)
    {
        // Load CHR-ROM banks:
        COPY_DATA(ines->chr_rom, header->num_chr * CHR_ROM_SIZE);
    }
    else
    {
        // Provide CHR-ROM and CHR-RAM if not in the rom:
        ines->chr_rom = malloc(CHR_ROM_SIZE);
        ines->chr_ram = malloc(CHR_RAM_SIZE);
    }

    // Check if the contents of rom have been completely read.
    if (data != data_end)
    {
        LOG("%X %X\n", (uint32_t)data, (uint32_t)data_end);
        return EINVALID_INES_CONTENT;
    }

    return EOK;
}