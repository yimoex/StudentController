#pragma once
#include "hashtable.h"
#include "database.h"

void call_fatel(const char* message);
void call_warn(const char* message);
void call_safe_error(const char* message);
void signal_init(Database* db, HashTable* table, const char* key);