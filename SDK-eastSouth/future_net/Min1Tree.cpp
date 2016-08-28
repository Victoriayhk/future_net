#include "Algorthm.h"
#include "Structure.h"
/*
 *返回最小1树的的权值,通过寻找图的最小生成树加上叶子节点的第二条最近的邻边的权值来获得
 */
void FindMST(int Sparse);
void MST2Min1Tree(Node * N1, int Maximum, int Sparse);
CostType GetMin1TreeCost(int Sparse)
{
    Node *tpN, *N1 = 0;
    CostType TotalCost = 0;
    int Maximum = INT_MIN;

    FindMST(Sparse);//利用Prim算法获得最小生成树
    tpN = FirstNode;////指向链表中的第一个节点
    do
    {
        tpN->Var = -2;//节点的度
        TotalCost += tpN->Pi;
    }
    while ((tpN = tpN->Suc) != FirstNode);//双向链表中节点的后继节点是否指到起点
    TotalCost *= -2;
    while ((tpN = tpN->Suc) != FirstNode)//判断是否指向链表中头节点
    {
        tpN->Var++;
        tpN->Dad->Var++;
        TotalCost += tpN->Cost;
        tpN->Next = 0;
    }
    FirstNode->Dad = FirstNode->Suc;
    FirstNode->Cost = FirstNode->Suc->Cost;//节点边权值进改变
    do
    {
        if (tpN->Var == -1)
        {
            MST2Min1Tree(tpN, Maximum, Sparse);//通过最小生成树,来获得权值最大的最小1树的权值
            if (tpN->NextCost > Maximum)
            {
                N1 = tpN;
                Maximum = tpN->NextCost;
            }
        }
    }
    while ((tpN = tpN->Suc) != FirstNode);
    N1->Next->Var++;
    N1->Var++;
    TotalCost += N1->NextCost;
    Discrepancy = 0;
    do//确定冲突点
        Discrepancy += tpN->Var * tpN->Var;
    while ((tpN = tpN->Suc) != FirstNode);
    if (N1 == FirstNode)
        N1->Suc->Dad = 0;
    else
    {
        FirstNode->Dad = 0;
        Precede(N1, FirstNode);
        FirstNode = N1;
    }
    if (Discrepancy == 0)//解决冲突点的问题
    {
        for (tpN = FirstNode->Dad; tpN; N1 = tpN, tpN = tpN->Dad)
            Follow(tpN, N1);
        for (tpN = FirstNode->Suc; tpN != FirstNode; tpN = tpN->Suc)
            tpN->Dad = tpN->Pred;
        FirstNode->Suc->Dad = 0;
    }
    return TotalCost;
}


/*
 *利用Prim算法获得最小生成树
 */
void FindMST(int Sparse)
{
    Node *NB;
    Node *NextNode = 0;
    Node *tpN;
    PromisedEdges *NodeB;
    int d;

    NB = tpN = FirstNode;
    NB->Dad = 0;
    if (Sparse && NB->PromisedEdgesSet) //在候选边集合中比较
    {
        NB->Location = 0;
        while ((tpN = tpN->Suc) != FirstNode)
        {
            tpN->Dad = NB;
            tpN->Cost = tpN->Priority = INT_MAX;//算法的优先级定义最大值
            HeapInsert(tpN);//将节点插入到堆中
        }

        for (NodeB = NB->PromisedEdgesSet; (tpN = NodeB->To); NodeB++)
        {
            if (FixedOrCommon(NB, tpN))
            {
                tpN->Dad = NB;
                tpN->Cost = NodeB->Cost + NB->Pi + tpN->Pi;
                tpN->Priority = INT_MIN;
                HeapSiftUp(tpN);//改变节点在堆中状态，向上移
            }
            else if (!NB->FixedTo2 && !tpN->FixedTo2) //不是度为2的情况
            {
                tpN->Dad = NB;
                tpN->Cost = tpN->Priority = NodeB->Cost + NB->Pi + tpN->Pi;
                HeapSiftUp(tpN);//改变节点在堆中状态，向上移
            }
        }

        while ((NextNode = HeapDeleteMin()))
        {
            Follow(NextNode, NB);
            NB = NextNode;//依次下一个节点
            for (NodeB = NB->PromisedEdgesSet; (tpN = NodeB->To); NodeB++)//在候选边集合中判断
            {
                if (!tpN->Location)
                    continue;
                if (FixedOrCommon(NB, tpN))
                {
                    tpN->Dad = NB;
                    tpN->Cost = NodeB->Cost + NB->Pi + tpN->Pi;
                    tpN->Priority = INT_MIN;
                    HeapSiftUp(tpN);//改变节点在堆中状态，向上移
                }
                else if (!NB->FixedTo2 && !tpN->FixedTo2 &&
                           (d = NodeB->Cost + NB->Pi + tpN->Pi) < tpN->Cost)
                {
                    tpN->Dad = NB;
                    tpN->Cost = tpN->Priority = d;
                    HeapSiftUp(tpN);//改变节点在堆中状态，向上移
                }
            }
        }
    }
    else
    {
        while ((tpN = tpN->Suc) != FirstNode)//先初始化一下
            tpN->Cost = INT_MAX;

        while ((tpN = NB->Suc) != FirstNode)//双向循环链表判断结束
        {
            int Min = INT_MAX;
            do
            {
                if (FixedOrCommon(NB, tpN))
                {
                    tpN->Dad = NB;
                    tpN->Cost = DISTANCE(NB, tpN);
                    NextNode = tpN;
                    Min = INT_MIN;
                }
                else
                {
                    if (!NB->FixedTo2 && !tpN->FixedTo2 &&
                        !Forbidden(NB, tpN) &&
                        (!c || c(NB, tpN) < tpN->Cost) &&
                        (d = DISTANCE(NB, tpN)) < tpN->Cost)//根据度数和权值操作
                        {
                        tpN->Cost = d;
                        tpN->Dad = NB;
                        }
                    if (tpN->Cost < Min)
                    {
                        Min = tpN->Cost;
                        NextNode = tpN;
                    }
                }
            }
            while ((tpN = tpN->Suc) != FirstNode);//连表中进行一轮操作，跳出循环
            Follow(NextNode, NB);
            NB = NextNode;
        }
    }
}


/*
 * 通过最小生成树,来获得权值最大的最小1树的权值
*/
void MST2Min1Tree(Node * N1, int Maximum, int Sparse)
{
    Node *tpN;
    PromisedEdges *NearestNode;//期望点集
    int d;

    N1->Next = 0;
    N1->NextCost = INT_MAX;//先将权值置为最大
    if (!Sparse || N1->PromisedEdgesSet == 0 ||
        N1->PromisedEdgesSet[0].To == 0 || N1->PromisedEdgesSet[1].To == 0)
        {
        tpN = FirstNode;
        do
        {
            if (tpN == N1 || tpN == N1->Dad || N1 == tpN->Dad)//节点和父节点都一样，边相同
                continue;
            if (FixedOrCommon(N1, tpN))
            {
                N1->NextCost = DISTANCE(N1, tpN);
                N1->Next = tpN;
                return;
            }
            if (!N1->FixedTo2 && !tpN->FixedTo2 &&
                !Forbidden(N1, tpN) &&
                (!c || c(N1, tpN) < N1->NextCost) &&
                (d = DISTANCE(N1, tpN)) < N1->NextCost)
                {
                N1->NextCost = d;
                if (d <= Maximum)//不是最大情况
                    return;
                N1->Next = tpN;
                }
        }
        while ((tpN = tpN->Suc) != FirstNode);//跳出循环
    }
     else
     {
        for (NearestNode = N1->PromisedEdgesSet; (tpN = NearestNode->To); NearestNode++)//在期望候选集中比较
        {
            if (tpN == N1->Dad || N1 == tpN->Dad)//搜索到父节点
                continue;
            if (FixedOrCommon(N1, tpN))
            {
                N1->NextCost = NearestNode->Cost + N1->Pi + tpN->Pi;
                N1->Next = tpN;
                return;
            }
            if (!N1->FixedTo2 && !tpN->FixedTo2 &&
                (d = NearestNode->Cost + N1->Pi + tpN->Pi) < N1->NextCost)//度数和权值作为判断
                {
                N1->NextCost = d;
                if (d <= Maximum)
                    return;
                N1->Next = tpN;
            }
        }
    }
}
