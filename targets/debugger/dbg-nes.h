#ifndef DBG_NES_H
#define DBG_NES_H

int dbg_nes_init(void);

void get_rom_list(cJSON *response);
void dbg_nes_load_file(cJSON *response, const char *filepath);
void dbg_cpu_disassembly(cJSON *response);
void dbg_cpu_registers(cJSON *response);
#endif // DBG_NES_H