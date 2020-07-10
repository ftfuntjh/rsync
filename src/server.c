#include "server.h"
#include "list.h"
#include "buffer.h"

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <unistd.h>
#include <poll.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_BUFFER_CAPACITY 16

typedef struct rsync_client rsync_client;
typedef struct rsync_server rsync_server;

int on_new_message(rsync_server * server, int fd) {
    struct list * p = server->clients;

    do {
        if (LIST_GET(p, rsync_client)->pollfd.fd == fd) {
            break;
        }
        p = p->next;
    } while (p != server->clients);

    if (LIST_GET(p, rsync_client)->pollfd.fd != fd) {
        printf("invalid fd found.\n");
        close(fd);
        return -1;
    }

    struct buffer * buf = LIST_GET(LIST_GET(p, rsync_client)->buf->prev, struct buffer);
    unsigned char * buf_ptr = buf->ptr;
    int n = 0, nbytes = 0;
    while (1) {
        if (buf->capacity - buf->used <= 0) {
            buf = LIST_GET(buffer_append((struct list *)((void *)buf - sizeof(struct list)), DEFAULT_BUFFER_CAPACITY), struct buffer);
            buf_ptr = buf->ptr;
            assert(buf->capacity - buf->used > 0);
        }

        if ((n = recv(fd, buf_ptr + buf->used, buf->capacity - buf->used, 0)) < 0) {
            if (errno == EAGAIN) {
                break;
            } else {
                perror("read");
                close(fd);
                return -1;
            }
        } else if (n == 0) {
                close(fd);
                break;
        } else {
            nbytes += n;
            buf->used += n;
            if (nbytes >= 10240) {
                break;
            }
        }
    }

    int buf_len = 0;
    struct list * buf_head = LIST_GET(p, rsync_client)->buf;
    buffer_concat(buf_head, NULL, &buf_len);
    assert(buf_len > 0);
    char * msg_buf = malloc(buf_len);
    printf("receivew message %.*s\n", buf_len, (const char *)buffer_concat(buf_head, msg_buf, &buf_len));
    free(msg_buf);
    return 0;
}

int rsync_server_accpet(rsync_server * server) {
    int fd = server->tcp_fd, client_fd = -1;
    struct sockaddr_in sin_client; 
    struct timeval tv;
    memset(&sin_client, 0, sizeof(struct sockaddr_in));
    socklen_t sockaddr_len;
    if ((client_fd = accept(fd, (struct sockaddr *)&sin_client, &sockaddr_len)) <= 0) {
        perror("accept");
        return -1;
    }

    struct list * client;
    LIST_INIT_ASSIGN(client, rsync_client);
    LIST_GET(client, rsync_client)->buf = buffer_init(DEFAULT_BUFFER_CAPACITY);
    LIST_GET(client, rsync_client)->pollfd.fd = client_fd;
    LIST_GET(client, rsync_client)->pollfd.events = POLLIN;
    LIST_INSERT(server->clients, client);

    tv.tv_sec = 0;
    tv.tv_usec = 500000;
    if (setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(struct timeval)) == -1) {
        perror("setsockopt");
        return -1;
    }
    return 0;
}

int rsync_server_init(rsync_server * server) {
    assert(server != NULL);
    assert(server->ip_addr != NULL);
    assert(server->port > 0);

    if(server->enable_tcp_connection) {
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0) {
            perror("socket");
            return -1;
        }

        struct sockaddr_in sin_server;
        memset(&sin_server, 0, sizeof(struct sockaddr_in));

        sin_server.sin_family = AF_INET;
        sin_server.sin_port = htons(server->port);
        sin_server.sin_addr.s_addr = inet_addr(server->ip_addr);

        if (bind(fd, (struct sockaddr *)&sin_server, sizeof(struct sockaddr_in)) == -1) {
            perror("bind");
            return -1;
        }

        if (listen(fd, 64) == -1) {
            perror("listen");
            return -1;
        }

        server->tcp_fd = fd; 
        LIST_INIT_ASSIGN(server->clients, rsync_client);
        LIST_GET(server->clients, rsync_client)->pollfd.fd = fd;
        LIST_GET(server->clients, rsync_client)->pollfd.events = POLLIN;
    }

    return 0;
}

int rsync_server_fini(rsync_server * server) {
    assert(server != NULL);
    return 0;
}

int rsync_event_loop(rsync_server * server) {
    int timeout = 3 * 1000;
    struct pollfd fds[64];
    int nfds = 0, n = 0;
    while (1) {
        nfds = n = 0;
        if (server->clients != NULL) {
            struct list * i = server->clients;
            do {
                fds[nfds].fd = LIST_GET(i, rsync_client)->pollfd.fd;
                fds[nfds].events = LIST_GET(i, rsync_client)->pollfd.events;
                fds[nfds].revents = 0;
                ++nfds; 
                i = i->next;
            } while(i != server->clients);
        }

        if ((n = poll(fds, nfds, timeout)) > 0) {
            for (int i = 0; i < nfds; ++i) {
                if (fds[i].revents & POLLIN && fds[i].fd == server->tcp_fd) {
                    rsync_server_accpet(server);
                } else if (fds[i].revents & POLLIN){
                    on_new_message(server, fds[i].fd);
                } else if (fds[i].revents != 0) {
                    struct list * ptr = server->clients;
                    do {
                        if (LIST_GET(ptr, rsync_client)->pollfd.fd == fds[i].fd) {
                            LIST_REMOVE(ptr);
                            break;
                        }

                        ptr = ptr->next;
                    } while(ptr != server->clients);
                }
            }
        } else if (n == 0) {
            // skip
        } else if (n < 0){
            perror("poll");
            return -1;
        }
    }
    return 0;
}
