#include <string.h>
#include "../includes/public.h"
#include "../includes/student.h"
#include "../includes/mem.h"

Student* student_create(uint id, const char* name, uint phone){
    Student* stu = (Student *)safe_malloc(sizeof(Student));
    stu -> id = id;
    strcpy(stu -> name, name);
    stu -> phone = phone;
    return stu;
}

Student* student_delete(Student* stu){
    safe_free(stu);
}

StudentList* student_list_create(){
    StudentList* list = (StudentList *)safe_malloc(sizeof(StudentList));
    list -> size = 0;
    return list;
}

void student_list_new(StudentList* list, uint sid){
    list -> ids[list -> size] = sid;
    list -> size++;
    return;
}

bool student_list_delete(StudentList* list, uint sid){
    uint ptr = 0;
    while(1){
        if(ptr == list -> size) return false;
        if(sid == list -> ids[ptr++]) break;
    }
    list -> ids[ptr - 1] = -1;
    list -> size--;
    return true;
}

