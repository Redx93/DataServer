#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "db_engine.h"
#include "tcpserver.h"

#define HELP_TEXT "-h \"prints help message\"\n -p <port number> \"sets the server to listen to port number for connections\""

int main(int argc, char* argv[]) {
    int port = 9999;

    initDB();

    // Handle command line arguments
    for (int i = 1; i < argc; i++) {
        char arg[256];
        strcpy(arg, argv[i]);

        if (!strcmp(arg, "-p")) {
            if (i + 1 < argc) {
                char* end;
                port = strtol(argv[i + 1], &end, 10);
                i += 1;
            } else {
                fprintf(stderr, "No port number provided after -p\n");
                exit(1);
            }

            if (port < 1000) {
                fprintf(stderr, "Invalid port number\n");
                exit(1);
            }
        }
        else if (!strcmp(arg, "-h")) {
            printf("%s", HELP_TEXT);
            exit(0);
        }
    }

    listenPort(port);
}

