#pragma once
#define STUDENT_DATABASE "database/student.db"
#define ID_MAX_LENGTH 9
#define NAME_MAX_LENGTH 12
#define PHONE_MAX_LENGTH 11

#include <stdio.h>
#include "hashtable.h"

typedef struct {
    FILE* fp;
} Database;

Database* database_init();
bool database_export(Database* db, HashTable* table, const char* key);
bool database_import(Database* db, HashTable* table, const char* key);