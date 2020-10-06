#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>

#include "db_engine.h"

/*
    .schema format
    COL_NAME0 COL_TYPE0 COL_MOD0
    COL_NAME1 COL_TYPE1 COL_MOD1
    COL_NAME2 COL_TYPE2 COL_MOD2

    COL_MOD for INT is 1 if it is primary key and 0 if not
    COL_MOD for VARCHAR is length
*/
const char* const SCHEMA_FORMAT = "%s %i %i\n";
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

int makeTableFileName(const request_t* req, char* tableFileName) {
    memset(tableFileName, 0, sizeof(tableFileName));
    strcat(tableFileName, req->table_name);
    strcat(tableFileName, TABLE_FILE_END);

    return strlen(tableFileName);
}

int makeSchemaFileName(const request_t* req, char* schemaFileName) {
    memset(schemaFileName, 0, sizeof(schemaFileName));
    strcat(schemaFileName, req->table_name);
    strcat(schemaFileName, SCHEMA_FILE_END);

    return strlen(schemaFileName);
}

int createTable(const request_t* req) {
    const int bufferSize = 128;
    char schemaFileName[bufferSize];
    char tableFileName[bufferSize];
    makeSchemaFileName(req, schemaFileName);
    makeTableFileName(req, tableFileName);
    
    FILE* fd = fopen(schemaFileName, "w");
    if (fd == NULL) {
        perror("Couldn't create schema file");
        return -1;
    }
    else {
        column_t* col = req->columns;
        while (col != NULL) {
            if (col->data_type == DT_INT) {
                fprintf(fd, SCHEMA_FORMAT, col->name, col->data_type, col->is_primary_key);
            }
            else {
                fprintf(fd, SCHEMA_FORMAT, col->name, col->data_type, col->char_size);
            }
            
            col = col->next;
        }

        fclose(fd);
    }

    fd = fopen(tableFileName, "w");
    if (fd == NULL) {
        perror("Couldn't create table file");
        return -1;
    }
    fclose(fd);

    return 0;
}

int deleteTable(const request_t* req) {
    const int bufferSize = 256;
    char schemaFileName[bufferSize];
    char tableFileName[bufferSize];
    makeSchemaFileName(req, schemaFileName);
    makeTableFileName(req, tableFileName);

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

int listTables(const request_t* req) {
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

char* getSchemaString(const request_t* req) {
    char name[256];
    makeSchemaFileName(req, name);

    FILE* fd = fopen(name, "r");
    if (fd == NULL) {
        perror("Failed to open schema file");
        return NULL;
    }

    char* returnString = malloc(1024);
    int offset = 0;

    memset(name, 0, 256); // reuse name buffer
    int type;
    int mod;
    while (fscanf(fd, SCHEMA_FORMAT, &name, &type, &mod) == 3) {
        if (type == DT_INT) {
            offset += sprintf(returnString + offset, "%-30s INT\n", name);
        }
        else {
            offset += sprintf(returnString + offset, "%-30s VARCHAR(%i)\n", name, mod);
        }
        
    }

    fclose(fd);
    
    return returnString;
}

int insertRecord(const request_t* req) {
    char schemaFileName[256];
    char tableFileName[256];
    makeSchemaFileName(req, schemaFileName);
    makeTableFileName(req, tableFileName);
    FILE* fd = fopen(schemaFileName, "r");
    if (fd == NULL) {
        perror("Couldn't open schema file");
        return -1;
    }

    // get schema format info
    column_t* schemaCol = malloc(sizeof(column_t));
    memset(schemaCol, 0, sizeof(column_t));
    column_t* currentSchemaCol = schemaCol;

    char name[256];
    int type;
    int mod;
    while (fscanf(fd, SCHEMA_FORMAT, &name, &type, &mod) == 3) {
        currentSchemaCol->name = malloc(strlen(name) + 1);
        strcpy(currentSchemaCol->name, name);
        if (type == DT_INT) {
            currentSchemaCol->data_type = type;
            currentSchemaCol->is_primary_key = mod;
        }
        else {
            currentSchemaCol->data_type = type;
            currentSchemaCol->char_size = mod;
        }
        currentSchemaCol->next = malloc(sizeof(column_t));
        memset(currentSchemaCol->next, 0, sizeof(column_t));
        currentSchemaCol = currentSchemaCol->next;
        currentSchemaCol->next = NULL;
    }
    fclose(fd);

    // write data
    fd = fopen(tableFileName, "a");
    if (fd == NULL) {
        perror("Couldn't open table file");
        return -1;
    }

    column_t* reqCol = req->columns;
    currentSchemaCol = schemaCol;
    char row[512];
    char* rowEnd = row;
    memset(row, 0, 512);
    while (reqCol != NULL) {
        if (currentSchemaCol->data_type != reqCol->data_type) {
            perror("Wrong format for insert");
            return -1;
        }
        
        if (reqCol->data_type == DT_INT) {
            rowEnd += sprintf(rowEnd, "%i ", reqCol->int_val);
        }
        else {
            char record[128] = { 0 };
            
            for (int i = 0; reqCol->char_val[i + 1] != '\''; i++) {
                record[i] = reqCol->char_val[i + 1];
            }
            rowEnd += snprintf(rowEnd, currentSchemaCol->char_size + 1, "%s ", record);
        }

        reqCol = reqCol->next;
        currentSchemaCol = currentSchemaCol->next;
    }
    
    fprintf(fd, "%s\n", row);
    fclose(fd);

    // clean up schema format info
    while (schemaCol != NULL)
    {
        column_t* next = schemaCol->next;
        if (schemaCol->name != NULL) {
            free(schemaCol->name);
        }
        free(schemaCol);
        schemaCol = next;
    }
}

char* selectRecord(const request_t* req) {
    char tableFileName[256];
    makeTableFileName(req, tableFileName);
    FILE* fd = fopen(tableFileName, "r");
    if (fd == NULL) {
        perror("Couldn't open table file");
    }
    
    fseek(fd, 0, SEEK_END);
    long fileLength = ftell(fd);
    char* returnString = malloc(fileLength + 1);
    memset(returnString, 0, fileLength + 1);
    fseek(fd, 0, SEEK_SET);
    
    if (req->columns == NULL) {
        // select all
        fread(returnString, 1, fileLength, fd);
    }
    else {
        perror("Feature not supported yet");
    }
    fclose(fd);

    return returnString;
}
