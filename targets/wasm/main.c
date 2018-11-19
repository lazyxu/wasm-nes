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

EMSCRIPTEN_KEEPALIVE
void nes_set_screen(uint8_t *screen) {
    DEBUG_MSG("nes_set_screen\n");
    DEBUG_MSG("%X\n", (uint32_t)screen);
    set_screen((uint32_t)screen);
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
    g_nes = nes_init();
    return 0;
}