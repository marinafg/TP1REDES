#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFSZ 500

void usage(int argc, char **argv) {
    printf("usage: %s <server IP> <server port>\n", argv[0]);
    printf("example: %s 127.0.0.1 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}


int main(int argc, char **argv) {
    
    if (argc < 3){
        usage(argc, argv);
    }

    while (1){

        struct sockaddr_storage storage;
        if (0 != addrparse(argv[1], argv[2], &storage)) {
            usage(argc, argv);
        }

        int s;
        s = socket(storage.ss_family, SOCK_STREAM, 0);
        if (s == -1) {
            logexit("socket");
        }

        struct sockaddr *addr = (struct sockaddr *)(&storage);
        if (0 != connect(s, addr, sizeof(storage))) {
            logexit("connect");
        }

        char buf[BUFSZ];
        memset(buf, 0, BUFSZ);
        printf("> ");
        fgets(buf, BUFSZ - 1, stdin);

        if (addPokemon(buf) == true) {
           printf("%s added\n", pokedex[0]);
        }

        if (addPokemon(buf) == false) {
            printf("invalid message\n");
        }

        listPokemon(buf);

        size_t count = send(s, buf, strlen(buf) + 1, 0);

        if (count != strlen(buf) + 1) {
            logexit("send");
        }

        memset(buf, 0, BUFSZ);
        unsigned total = 0;

        count = recv(s, buf + total, BUFSZ - total, 0);
        if (count == 0) {
            // Connection terminated.
            break;
        }
        total += count;

        //close(s);

        if (killTerminal(buf, s) == true) {
            close(s);
        }
        
    }

    exit(EXIT_SUCCESS);
}