#include "dbg-utils.h"

static bool g_lock = false;
int ws_send(struct lws *wsi, const char *const str) {
    if (g_lock == false) {
        g_lock = true;
        printf("ws send: %s\n", str);
        size_t len = strlen(str);
        unsigned char *out = (unsigned char *)malloc(
            sizeof(unsigned char) * (LWS_SEND_BUFFER_PRE_PADDING + len + LWS_SEND_BUFFER_POST_PADDING));
        /* setup the buffer */
        memcpy(out + LWS_SEND_BUFFER_PRE_PADDING, str, len);
        /* write out */
        lws_write(wsi, out + LWS_SEND_BUFFER_PRE_PADDING, len, LWS_WRITE_TEXT);
        /* free the buffer */
        free(out);
        g_lock = false;
    }
}

#include <signal.h>
#include <sys/time.h>
#include <time.h>

void clear_interval() {
    struct itimerval value;
    value.it_value.tv_sec = 0;
    value.it_value.tv_usec = 0;
    value.it_interval = value.it_value;
    setitimer(ITIMER_REAL, &value, NULL);
}

void set_interval(void (*signal_handler)(int), int32_t ms) {
    signal(SIGALRM, signal_handler);
    struct itimerval oldtv;
    struct itimerval itv;
    itv.it_interval.tv_sec = 0;
    itv.it_interval.tv_usec = 1000 * ms;
    itv.it_value.tv_sec = 0;
    itv.it_value.tv_usec = 1;
    setitimer(ITIMER_REAL, &itv, &oldtv);
}