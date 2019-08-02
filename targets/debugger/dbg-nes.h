#ifndef DBG_NES_H
#define DBG_NES_H

int dbg_nes_init(void);

void get_rom_list(struct lws *wsi, cJSON *in, const char *topic);
void dbg_nes_load_file(struct lws *wsi, cJSON *in, const char *topic);
void dbg_cpu_disassembly(struct lws *wsi, cJSON *in, const char *topic);
void dbg_cpu_info(struct lws *wsi, cJSON *in, const char *topic);
void dbg_cpu_step(struct lws *wsi, cJSON *in, const char *topic);
void dbg_breakpoint(struct lws *wsi, cJSON *in, const char *topic);
void dbg_nes_reset(struct lws *wsi, cJSON *in, const char *topic);
void dbg_cpu_run(struct lws *wsi, cJSON *in, const char *topic);
void dbg_cpu_pause(struct lws *wsi, cJSON *in, const char *topic);
void dbg_cpu_mem(struct lws *wsi, cJSON *in, const char *topic);
#endif // DBG_NES_H
