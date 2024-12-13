#pragma once
#define HASH_SIZE 1024
#include "student.h"

typedef struct HashNode {
    Student* stu;
    struct HashNode* next;
} HashNode;

typedef struct {
    HashNode* nodes[HASH_SIZE];
    StudentList* list;
    uint size;
} HashTable;

HashNode* hashnode_create(Student* stu);
HashTable* hashtable_create();
bool hashtable_add(HashTable* table, Student* stu);
HashNode* hashtable_find_custom(HashTable* table, Student* stu, void* custom);
HashNode* hashtable_find_id(HashTable* table, uint id);
bool hashtable_delete(HashTable* table, uint id);