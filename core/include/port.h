//
// Created by meteor on 2018/11/4.
//

#ifndef WASM_NES_PORT_H
#define WASM_NES_PORT_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef LOG_DEBUG
#define PRINT_LOG_LEVEL(...)                                                                                           \
    do {                                                                                                               \
        printf("LOG LEVEL: DEBUG\n");                                                                                  \
    } while (0)
#elif defined(LOG_TRACE)
#define PRINT_LOG_LEVEL(...)                                                                                           \
    do {                                                                                                               \
        printf("LOG LEVEL: TRACE\n");                                                                                  \
    } while (0)
#else
#define PRINT_LOG_LEVEL(...)
#endif

#define EOK 0
#define EINVALID_ARGUMENT -1
#define EINVALID_INES_HEADER -2
#define EINVALID_INES_CONTENT -3
#define EINVALID_INES_END -4

#define FREE(ptr)                                                                                                      \
    do {                                                                                                               \
        free(ptr);                                                                                                     \
        ptr = NULL;                                                                                                    \
    } while (0)

#ifdef LOG_OFF

#define ASSERT(...)
#define DEBUG_MSG(...)
#define TRACE_MSG(...)
#define HEX_DUMP(...)

#else // !LOG_OFF

#include <stdio.h>

#define ASSERT(test)                                                                                                   \
    if (!(test)) {                                                                                                     \
        printf("%s:%d[%s]: Assertion failed: " #test "\n", __FILE__, __LINE__, __FUNCTION__);                          \
        abort();                                                                                                       \
    }

#define DEBUG_MSG printf

#ifdef LOG_TRACE
#define TRACE_MSG printf
#define HEX_DUMP(data, data_len)                                                                                       \
    do {                                                                                                               \
        printf("  Length: %u\n", data_len);                                                                            \
        printf("  Offset: 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");                                         \
        for (uint32_t i = 0; i < data_len; i++) {                                                                      \
            if ((i & 0xF) == 0) {                                                                                      \
                printf("%08X: ", i);                                                                                   \
            }                                                                                                          \
            printf("%02X", (uint8_t)(*(data + i)));                                                                    \
            if (((i + 1) & 0xF) == 0) {                                                                                \
                printf("\n");                                                                                          \
            } else {                                                                                                   \
                printf(" ");                                                                                           \
            }                                                                                                          \
        }                                                                                                              \
        printf("\n");                                                                                                  \
    } while (0)
#else
#define TRACE_MSG(...)
#define HEX_DUMP(...)
#endif // TRACE

#endif // LOG_OFF

#ifdef TEST_CPU
#include <stdio.h>
#endif

#endif // WASM_NES_PORT_H
