#ifndef DBG_NES_H
#define DBG_NES_H

int dbg_nes_init(void);

void get_rom_list(cJSON *response);
void dbg_nes_load_file(cJSON *response, const char *filepath);
#endif // DBG_NES_H
