#ifndef DBG_UTILS_H
#define DBG_UTILS_H

#include <stdbool.h>
#include <libwebsockets.h>

int ws_send(struct lws *wsi, const char *str);
void set_interval(void (*signal_handler)(int), int32_t ms);
void clear_interval();

#endif // DBG_UTILS_H
