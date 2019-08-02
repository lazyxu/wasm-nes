#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cJSON/cJSON.h"
#include <libwebsockets.h>

#include "command.h"
#include "dbg-nes.h"
#include "dbg-utils.h"

#define KGRN "\033[0;32;32m"
#define KCYN "\033[0;36m"
#define KRED "\033[0;32;31m"
#define KYEL "\033[1;33m"
#define KMAG "\033[0;35m"
#define KBLU "\033[0;32;34m"
#define KCYN_L "\033[1;36m"
#define RESET "\033[0m"

static int destroy_flag = 0;

static void INT_HANDLER(int signo) {
    destroy_flag = 0;
}

static char *topics[] = {"romlist",    "loadrom", "cpu_instructions", "cpu_info", "cpu_step", "reset",
                         "breakpoint", "cpu_run", "cpu_pause",        "cpu_mem",  NULL};
static void (*funcs[])(struct lws *, cJSON *, const char *) = {
    get_rom_list,   dbg_nes_load_file, dbg_cpu_disassembly, dbg_cpu_info, dbg_cpu_step, dbg_nes_reset,
    dbg_breakpoint, dbg_cpu_run,       dbg_cpu_pause,       dbg_cpu_mem,  NULL};

extern struct lws *g_wsi;
void websocket_on_receive(struct lws *wsi, char *str, int len) {
    if (str == NULL || wsi == NULL) {
        return;
    }
    if (g_wsi == NULL) {
        g_wsi = wsi;
    }
    cJSON *in = cJSON_Parse(str);
    printf("%s\n", cJSON_Print(in));
    char *topic = cJSON_GetStringValue(cJSON_GetObjectItem(in, "topic"));
    for (size_t i = 0; topics[i] != NULL && funcs[i] != NULL; i++) {
        if (strcmp(topic, topics[i]) == 0) {
            funcs[i](wsi, in, topic);
        }
    }
    cJSON_free(in);
}

static int ws_service_callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {

    switch (reason) {

    case LWS_CALLBACK_ESTABLISHED: printf(KYEL "[Main Service] Connection established\n" RESET); break;

    //* If receive a data from client*/
    case LWS_CALLBACK_RECEIVE:
        ((char *)in)[len] = 0;
        printf(KCYN_L "[Main Service] Server recvived:%s - %zu\n" RESET, (char *)in, len);

        //* echo back to client*/
        websocket_on_receive(wsi, (char *)in, len);

        break;
    case LWS_CALLBACK_CLOSED: printf(KYEL "[Main Service] Client close.\n" RESET); break;

    default: break;
    }

    return 0;
}

struct per_session_data {
    int fd;
};

int main(void) {
    dbg_nes_init();
    // server url will usd port 5000
    int port = 5000;
    const char *interface = NULL;
    struct lws_context_creation_info info;
    struct lws_protocols protocol;
    struct lws_context *context;
    // Not using ssl
    const char *cert_path = NULL;
    const char *key_path = NULL;
    // no special options
    int opts = 0;

    //* register the signal SIGINT handler */
    struct sigaction act;
    act.sa_handler = INT_HANDLER;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGINT, &act, 0);

    //* setup websocket protocol */
    protocol.name = "my-echo-protocol";
    protocol.callback = ws_service_callback;
    protocol.per_session_data_size = sizeof(struct per_session_data);
    protocol.rx_buffer_size = 0;

    //* setup websocket context info*/
    memset(&info, 0, sizeof info);
    info.port = port;
    info.iface = interface;
    info.protocols = &protocol;
    info.extensions = NULL;
    info.ssl_cert_filepath = cert_path;
    info.ssl_private_key_filepath = key_path;
    info.gid = -1;
    info.uid = -1;
    info.options = opts;

    //* create libwebsocket context. */
    context = lws_create_context(&info);
    if (context == NULL) {
        printf(KRED "[Main] Websocket context create error.\n" RESET);
        return -1;
    }

    printf(KGRN "[Main] Websocket context create success.\n" RESET);

    //* websocket service */
    while (!destroy_flag) {
        lws_service(context, 50);
    }
    usleep(10);
    lws_context_destroy(context);

    return 0;
}