#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "../includes/hashtable.h"
#include "../includes/database.h"

Database* all_db;
HashTable* all_table;
const char* all_key;

void call_fatel(const char* message){
    printf("[Fatel]: %s\n", message);
    exit(0);
}

void call_warn(const char* message){
    printf("[Warn]: %s\n", message);
}

void call_safe_error(const char* message){
    write(STDOUT_FILENO, message, strlen(message));
    write(STDOUT_FILENO, "\n", 1);
    exit(0);
}

void signal_handle(int sig){
    if(sig == SIGSEGV) call_safe_error("Error => 尝试无效内存地址");
    if(sig == SIGFPE) call_safe_error("Error => 浮点异常");
    if(sig == SIGILL) call_safe_error("Error => 未定义非法行为");
    if(sig == SIGABRT) call_safe_error("Error => 异常终止");
    if(sig == SIGINT || sig == SIGTERM){
        database_export(all_db, all_table, all_key);
        call_safe_error("[System] 已自动完成数据导出!");
    }
    exit(0);
}

void signal_init(Database* db, HashTable* table, const char* key){
    all_db = db;
    all_table = table;
    all_key = key;
    signal(SIGSEGV, signal_handle);
    signal(SIGFPE, signal_handle);
    signal(SIGILL, signal_handle);
    signal(SIGABRT, signal_handle);
    signal(SIGINT, signal_handle);
    signal(SIGTERM, signal_handle);
}
