
#include "nes.h"
#include "port.h"
#include <stdio.h>

#define MAX_ROM_SIZE 1024 * 1024
#define MAX_MSG_SIZE 1024

size_t get_file_size(const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        printf("failed to open the file\n");
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

const char *filepath = "./tests/nestest/nestest.nes";
const char *outputpath = "./build/nestest/nestest.log";

int main() {
    PRINT_LOG_LEVEL();

    size_t real_size = get_file_size(filepath);
    ASSERT(real_size != 0);

    uint8_t *rom = malloc(real_size);
    memset(rom, 0, real_size);

    FILE *fp = fopen(filepath, "r");
    ASSERT(fp != NULL);
    fread(rom, MAX_ROM_SIZE, sizeof(uint8_t), fp);
    fclose(fp);
    fp = NULL;

    nes_t *nes = nes_init();
    int32_t ret = nes_load(nes, rom, real_size);
    ASSERT(ret == EOK);

    nes->cpu->pc = 0xC000;

    fp = fopen(outputpath, "w");
    ASSERT(fp != NULL);

    size_t i = 0;
    do {
        ret = cpu_status(fp, nes->cpu);
        ASSERT(ret >= 0);
        cpu_step(nes->cpu);
        i++;
    } while (i < 8991);
    fclose(fp);

    return 0;
}