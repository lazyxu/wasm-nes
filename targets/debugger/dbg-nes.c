#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"
#include "dbg-utils.h"
#include "mirror.h"
#include "nes.h"
#include "port.h"
#include "ppu.h"

static nes_t *g_nes = NULL;
uint32_t *screen = NULL;

int dbg_nes_init() {
    printf("Module nes-debugger loaded.\n");
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

#include "cJSON/cJSON.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ROM_DIR "./targets/debugger/roms"
void get_rom_list(struct lws *wsi, cJSON *in, const char *topic) {
    DIR *dir;
    struct dirent *ptr;
    char base[1000];

    if ((dir = opendir(ROM_DIR)) == NULL) {
        perror("Open dir error...");
        return;
    }

    cJSON *response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, "topic", topic);
    cJSON_AddArrayToObject(response, "romlist");
    cJSON *romlist = cJSON_GetObjectItem(response, "romlist");
    while ((ptr = readdir(dir)) != NULL) { /// current dir OR parrent dir
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) {
            continue;
        } else if (ptr->d_type == DT_REG) { /// file
            cJSON_AddItemToArray(romlist, cJSON_CreateString(ptr->d_name));
            printf("d_name:%s\n", ptr->d_name);
        }
    }
    ws_send(wsi, cJSON_PrintUnformatted(response));
    cJSON_free(response);
    closedir(dir);
}

void dbg_nes_load_file(struct lws *wsi, cJSON *in, const char *topic) {
    const char *filename = cJSON_GetStringValue(cJSON_GetObjectItem(in, "payload"));
    size_t len = sizeof(char) * (strlen(ROM_DIR) + 2 + strlen(filename));
    char *path = malloc(len);
    memset(path, 0, len);
    strcpy(path, ROM_DIR);
    strcpy(path + strlen(ROM_DIR), "/");
    strcpy(path + strlen(ROM_DIR) + 1, filename);
    printf("%s\n", path);
    int ret = nes_load_file(g_nes, path);
    if (ret == EOK) {
        cJSON *response = cJSON_CreateObject();
        cJSON_AddStringToObject(response, "topic", "nesinfo");
        cJSON_AddObjectToObject(response, "nesinfo");
        cJSON *nesinfo = cJSON_GetObjectItem(response, "nesinfo");
        cJSON_AddBoolToObject(nesinfo, "isChrRam", g_nes->cart->is_chr_ram);
        cJSON_AddNumberToObject(nesinfo, "chrRom", g_nes->cart->num_chr_rom_bank);
        cJSON_AddNumberToObject(nesinfo, "prgRom", g_nes->cart->num_prg_rom_bank);
        cJSON_AddNumberToObject(nesinfo, "mapper", g_nes->cart->mapper_no);
        cJSON_AddStringToObject(nesinfo, "mirror", get_mirror_name(g_nes->cart->mirror));
        ws_send(wsi, cJSON_PrintUnformatted(response));
        cJSON_free(response);
    }
}

void dbg_cpu_disassembly(struct lws *wsi, cJSON *in, const char *topic) {
    cJSON *response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, "topic", topic);
    cJSON_AddArrayToObject(response, "instructions");
    cJSON *instructions = cJSON_GetObjectItem(response, "instructions");
    char *hex = NULL;
    char *opcode = NULL;
    char *opdata = NULL;
    uint16_t pc = cpu_read16(g_nes->cpu, RST_VECTOR);
    char pc_s[5] = {0};
    while (pc < RST_VECTOR) {
        sprintf(pc_s, "%04X", pc);
        pc += cpu_disassembly(g_nes->cpu, pc, &hex, &opcode, &opdata);
        cJSON *instruction = cJSON_CreateObject();
        cJSON_AddStringToObject(instruction, "address", pc_s);
        cJSON_AddStringToObject(instruction, "hex", hex);
        cJSON_AddStringToObject(instruction, "opcode", opcode);
        cJSON_AddStringToObject(instruction, "opdata", opdata);
        cJSON_AddItemToArray(instructions, instruction);
    }
    cJSON_AddObjectToObject(response, "vectors");
    cJSON *vectors = cJSON_GetObjectItem(response, "vectors");
    char nmi[5] = {0};
    sprintf(nmi, "%04X", cpu_read16(g_nes->cpu, NMI_VECTOR));
    cJSON_AddStringToObject(vectors, "nmi", nmi);
    char rst[5] = {0};
    sprintf(rst, "%04X", cpu_read16(g_nes->cpu, RST_VECTOR));
    cJSON_AddStringToObject(vectors, "rst", rst);
    char irq[5] = {0};
    sprintf(irq, "%04X", cpu_read16(g_nes->cpu, IRQ_VECTOR));
    cJSON_AddStringToObject(vectors, "irq", irq);
    ws_send(wsi, cJSON_PrintUnformatted(response));
    cJSON_free(response);
}

struct lws *g_wsi = NULL;
static void send_cpu_info() {
    ASSERT(g_wsi != NULL);
    cJSON *response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, "topic", "cpu_info");
    cJSON_AddObjectToObject(response, "registers");
    cJSON *registers = cJSON_GetObjectItem(response, "registers");
    char pc[5] = {0};
    sprintf(pc, "%04X", g_nes->cpu->pc);
    cJSON_AddStringToObject(registers, "PC", pc);
    char a[3] = {0};
    sprintf(a, "%02X", g_nes->cpu->a);
    cJSON_AddStringToObject(registers, "A", a);
    char x[3] = {0};
    sprintf(x, "%02X", g_nes->cpu->x);
    cJSON_AddStringToObject(registers, "X", x);
    char y[3] = {0};
    sprintf(y, "%02X", g_nes->cpu->y);
    cJSON_AddStringToObject(registers, "Y", y);
    char sp[3] = {0};
    sprintf(sp, "%02X", g_nes->cpu->sp);
    cJSON_AddStringToObject(registers, "SP", sp);
    char p[3] = {0};
    sprintf(p, "%02X", g_nes->cpu->ps);
    cJSON_AddStringToObject(registers, "P", p);
    cJSON_AddObjectToObject(response, "stack");
    cJSON *stack = cJSON_GetObjectItem(response, "stack");
    char addr[3] = {0};
    char val[3] = {0};
    for (uint8_t i = 0xFF; i > g_nes->cpu->sp; i--) {
        sprintf(addr, "%02X", i);
        sprintf(val, "%02X", cpu_read(g_nes, i));
        cJSON_AddItemToObject(stack, addr, cJSON_CreateString(val));
    }
    ws_send(g_wsi, cJSON_PrintUnformatted(response));
    cJSON_free(response);
}

void dbg_cpu_step(struct lws *wsi, cJSON *in, const char *topic) {
    printf("dbg_cpu_info\n");
    cpu_step(g_nes->cpu);
    printf("cpu_step done\n");
    send_cpu_info();
}

void dbg_cpu_info(struct lws *wsi, cJSON *in, const char *topic) {
    send_cpu_info();
}

void dbg_nes_reset(struct lws *wsi, cJSON *in, const char *topic) {
    if (g_nes != NULL && g_nes->cart->rom != NULL) {
        nes_reset(&g_nes);
        send_cpu_info();
    }
}
#define BREAKPOINT_SIZE 100
#define INVALID_BREAKPOINT 0
static uint16_t breakpoint[BREAKPOINT_SIZE] = {INVALID_BREAKPOINT};
uint8_t breakpoint_cnt = 0;
void dbg_breakpoint(struct lws *wsi, cJSON *in, const char *topic) {
    char *addr = cJSON_GetObjectItem(in, "address")->valuestring;
    uint16_t address;
    sscanf(addr, "%hX", &address);
    bool enable = cJSON_GetObjectItem(in, "enable")->valueint;
    for (uint8_t i = 0; i < BREAKPOINT_SIZE; i++) {
        if (enable && breakpoint[i] == INVALID_BREAKPOINT) {
            breakpoint[i] = address;
            break;
        }
        if (!enable && breakpoint[i] == address) {
            breakpoint[i] = INVALID_BREAKPOINT;
        }
    }
    for (uint8_t i = 0; i < BREAKPOINT_SIZE; i++) {
        if (breakpoint[i] != INVALID_BREAKPOINT) {
            printf("breakpoint: %d\n", breakpoint[i]);
            cJSON *response = cJSON_CreateObject();
            cJSON_AddStringToObject(response, "topic", topic);
            cJSON_AddStringToObject(response, "address", addr);
            cJSON_AddBoolToObject(response, "enable", enable);
            ws_send(wsi, cJSON_PrintUnformatted(response));
            cJSON_free(response);
        }
    }
}

void dbg_cpu_pause(struct lws *wsi, cJSON *in, const char *topic) {
    clear_interval();
}

void signal_handler(int m) {
    cpu_step(g_nes->cpu);
    send_cpu_info();
    for (uint8_t i = 0; i < BREAKPOINT_SIZE; i++) {
        if (breakpoint[i] == g_nes->cpu->pc) {
            clear_interval();
        }
    }
}

void dbg_cpu_run(struct lws *wsi, cJSON *in, const char *topic) {
    g_wsi = wsi;
    uint16_t ms = cJSON_GetObjectItem(in, "runInterval")->valuedouble;
    if (ms < 10) {
        ms = 10;
    }
    set_interval(signal_handler, ms);
}

static uint16_t g_mem_start = 0;
static void send_cpu_mem() {
    ASSERT(g_wsi != NULL);
    ASSERT(g_mem_start < 0xFFFF);
    cJSON *response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, "topic", "cpu_mem");
    cJSON_AddArrayToObject(response, "memory");
    cJSON *memory = cJSON_GetObjectItem(response, "memory");
    uint32_t address = g_mem_start;
    char val[3] = {0};
    for (uint32_t i = 0; i < 0x10; i++) {
        cJSON *line = cJSON_CreateObject();
        char addr[5] = {0};
        sprintf(addr, "%04X", address);
        cJSON_AddStringToObject(line, "start", addr);
        char hex[3 * 0x10] = {0};
        char chars[0x10 + 1] = {0};
        for (uint32_t j = 0; j < 0x10; j++) {
            if (address > 0xFFFF) {
                break;
            }
            uint8_t val = cpu_read(g_nes, address);
            if (j != 0x10 - 1) {
                sprintf(hex + j * 3, "%02X ", val);
            } else {
                sprintf(hex + j * 3, "%02X", val);
            }
            if (val < 0x20 || val > 0x7e) {
                val = 0x20;
            }
            chars[j] = val;
            address++;
        }
        cJSON_AddStringToObject(line, "hex", (const char *)hex);
        cJSON_AddStringToObject(line, "chars", (const char *)chars);
        cJSON_AddItemToArray(memory, line);
    }
    ws_send(g_wsi, cJSON_PrintUnformatted(response));
    cJSON_free(response);
}

void dbg_cpu_mem(struct lws *wsi, cJSON *in, const char *topic) {
    printf("dbg_cpu_mem\n");
    g_wsi = wsi;
    g_mem_start = cJSON_GetObjectItem(in, "start")->valuedouble;
    send_cpu_mem();
}
