//
// Created by meteor on 2018/11/8.
//

#ifndef WASM_NES_MAPPER_H
#define WASM_NES_MAPPER_H

#include "port.h"
#include "mapper1.h"

typedef struct
{
    void (*init)();
    void (*free)();
    uint8_t *(*read)(uint16_t addr);
    void (*write)(uint16_t addr, uint8_t value);
} mapper_t;

static mapper_t m1 = {
    init1,
    NULL,
    NULL,
    NULL,
};

// mapper list
static mapper_t *g_mapper_list[256] =
    {
        NULL,
        &m1,
        NULL,
};
#endif //WASM_NES_MAPPER_H
