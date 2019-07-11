#include "nes.h"
#include "port.h"
#include "ppu.h"
#include <emscripten/emscripten.h>

static nes_t *g_nes = NULL;

EMSCRIPTEN_KEEPALIVE
uint8_t nes_cpu_step() {
    DEBUG_MSG("nes_cpu_step\n");
    return cpu_step(g_nes->cpu);
}

uint32_t *screen = NULL;
EMSCRIPTEN_KEEPALIVE
uint32_t *nes_get_screen() {
    DEBUG_MSG("nes_get_screen: %d\n", screen[0]);
    return screen;
}

EMSCRIPTEN_KEEPALIVE
void nes_step_frame() {
    // DEBUG_MSG("nes_step_frame\n");
    step_frame(g_nes);
}

EMSCRIPTEN_KEEPALIVE
int32_t _nes_load(uint8_t *data, uint32_t data_len) {
    return nes_load(g_nes, data, data_len);
}

int main() {
    DEBUG_MSG("Module wasm-nes loaded.\n");
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