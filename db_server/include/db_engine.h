#ifndef DB_ENGINE_H
#define DB_ENGINE_H

#include "request.h"

int initDB();
void exitDB();

int createTable(const request_t*);
int deleteTable(const request_t*);
int listTables(const request_t*);
int printSchema(const request_t*);
int insertRecord(const request_t*);

#endif