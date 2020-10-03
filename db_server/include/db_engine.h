#ifndef DB_ENGINE_H
#define DB_ENGINE_H

#include "request.h"

int initDB();
void exitDB();

int createTable(request_t*);
int deleteTable(request_t*);
int listTables(request_t*);
int printSchema(request_t*);
int insertRecord(request_t*);

#endif