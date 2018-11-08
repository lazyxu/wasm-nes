//
// Created by meteor on 2018/11/5.
//

#include "nes.h"
#include "cartridge.h"

EMSCRIPTEN_KEEPALIVE
int32_t nes_load(uint8_t *data, uint32_t data_len)
{
    int32_t ret;
    DEBUG_MSG("DEBUG MODE\n");
    g_nes.cart = malloc(sizeof(cartridge_t));
    HEX_DUMP(data, data_len);
    if ((ret = cartridge_load(g_nes.cart, data, data_len)) && ret != EOK)
    {
        DEBUG_MSG("cartridge_load exit\n");
        return ret;
    }
    mmc_init(g_nes.mmc, g_nes.cart, g_nes.cpu, g_nes.ppu);
    return ret;
}

void nes_free()
{
    DEBUG_MSG("NES FREE\n");
    cartridge_free(g_nes.cart);
    FREE(g_nes.cart);
}