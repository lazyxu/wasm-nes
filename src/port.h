//
// Created by meteor on 2018/11/4.
//

#ifndef WASM_NES_PORT_H
#define WASM_NES_PORT_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <emscripten/emscripten.h>

#define EOK 0
#define EINVALID_ARGUMENT -1
#define EINVALID_INES_HEADER -2
#define EINVALID_INES_CONTENT -3
#define EINVALID_INES_END -4

#ifdef DEBUG

#include <stdio.h>

#define LOG printf

#define HEX_DUMP(data, data_len)                                            \
    do                                                                      \
    {                                                                       \
        LOG("  Length: %u\n", data_len);                                    \
        LOG("  Offset: 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n"); \
        for (uint32_t i = 0; i < data_len; i++)                             \
        {                                                                   \
            if ((i & 0xF) == 0)                                             \
            {                                                               \
                LOG("%08X: ", i);                                           \
            }                                                               \
            LOG("%02X", (uint8_t)(*(data + i)));                            \
            if (((i + 1) & 0xF) == 0)                                       \
            {                                                               \
                LOG("\n");                                                  \
            }                                                               \
            else                                                            \
            {                                                               \
                LOG(" ");                                                   \
            }                                                               \
        }                                                                   \
        LOG("\n");                                                          \
    } while (0);
#else

#define LOG(...)
#define HEX_DUMP(...)

#endif

//extern void console_log (char *format, ...);
#endif //WASM_NES_PORT_H
