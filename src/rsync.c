#include "server.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_help() {
   printf("Usage: rsync {Options}\n"
          "Options:\n"
          "-p port tcp listen port.\n");
}
static  struct rsync_server server_ins;
int main(int argc, char * argv[]) {
    if (argc < 2) {
        print_help();
        return 0;
    }

    for (char ** i = &argv[1]; *i != NULL; ++i) {
        if (strcmp(*i, "-p") == 0) {
            ++i;
            if (*i == NULL) {
                print_help();
                return 0;
            }
            long port = strtol(*i, NULL, 10);
            assert(port > 0 && port < 65535);
            server_ins.port = (uint16_t)port;
            server_ins.enable_tcp_connection = 1;
        } else if (strcmp(*i, "-a") == 0) {
            ++i;
            if (*i == NULL) {
                print_help();
                return 0;
            }
            server_ins.ip_addr = malloc(strlen(*i) + 1);
            memset(server_ins.ip_addr, 0, strlen(*i) + 1);
            strcpy(server_ins.ip_addr, *i);
        }
    }

    rsync_server_init(&server_ins);
    rsync_event_loop(&server_ins);
    rsync_server_fini(&server_ins);
    return 0;
}
