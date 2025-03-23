#include "config.h"

int registerUser(Message *msg, int sockfd){
    int ret;

    User user;
    char buf[MAX_LINE];

    sqlite3 *db;
    sqlite3_stmt *stmt;
    const char *tail;

    char sql[128];

    time_t timeNow;

    Message message;

    recv(sockfd, buf, sizeof user, 0);
    memset(&user, 0, sizeof user);
    memcpy(&user, buf, sizeof buf);
    user.userAddr = (*msg).sendAddr;
    user.sockfd = sockfd;

    if(strlen(user.userName) > 20) return INVALID;

    /*1*/
    ret = sqlite3_open(DB_NAME, &db);
    if(ret != SQLITE_OK){
		printf("unable open database.\n");
		return FAILED;
	}//if
	printf("Opened database successfully.\n");

    /*2*/
    memset(sql, 0, sizeof sql);
    sprintf(sql , "select * from User where userName='%s';",(user.userName));

	ret = sqlite3_prepare(db , sql , strlen(sql) , &stmt , &tail);	
	if(ret != SQLITE_OK){
		ret = sqlite3_step(stmt);
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		printf("database select fail!\n");
		return FAILED;
	}//if
    ret = sqlite3_step(stmt);
    while (ret == SQLITE_ROW){
        ret = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return FAILED;
    }
    sqlite3_finalize(stmt);

    	memset(sql , 0 , sizeof(sql));
	time(&timeNow);
	sprintf(sql , "insert into User(userName , password , userAddr , sockfd , speak , registerTime)\
			values('%s','%s','%s',%d, %d , '%s');",user.userName , user.password , 
			inet_ntoa(user.userAddr.sin_addr),user.sockfd , YES, asctime(gmtime(&timeNow)));

	ret = sqlite3_prepare(db , sql , strlen(sql) , &stmt , &tail);	
	if(ret != SQLITE_OK)
	{
		ret = sqlite3_step(stmt);
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return FAILED;
	}//if
    	ret = sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	sqlite3_close(db);
        /* code */
    return SUCCESS;
}