#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "listen.h"
#include "db_engine.h"

#define HELP_TEXT "-h \"prints help message\"\n -p <port number> \"sets the server to listen to port number for connections\""

int main(int argc, char* argv[]) {
    int port = 9999;

    initDB();
    if (argc >= 2) {
        request_t* request = parse_request(argv[1], NULL);
        print_request(request);
        switch(request->request_type) {
            case RT_CREATE: {
                createTable(request);
            } break;
            case RT_TABLES: {
                listTables(request);
            } break;
            case RT_SCHEMA: {
                printSchema(request);
            } break;
            case RT_INSERT: {
                insertRecord(request);
            } break;
            case RT_SELECT: {
                char* select = selectRecord(request);
                printf("%s", select);
                free(select);
            } break;
        }
    }
    exit(0);

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

    //listenPort(port);
}

