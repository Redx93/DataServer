#include <stdio.h>
#include <stdlib.h>

#include "request.h"

int main(int argc, char* argv[]) {

    request_t *request;
    char *error;

    if (argc != 2) {
        printf("Provide one request string in quotation marks!\n");
        exit(1);
    }

    request = parse_request(argv[1], &error);

    if(request != NULL) {
        print_request(request);
        destroy_request(request);
    }
    else {
        printf("%s\n", error);
        free(error);
    }

}
