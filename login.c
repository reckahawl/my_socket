#include "config.h"

extern ListNode *userList;

int loginUser(Message *msg, int sockfd){
    int ret;

    User user;
    char buf[MAX_LINE];

    sqlite3 *db;
    sqlite3_stmt *stmt;
    const char *tail;

    char sql[128];

    Message message;

    recv(sockfd , buf , sizeof(user) , 0);
	memset(&user , 0 , sizeof(user));
	memcpy(&user , buf , sizeof(buf));
	user.userAddr = (*msg).sendAddr;
	user.sockfd = sockfd;


    if(isOnLine(userList , &user) == 1) return ALREADY_ONLINE;
    /*1*/
    ret = sqlite3_open(DB_NAME, &db);
    if(ret != SQLITE_OK) return FAILED;

    /*2*/
    memset(sql , 0 , sizeof(sql));
	sprintf(sql , "select * from User where userName='%s' and password='%s';",user.userName , user.password);
	
	ret = sqlite3_prepare(db , sql , strlen(sql) , &stmt , &tail);	
	if(ret != SQLITE_OK)
	{
		ret = sqlite3_step(stmt);
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		printf("database select fail!\n");
		return FAILED;		
	}//if

    ret = sqlite3_step(stmt);
    while(ret == SQLITE_ROW){
        ret = sqlite3_step(stmt);
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		ret = SUCCESS;

        userList = insertNode(userList, &user);
        return ret;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return FAILED;
}