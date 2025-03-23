#include "config.h"
ListNode *insertNode(ListNode *list, User *user){
    ListNode *node = (ListNode *)calloc(1, sizeof(ListNode));

    copyUser(&(node->user) , user);

    node->next = NULL;
    if(list == NULL){
        list = node;
    }else{
        ListNode *p = list;
        while(p->next != NULL){
            p=p->next;
        }//while
        p->next = node;
    }//else
    printf("更新在线\n");
    return list;
}

int isOnLine(ListNode *list , User *user)
{
	ListNode *p = list , *pre = p;
	while(p!=NULL && strcmp(p->user.userName , (*user).userName) != 0)
	{
		pre = p;
		p = p->next;
	}//while
	
	if(p == NULL)
		return 0;
	return 1;
}

void deleteNode(ListNode *list , User *user)
{
	if(list == NULL)
		return;

	ListNode *p = list , *pre = p;
	while(p!=NULL && strcmp(p->user.userName , (*user).userName) != 0)
	{
		pre = p;
		p = p->next;
	}//while
	
	if(p == NULL)
		return ;
	else if(p == list)
	{
		list = list->next;
	}//elif
	else if(p->next == NULL)
	{
		pre->next = NULL;
	}//elif
	else
	{
		pre->next = p->next;
	}//else
	free(p);
	p = NULL;
}
void displayList(ListNode *list)
{
	if(list == NULL)
		return;
	else
	{
		ListNode *p = list;
		while(p->next != NULL)
		{
			printf("%s --> ", p->user.userName);
			p = p->next;
		}//while
		printf("%s\n", p->user.userName);
	}//else
}
