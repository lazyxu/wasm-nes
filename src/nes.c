//
// Created by meteor on 2018/11/5.
//

#include "nes.h"
#include "ines.h"

EMSCRIPTEN_KEEPALIVE
int32_t load(uint8_t *data_p, uint32_t data_len)
{
    LOG("DEBUG mode\n");
    // HEX_DUMP(data_p, data_len);
    return load_ines_rom(&ines, data_p, data_len);
}