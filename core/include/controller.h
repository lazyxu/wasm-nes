//
// Created by meteor on 2018/11/11.
//

#ifndef WASM_NES_CONTROLLER_H
#define WASM_NES_CONTROLLER_H

#include "port.h"

typedef struct {
} controller_t;

uint8_t controller_read(controller_t *controller);
void controller_write(controller_t *controller, uint8_t val);

#endif // WASM_NES_CONTROLLER_H
