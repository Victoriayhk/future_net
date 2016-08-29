#include "Algorthm.h"
#include <math.h>
int IsPromisedEdge(Node * From, Node * To);
void ResetNodeEdgesSet();       //重置顶点的候选边集合
CostType GetMin1TreeCost(int Sparse);  //获得最小1树的权值

static CostType  GetLowerBound();   //获取路径权值的下界
void CalculateEdgesSet(int MAXSelectedEdgesNum, CostType MAXAlpha);
/*
 *    AddPromisedEdges函数将一个给定的边加入起点的候选边集合,如果加入成功,
 * 返回1,如果失败,返回0,如果该条边已经在集合里,则没有什么影响
 *
 */

int AddPromisedEdges(Node * From, Node * To, int Cost, int Alpha)
{
    int Count;
    PromisedEdges *NodeFrom;


    if (From->PromisedEdgesSet == 0)
        assert(From->PromisedEdgesSet =
               (PromisedEdges *) calloc(3, sizeof(PromisedEdges)));
    if (From == To  ||  !IsPromisedEdge(From, To))
        return 0;
    Count = 0;
    for (NodeFrom = From->PromisedEdgesSet; NodeFrom->To && NodeFrom->To != To; NodeFrom++)
        Count++;
    if (NodeFrom->To) {
        if (NodeFrom->Alpha == INT_MAX)
            NodeFrom->Alpha = Alpha;
        return 0;
    }
    NodeFrom->Cost = Cost;
    NodeFrom->Alpha = Alpha;
    NodeFrom->To = To;
    assert(From->PromisedEdgesSet =
           (PromisedEdges *) realloc(From->PromisedEdgesSet,
                                 (Count + 2) * sizeof(PromisedEdges)));
    From->PromisedEdgesSet[Count + 1].To = 0;
    return 1;
}


/*
 * 重置候选边集合
 */

void ResetNodeEdgesSet()
{
    Node *From;
    PromisedEdges *NodeFrom, *NearNode, Temp;

    From = FirstNode;

    do {
        if (!From->PromisedEdgesSet)
            continue;
        for (NodeFrom = From->PromisedEdgesSet; NodeFrom->To; NodeFrom++) {
            Temp = *NodeFrom;
            for (NearNode = NodeFrom - 1;
                 NearNode >= From->PromisedEdgesSet &&
                 (Temp.Alpha < NearNode->Alpha ||
                  (Temp.Alpha == NearNode->Alpha && Temp.Cost < NearNode->Cost)); NearNode--)
                *(NearNode + 1) = *NearNode;
            *(NearNode + 1) = Temp;
        }
        NodeFrom--;

        while (NodeFrom >= From->PromisedEdgesSet + 2 && NodeFrom->Alpha == INT_MAX)
            NodeFrom--;
        NodeFrom++;
        NodeFrom->To = 0;

        for (NodeFrom = From->PromisedEdgesSet; NodeFrom->To; NodeFrom++) {
            if (!IsPromisedEdge(From, NodeFrom->To)) {
                for (NearNode = NodeFrom; NearNode->To; NearNode++)
                    *NearNode = *(NearNode + 1);
                NodeFrom--;
            }
        }
    }
    while ((From = From->Suc) != FirstNode);
}




/*
 *FindPromisingEdges函数用来为每个顶点确定其相邻的候选边集合,该函数调用Ascent函数
 * 利用子梯度优化算法来确定optimal tour的下界.同时该函数还将调用CalculateEdgesSet来计算
 * alpha-value,并为每个顶点确定相邻的候选边集合
 *FindPromisingEdges被FindOptimalPath调用
 */

void FindPromisingEdges()
{
    CostType Cost, MAXAlpha, A;
    Node *Na;
    Discrepancy = 9999;

     Na = FirstNode;
        do
            Na->Pi = 0;
        while ((Na = Na->Suc) != FirstNode);
        Cost = GetLowerBound();
        LB = (double) Cost / Accuracy;

    MAXAlpha = (CostType) fabs(Ex * Cost);
    if ((A = OptimalCost * Accuracy - Cost) > 0 && A < MAXAlpha)
        MAXAlpha = A;
     CalculateEdgesSet(MAXSelectedEdgesNum, MAXAlpha);

    ResetNodeEdgesSet();
    Na = FirstNode;
    do {
        if (!Na->PromisedEdgesSet || !Na->PromisedEdgesSet[0].To) {

                printf("Node %d has no candidates", Na->Id);
        }
    }
    while ((Na = Na->Suc) != FirstNode);

}




static int MAX( int a,  int b)
{
    return a > b ? a : b;
}

//为每个节点创建候选边集合
void CalculateEdgesSet(int MAXSelectedEdgesNum, CostType MAXAlpha)
{
    Node *From, *To;
    PromisedEdges *NodeFrom, *NearNode;
    int a, d, Count;
    if (MAXAlpha < 0 || MAXAlpha > INT_MAX)
        MAXAlpha = INT_MAX;

    FreeCandidateSets();
    From = FirstNode;
    do
        From->Visited = 0;
    while ((From = From->Suc) != FirstNode);

    if (MAXSelectedEdgesNum > 0) {
        do {
            assert(From->PromisedEdgesSet =
                   (PromisedEdges *) malloc((MAXSelectedEdgesNum + 1) *
                                        sizeof(PromisedEdges)));
            From->PromisedEdgesSet[0].To = 0;
        }
        while ((From = From->Suc) != FirstNode);
    }


    do {
        NodeFrom = From->PromisedEdgesSet;
        if (From != FirstNode) {
            From->BValue = INT_MIN;
            for (To = From; To->Dad != 0; To = To->Dad) {
                To->Dad->BValue =
                    !FixedOrCommon(To, To->Dad) ?
                    MAX(To->BValue, To->Cost) : To->BValue;
                To->Dad->Visited = From;
            }
        }
        Count = 0;

        To = FirstNode;
        do {
            if (To == From)
                continue;
            d = c && !FixedOrCommon(From, To) ? c(From, To) : DISTANCE(From, To);
            if (From == FirstNode)
                a = To == From->Dad ? 0 : d - From->NextCost;
            else if (To == FirstNode)
                a = From == To->Dad ? 0 : d - To->NextCost;
            else {
                if (To->Visited != From)
                    To->BValue =
                        !FixedOrCommon(To, To->Dad) ?
                        MAX(To->Dad->BValue, To->Cost) : To->Dad->BValue;
                a = d - To->BValue;
            }
            if (FixedOrCommon(From, To))
                a = INT_MIN;
            else {
                if (From->FixedTo2 || To->FixedTo2 || Forbidden(From, To))
                    continue;
            if (c) {
                    if (a > MAXAlpha ||
                        (Count == MAXSelectedEdgesNum &&
                         (a > (NodeFrom - 1)->Alpha ||
                          (a == (NodeFrom - 1)->Alpha
                           && d >= (NodeFrom - 1)->Cost))))
                        continue;
                    if (To == From->Dad) {
                        d = From->Cost;
                        a = 0;
                    } else if (From == To->Dad) {
                        d = To->Cost;
                        a = 0;
                    } else {
                        a -= d;
                        a += (d = DISTANCE(From, To));
                    }
                }
            }
            if (a <= MAXAlpha && IsPromisedEdge(From, To)) {

                NearNode = NodeFrom;
                while (--NearNode >= From->PromisedEdgesSet) {
                    if (a > NearNode->Alpha || (a == NearNode->Alpha && d >= NearNode->Cost))
                        break;
                    *(NearNode + 1) = *NearNode;
                }
                NearNode++;
                NearNode->To = To;
                NearNode->Cost = d;
                NearNode->Alpha = a;
                if (Count < MAXSelectedEdgesNum) {
                    Count++;
                    NodeFrom++;
                }
                NodeFrom->To = 0;
            }
        }
        while ((To = To->Suc) != FirstNode);
    }
    while ((From = From->Suc) != FirstNode);


}



/*
 * 判断某条边是否可能时候选边
 */

int IsPromisedEdge(Node * From, Node * To)
{
    if ( FixedOrCommon(From, To))
        return 1;
    if (From->FixedTo2 || To->FixedTo2)
        return 0;

     return 1;
}




/*
 *GetLowerBound 函数利用子梯度优化迭代算法计算出最短路径的下界,同时将原始问题
 * 转化为alpha值的大小来反映某条边是否属于最短路径
 */

static CostType  GetLowerBound()
{
    Node *t;
    CostType BestW, W, W0, Alpha, MAXAlpha = INT_MAX;
    int T, Iteration, P, InitialState, BestNorm;

  START:

    t = FirstNode;
    do
        t->Pi = t->BestPi = 0;
    while ((t = t->Suc) != FirstNode);


    W = GetMin1TreeCost(MAXSelectedEdgesNum == 0);


    if ( !Discrepancy)
        return W;

    if (OptimalCost != MINUS_INFINITY && (Alpha = OptimalCost * Accuracy - W) >= 0)
        MAXAlpha = Alpha;
    if (MAXSelectedEdgesNum > 0) {

            CalculateEdgesSet(SelectedEdgesNum, MAXAlpha);

    }


    t = FirstNode;
    do
        t->LastVar = t->Var;
    while ((t = t->Suc) != FirstNode);

    BestW = W0 = W;
    BestNorm = Discrepancy;
    InitialState = 1;

    for (Iteration = InitIterationTimes, T = StepLength * Accuracy;
         Iteration > 0 && T > 0 && Discrepancy != 0; Iteration /= 2, T /= 2) {

        for (P = 1; T && P <= Iteration && Discrepancy != 0; P++) {

            t = FirstNode;
            do {
                if (t->Var != 0) {
                    t->Pi += T * (7 * t->Var + 3 * t->LastVar) / 10;
                    if (t->Pi > INT_MAX / 4)
                        t->Pi = INT_MAX / 4;
                    else if (t->Pi < -INT_MAX / 4)
                        t->Pi = -INT_MAX / 4;
                }
                t->LastVar = t->Var;
            }
            while ((t = t->Suc) != FirstNode);

            W = GetMin1TreeCost(1);

            if (W > BestW || (W == BestW && Discrepancy < BestNorm)) {

                if (W - W0 > (W0 >= 0 ? W0 : -W0) && SelectedEdgesNum > 0
                    && SelectedEdgesNum < Dim) {
                    W = GetMin1TreeCost(MAXSelectedEdgesNum == 0);
                    if (W < W0) {


                        if ((SelectedEdgesNum *= 2) > Dim)
                            SelectedEdgesNum = Dim;
                        goto START;
                    }
                    W0 = W;
                }
                BestW = W;
                BestNorm = Discrepancy;

                t = FirstNode;
                do
                    t->BestPi = t->Pi;
                while ((t = t->Suc) != FirstNode);


                if (InitialState && T * sqrt((double) Discrepancy) > 0)
                    T *= 2;

                if ((P == Iteration )&& (Iteration *= 2) > InitIterationTimes)
                    Iteration = InitIterationTimes;
            } else {

                if (InitialState && P > Iteration / 2) {

                    InitialState = 0;
                    P = 0;
                    T = 3 * T / 4;
                }
            }
        }
    }

    t = FirstNode;
    do {
        t->Pi = t->BestPi;
        t->BestPi = 0;
    } while ((t = t->Suc) != FirstNode);


    W = BestW = GetMin1TreeCost(MAXSelectedEdgesNum == 0);

    if (MAXSelectedEdgesNum > 0) {
        FreeCandidateSets();
    } else {
        PromisedEdges *Nt;
        t = FirstNode;
        do {
            for (Nt = t->PromisedEdgesSet; Nt && Nt->To; Nt++)
                Nt->Cost += t->Pi + Nt->To->Pi;
        }
        while ((t = t->Suc) != FirstNode);
    }
    return W;
}
