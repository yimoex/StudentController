#include "../includes/public.h"
#include "../includes/hashtable.h"
#include "../includes/database.h"
#include "../includes/console.h"
#include "../includes/error.h"

int main(){
    char key[] = "hello";
    HashTable* table = hashtable_create();

    Database* db = database_init();
    ConsoleData* cb = console_init(db, table, key);
    if(db == NULL) printf("[Error]: 导出失败,无法打开数据库文件!\n");
    database_import(db, table, key);
    signal_init(db, table, key);

    char command, any;
    console_header();
    console_menu();
    while(1){
        printf("admin> ");
        scanf("%c", &command);
        console_catch(cb, command);
        scanf("%c", &any);
    }
    console_clear();
}
