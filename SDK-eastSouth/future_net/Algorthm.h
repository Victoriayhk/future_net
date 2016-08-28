#ifndef _ALGORTHM_H
#define _ALGORTHM_H

/*该头文件将被算法使用的绝大多数函数引用,它定义了算法
 * 需要的宏和数据结构及函数原型
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <limits.h>


typedef long long  CostType;
#define INFINITY 99999999
#define MINUS_INFINITY -99999999
/*相关宏定义 */

#define Fixed(a, b) ((a)->PointTo1 == (b) || (a)->FixedTo2 == (b))
#define FixedOrCommon(a, b) (Fixed(a, b) )
#define InBestTour(a, b) ((a)->BestSuc == (b) || (b)->BestSuc == (a))
#define InNextBestTour(a, b)\
    ((a)->NextBestSuc == (b) || (b)->NextBestSuc == (a))
#define Near(a, b)\
    ((a)->BestSuc ? InBestTour(a, b) : (a)->Dad == (b) || (b)->Dad == (a))

#define Link(a, b) { ((a)->Suc = (b))->Pred = (a); }
#define Follow(b, a)\
    { Link((b)->Pred, (b)->Suc); Link(b, b); Link(b, (a)->Suc); Link(a, b); }
#define Precede(a, b)\
    { Link((a)->Pred, (a)->Suc); Link(a, a); Link((b)->Pred, a); Link(a, b); }
#define SLink(a, b) { (a)->Suc = (b); (b)->Pred = (a); }

typedef struct Node Node;
typedef struct PromisedEdges PromisedEdges;
typedef struct Segment Segment;
typedef struct SSegment SSegment;
typedef struct RecordOfExchange RecordOfExchange;
typedef int (*CostFunction) (Node * Na, Node * Nb);

#define HashTableSize 65521
#define MAXLoadFactor 0.75
//哈希表词条数据结构定义
typedef struct HashTableEntry {
    unsigned KeyValue;
    CostType Cost;
} HashTableEntry;
//哈希表结构
typedef struct HashTable {
    HashTableEntry Entry[HashTableSize];
    int Count;
} HashTable;


/* 节点的数据存储结构 */
struct Node {
    int Id;     /* 节点ID*/
    int Location;    /*节点在堆中的位置*/
    int Priority;   /* 子梯度上升算法节点的优先级*/
    int Var;      // 节点的度减2
    int LastVar;  //上一次的Var值
    int Cost;   //节点"最好的"的边的权值
    int NextCost; //与节点相邻的次好的边的权值
    int PredCost,
        SucCost;
    int SavedCost;
    int Pi;     /* 节点的Pi值 */
    int BestPi; /* 当前找到的最优的Pi值 */
    int BValue;
    int Sons;   /*最小生成树中节点的子节点数 */
    int *Row;     /* 权值矩阵的一行 */
    Node *Pred, *Suc; // 双向链表中节点的前驱和后继节点
    Node *OldPred, *OldSuc; //上一次的前驱和后继节点
    Node *BestSuc,     //当前最优的后继节点
         *NextBestSuc; //当前次优的后继节点
    Node *Dad;  //最小1树中节点的父节点
    Node *Next; //指向该节点的下一个节点的指针
    Node *Prev; //指向该节点前一个节点的指针
    Node *Visited; //访问标记
    Node *PointTo1,   //指向边的另一个顶点
         *FixedTo2;
    Node *FixedTo1Saved,
         *FixedTo2Saved;
    Node *Head;
    Node *Tail;
    Node *Added1, *Added2;

    Node *Deleted1, *Deleted2;
    PromisedEdges *PromisedEdgesSet;    //候选边集合
    Segment *Parent; //一个节点的父段


    char OldPredExcluded, OldSucExcluded;  //指示两个相邻顶点是否被排除在最短路径之外
};

/* 候选边的数据结构*/

struct PromisedEdges {
    Node *To;  //边的终点
    int Cost;   //边的权值
    int Alpha;  //alpha值
};

/* 路径的段结构表示 */

struct Segment {
    char IsPathReversed;     //段是否翻转
    Node *First, *Last;//段中的第一个和最后一个节点
    Segment *Pred, *Suc; //双向链表中指向当前段的前驱和后继段的指针
    int Priority;   //当前段在链表中的优先级
    int Size;   //段中节点数量
    SSegment *Parent;    //大段的父段
};

struct SSegment {
    char IsPathReversed;        //段是否翻转
    Segment *First, *Last; //段中的第一个和最后一个节点
    SSegment *Pred, *Suc;  //双向链表中指向当前段的前驱和后继段的指针
    int Priority;   //当前段在链表中的优先级
    int Size;   //段中节点的数量
};

/* 记录2-opt交换的数据结构 */

struct RecordOfExchange {
    Node *t1, *t2, *t3, *t4;  //2-opt交换中用到的四个节点
};

extern int SelectedEdgesNum;   //每个节点的候选边的数量
extern CostType MinimumCost;      //最好路径的权值
extern int *BestPath;  //当前找到的最好路径
extern CostType BetterCost;    //较好路径的权值
extern int *BetterPath;    // 较好的路径
extern int CacheMask;
extern int *CacheVal;
extern int *SavedFlag;

extern int *CostMatrix;   //权值矩阵
extern int Dim;                //节点个数
extern int SavedDim;    //保存的节点个数
extern double Ex;
extern Node *FirstActive, *LastActive; //激活节点集合的第一个和最后一个顶点
extern Node *FirstNode;      //链表中的第一个节点
extern Segment *FirstSegment;  //指向第一个段的指针
extern SSegment *FirstSSegment;    //指向第一个大段的指针

extern int SegSize;
extern int SSegmentSize;
extern int SegmentNums;    //段的数量
extern int SSegmentNums;   //大段的数量
extern unsigned KeyValue;  //当前路径的哈希值
extern Node **Heap;    //用于计算最小生成树的堆
extern HashTable *HTable;      //哈希表用于存储路径信息
extern int InitIterationTimes;      //子梯度上升算法的初始迭代次数
extern int StepLength;    //子梯度上升算法的初始步长

extern double LB;      //子梯度上升算法得到的下界
extern int M;      //将ATSP问题转化为STSP时使用的参数
extern int MAXWidth; //为进行一次有效边交换的最大搜索次数
extern int MAXSelectedEdgesNum;    //每个节点允许的最多节点数
extern int MAXExchanges;   //最大可交换次数
extern int MAXTests;  //算法一次运行的最多尝试次数
extern int ExchangeType;   //定义使用的边交换类型
extern Node *NodeSet;  //节点数组
extern int Discrepancy;
extern CostType OptimalCost;     //已知的最小权值
extern int Accuracy;  //内部计算精度
extern int IsCostAvailable;
extern unsigned *RandomNumSet; //存放随机数的数组
extern short IsPathReversed;
extern int Run; /* 当前运行次数 */
extern int RunTimes;       /* 运行总次数 */
extern unsigned RandNum;  /* 生成随机数的种子*/
extern RecordOfExchange *SwapStack;  /* 存储边交换记录的栈 */
extern int ExchangeTimes;      /* 边交换次数 */
extern int Test;      /* 当前尝试的次数 */



extern CostFunction DISTANCE, COST, c;   //查找权值函数的指针
/*函数原型声明*/
int D_FUNCTION(Node * Na, Node * Nb);
int C_FUNCTION(Node * Na, Node * Nb);
void ReadPathMatrix(int **WeightMatrix);
CostType CombineCurrentPathWithBestPath();
void BuildDataStructure(void);
void FindPromisingEdges(void);
CostType FindTour(void);
void InitParameters(void);
void SaveBestResult(void);
void SaveBetterResult(void);


int Forbidden(const Node * ta, const Node * tb);
void FreeCandidateSets(void);
void ReleaseAll(void);
void RestoreTour(void);
int SegmentSize(Node *ta, Node *tb);

void Make3OptMove(Node * t1, Node * t2, Node * t3, Node * t4,
                  Node * t5, Node * t6, int Case);
void Make4OptMove(Node * t1, Node * t2, Node * t3, Node * t4,
                  Node * t5, Node * t6, Node * t7, Node * t8,
                  int Case);

#endif
