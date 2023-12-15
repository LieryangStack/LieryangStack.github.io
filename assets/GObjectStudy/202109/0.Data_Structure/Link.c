#include<stdio.h>
#include<stdlib.h>
/**
 * 单链表中数据元素由两部分组成，这两部分信息组成数据元素的存储结构称之为“结点”
 * （1）数据域
 * （2）指针域
 * n个节点通过指针域链接，组成一个链表
*/
typedef struct Link{
  char data;/*代表数据域*/
  struct Link *next;/*代表指针域，指向直接后继元素*/
}link;

/* 创建链表 */
link * initLink(); 
/* 查找结点的函数，data为目标结点的数据域的值 */
int selectData(link *p, int data);
/* 更新结点的函数，newData为新的数据 */
link * amendData(link *p, int add, int newData);
/* 插入结点 */
link * insertDta(link *p, int data, int add);
link * delData(link *p, int add);

int
main(){
  /* 初始化链表( 1, 2, 3, 4) */
  /**
   *  头指针(p)
   *    |
   *  头结点 -> 数据1 -> 数据2 -> 数据3 -> 数据4
   * 
   * 单链表可以没有头结点，但是不是没有头指针p
  */
  link *p = initLink();/*该初始化链表有头结点*/
  insertDta(p, 8, 5);
  p = amendData(p, 3, 13);
  int num = selectData(p, 5);

  printf("num = %d\n", num);

return 0;
}

/*链表的创建和遍历*/
link * 
initLink(){
  link *p = (link*)malloc(sizeof(link));/*创建头结点*/
  link * temp = p;/*声明一个指针指向头结点，用于遍历链表*/

  /*生成链表*/
  for(int i = 1; i < 5; i++){
    link *a = (link*)malloc(sizeof(link));
    a->data = i;
    a->next = NULL;
    temp->next = a;/*头节点的指针指向下一个节点a*/
    temp = temp->next;/*同时，临时指针移动到下一个节点，即a*/
  }

  return p;
}

/*链表中查找某结点*/
int 
selectData(link *p, int data){
  link *t = p;/*创建一个临时link*/
  int i = 1;
  while(t->next){
    t = t->next;/*因为有头结点，所以要先向前移动*/
    if(i == data){
      return t->data;
    }
    i++;
  }
  return -1;
}

/**
 * 链表中更改某结点的数据域
 * add 表示更改结点在链表中的位置
 * newData 为新的数据域的值
 * */
link *
amendData(link *p, int add, int newData){
  link *temp = p;
  temp = temp->next;/*遍历之前，temp指向首元结点*/
  for(int i = 1; i < add; i++){
    temp = temp -> next;
  }
  temp->data = newData;
  return p;
}

/*插入结点*/
link*
insertDta(link *p, int data, int add){
  link *temp = p;/*创建临时结点temp*/
  /*首先要找到插入位置的上一个结点*/
  for(int i = 1; i < add; i++){
    if(temp==NULL){
      printf("insert invaild\n");
      return p;
    }
    temp = temp-> next;
  }

  /*创建插入结点c*/
  link *c = (link*)malloc(sizeof(link));
  c->data = data;
  /*向链表中插入结点*/
  c->next = temp->next;
  temp->next = c;
  return p;
}

/*删除结点*/
link* 
delData(link *p, int add){
  link *temp = p;
  for(int i = 1; i < add; i++){
    temp = temp->next;
  }
  link *del = temp->next;
  temp->next = temp->next->next;
  free(del);/*手动释放该节点，防止内存泄漏*/
  return p;
}


