#ifndef _STRUCTURE_H
#define _STRUCTURE_H
#include "Algorthm.h"
/*
 * 定义哈希表的接口
 */

//哈希表初始化
void HashTableInitialize(HashTable * T);
//哈希表插入操作
void HashTableInsert(HashTable * T, unsigned KeyValue, CostType Cost);
//哈希表搜索
int HashTableSearch(HashTable * T, unsigned KeyValue, CostType Cost);

/*
 * 定义进行边交换的宏操作
 */
#define PRED(a) (IsPathReversed == (a)->Parent->IsPathReversed ? (a)->Pred : (a)->Suc)
#define SUC(a) (IsPathReversed == (a)->Parent->IsPathReversed ? (a)->Suc : (a)->Pred)
void FLIP(Node * N1, Node * N2, Node * N3);
#define SWAP1(a1,a2,a3)\
        FLIP(a1,a2,a3)
#define SWAP2(a1,a2,a3, b1,b2,b3)\
        (SWAP1(a1,a2,a3), SWAP1(b1,b2,b3))
#define SWAP3(a1,a2,a3, b1,b2,b3, c1,c2,c3)\
        (SWAP2(a1,a2,a3, b1,b2,b3), SWAP1(c1,c2,c3))
#define SWAP4(a1,a2,a3, b1,b2,b3, c1,c2,c3, d1,d2,d3)\
        (SWAP3(a1,a2,a3, b1,b2,b3, c1,c2,c3), SWAP1(d1,d2,d3))


/*
 * 定义堆的使用接口
 */

//创建堆
void HeapBuild(int Size);
//删除最小的节点
Node *HeapDeleteMin(void);
//插入节点操作
void HeapInsert(Node * N);
//堆中节点上移操作
void HeapSiftUp(Node * N);
//节点下移操作
void HeapSiftDown(Node * N);

/*
 * 定义操作节点序列需使用的变量
 */
extern Node **t;
extern Node **T;
extern Node **tSaved;
extern int *p;

extern int *q;
extern int *incl;
extern int *cycle;
extern CostType *G;
extern int K;
#endif
