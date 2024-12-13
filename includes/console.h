#pragma once
#include "hashtable.h"
#include "database.h"

typedef struct {
    HashTable* table;
    Database* db;
    char* key;
} ConsoleData;

ConsoleData* console_init(Database* db, HashTable* table, const char* key);
void console_free(ConsoleData* cb);
void console_header();
void console_menu();
void console_clear();
void console_sub_list(ConsoleData* cb);
void console_sub_add(ConsoleData* cb);
void console_catch(ConsoleData* cb, char command);