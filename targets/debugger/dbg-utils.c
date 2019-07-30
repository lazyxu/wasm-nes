#include "dbg-utils.h"

int ws_send(struct lws *wsi, const char *str) {
    size_t len = strlen(str);
    unsigned char *out = (unsigned char *)malloc(sizeof(unsigned char) *
                                                 (LWS_SEND_BUFFER_PRE_PADDING + len + LWS_SEND_BUFFER_POST_PADDING));
    /* setup the buffer */
    memcpy(out + LWS_SEND_BUFFER_PRE_PADDING, str, len);
    /* write out */
    int n = lws_write(wsi, out + LWS_SEND_BUFFER_PRE_PADDING, len, LWS_WRITE_TEXT);
    /* free the buffer */
    free(out);
    return n;
}