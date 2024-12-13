#pragma once
#define ENABLE_STUDENT 1
#define STUDENT_LIST_SIZE 1024
#include "public.h"

typedef struct {
    usint id;
    char name[32];
    usint phone;
} Student;

typedef struct {
    uint size;
    uint ids[STUDENT_LIST_SIZE];
} StudentList;

Student* student_create(uint id, const char* name, uint phone);
Student* student_delete(Student* stu);
StudentList* student_list_create();
void student_list_new(StudentList* list, uint sid);
bool student_list_delete(StudentList* list, uint sid);