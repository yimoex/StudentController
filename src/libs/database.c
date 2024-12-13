#include <stdio.h>
#include <string.h>
#include "../includes/public.h"
#include "../includes/hashtable.h"
#include "../includes/student.h"
#include "../includes/database.h"
#include "../includes/error.h"
#include "../includes/buffer.h"
#include "../includes/convert.h"
#include "../includes/mem.h"
#include "../platform/encoder/encoder.h"

Database* database_init(){
    Database* db = (Database *)safe_malloc(sizeof(Database));
    FILE* fp = fopen(STUDENT_DATABASE, "r+");
    if(fp == NULL) return NULL;
    db -> fp = fp;
    return db;
}

void database_buffer_write(Buffer* buf, uint allLength, const char* data, uint realSize, const char* errorMessage){
    if(realSize > allLength) call_fatel(errorMessage);
    uint rec, fillSize;
    if(realSize < allLength){
        fillSize = (allLength - realSize); //需要前补的长度
        buffer_add(buf, "00000000000000000000", fillSize);
    }
    buffer_add(buf, data, realSize);
}

bool database_export(Database* db, HashTable* table, const char* key){
    fclose(db -> fp);
    FILE* fp;
    db -> fp = fp = fopen(STUDENT_DATABASE, "r+");
    StudentList* list = table -> list;
    uint id, size = list -> size, rawLen, length = 0, i;
    Student* student;
    MemPool* pool = mem_pool_create();
    Buffer* buffer = buffer_create();
    char* string;
    buffer_write(buffer, "yimoex");
    string = uint2string(pool, list -> size, &rawLen);
    database_buffer_write(buffer, 4, string, rawLen, "无法写入数据库大小!");
    printf("[Process] 开始导出数据库\n");
    for(i = 0;i < size;i++){
        uint id = list -> ids[i];
        if(id == -1){
            size++;
            continue;
        }
        student = (hashtable_find_id(table, id)) -> stu;
        printf("[Process] 正在导出(%d) %s \n", i + 1, student -> name);
        //ID部分
        string = uint2string(pool, student -> id, &rawLen);
        database_buffer_write(buffer, ID_MAX_LENGTH, string, rawLen, "无法储存超过9位的ID");
        //名字部分
        database_buffer_write(buffer, NAME_MAX_LENGTH, student -> name, strlen(student -> name), "无法储存超过4个字的名字");
        //手机号部分
        string = uint2string(pool, student -> phone, &rawLen);
        database_buffer_write(buffer, PHONE_MAX_LENGTH, string, rawLen, "无法储存超过11个字符的手机号");
    }
    printf("\n");
    Buffer* new = encoder_buffer_run(buffer, key, true, 0);
    uint rec = buffer_to_file(new, fp);

    buffer_delete(buffer);
    buffer_delete(new);
    mem_pool_free(pool);
    fclose(fp);
    return rec;
}

void database_name_pow(char* name, char* dest){
    uint ptr = 0, i;
    for(i = 0;i < NAME_MAX_LENGTH;i+=3){
        if(name[i] == '0') continue;
        dest[ptr] = name[i];
        dest[ptr + 1] = name[i + 1];
        dest[ptr + 2] = name[i + 2];
        ptr += 3;
    }
    dest[ptr] = '\0';
}

bool database_import(Database* db, HashTable* table, const char* key){
    printf("[Logs]: 开始导入数据库!\n");
    FILE* fp = db -> fp;
    Student* student;
    fseek(fp, 0, SEEK_SET);
    MemPool* pool = mem_pool_create();
    Buffer* buffer = buffer_create(), *realBuffer;
    
    //读取数据头-标识符
    if(!buffer_fread(buffer, 6, 1, fp)) call_fatel("读取失败!");
    realBuffer = encoder_buffer_run(buffer, key, false, 0);
    if(!buffer_is(realBuffer, "yimoex")) call_fatel("无法读取数据库文件\n1. 无法读取非本程序驱动数据库类型\n2. 数据库密钥错误");
    buffer_delete(realBuffer);
    printf("[Logs]: 专属数据库认证成功!\n");

    //读取数据头-大小
    if(!buffer_fread(buffer, 4, 1, fp)) call_fatel("数据大小读取失败!");
    realBuffer = encoder_buffer_run(buffer, key, false, 6);
    uint length = string2uint(realBuffer -> ptr);
    printf("[Logs]: 读取数据库大小: %d\n", length);

    uint id, phone, i;
    Buffer* bid, *bname, *bphone;
    char name[NAME_MAX_LENGTH + 1];
    int bias = 10; //数据头偏移量
    for(i = 0;i < length;i++){
        //读入ID
        if(!buffer_fread(buffer, ID_MAX_LENGTH, 1, fp)) call_fatel("ID读取失败!");
        bid = encoder_buffer_run(buffer, key, false, bias);
        id = string2uint(bid -> ptr);
        bias += ID_MAX_LENGTH;
        //读入名字
        if(!buffer_fread(buffer, NAME_MAX_LENGTH, 1, fp)) call_fatel("名称读取失败!");
        bname = encoder_buffer_run(buffer, key, false, bias);
        database_name_pow(bname -> ptr, name);
        bias += NAME_MAX_LENGTH;
        //读入手机号
        if(!buffer_fread(buffer, PHONE_MAX_LENGTH, 1, fp)) call_fatel("ID读取失败!");
        bphone = encoder_buffer_run(buffer, key, false, bias);
        phone = string2uint(bphone -> ptr);
        bias += PHONE_MAX_LENGTH;

        student = student_create(id, name, phone);
        hashtable_add(table, student);

        buffer_delete(bid);
        buffer_delete(bname);
        buffer_delete(bphone);
    }
    buffer_delete(buffer);
    mem_pool_free(pool);
    fclose(fp);
    printf("[Logs] 已读取 %d 个学生记录!\n", length);
    return true;

}

