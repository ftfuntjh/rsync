#ifndef SERVER_H
#define SERVER_H

#include "buffer.h"

#include <poll.h>
#include <stdint.h>

struct rsync_client {
    struct list * buf;
    struct pollfd pollfd;
};

struct rsync_server {
    char * db_path;
    char ** monitor_dirs;
    char * ip_addr;
    int enable_tcp_connection;
    int tcp_fd;
    struct list * clients;
    uint16_t port;
};


extern int rsync_server_init(struct rsync_server * server);
extern int rsync_server_fini(struct rsync_server * server);

extern int rsync_event_loop(struct rsync_server * server);

#endif // SERVER_H
