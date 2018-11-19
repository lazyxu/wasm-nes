//
// Created by meteor on 2018/11/5.
//

#include "nes.h"

nes_t *nes_init() {
    nes_t *nes = malloc(sizeof(nes_t));
    nes->cart = cartridge_init();
    nes->cpu = malloc(sizeof(cpu_t));
    nes->cpu->nes = nes;
    nes->ppu = malloc(sizeof(ppu_t));
    nes->ppu->nes = nes;
    nes->apu = malloc(sizeof(apu_t));
    nes->apu->nes = nes;
    nes->mmc = malloc(sizeof(mmc_t));
    return nes;
}

void nes_free(nes_t *nes) {
    DEBUG_MSG("NES FREE\n");
    cartridge_free(nes->cart);
    FREE(nes->cart);
}

int32_t nes_load(nes_t *nes, uint8_t *data, uint32_t data_len) {
    int32_t ret;
    DEBUG_MSG("rom size: %u\n", data_len);
    HEX_DUMP(data, data_len);
    if ((ret = cartridge_load(nes->cart, data, data_len)) && ret != EOK) {
        DEBUG_MSG("cartridge_load exit\n");
        return ret;
    }
    mmc_init(nes->mmc, nes->cart, nes->cpu, nes->ppu);
    cpu_reset(nes->cpu);
    ppu_reset(nes->ppu);
    return ret;
}

void step(nes_t *nes) {
    // DEBUG_MSG("step\n");
    cpu_t *cpu = nes->cpu;
    ppu_t *ppu = nes->ppu;
    apu_t *apu = nes->apu;
    cpu_step(cpu);
    apu_step(apu);
    ppu_step(ppu);
    ppu_step(ppu);
    ppu_step(ppu);
    if (ppu_nmi(ppu)) {
        cpu_nmi(cpu);
    }
}

void step_frame(nes_t *nes) {
    // DEBUG_MSG("step_frame\n");
    cpu_t *cpu = nes->cpu;
    ppu_t *ppu = nes->ppu;
    uint32_t frame = ppu->frame;

    for (uint32_t frame = ppu->frame; frame == ppu->frame;) {
        step(nes);
    }
}