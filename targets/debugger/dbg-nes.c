#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"
#include "mirror.h"
#include "nes.h"
#include "port.h"
#include "ppu.h"
#include "dbg-utils.h"

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
void get_rom_list(cJSON *response) {
    DIR *dir;
    struct dirent *ptr;
    char base[1000];

    if ((dir = opendir(ROM_DIR)) == NULL) {
        perror("Open dir error...");
        return;
    }

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
    closedir(dir);
}

void dbg_nes_load_file(cJSON *response, const char *filename) {
    size_t len = sizeof(char) * (strlen(ROM_DIR) + 2 + strlen(filename));
    char *path = malloc(len);
    memset(path, 0, len);
    strcpy(path, ROM_DIR);
    strcpy(path + strlen(ROM_DIR), "/");
    strcpy(path + strlen(ROM_DIR) + 1, filename);
    printf("%s\n", path);
    int ret = nes_load_file(g_nes, path);
    if (ret == EOK) {
        cJSON_AddStringToObject(response, "topic", "nesinfo");
        cJSON_AddObjectToObject(response, "nesinfo");
        cJSON *nesinfo = cJSON_GetObjectItem(response, "nesinfo");
        cJSON_AddBoolToObject(nesinfo, "isChrRam", g_nes->cart->is_chr_ram);
        cJSON_AddNumberToObject(nesinfo, "chrRom", g_nes->cart->num_chr_rom_bank);
        cJSON_AddNumberToObject(nesinfo, "prgRom", g_nes->cart->num_prg_rom_bank);
        cJSON_AddNumberToObject(nesinfo, "mapper", g_nes->cart->mapper_no);
        cJSON_AddStringToObject(nesinfo, "mirror", get_mirror_name(g_nes->cart->mirror));
    }
}

void dbg_cpu_disassembly(cJSON *response) {
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
}

void dbg_cpu_info(cJSON *response) {
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
    // cJSON_AddArrayToObject(response, "stack");
    cJSON *stack = cJSON_GetObjectItem(response, "stack");
    char addr[3] = {0};
    char val[3] = {0};
    for (uint8_t i = 0xFF; i > g_nes->cpu->sp; i--) {
        sprintf(addr, "%02X", i);
        sprintf(val, "%02X", cpu_read(g_nes, i));
        cJSON_AddItemToObject(stack, addr, cJSON_CreateString(val));
        // cJSON_AddItemToArray(stack, cJSON_CreateNumber(cpu_read(g_nes, i)));
    }
}

void dbg_cpu_step(cJSON *response) {
    cpu_step(g_nes->cpu);
    cJSON_AddStringToObject(response, "topic", "cpu_info");
    dbg_cpu_info(response);
}

void dbg_nes_reset(cJSON *response) {
    nes_reset(&g_nes);
    cJSON_AddStringToObject(response, "topic", "cpu_info");
    dbg_cpu_info(response);
}
#define BREAKPOINT_SIZE 100
#define INVALID_BREAKPOINT 0
static uint16_t breakpoint[BREAKPOINT_SIZE] = {INVALID_BREAKPOINT};
uint8_t breakpoint_cnt = 0;
void dbg_breakpoint(cJSON *in) {
    char *str = cJSON_GetObjectItem(in, "address")->valuestring;
    uint16_t address;
    sscanf(str, "%hX", &address);
    bool enable = cJSON_GetObjectItem(in, "enable")->valueint;
    printf("%d %d\n", address, enable);
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
        }
    }
}

#include <signal.h>
#include <sys/time.h>
#include <time.h>

static int count = 0;
static struct itimerval oldtv;

void set_timer() {
    struct itimerval itv;
    itv.it_interval.tv_sec = 0;
    itv.it_interval.tv_usec = 1000*500;
    itv.it_value.tv_sec = 0;
    itv.it_value.tv_usec = 1;
    setitimer(ITIMER_REAL, &itv, &oldtv);
}

void clear_time() {
    struct itimerval value;
    value.it_value.tv_sec = 0;
    value.it_value.tv_usec = 0;
    value.it_interval = value.it_value;
    setitimer(ITIMER_REAL, &value, NULL);
}

void dbg_cpu_pause(cJSON *response) {
    clear_time();
}

static struct lws *g_wsi = NULL;
void signal_handler(int m) {
    cpu_step(g_nes->cpu);
    cJSON *response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, "topic", "cpu_info");
    dbg_cpu_info(response);
    int n = ws_send(g_wsi, cJSON_PrintUnformatted(response));
    cJSON_free(response);
    for (uint8_t i = 0; i < BREAKPOINT_SIZE; i++) {
        if (breakpoint[i] == g_nes->cpu->pc) {
            clear_time();
        }
    }
}

void dbg_cpu_run(struct lws *wsi_in, cJSON *response) {
    g_wsi = wsi_in;
    signal(SIGALRM, signal_handler);
    set_timer();
}