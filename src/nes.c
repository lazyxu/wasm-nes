//
// Created by meteor on 2018/11/5.
//

#include "nes.h"
#include "cartridge.h"
nes_t *new_nes() {
    nes_t *nes = malloc(sizeof(nes_t));
    nes->cart = malloc(sizeof(cartridge_t));
    nes->cpu = malloc(sizeof(cpu_t));
    nes->cpu->nes = nes;
    nes->ppu = malloc(sizeof(cpu_t));
    nes->ppu->nes = nes;
    nes->apu = malloc(sizeof(cpu_t));
    nes->apu->nes = nes;
    nes->mmc = malloc(sizeof(mmc_t));
    return nes;
}

EMSCRIPTEN_KEEPALIVE
int32_t nes_load(uint8_t *data, uint32_t data_len) {
    nes_t *nes = new_nes();
    int32_t ret;
    DEBUG_MSG("DEBUG MODE\n");
    HEX_DUMP(data, data_len);
    if ((ret = cartridge_load(nes->cart, data, data_len)) && ret != EOK) {
        DEBUG_MSG("cartridge_load exit\n");
        return ret;
    }
    mmc_init(nes->mmc, nes->cart, nes->cpu, nes->ppu);
    cpu_reset(nes->cpu);
    return ret;
}

void nes_free(nes_t *nes) {
    DEBUG_MSG("NES FREE\n");
    cartridge_free(nes->cart);
    FREE(nes->cart);
}