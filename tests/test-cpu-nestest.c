
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

int main() {
    printf("Test of CPU!\n");
    PRINT_LOG_LEVEL();

    const char *filepath = "./tests/roms/nestest/nestest.nes";
    const char *outputpath = "./build/nestest.log";

    size_t real_size = get_file_size(filepath);
    if (real_size == 0) {
        return -1;
    }
    uint8_t *rom = malloc(real_size);
    memset(rom, 0, real_size);

    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        printf("failed to open the file\n");
        return -1;
    }
    fread(rom, MAX_ROM_SIZE, sizeof(uint8_t), fp);
    fclose(fp);
    fp = NULL;

    nes_t *nes = nes_init();
    int32_t ret = nes_load(nes, rom, real_size);
    if (ret != EOK) {
        printf("failed to load nes_load: %d\n", ret);
        return -2;
    }

    nes->cpu->pc = 0xC000;

    fp = fopen(outputpath, "w");
    if (!fp) {
        printf("failed to open the file\n");
        return -1;
    }
    size_t i = 0;
    do {
        ret = cpu_status(fp, nes->cpu);
        if (ret < 0) {
            printf("failed to get cpu_status: %d\n", ret);
            return -1;
        }
        cpu_step(nes->cpu);
        i++;
    } while (i < 8991);
    fclose(fp);

    return 0;
}