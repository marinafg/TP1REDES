#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#define BUFSZ 1024

void usage(int argc, char **argv) {
    
    printf("%s <server IP> <server port>\n", argv[0]);
    exit(EXIT_FAILURE);

}

int main(int argc, char **argv) {

    if (argc < 3) {
        usage(argc, argv);
    }

    struct sockaddr_storage storage;

    if (0 != server_sockaddr_init(argv[1], argv[2], &storage)) {
        usage(argc, argv);
    }

    int sock;

    sock = socket(storage.ss_family, SOCK_STREAM, 0);
    if (sock == -1) {
        logexit("socket");
    }

    int enable = 1;

    if (0 != setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))) {
        logexit("setsockopt");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);

    if (0 != bind(sock, addr, sizeof(storage))) {
        logexit("bind");
    }

    if (0 != listen(sock, 10)) {
        logexit("listen");
    }

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);
    printf("bound to %s, waiting connections\n", addrstr);

    while (1) {
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
        socklen_t caddrlen = sizeof(cstorage);

        int csock = accept(sock, caddr, &caddrlen);
        if (csock == -1) {
            logexit("accept");
        }

        char caddrstr[BUFSZ];

        addrtostr(caddr, caddrstr, BUFSZ);
        printf("[log] connection from %s\n", caddrstr);

        char buf[BUFSZ];

        memset(buf, 0, BUFSZ);

        size_t count = recv(csock, buf, BUFSZ - 1, 0);

        printf("[msg] %s, %d bytes: %s\n", caddrstr, (int)count, buf);

        sprintf(buf, "remote endpoint: %.1000s\n", caddrstr);

        count = send(csock, buf, strlen(buf) + 1, 0);

        if (count != strlen(buf) + 1) {
            logexit("send");
        }

        close(csock);

    }

    exit(EXIT_SUCCESS);
}