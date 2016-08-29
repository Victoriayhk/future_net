#include "Structure.h"
#include "Algorthm.h"
#include <math.h>
/*
 *创建算法所需的存储及操作数据结构
 */

/*
 * 定义操作节点序列需使用的变量
 */
Node **t;
Node **T;
Node **tSaved;
int *p;

int *q;
int *incl;
int *cycle;
CostType *G;
int K;

#define Free(s) { free(s); s = 0; }
//申请分配结构体内存空间
void AllocateCallSegments();
void FreeSegSpace();
void FreeSegSpace();
void BuildDataStructure()
{
    int i, K;

    Free(Heap);
    Free(BestPath);
    Free(BetterPath);
    Free(HTable);
    Free(RandomNumSet);
    Free(SavedFlag);
    Free(CacheVal);
    Free(T);
    Free(G);
    Free(t);
    Free(p);
    Free(q);
    Free(SwapStack);
    Free(tSaved);

    HeapBuild(Dim);
    assert(BestPath = (int *) calloc(1 + Dim, sizeof(int)));
    assert(BetterPath = (int *) calloc(1 + Dim, sizeof(int)));
    assert(HTable = (HashTable *) malloc(sizeof(HashTable)));
    HashTableInitialize((HashTable *) HTable);
    srand(RandNum);
    assert(RandomNumSet = (unsigned *)
           malloc((Dim + 1) * sizeof(unsigned)));
    for (i = 1; i <= Dim; i++)
        RandomNumSet[i] = rand();
    srand(RandNum);
    AllocateCallSegments();
    K = ExchangeType;
    assert(T = (Node **) malloc((1 + 2 * K) * sizeof(Node *)));
    assert(G = (CostType *) malloc(2 * K * sizeof(CostType)));
    assert(t = (Node **) malloc(6 * K * sizeof(Node *)));
    assert(tSaved = (Node **) malloc((1 + 2 * K) * sizeof(Node *)));
    assert(p = (int *) malloc(6 * K * sizeof(int)));
    assert(q = (int *) malloc(6 * K * sizeof(int)));
    assert(incl = (int *) malloc(6 * K * sizeof(int)));
    assert(cycle = (int *) malloc(6 * K * sizeof(int)));
    assert(SwapStack =
           (RecordOfExchange *) malloc((MAXExchanges + 6 * K) * sizeof(RecordOfExchange)));
}

/*
 * 申请分配段内存空间
 */
void AllocateCallSegments()
{
    Segment *Seg = 0, *SPrev;
    SSegment *SSeg = 0, *SSPrev;
    int i;

    FreeSegSpace();

    SegSize = (int) sqrt((double) Dim);

    SegmentNums = 0;
    for (i = Dim, SPrev = 0; i > 0; i -= SegSize, SPrev = Seg) {
        assert(Seg = (Segment *) malloc(sizeof(Segment)));
        Seg->Priority = ++SegmentNums;
        if (!SPrev)
            FirstSegment = Seg;
        else
            SLink(SPrev, Seg);
    }
    SLink(Seg, FirstSegment);
   SSegmentSize = Dim;

    SSegmentNums = 0;
    for (i = SegmentNums, SSPrev = 0; i > 0; i -= SSegmentNums, SSPrev = SSeg) {
        SSeg = (SSegment *) malloc(sizeof(SSegment));
        SSeg->Priority = ++SSegmentNums;
        if (!SSPrev)
            FirstSSegment = SSeg;
        else
            SLink(SSPrev, SSeg);
    }
    SLink(SSeg, FirstSSegment);
}

/*
 * 函数HashTableInitialize, HashTableInsert 和 HashTableSearch用来维护一个哈希表结构的Tour
 */
void HashTableInitialize(HashTable * T)
{
    int k;
    for (k = 0; k< HashTableSize; k++) {
        T->Entry[k].KeyValue = UINT_MAX;
        T->Entry[k].Cost = MINUS_INFINITY;
    }
    T->Count = 0;
}

/*
 * 哈希表插入操作
 */
void HashTableInsert(HashTable * T, unsigned KeyValue, CostType Cost)
{
    int i = KeyValue % HashTableSize;
    if (T->Count >= MAXLoadFactor * HashTableSize) {
        if (Cost > T->Entry[i].Cost)
            return;
    } else {
        int p = KeyValue % 97 + 1;
        while (T->Entry[i].Cost != MINUS_INFINITY)
            if ((i -= p) < 0)
                i += HashTableSize;
        T->Count++;
    }
    T->Entry[i].KeyValue = KeyValue;
    T->Entry[i].Cost = Cost;
}

/*
 * 哈希表查找操作
 */
int HashTableSearch(HashTable * T, unsigned KeyValue, CostType Cost)
{
    int m, n;

    m = KeyValue % HashTableSize;
    n = KeyValue % 97 + 1;
    while ((T->Entry[m].KeyValue != KeyValue || T->Entry[m].Cost != Cost)
           && T->Entry[m].Cost != MINUS_INFINITY)
        if ((m -= n) < 0)
           m += HashTableSize;
    return T->Entry[m].KeyValue == KeyValue;
}

/*
 * 堆用来实现一个优先队列
 */

static int HeapCountNum;
static int HeapCapacityNum; //堆的容量

/*
 * 创建空堆
 */

void HeapBuild(int Size)
{
    assert(Heap = (Node **) malloc((Size + 1) * sizeof(Node *)));
    HeapCapacityNum = Size;
    HeapCountNum = 0;
}

/*
 * 堆的一些基本操作
 */

void HeapSiftUp(Node * tempN)
{
    int Location = tempN->Location, Parent = Location / 2;

    while (Parent && tempN->Priority < Heap[Parent]->Priority) {
        Heap[Location] = Heap[Parent];
        Heap[Location]->Location = Location;
        Location = Parent;
        Parent /= 2;
    }
    Heap[Location] = tempN;
    tempN->Location = Location;
}

void HeapSiftDown(Node * tempN)
{
    int Location = tempN->Location, Child;

    while (Location <= HeapCountNum / 2) {
        Child = 2 * Location;
        if (Child < HeapCountNum && Heap[Child + 1]->Priority < Heap[Child]->Priority)
            Child++;
        if (tempN->Priority <= Heap[Child]->Priority)
            break;
        Heap[Location] = Heap[Child];
        Heap[Location]->Location = Location;
        Location = Child;
    }
    Heap[Location] = tempN;
    tempN->Location = Location;
}
//删除堆中最小的节点
Node *HeapDeleteMin()
{
    Node *Remove;

    if (!HeapCountNum)
        return 0;
    Remove = Heap[1];
    Heap[1] = Heap[HeapCountNum--];
    Heap[1]->Location = 1;
    HeapSiftDown(Heap[1]);
    Remove->Location = 0;
    return Remove;
}

//向堆中插入节点
void HeapInsert(Node * tempN)
{
    assert(HeapCountNum < HeapCapacityNum);
    Heap[++HeapCountNum] = tempN;
    tempN->Location = HeapCountNum;
}

/*
 *返回两个节点间的段的长度
 */
int SegmentSize(Node * tempN1, Node * tempN2)
{
    Segment *Sega, *Segb;
    int nLeft, nMid, nRight;

    Sega = tempN1->Parent;
    Segb = tempN2->Parent;
    if (Sega == Segb) {
        int n = IsPathReversed == Sega->IsPathReversed ? tempN2->Priority - tempN1->Priority :
            tempN1->Priority - tempN2->Priority;
        return (n < 0 ? n + Dim : n) + 1;
    }
    nLeft =
        IsPathReversed ==
        Sega->IsPathReversed ? Sega->Last->Priority - tempN1->Priority : tempN1->Priority -
        Sega->First->Priority;
    if (nLeft < 0)
        nLeft += Sega->Size;
    nMid = !IsPathReversed ? Segb->Priority - Sega->Priority : Sega->Priority - Segb->Priority;
    if (nMid < 0)
        nMid += SegmentNums;
    nMid = nMid == 2 ? (!IsPathReversed ? Sega->Suc : Sega->Pred)->Size
        : (nMid - 1) * SegSize;
    nRight =
        IsPathReversed ==
        Segb->IsPathReversed ? tempN2->Priority -
        Segb->First->Priority : Segb->Last->Priority - tempN2->Priority;
    if (nRight < 0)
        nRight += Segb->Size;
    return nLeft + nMid + nRight + 2;
}

/*
 * 释放所有分配的结构体内存空间
 */


void ReleaseAll()
{
    FreeCandidateSets();
    FreeSegSpace();
    if (NodeSet) {
        int i;
        for (i = 1; i <= Dim; i++) {
            Node *N = &NodeSet[i];
            N->Row = 0;
        }
        Free(NodeSet);
    }
    Free(CostMatrix);
    Free(BestPath);
    Free(BetterPath);
    Free(SwapStack);
    Free(HTable);
    Free(RandomNumSet);
    Free(SavedFlag);
    Free(CacheVal);
    Free(Heap);
    Free(t);
    Free(T);
    Free(tSaved);
    Free(p);
    Free(q);
    Free(incl);
    Free(cycle);
    Free(G);
}

/*
  释放段空间
 */

void FreeSegSpace()
{
    if (FirstSegment) {
        Segment *Seg = FirstSegment, *SPrev;
        do {
            SPrev = Seg->Pred;
            Free(Seg);
        }
        while ((Seg = SPrev) != FirstSegment);
        FirstSegment = 0;
    }
    if (FirstSSegment) {
        SSegment *SSeg = FirstSSegment, *SSPrev;
        do {
            SSPrev = SSeg->Pred;
            Free(SSeg);
        }
        while ((SSeg = SSPrev) != FirstSSegment);
        FirstSSegment = 0;
    }
}

/*
 * 释放候选边的集合
 */

void FreeCandidateSets()
{
    Node *tempN = FirstNode;
    if (!tempN)
        return;
    do {
        Free(tempN->PromisedEdgesSet);
    }
    while ((tempN = tempN->Suc) != FirstNode);
}
