//
// Created by meteor on 2018/11/5.
//

#include "nes.h"

#include <stdio.h>

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

void nes_free(nes_t **nes) {
    if (nes != NULL) {
        DEBUG_MSG("NES FREE\n");
        cartridge_free(&(*nes)->cart);
    }
}

void nes_reset(nes_t **nes) {
    ASSERT(*nes != NULL);
    cpu_reset((*nes)->cpu);
}

int32_t nes_load(nes_t *nes, uint8_t *data, uint32_t data_len) {
    int32_t ret;
    nes_free(&nes);
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

static size_t get_file_size(const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    if (fp == NULL) {
        printf("failed to open the file: %s\n", filepath);
        return 0;
    }

    int ret = fseek(fp, 0L, SEEK_END);
    if (ret < 0) {
        printf("failed to fseek the file\n");
        return 0;
    }
    size_t size = ftell(fp);
    fclose(fp);
    return size;
}

#define MAX_ROM_SIZE 1024 * 1024
int32_t nes_load_file(nes_t *nes, const char *filepath) {
    size_t real_size = get_file_size(filepath);
    if (real_size == 0) {
        return EERROR;
    }

    uint8_t *rom = malloc(real_size);
    memset(rom, 0, real_size);

    FILE *fp = fopen(filepath, "r");
    if (fp == NULL) {
        return EERROR;
    }
    fread(rom, MAX_ROM_SIZE, sizeof(uint8_t), fp);
    fclose(fp);
    fp = NULL;

    return nes_load(nes, rom, real_size);
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