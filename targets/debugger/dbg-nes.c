#include <stdio.h>

#include "nes.h"
#include "port.h"
#include "ppu.h"

static nes_t *g_nes = NULL;
uint32_t *screen = NULL;

int dbg_nes_init(){
    printf("Module wasm-nes loaded.\n");
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