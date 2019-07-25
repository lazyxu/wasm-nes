#include <stdio.h>

#include "nes.h"
#include "port.h"
#include "ppu.h"
#include "mirror.h"

static nes_t *g_nes = NULL;
uint32_t *screen = NULL;

int dbg_nes_init() {
    printf("Module nes-debugger loaded.\n");
    PRINT_LOG_LEVEL();
    screen = malloc(sizeof(uint32_t) * 256 * 240);
    DEBUG_MSG("screen:%lu", sizeof(uint32_t) * 256 * 240);
    if (screen == NULL) {
        return -1;
    }
    *screen = 1;
    set_screen((uintptr_t)&screen);
    g_nes = nes_init();
    return 0;
}

#include "cJSON/cJSON.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ROM_DIR "./targets/debugger/roms"
void get_rom_list(cJSON *response) {
    DIR *dir;
    struct dirent *ptr;
    char base[1000];

    if ((dir = opendir(ROM_DIR)) == NULL) {
        perror("Open dir error...");
        return;
    }

    cJSON_AddArrayToObject(response, "romlist");
    cJSON *romlist = cJSON_GetObjectItem(response, "romlist");
    while ((ptr = readdir(dir)) != NULL) { /// current dir OR parrent dir
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) {
            continue;
        } else if (ptr->d_type == DT_REG) { /// file
            cJSON_AddItemToArray(romlist, cJSON_CreateString(ptr->d_name));
            printf("d_name:%s\n", ptr->d_name);
        }
    }
    closedir(dir);
}

void dbg_nes_load_file(cJSON *response, const char *filename) {
    size_t len = sizeof(char) * (strlen(ROM_DIR) + 2 + strlen(filename));
    char *path = malloc(len);
    memset(path, 0, len);
    strcpy(path, ROM_DIR);
    strcpy(path + strlen(ROM_DIR), "/");
    strcpy(path + strlen(ROM_DIR) + 1, filename);
    printf("%s\n", path);
    int ret = nes_load_file(g_nes, path);
    if (ret == EOK) {
        cJSON_AddStringToObject(response, "topic", "nesinfo");
        cJSON_AddObjectToObject(response, "nesinfo");
        cJSON *nesinfo = cJSON_GetObjectItem(response, "nesinfo");
        cJSON_AddBoolToObject(nesinfo, "isChrRam", g_nes->cart->is_chr_ram);
        cJSON_AddNumberToObject(nesinfo, "chrRom", g_nes->cart->num_chr_rom_bank);
        cJSON_AddNumberToObject(nesinfo, "prgRom", g_nes->cart->num_prg_rom_bank);
        cJSON_AddNumberToObject(nesinfo, "mapper", g_nes->cart->mapper_no);
        cJSON_AddStringToObject(nesinfo, "mirror", get_mirror_name(g_nes->cart->mirror));
        printf("done!\n");
    }
}