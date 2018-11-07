//
// Created by meteor on 2018/11/5.
//

#include "nes.h"
#include "ines.h"

void nes_free()
{
    LOG("NES FREE\n");
    ines_free(&ines);
}

EMSCRIPTEN_KEEPALIVE
int32_t nes_load(uint8_t *data_p, uint32_t data_len)
{
    LOG("DEBUG MODE\n");
    // HEX_DUMP(data_p, data_len);
    return ines_init(&ines, data_p, data_len);
}