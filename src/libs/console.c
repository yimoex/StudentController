#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "../../includes/public.h"
#include "../../includes/database.h"
#include "../../includes/mem.h"
#include "../../includes/student.h"
#include "../../includes/hashtable.h"
#include "../../includes/console.h"

ConsoleData* console_init(Database* db, HashTable* table, const char* key){
    #ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    #endif
    ConsoleData* cb = (ConsoleData *)safe_malloc(sizeof(ConsoleData));
    cb -> db = db;
    cb -> table = table;
    cb -> key = (char *)safe_malloc(sizeof(char) * (strlen(key) + 2));
    strcpy(cb -> key, key);
    return cb;
}

void console_free(ConsoleData* cb){
    free(cb -> key);
    free(cb);
}

void console_header(){
    printf("===Student Controller===\n");
}

void console_menu(){
    printf("1. 查看学生列表\n");
    printf("2. 新增学生\n");
    printf("3. 删除学生\n");
    printf("4. 查找学生\n");
    printf("5. 导出学生\n");
}

void console_clear(){
    printf("\033[H\033[J");
}

void console_sub_list(ConsoleData* cb){
    StudentList* list = cb -> table -> list;
    int size = list -> size;
    int id = -1;
    HashNode* node;
    printf("[管理系统] 查找到 %d 个记录\n", size);
    for(int i = 0;i < size;i++){
        id = list -> ids[i];
        if(id == -1){
            size++;
            continue;
        }
        node = hashtable_find_id(cb -> table, id);
        printf(" - [%s](%d)\n", node -> stu -> name, node -> stu -> id);
    }
}

void console_sub_add(ConsoleData* cb){
    uint id, phone, command;
    char name[16];
    HashNode* node;

    printf("[管理系统] 开始注册学生,请输入要注册的学号: ");
    scanf("%d", &id);
    node = hashtable_find_id(cb -> table, id);
    if(node != NULL){
        printf("[管理系统] 已经存在学号为 %d 的学生了!\n", id);
        return;
    }


    printf("请输入学生的姓名(最多4个汉字): ");
    scanf("%12s", name);

    printf("请输入学生的手机号: ");
    scanf("%d", &phone);
    printf("[管理系统] 信息录入完成,开始校验!\n");
    printf("==注册的学生个人信息==\n学生姓名: %s\n学号: %d\n手机号: %d\n", name, id, phone);
    printf("[管理系统] 在确认后输入1以录入系统: ");
    scanf("%d", &command);
    if(command != 1) return;
    Student* stu = student_create(id, name, phone);
    if(hashtable_add(cb -> table, stu)){
        printf("[管理系统] 学生添加成功!\n");
        database_export(cb -> db, cb -> table, cb -> key);
    }else{
        printf("[管理系统] 学生添加失败!\n");
    }
}

void console_sub_find(ConsoleData* cb){
    int id;
    HashNode* node;
    printf("[管理系统] 请输入要查找的学生的学号: ");
    scanf("%d", &id);
    node = hashtable_find_id(cb -> table, id);
    if(node == NULL){
        printf("[管理系统] 找不到ID为 [%d] 学生\n", id);
        return;
    }
    printf("==学生个人信息==\n学生姓名: %s\n学号: %d\n手机号: %d\n", node -> stu -> name, node -> stu -> id, node -> stu -> phone);
}

void console_sub_delete(ConsoleData* cb){
    int id;
    printf("[管理系统] 请输入要删除的学号: ");
    scanf("%d", &id);
    HashNode* node = hashtable_find_id(cb -> table, id);
    if(node == NULL){
        printf("[管理系统] 并没有找到该学生!\n");
        return;
    }
    int yes;
    printf("确定要删除 [%d](%s) 吗? (输入1确认): ", id, node -> stu -> name);
    scanf("%d", &yes);
    if(yes == 1){
        if(hashtable_delete(cb -> table, id)){
            printf("[管理系统] 删除成功!自动保存数据库ing\n");
            database_export(cb -> db, cb -> table, cb -> key);
        }else{
            printf("[管理系统] 删除失败!\n");
        }
    }
}

void console_catch(ConsoleData* cb, char command){
    if(command == '1'){ //列表
        console_sub_list(cb);
    }else if(command == '2'){
        console_sub_add(cb);
    }else if(command == '3'){
        console_sub_delete(cb);
    }else if(command == '4'){
        console_sub_find(cb);
    }else if(command == '5'){
        printf(database_export(cb -> db, cb -> table, cb -> key) ? "导出成功!\n" : "导出失败");
    }
}
