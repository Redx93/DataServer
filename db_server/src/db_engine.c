#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>

#include "db_engine.h"

const char* const SCHEMA_FILE_END = ".schema";
const char* const TABLE_FILE_END = ".table";
const char* const DB_PATH = "../database/";

int initDB() {
    if (chdir(DB_PATH) != 0) {
        perror("Couldn't set working directory to database directory.");
        return -1;
    }
}

void exitDB() {

}

int makeTableFileNames(request_t* req, char* schemaFileName, char* tableFileName) {
    memset(schemaFileName, 0, sizeof(schemaFileName));
    strcat(schemaFileName, req->table_name);
    strcat(schemaFileName, SCHEMA_FILE_END);

    memset(tableFileName, 0, sizeof(tableFileName));
    strcat(tableFileName, req->table_name);
    strcat(tableFileName, TABLE_FILE_END);
}

int createTable(request_t* req) {
    const int bufferSize = 128;
    char schemaFileName[bufferSize];
    char tableFileName[bufferSize];
    makeTableFileNames(req, schemaFileName, tableFileName);
    
    FILE* fd = fopen(schemaFileName, "w");
    if (fd == NULL) {
        perror("Couldn't create schema file");
        return -1;
    }
    else {
        /*
            .schema format
            COL_NAME0 COL_TYPE0 COL_MOD0
            COL_NAME1 COL_TYPE1 COL_MOD1
            COL_NAME2 COL_TYPE2 COL_MOD2

            COL_MOD for INT is 1 if it is primary key and 0 if not
            COL_MOD for VARCHAR is length
        */

        column_t* col = req->columns;
        while (col != NULL) {
            if (col->data_type == DT_INT) {
                fprintf(fd, "%s %i %i\n", col->name, col->data_type, col->is_primary_key);
            }
            else {
                fprintf(fd, "%s %i %i\n", col->name, col->data_type, col->char_size);
            }
            
            col = col->next;
        }

        fclose(fd);
    }

    fd = fopen(tableFileName, "wb");
    if (fd == NULL) {
        perror("Couldn't create table file");
        return -1;
    }
    fclose(fd);

    return 0;
}

int deleteTable(request_t* req) {
    const int bufferSize = 256;
    char schemaFileName[bufferSize];
    char tableFileName[bufferSize];
    makeTableFileNames(req, schemaFileName, tableFileName);
    if (unlink(schemaFileName)) {
        perror("Couldn't delete schema file");
        return -1;
    }

    if (unlink(tableFileName)) {
        perror("Couldn't delete table file");
        return -1;
    }

    return 0;
}

int listTables(request_t* req) {
    DIR *dp;
    struct dirent *ep;

    dp = opendir ("./");
    if (dp != NULL) {
        char temp[256];
        
        while (ep = readdir(dp)) {
            char* end = strstr(ep->d_name, TABLE_FILE_END);
            if (end != NULL) {
                memset(temp, 0, 256);
                strncpy(temp, ep->d_name, end - ep->d_name);
                puts(temp);
            }
        }
        closedir(dp);
    }
    else {
        perror("Couldn't open the directory");
    }
}

int printSchema(request_t* req) {
    
}

int insertRecord(request_t* req) {

}
