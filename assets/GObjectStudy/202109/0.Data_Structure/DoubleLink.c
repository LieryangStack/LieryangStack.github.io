#include<stdlib.h>
#include<stdio.h>

typedef struct line{
  struct line *prior;
  struct line *next;
  int data;
}line;


int
main(){


  return 0;
}

line* 
initLine(line *head){
  head = (line*)malloc(sizeof(line));/*创建链表第一个结点(首元结点)*/
  head->prior = NULL;
  head->next = NULL;
  head->data = 1;

  line *list = head;
  for(int i = 2; i <=3; i++){
    /*创建并初始化一个新结点*/
    line *body = (line*)malloc(sizeof(line));
    body->prior = NULL;
    body->next = NULL;
    body->data = i;

    list->next = body;
    body->prior = list;
    list = list->next;
  }

  return head;
}