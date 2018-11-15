//
// Created by meteor on 2018/11/8.
//

#ifndef WASM_NES_MAPPER2_H
#define WASM_NES_MAPPER2_H

#include "mmc.h"

mapper_t m2 = {
    mapper_init, mapper_free, mapper_cpu_read, mapper_cpu_write, NULL, NULL,
};

#endif // WASM_NES_MAPPER2_H
