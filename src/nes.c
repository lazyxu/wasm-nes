//
// Created by meteor on 2018/11/5.
//

#include "nes.h"
#include "cartridge.h"

EMSCRIPTEN_KEEPALIVE
int32_t nes_load(uint8_t *data, uint32_t data_len)
{
    LOG("DEBUG MODE\n");
    g_nes.cart = malloc(sizeof(cartridge_t));
    // HEX_DUMP(data, data_len);
    return cartridge_load(g_nes.cart, data, data_len);
}

void nes_free()
{
    LOG("NES FREE\n");
    cartridge_free(g_nes.cart);
    FREE(g_nes.cart);
}