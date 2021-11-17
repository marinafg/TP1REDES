#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "common.h"

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

    if (0 != addrparse(argv[1], argv[2], &storage)) {
        usage(argc, argv);
    }

    int sock = socket(storage.ss_family, SOCK_STREAM, 0);
    if (sock == -1) {
        logexit("socket");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);

	if (0 != connect(sock, addr, sizeof(storage))) {
		logexit("connect");
	}

    char addrstr[BUFSZ];
	addrtostr(addr, addrstr, BUFSZ);

    printf("connected to %s\n", addrstr);

    char buf[BUFSZ];

	memset(buf, 0, BUFSZ);

	

	size_t count = send(sock, buf, strlen(buf)+1, 0);

	if (count != strlen(buf)+1) {
		logexit("send");
	}

    memset(buf, 0, BUFSZ);

	unsigned total = 0;

	while (1) {
		count = recv(sock, buf + total, BUFSZ - total, 0);
		if (count == -1) {

			// Connection terminated.
			break;
            
		} else {

            printf("> ");
	        fgets(buf, BUFSZ-1, stdin);

        }
		total += count;
	}

	close(sock);

    puts(buf);

    exit(EXIT_SUCCESS);

}