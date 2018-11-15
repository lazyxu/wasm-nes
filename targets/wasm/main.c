#include "nes.h"
#include "port.h"
#include <emscripten/emscripten.h>

static nes_t *g_nes = NULL;

EMSCRIPTEN_KEEPALIVE
uint8_t nes_cpu_step() {
    return cpu_step(g_nes->cpu);
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