#ifndef __TINY_H
#define __TINY_H

#include <arpa/inet.h> /* inet_ntoa */
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>


typedef struct {
    char filename[512];
    off_t offset; /* for support Range */
    size_t end;
} http_request;

/* Simplifies calls to bind(), connect(), and accept() */
typedef struct sockaddr SA;

int open_listenfd(int port);
char *process(int fd, struct sockaddr_in *clientaddr);
void parse_request(int fd, http_request *req);

#endif /* __TINY_H */