//
// Created by meteor on 2018/11/8.
//

#ifndef WASM_NES_MAPPER0_H
#define WASM_NES_MAPPER0_H

#include "mmc.h"

mapper_t m0 = {
    mapper_init, mapper_free, mapper_cpu_read, mapper_cpu_write, mapper_ppu_read, mapper_ppu_write,
};

#endif // WASM_NES_MAPPER0_H
