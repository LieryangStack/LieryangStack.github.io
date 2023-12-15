/* file name : double-list.h */
#ifndef DOUBLE_LIST_H
#define DOUBLE_LIST_H

/*
struct double_list_node{
    struct double_list_node *prev;
    struct double_list_node *next;
    void *data;
};

struct double_list{
    struct double_list_node *head;
    struct double_list_node *tail;
};
*/

/*通过用typedef定义数据类型，避免重复去写 struct double_list_XXXX */
typedef struct _DListNode DListNode;
struct _DListNode{
    DListNode *prev;
    DListNode *next;
    void *data;    
};

typedef struct _DList DList;
struct _DList{
    DListNode *head;
    DListNode *tail;
};



#endif