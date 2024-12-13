#include <string.h>
#include "../includes/public.h"
#include "../includes/mem.h"
#include "../includes/hashtable.h"

uint string2hash(const char* name){
    uint rec = 0, ptr = 0, length = strlen(name);
    for(ptr = 0;ptr < length;ptr++) rec += name[ptr];
    return rec % HASH_SIZE;
}

uint id2hash(uint id){
    return id % HASH_SIZE;
}

HashNode* hashnode_create(Student* stu){
    HashNode* node = (HashNode *)safe_malloc(sizeof(HashNode));
    node -> stu = stu;
    node -> next = NULL;
    return node;
}

HashTable* hashtable_create(){
    HashTable* table = (HashTable *)safe_malloc(sizeof(HashTable));
    table -> list = student_list_create();
    table -> size = 0;
    return table;
}

bool hashtable_add(HashTable* table, Student* stu){
    uint hash = id2hash(stu -> id);
    HashNode* node = table -> nodes[hash];
    student_list_new(table -> list, stu -> id);
    if(node == NULL){
        node = hashnode_create(stu);
        table -> nodes[hash] = node;
        table -> size++;
        return true;
    }
    while(node -> next != NULL){
        node = node -> next;
    }
    node -> next = hashnode_create(stu);
    table -> size++;
    return true;
}

HashNode* hashtable_find_custom(HashTable* table, Student* stu, void* custom){
    uint hash = id2hash(stu -> id);
    HashNode* node = table -> nodes[hash];
    bool (*call_custom)(Student *) = custom;
    while(node != NULL){
        if(call_custom(stu)) return node;
        node = node -> next;
    }
    return NULL;
}

HashNode* hashtable_find_id(HashTable* table, uint id){
    uint hash = id2hash(id);
    HashNode* node = table -> nodes[hash];
    while(node != NULL){
        if(id == node -> stu -> id) return node;
        node = node -> next;
    }
    return NULL;
}

bool hashtable_delete(HashTable* table, uint id){
    uint hash = id2hash(id);
    HashNode* node = table -> nodes[hash], *last = NULL, *next = NULL;
    if(node == NULL) return false;
    while(node -> next != NULL){
        if(node -> next -> stu -> id == id) break;
        last = node;
        node = node -> next;
    }
    if(node -> stu -> id != id) return false;
    next = node -> next;
    safe_free(node);
    table -> size--;
    student_list_delete(table -> list, id);
    if(last == NULL){
        table -> nodes[hash] = next;
    }else{
        last -> next = next;
    }
    return true;
}

