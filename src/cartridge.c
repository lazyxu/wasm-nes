//
// Created by meteor on 2018/11/4.
//

#include "cartridge.h"
#include "mirror.h"

int32_t cartridge_load(cartridge_t *cart, uint8_t *data, uint32_t data_len) {
    ASSERT(cart != NULL);
    ASSERT(cart->trainer == NULL);
    ASSERT(cart->prg_rom == NULL);
    ASSERT(cart->chr_rom == NULL);
    ASSERT(cart->chr_ram == NULL);
    if (data == NULL || data_len == 0) {
        return EINVALID_ARGUMENT;
    }
    uint8_t *data_end = data + data_len;

    ines_header_t *header = (ines_header_t *)data;

    cart->num_prg_rom_bank = header->num_prg_rom_bank;
    cart->num_chr_rom_bank = header->num_chr_rom_bank;
    DEBUG_MSG("num_prg_rom_bank:%u, num_chr_rom_bank:%u\n", header->num_prg_rom_bank, header->num_chr_rom_bank);
    // Identify the rom as an iNES file: NES\x1a.
    if (header->magic != INES_FILE_MAGIC) {
        return EINVALID_INES_HEADER;
    }
    // Reserved for future usage and should all be 0.
    for (uint8_t i = 0; i < sizeof(header->padding); ++i) {
        if (header->padding[i] != 0) {
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
    cart->mapper_no = (header->control_2 & 0xf0) | (header->control_1 >> 4);
    DEBUG_MSG("using mapper: %u\n", cart->mapper_no);

    mirror = ((header->control_1 >> 3) & 1) ? FOURSCREEN_MIRRORING : (header->control_1 & 1);

    // battery-backed RAM
    bool battery = (header->control_1 >> 1) & 1;
    data = data + sizeof(ines_header_t);

    // Following the header is the 512-byte trainer, if one is present,
    // otherwise the ROM banks begin here, starting with PRG-ROM then CHR-ROM.
    if ((header->control_1 & 0b100) == 0b100) {
        COPY_DATA(cart->trainer, TRAINER_SIZE);
    }

    // Load PRG-ROM banks:
    COPY_DATA(cart->prg_rom, header->num_prg_rom_bank * PRG_ROM_SIZE);
    if (header->num_chr_rom_bank != 0) {
        // Load CHR-ROM banks:
        COPY_DATA(cart->chr_rom, header->num_chr_rom_bank * CHR_ROM_SIZE);
    } else {
        // Provide CHR-ROM and CHR-RAM if not in the rom:
        cart->chr_rom = malloc(CHR_ROM_SIZE);
        cart->chr_ram = malloc(CHR_RAM_SIZE);
    }

    // Check if the contents of rom have been completely read.
    if (data != data_end) {
        DEBUG_MSG("%X %X\n", (uint32_t)data, (uint32_t)data_end);
        return EINVALID_INES_CONTENT;
    }

    return EOK;
}

void cartridge_free(cartridge_t *cart) {
    ASSERT(cart != NULL);
    FREE(cart->trainer);
    FREE(cart->prg_rom);
    FREE(cart->chr_rom);
    FREE(cart->chr_ram);
}