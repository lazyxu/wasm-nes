#ifndef DBG_UTILS_H
#define DBG_UTILS_H

#include <libwebsockets.h>

int ws_send(struct lws *wsi, const char *str);

#endif // DBG_UTILS_H
