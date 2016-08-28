#include "Algorthm.h"
#include "Structure.h"
extern int runtimeFlag;
CostType Gain23();
CostType LinKernighan();
CostType CombinePath();
void AdjustCandidateSet();
void ResetNodeEdgesSet();
int IsBetween(const Node * Na, const Node * Nb, const Node * Nc);
Node *Optimal4Exchange(Node * N1, Node * N2, CostType * Gain0, CostType * Gain);
/*
 *当通过边交换算法得到更小的权值时,调用该函数来保存路径
 */

void SaveBetterResult()
{
    Node *tN;//定义一个临时节点
    int i = 0;

    tN = FirstNode;
    do {
            if (tN->Id <= Dim / 2)
            {
                i++;
                if (tN->Suc->Id != tN->Id + Dim / 2)
                    BetterPath[i] = tN->Id;
                else
                    BetterPath[Dim / 2 - i + 1] = tN->Id;
            }
            tN->NextBestSuc = tN->BestSuc;
            tN->BestSuc = tN->Suc;
    }
    while ((tN = tN->Suc) != FirstNode);//找到比较优的结果
    BetterPath[0] = BetterPath[Dim / 2];
}


/*
 *此函数用于保存计算出来的最优路径结果
 */
void SaveBestResult()
{
    int NodeNum = (int) (Dim / 2);
    for (int i = 0; i <= NodeNum; i++)
       BestPath[i] = BetterPath[i];//将计算出来的最优结果存入BestPath

}


/*
 * 每进行一次边交换,如果能够使路径变短,就执行该函数保存交换结果
 */
void Activate(Node * tpN);//函数前申明函数中要掉用的函数
void StoreTour()
{
    Node *tp, *u;
    PromisedEdges *Nt;

    while (ExchangeTimes > 0)
    {
        ExchangeTimes--;//边交换的次数，进行一次交换减一
        for (int i = 1; i <= 4; i++)
        {
            tp = i == 1 ? SwapStack[ExchangeTimes].t1 :
                i == 2 ? SwapStack[ExchangeTimes].t2 :
                i == 3 ? SwapStack[ExchangeTimes].t3 : SwapStack[ExchangeTimes].t4;
            Activate(tp);//激活顶点t,作为边交换的初始节点
            tp->OldPred = tp->Pred;
            tp->OldSuc = tp->Suc;
            tp->OldPredExcluded = tp->OldSucExcluded = 0;
            tp->Cost = INT_MAX;
            for (Nt = tp->PromisedEdgesSet; (u = Nt->To); Nt++)
                if (u != tp->Pred && u != tp->Suc && Nt->Cost < tp->Cost)
                    tp->Cost = Nt->Cost;
        }
    }
}


/*
 * 激活某一顶点,使其作为边交换的初始节点,被 StoreTour()调用
*/
void Activate(Node * tpN)
{
    if (tpN->Next != 0)
        return;

    if (FirstActive == 0)//等于零直接传递赋值
        FirstActive = LastActive = tpN;
    else//否则给下一个定点
        LastActive = LastActive->Next = tpN;

    LastActive->Next = FirstActive;
}


/*
 * 撤销之前进行的可能的错误的边交换操作
 */
void RestoreTour()
{
    Node *tp1, *tp2, *tp3, *tp4;//定义临时节点

    while (ExchangeTimes > 0)//交换次数作为循环控制条件
     {
        ExchangeTimes--;
        tp1 = SwapStack[ExchangeTimes].t1;
        tp2 = SwapStack[ExchangeTimes].t2;
        tp3 = SwapStack[ExchangeTimes].t3;
        tp4 = SwapStack[ExchangeTimes].t4;
        SWAP1(tp3, tp2, tp1);
        ExchangeTimes--;//交换之后，交换次数限制做相应的减一变化

        tp1->OldPredExcluded = tp1->OldSucExcluded = 0;
        tp2->OldPredExcluded = tp2->OldSucExcluded = 0;
        tp3->OldPredExcluded = tp3->OldSucExcluded = 0;
        tp4->OldPredExcluded = tp4->OldSucExcluded = 0;
    }
}

/*
 * FindTour函数进行多次尝试,在每次尝试中,通过利用启发式边交换算法来缩小初始
 * 路径的权值
 */
 void ChooseInitialTour();//调用函数先进行声明
static CostType OrdinalTourCost;

CostType FindTour()
{
    CostType Cost;
    Node *tp;

    tp = FirstNode;//定义的临时Node进行赋值
    do
        tp->OldPred = tp->OldSuc = tp->NextBestSuc = tp->BestSuc = 0;
    while ((tp = tp->Suc) != FirstNode);//判断是否是头节点，不是继续进行循环
    if (Run == 1 && Dim == SavedDim)//跳出循环进行权值计算比较
    {
        OrdinalTourCost = 0;
        for (int i = 1; i < Dim; i++)
            OrdinalTourCost += COST(&NodeSet[i], &NodeSet[i + 1])
                - NodeSet[i].Pi - NodeSet[i + 1].Pi;
        OrdinalTourCost += COST(&NodeSet[Dim], &NodeSet[1])
            - NodeSet[Dim].Pi - NodeSet[1].Pi;
        OrdinalTourCost /= Accuracy;
    }
    BetterCost = INFINITY;
    if (MAXTests > 0)
        HashTableInitialize(HTable);//初始化Hash表
    else {
        Test = 1;//Test表示当前尝试的次数在Algorthm.h中给出了定义
        ChooseInitialTour();
    }

    for (Test = 1; Test <= MAXTests; Test++)//最大尝试次数作为循环终止条件
    {
            if (Dim == SavedDim)
                FirstNode = &NodeSet[1 + rand() % Dim];
            else
                for (int i = rand() % Dim; i > 0; i--)//有一个随机处理的过程
                    FirstNode = FirstNode->Suc;
            ChooseInitialTour();
            Cost = LinKernighan();
            if (FirstNode->BestSuc)
            {
                tp = FirstNode;
                while ((tp = tp->Next = tp->BestSuc) != FirstNode);//循环直到tp变为FirstNode
                Cost = CombinePath();
            }
            if (Dim == SavedDim && Cost >= OrdinalTourCost &&
                BetterCost > OrdinalTourCost)//权值计算比较
                {
                for (int i = 1; i < Dim; i++)
                    NodeSet[i].Next = &NodeSet[i + 1];
                NodeSet[Dim].Next = &NodeSet[1];
                Cost = CombinePath();
            }
            if (Cost < BetterCost)//权值计算比较
             {
                BetterCost = Cost;
                SaveBetterResult();
                if ( BetterCost == OptimalCost)
                    break;
                AdjustCandidateSet();//调整必经的候选子集
                HashTableInitialize(HTable);//Hash表进行初始化
                HashTableInsert(HTable, KeyValue, Cost);//Hash插入操作
            }
    }
    tp = FirstNode;
    if (Discrepancy == 0)
    {
        do
            tp = tp->BestSuc = tp->Suc;
        while (tp != FirstNode);//依然是一个恢复tp的操作
    }
    do
        (tp->Suc = tp->BestSuc)->Pred = tp;
    while ((tp = tp->BestSuc) != FirstNode);
    if (Test > MAXTests)
        Test = MAXTests;
    ResetNodeEdgesSet();
    return BetterCost;
}


/*
 *随机生成初始的路径,决定计算的复杂度可能有一定的随机性
 */
static int FixedOrCommonCandidates(Node * tpN);//函数申明

void ChooseInitialTour()
{
    Node *tpN, *NextN, *FirstAlternative, *Last;
    PromisedEdges *NN;
    int Alternatives, Count, i;
//定义一个可能循环的初始位置
Start:

    tpN = FirstNode;
    do
        tpN->Var = 0;
    while ((tpN = tpN->Suc) != FirstNode);//做一个初始循环工作
    Count = 0;

    do {
        if (FixedOrCommonCandidates(tpN) < 2)// 给定节点的周围的候选边数量小于2条直接跳出循环
            break;
    }
    while ((tpN = tpN->Suc) != FirstNode);//多于两条候选边，进行后续
    FirstNode = tpN;

    for (Last = FirstNode->Pred; tpN != Last; tpN = NextN)//循环结束直到末尾节点
    {
        NextN = tpN->Suc;
        if (FixedOrCommonCandidates(tpN) == 2)
            Follow(tpN, Last);
    }

    FirstNode->Var = 1;
    tpN = FirstNode;

    while (tpN->Suc != FirstNode)//重新计算
    {
        FirstAlternative = 0;
        Alternatives = 0;
        Count++;

        for (NN = tpN->PromisedEdgesSet; (NextN = NN->To); NN++)
        {
            if (!NextN->Var && FixedOrCommon(tpN, NextN)) {
                Alternatives++;
                NextN->Next = FirstAlternative;
                FirstAlternative = NextN;
            }
        }

        if (Alternatives == 0 && Test > 1 )//可以继续尝试
        {
            for (NN = tpN->PromisedEdgesSet; (NextN = NN->To); NN++) {
                if (!NextN->Var && FixedOrCommonCandidates(NextN) < 2 &&
                    NN->Alpha == 0 && (InBestTour(tpN, NextN) ||
                                       InNextBestTour(tpN, NextN))) {
                    Alternatives++;
                    NextN->Next = FirstAlternative;
                    FirstAlternative = NextN;
                }
            }
        }
        if (Alternatives == 0)
        {
            for (NN = tpN->PromisedEdgesSet; (NextN = NN->To); NN++)
            {
                if (!NextN->Var && FixedOrCommonCandidates(NextN) < 2)
                {
                    Alternatives++;
                    NextN->Next = FirstAlternative;
                    FirstAlternative = NextN;
                }
            }
        }
       if (Alternatives == 0)
        {
            NextN = tpN->Suc;
            while ((FixedOrCommonCandidates(NextN) == 2 || Forbidden(tpN, NextN))
                   && NextN->Suc != FirstNode)
                NextN = NextN->Suc;
            if (FixedOrCommonCandidates(NextN) == 2 || Forbidden(tpN, NextN))
            {
                FirstNode = FirstNode->Suc;
                goto Start;//回到最前面，重复进行搜索
            }
        }
        else
        {
            NextN = FirstAlternative;
            if (Alternatives > 1)
            {
                i = rand() % Alternatives;
                while (i--)
                    NextN = NextN->Next;
            }
        }
        Follow(NextN, tpN);
        tpN = NextN;
        tpN->Var = 1;
    }
    if (Forbidden(tpN, tpN->Suc))
    {
        FirstNode = FirstNode->Suc;
        goto Start;
    }
    if (MAXTests == 0)
    {
        CostType Cost = 0;
        tpN = FirstNode;
        do
            Cost += COST(tpN, tpN->Suc) - tpN->Pi - tpN->Suc->Pi;
        while ((tpN = tpN->Suc) != FirstNode);
        Cost /= Accuracy;
        if (Cost < BetterCost)//说明有更好的边，进行保存
        {
            BetterCost = Cost;
            SaveBetterResult();
        }
    }
}

/*
 * 获取给定节点的周围的候选边数量
 */
static int FixedOrCommonCandidates(Node * tpN)
{
    int Count = 0;
    PromisedEdges *NN;

    if (tpN->FixedTo2)
        return 2;
    if (!tpN->PointTo1 )
        return 0;
    for (NN = tpN->PromisedEdgesSet; NN->To; NN++)
    {
        if (FixedOrCommon(tpN, NN->To))
            Count++;
    }
    return Count;
}

/*
 * 通过合并两条路径来找到更优的路径
 */
CostType CombinePath()
{
    int  NewDimension = 0;
    CostType Cost1 = 0, Cost2 = 0,OldCost1;
    Node *N,*First = 0, *Last;

    N = FirstNode;
    do
        N->Suc->Pred = N->Next->Prev = N;
    while ((N = N->Suc) != FirstNode);//初始化循环
    do
     {
            Cost1 += N->Cost = COST(N, N->Suc) - N->Pi - N->Suc->Pi;
            if ((N->Suc == N->Prev || N->Suc == N->Next) &&
                (N->Pred == N->Prev || N->Pred == N->Next))
                N->Var = 0;
            else
            {
                N->Var = 1;
                NewDimension++;
                First = N;
            }
    } while ((N = N->Suc) != FirstNode);

    if (NewDimension == 0)//维数降为0直接返回
        return Cost1 / Accuracy;
    do
    {
        Cost2 += N->NextCost = N->Next == N->Pred ? N->Pred->Cost :
            N->Next == N->Suc ? N->Cost :
            COST(N, N->Next) - N->Pi - N->Next->Pi;
    } while ((N = N->Suc) != FirstNode);//计算cost用于比较分析
    OldCost1 = Cost1;

    int Priority = 0, Improved1 = 0, Improved2 = 0;
    N = First;
    Last = 0;
    do {
        if (N->Var) {
            N->Priority = ++Priority;
            if (Last) {
                (Last->OldSuc = N)->OldPred = Last;
                if (Last != N->Pred)
                    Last->Cost = 0;
            }
            Last = N;
        }
    } while ((N = N->Suc) != First);

    (Last->OldSuc = First)->OldPred = Last;
    if (Last != First->Pred)
        Last->Cost = 0;
    N = First;
    Last = 0;
    do {
        if (N->Var) {
            if (Last) {
                Last->Next = N;
                if (Last != N->Prev) {
                    N->Prev = Last;
                    Last->NextCost = 0;
                }
            }
            Last = N;
        }
    } while ((N = N->Next) != First);

    Last->Next = First;
    if (Last != First->Prev) {
        First->Prev = Last;
        Last->NextCost = 0;
    }
    //合并重要部分，需要多次循环嵌套判断
    Node *NNext, *N1, *N2, *MinN1, *MinN2;
    int MinSubSize, BestMinSubSize = 3, MinForward = 0;
    int SubSize1, SubSize2, MaxSubSize1,  Forward;
    CostType Gain,  MinGain = 0;
    do
    {
        MinN1 = MinN2 = 0;
        MinSubSize = NewDimension / 2;
        N1 = First;
        do
        {
            while (N1->OldSuc != First &&
                   (N1->OldSuc == N1->Next || N1->OldSuc == N1->Prev))
                N1 = N1->OldSuc;
            if (N1->OldSuc == First &&
                (N1->OldSuc == N1->Next || N1->OldSuc == N1->Prev))
                break;
            for (Forward = 1, N2 = N1->Next; Forward >= 0;
                 Forward--, N2 = N1->Prev)
                 {
                    if (N2 == N1->OldSuc || N2 == N1->OldPred)
                        continue;
                    SubSize2 = MaxSubSize1 = 0;
                    do
                    {
                        if (++SubSize2 >= MinSubSize)
                            break;
                        if ((SubSize1 = N2->Priority - N1->Priority) < 0)
                            SubSize1 += NewDimension;
                        if (SubSize1 >= MinSubSize)
                            break;
                        if (SubSize1 > MaxSubSize1)
                        {
                            if (SubSize1 == SubSize2)
                            {
                                for (N = N1, Gain = 0; N != N2; N = N->OldSuc)
                                    Gain += N->Cost - N->NextCost;
                                if (!Forward)
                                    Gain += N1->NextCost - N2->NextCost;
                                if (Gain != 0)
                                {
                                    MinSubSize = SubSize1;
                                    MinN1 = N1;
                                    MinN2 = N2;
                                    MinGain = Gain;
                                    MinForward = Forward;
                                }
                                break;
                            }
                            MaxSubSize1 = SubSize1;
                        }
                    } while ((N2 = Forward ? N2->Next : N2->Prev) != N1);
                }
        } while ((N1 = N1->OldSuc) != First &&
                 MinSubSize != BestMinSubSize);

        if (MinN1)
        {
            BestMinSubSize = MinSubSize;
            if (MinGain > 0)
            {
                Improved1 = 1;
                Cost1 -= MinGain;
                Priority = MinN1->Priority;
                for (N = MinN1; N != MinN2; N = NNext) {
                    NNext = MinForward ? N->Next : N->Prev;
                    (N->OldSuc = NNext)->OldPred = N;
                    N->Priority = Priority;
                    N->Cost = MinForward ? N->NextCost : NNext->NextCost;
                    if (++Priority > NewDimension)
                        Priority = 1;
                }
            } else {
                Improved2 = 1;
                Cost2 += MinGain;
                for (N = MinN1; N != MinN2; N = N->OldSuc)
                {
                        if (MinForward)
                        {
                            (N->Next = N->OldSuc)->Prev = N;
                            N->NextCost = N->Cost;
                        } else {
                            (N->Prev = N->OldSuc)->Next = N;
                            N->Prev->NextCost = N->Cost;
                        }
                    }
                    if (MinForward)
                        MinN2->Prev = N->OldPred;
                    else {
                        MinN2->Next = N->OldPred;
                        MinN2->NextCost = N->OldPred->Cost;
                    }
                }
            First = MinForward ? MinN2 : MinN1;
            }
    } while (MinN1);//最小的MinN1补位0继续进行循环

    if (Cost1 < Cost2 ? !Improved1 : Cost2 < Cost1 ? !Improved2 :
        !Improved1 || !Improved2)
        return OldCost1 / Accuracy;//权值结果已经满足，直接return

    N = FirstNode;
    do
        N->Visited = 0;
    while ((N = N->Suc) != FirstNode);
    N = First;
    N->Visited = N;
    do
    {
        if (!N->Suc->Visited && (!N->Var || !N->Suc->Var))
            N->OldSuc = N->Suc;
        else if (!N->Pred->Visited && (!N->Var || !N->Pred->Var))
            N->OldSuc = N->Pred;
        else if (Cost1 <= Cost2)
        {
            if (N->OldSuc->Visited)
                N->OldSuc = !N->OldPred->Visited ? N->OldPred : First;
        }
        else if (!N->Next->Visited)
            N->OldSuc = N->Next;
        else if (!N->Prev->Visited)
            N->OldSuc = N->Prev;
        else
            N->OldSuc = First;
        N->Visited = N;
    } while ((N = N->OldSuc) != First);//结束循环
    KeyValue = 0;
    do {
        N->OldSuc->Pred = N;
        KeyValue ^= RandomNumSet[N->Id] * RandomNumSet[N->OldSuc->Id];
    }
    while ((N = N->Suc = N->OldSuc) != First);
    return (Cost1 <= Cost2 ? Cost1 : Cost2) / Accuracy;
}


/*
 * 将当前的路径和已知的最优路径进行合并,优化最优路径
 */

CostType CombineCurrentPathWithBestPath()
{
    Node *N1, *N2, *M1, *M2;//先定义几个临时节点
    int NodeNum = Dim / 2;//节点数目不大于Dim的一半
        for (int i = 1; i <= NodeNum; i++) {
            N1 = &NodeSet[BestPath[i - 1]];
            N2 = &NodeSet[BestPath[i]];
            M1 = &NodeSet[N1->Id + NodeNum];
            M2 = &NodeSet[N2->Id + NodeNum];
            M1->Next = N1;
            N1->Next = M2;
            M2->Next = N2;
        }
    //通过合并两条路径来找到更优的路径
    return CombinePath();
}


/*
 *当一次边交换改善当前路径之后,需对原来的候选边集合进行调整
 */
void AdjustCandidateSet()
{
    PromisedEdges *NFrom, *NN, Temp;
    Node *From = FirstNode, *To;

    do {
        if (!From->PromisedEdgesSet)
            assert(From->PromisedEdgesSet =
                   (PromisedEdges *) calloc(3, sizeof(PromisedEdges)));
        //需要对边进行调整
        for (To = From->Pred; To; To = To == From->Pred ? From->Suc : 0)
        {
            int Count = 0;
            for (NFrom = From->PromisedEdgesSet; NFrom->To && NFrom->To != To;
                NFrom++)
                Count++;
            if (!NFrom->To)
            {
                NFrom->Cost = COST(From, To);
                NFrom->To = To;
                NFrom->Alpha = INT_MAX;
                assert(From->PromisedEdgesSet =
                       (PromisedEdges *) realloc(From->PromisedEdgesSet,
                                             (Count +
                                              2) * sizeof(PromisedEdges)));
                From->PromisedEdgesSet[Count + 1].To = 0;
            }
        }

        for (NFrom = From->PromisedEdgesSet + 1; (To = NFrom->To); NFrom++)
            if (InBestTour(From, To) &&(InNextBestTour(From, To)))
            {
                Temp = *NFrom;
                for (NN = NFrom - 1; NN >= From->PromisedEdgesSet; NN--)
                    *(NN + 1) = *NN;
                *(NN + 1) = Temp;
            }
    }
    while ((From = From->Suc) != FirstNode);//跳出循环，调整完成
}


/*
 * 启发式边交换算法通过顺序和非顺序的边交换来改进当前路径
 */
static void DiscrepancyNodeList();
static void DiscrepancySegmentList();
static Node *RemoveFirstActive();//函数中需要调用的函数先进行申明

CostType LinKernighan()
{
    int X2, i;
    Segment *S;//定义Segment类型指针，链表进行数据操作
    SSegment *SS;//定义SSegment,段数据操作

    IsPathReversed = 0;
    S = FirstSegment;
    i = 0;
    do
    {
        S->Size = 0;
        S->Priority = ++i;
        S->IsPathReversed = 0;
        S->First = S->Last = 0;
    }
    while ((S = S->Suc) != FirstSegment);
    SS = FirstSSegment;
    i = 0;
    do {
        SS->Size = 0;
        SS->Priority = ++i;
        SS->IsPathReversed = 0;
        SS->First = SS->Last = 0;
    }
    while ((SS = SS->Suc) != FirstSSegment);

    FirstActive = LastActive = 0;
    ExchangeTimes = 0;

    Node *t1, *t2, *SUCt1;
    CostType Gain, G0, Cost;
    PromisedEdges *Nt1;
    Cost = 0;
    KeyValue = 0;
    i = 0;
    t1 = FirstNode;
    do//段数据中边权值比较交换改变当前路径
    {
        t2 = t1->OldSuc = t1->Suc;
        t1->OldPred = t1->Pred;
        t1->Priority = ++i;
        Cost += (t1->SucCost = t2->PredCost = COST(t1, t2)) - t1->Pi - t2->Pi;
        KeyValue ^= RandomNumSet[t1->Id] * RandomNumSet[t2->Id];
        t1->Cost = INT_MAX;
        for (Nt1 = t1->PromisedEdgesSet; (t2 = Nt1->To); Nt1++)
            if (t2 != t1->Pred && t2 != t1->Suc && Nt1->Cost < t1->Cost)
                t1->Cost = Nt1->Cost;
        t1->Parent = S;
        S->Size++;
        if (S->Size == 1)
            S->First = t1;
        S->Last = t1;
        if (SS->Size == 0)
            SS->First = S;
        S->Parent = SS;
        SS->Last = S;
        if (S->Size == SegSize)
        {
            S = S->Suc;
            SS->Size++;
            if (SS->Size == SSegmentSize)
                SS = SS->Suc;
        }
        t1->OldPredExcluded = t1->OldSucExcluded = 0;
        t1->Next = 0;
            Activate(t1);
    }
    while ((t1 = t1->Suc) != FirstNode);
    if (S->Size < SegSize)
        SS->Size++;
    Cost /= Accuracy;
    IsCostAvailable = 1;


    do
    {
        while ((t1 = RemoveFirstActive()))
        {
            SUCt1 = SUC(t1);

            for (X2 = 1; X2 <= 2; X2++)
            {
                t2 = X2 == 1 ? PRED(t1) : SUCt1;
                if (FixedOrCommon(t1, t2) ||
                    ( Near(t1, t2) ))
                    continue;
                G0 = COST(t1, t2);

                do
                    t2 = ExchangeTimes == 0 ? Optimal4Exchange(t1, t2, &G0, &Gain) :
                        Optimal4Exchange(t1, t2, &G0, &Gain);
                while (t2);
                if (Gain > 0)
                {
                    assert(Gain % Accuracy == 0);
                    Cost -= Gain / Accuracy;

                    StoreTour();
                    if (HashTableSearch(HTable, KeyValue, Cost))
                        goto End_LinKernighan;

                    Activate(t1);
                    break;
                }
                RestoreTour();
            }
        }
        if (HashTableSearch(HTable, KeyValue, Cost))
            goto End_LinKernighan;
        HashTableInsert(HTable, KeyValue, Cost);

        Gain = 0;
        if ((Gain = Gain23()) > 0)
        {
            assert(Gain % Accuracy == 0);
            Cost -= Gain / Accuracy;
            StoreTour();
            if (HashTableSearch(HTable, KeyValue, Cost))
                goto End_LinKernighan;
        }
    }
    while (Gain > 0);

End_LinKernighan:
    IsCostAvailable = 0;
    DiscrepancyNodeList();
    DiscrepancySegmentList();
    return Cost;
}

//针对段表中矛盾冲突部分一些操作
static void DiscrepancySegmentList()
{
    Segment *s1, *s2;

    s1 = FirstSegment;
    do {
        if (!s1->Parent->IsPathReversed)
            s2 = s1->Suc;
        else {
            s2 = s1->Pred;
            s1->Pred = s1->Suc;
            s1->Suc = s2;
        }
    }
    while ((s1 = s2) != FirstSegment);
}


//针对冲突点的操作
static void DiscrepancyNodeList()
{
    Node *t1, *t2;

    t1 = FirstNode;
    do {
        t2 = SUC(t1);
        t1->Pred = PRED(t1);
        t1->Suc = t2;
    }
    while ((t1 = t2) != FirstNode);
}


/*
 * 完成一次边交换之后,去除当前激活的起点
 */

static Node *RemoveFirstActive()
{
    Node *N = FirstActive;
    if (FirstActive == LastActive)
        FirstActive = LastActive = 0;
    else
        LastActive->Next = FirstActive = FirstActive->Next;
    if (N)
        N->Next = 0;
    return N;
}


static CostType BridgeGain(Node * s1, Node * s2, Node * s3, Node * s4,
           Node * s5, Node * s6, Node * s7, Node * s8, int Case6,
           CostType G);
/*
 * 通过非序列交换来改善路径
 */
CostType Gain23()
{
     Node *s1 = 0;
     short OldReversed = 0;
    Node *s2, *s3, *s4, *s5, *s6 = 0, *s7, *s8 = 0, *s1Stop;
    PromisedEdges *Ns2, *Ns4, *Ns6;
    CostType G0, G1, G2, G3, G4, G5, G6, Gain, Gain6;
    int X2, X4, X6, X8, Case6 = 0, Case8 = 0;
    int Breadth2, Breadth4, Breadth6;

    if (!s1 )
        s1 = FirstNode;
    s1Stop = s1;
    for (X2 = 1; X2 <= 2; X2++) {
        IsPathReversed = (X2 == 1 ? OldReversed : (OldReversed ^= 1));
        do {
          assert(s2 = SUC(s1));
            if (FixedOrCommon(s1, s2))
                continue;
            G0 = COST(s1, s2);
            Breadth2 = 0;

            for (Ns2 = s2->PromisedEdgesSet; (s3 = Ns2->To); Ns2++) {
                if (s3 == s2->Pred || s3 == s2->Suc)
                    continue;
                if (++Breadth2 > MAXWidth)
                    break;
                G1 = G0 - Ns2->Cost;
                for (X4 = 1; X4 <= 2; X4++) {
                    s4 = X4 == 1 ? SUC(s3) : PRED(s3);
                    if (FixedOrCommon(s3, s4))
                        continue;
                    G2 = G1 + COST(s3, s4);

                    if (X4 == 1 && s4 != s1 && !Forbidden(s4, s1) &&
                        2 * SegmentSize(s2, s3) <= Dim &&
                        (!c || G2 - c(s4, s1) > 0) &&
                        (G3 = G2 - COST(s4, s1)) > 0 &&
                        (Gain = BridgeGain(s1, s2, s3, s4, 0, 0, 0, 0, 0,
                                           G3)) > 0)
                        return Gain;
                    if (X4 == 2 &&
                        !Forbidden(s4, s1) &&
                        (!c || G2 - c(s4, s1) > 0) &&
                        (Gain = G2 - COST(s4, s1)) > 0) {
                        SWAP1(s1, s2, s3);
                        return Gain;
                    }
                    if (G2 - s4->Cost <= 0)
                        continue;
                    Breadth4 = 0;

                    for (Ns4 = s4->PromisedEdgesSet; (s5 = Ns4->To); Ns4++) {
                        if (s5 == s4->Pred || s5 == s4->Suc ||
                            (G3 = G2 - Ns4->Cost) <= 0)
                            continue;
                        if (++Breadth4 > MAXWidth)
                            break;

                        for (X6 = 1; X6 <= 2; X6++) {
                            if (X4 == 2) {
                                if (X6 == 1) {
                                    Case6 = 1 + !IsBetween(s2, s5, s4);
                                    s6 = Case6 == 1 ? SUC(s5) : PRED(s5);
                                } else {
                                    s6 = s6 ==
                                        s5->Pred ? s5->Suc : s5->Pred;
                                    if (s5 == s1 || s6 == s1)
                                        continue;
                                    Case6 += 2;
                                }
                            } else if (IsBetween(s2, s5, s3)) {
                                Case6 = 4 + X6;
                                s6 = X6 == 1 ? SUC(s5) : PRED(s5);
                                if (s6 == s1)
                                    continue;
                            } else {
                                if (X6 == 2)
                                    break;
                                Case6 = 7;
                                s6 = PRED(s5);
                            }
                            if (FixedOrCommon(s5, s6))
                                continue;
                            G4 = G3 + COST(s5, s6);
                            Gain6 = 0;
                            if (!Forbidden(s6, s1) &&
                                (!c || G4 - c(s6, s1) > 0) &&
                                (Gain6 = G4 - COST(s6, s1)) > 0) {
                                if (Case6 <= 2 || Case6 == 5 || Case6 == 6) {
                                    Make3OptMove(s1, s2, s3, s4, s5, s6,
                                                 Case6);
                                    return Gain6;
                                }
                                if ((Gain =
                                     BridgeGain(s1, s2, s3, s4, s5, s6, 0,
                                                0, Case6, Gain6)) > 0)
                                    return Gain;
                            }
                            Breadth6 = 0;

                            for (Ns6 = s6->PromisedEdgesSet; (s7 = Ns6->To);
                                 Ns6++) {
                                if (s7 == s6->Pred || s7 == s6->Suc
                                    || (s6 == s2 && s7 == s3) || (s6 == s3
                                                                  && s7 ==
                                                                  s2)
                                    || (G5 = G4 - Ns6->Cost) <= 0)
                                    continue;
                                if (++Breadth6 > MAXWidth)
                                    break;

                                for (X8 = 1; X8 <= 2; X8++) {
                                    if (X8 == 1) {
                                        Case8 = Case6;
                                        switch (Case6) {
                                        case 1:
                                            s8 = IsBetween(s2, s7,
                                                         s5) ? SUC(s7) :
                                                PRED(s7);
                                            break;
                                        case 2:
                                            s8 = IsBetween(s3, s7,
                                                         s6) ? SUC(s7) :
                                                PRED(s7);
                                            break;
                                        case 3:
                                            if (IsBetween(s5, s7, s4))
                                                s8 = SUC(s7);
                                            else {
                                                s8 = IsBetween(s3, s7,
                                                             s1) ? PRED(s7)
                                                    : SUC(s7);
                                                Case8 = 17;
                                            }
                                            break;
                                        case 4:
                                            if (IsBetween(s2, s7, s5))
                                                s8 = IsBetween(s2, s7,
                                                             s4) ? SUC(s7)
                                                    : PRED(s7);
                                            else {
                                                s8 = PRED(s7);
                                                Case8 = 18;
                                            }
                                            break;
                                        case 5:
                                            s8 = PRED(s7);
                                            break;
                                        case 6:
                                            s8 = IsBetween(s2, s7,
                                                         s3) ? SUC(s7) :
                                                PRED(s7);
                                            break;
                                        case 7:
                                            if (IsBetween(s2, s7, s3))
                                                s8 = SUC(s7);
                                            else {
                                                s8 = IsBetween(s5, s7,
                                                             s1) ? PRED(s7)
                                                    : SUC(s7);
                                                Case8 = 19;
                                            }
                                        }
                                    } else {
                                        if (Case8 >= 17 ||
                                            (Case6 != 3 && Case6 != 4
                                             && Case6 != 7))
                                            break;
                                        s8 = s8 ==
                                            s7->Pred ? s7->Suc : s7->Pred;
                                        Case8 += 8;
                                    }
                                    if (s8 == s1 ||
                                        (s7 == s1 && s8 == s2) ||
                                        (s7 == s3 && s8 == s4) ||
                                        (s7 == s4 && s8 == s3))
                                        continue;
                                    if (FixedOrCommon(s7, s8)
                                        || Forbidden(s8, s1))
                                        continue;
                                    G6 = G5 + COST(s7, s8);
                                    if ((!c || G6 - c(s8, s1) > 0) &&
                                        (Gain = G6 - COST(s8, s1)) > 0) {
                                        if (Case8 <= 15) {
                                            Make4OptMove(s1, s2, s3, s4,
                                                         s5, s6, s7, s8,
                                                         Case8);
                                            return Gain;
                                        }
                                        if (Gain > Gain6 &&
                                            (Gain =
                                             BridgeGain(s1, s2, s3, s4, s5,
                                                        s6, s7, s8, Case6,
                                                        Gain)) > 0)
                                            return Gain;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        while ((s1 = s2) != s1Stop);
    }
    return 0;
}


/*
 * BridgeGain函数尝试利用非序列交换来改善当前路径,它被Gain23调用
 */
static CostType BridgeGain(Node * s1, Node * s2, Node * s3, Node * s4,
           Node * s5, Node * s6, Node * s7, Node * s8, int Case6,
           CostType G)
{
    Node *u2 = 0, *u3 = 0;
    //PromisedEdges *Nt2, *Nt4, *Nt6;
    //CostType G0, G1, G2, G3, G4, G5, G6, Gain;
    //int X4;
   //int Breadth2, Breadth4, Breadth6;

    switch (Case6) {
    case 3:
        if (2 * SegmentSize(s5, s4) <= Dim) {
            u2 = s5;
            u3 = s4;
        } else {
            u2 = s3;
            u3 = s6;
        }
        break;
    case 4:
        if (2 * SegmentSize(s2, s5) <= Dim) {
            u2 = s2;
            u3 = s5;
        } else {
            u2 = s6;
            u3 = s1;
        }
        break;
    case 0:
    case 7:
        if (2 * SegmentSize(s2, s3) <= Dim) {
            u2 = s2;
            u3 = s3;
        } else {
            u2 = s4;
            u3 = s1;
        }
    }

    PromisedEdges *Nt2, *Nt4, *Nt6;
    CostType G0, G1, G2, G3, G4, G5, G6, Gain;
    int X4;
    int Breadth2, Breadth4, Breadth6;
    Node *t1, *t2, *t3, *t4, *t5, *t6, *t7, *t8;
    for (t1 = u2; t1 != u3; t1 = t2) {

        t2 = SUC(t1);
        //判断是否有相等的冲突情况
        if ((t1 == s1 && t2 == s2) ||
            (t1 == s2 && t2 == s1) ||
            (t1 == s3 && t2 == s4) ||
            (t1 == s4 && t2 == s3) ||
            (t1 == s5 && t2 == s6) ||
            (t1 == s6 && t2 == s5) ||
            (t1 == s7 && t2 == s8) ||
            (t1 == s8 && t2 == s7) || FixedOrCommon(t1, t2))
            continue;
        G0 = G + COST(t1, t2);

        Breadth2 = 0;
        for (Nt2 = t2->PromisedEdgesSet; (t3 = Nt2->To); Nt2++) {
            if (t3 == t2->Pred || t3 == t2->Suc || IsBetween(u2, t3, u3))
                continue;
            G1 = G0 - Nt2->Cost;
            if (++Breadth2 > MAXWidth)
                break;

            for (X4 = 1; X4 <= 2; X4++) {
                t4 = X4 == 1 ? SUC(t3) : PRED(t3);
                if (t4 == t2 ||
                    (t3 == s1 && t4 == s2) ||
                    (t3 == s2 && t4 == s1) ||
                    (t3 == s3 && t4 == s4) ||
                    (t3 == s4 && t4 == s3) ||
                    (t3 == s5 && t4 == s6) ||
                    (t3 == s6 && t4 == s5) ||
                    (t3 == s7 && t4 == s8) ||
                    (t3 == s8 && t4 == s7) || FixedOrCommon(t3, t4))
                    continue;
                G2 = G1 + COST(t3, t4);

                if (!Forbidden(t4, t1) && (!c || G2 - c(t4, t1) > 0)
                    && (Gain = G2 - COST(t4, t1)) > 0)
                    {
                    switch (Case6)
                    {
                    case 0:
                        if (X4 == 1)
                            SWAP3(s1, s2, s4, t3, t4, t1, s1, s3, s2);
                        else
                            SWAP2(t1, t2, t3, s1, s2, s3);
                        return Gain;
                    case 3:
                        if ((X4 == 1) ==
                            (!IsBetween(s2, t1, s6) && !IsBetween(s2, t3, s6)))
                            SWAP3(s1, s2, s3, t1, t2, t3, s5, s6, s1);
                        else
                            SWAP4(s1, s2, s3, t1, t2, t4, s5, s6, s1, t2,
                                  t4, t1);
                        if (s8)
                            SWAP1(s7, s8, s1);
                        return Gain;
                    case 4:
                        if ((X4 == 1) ==
                            (!IsBetween(s3, t1, s5) && !IsBetween(s3, t3, s5)))
                            SWAP3(s1, s2, s3, t1, t2, t3, s5, s6, s1);
                        else
                            SWAP4(s1, s2, s3, t1, t2, t4, s5, s6, s1, t2,
                                  t4, t1);
                        if (s8)
                            SWAP1(s7, s8, s1);
                        return Gain;
                    case 7:
                        if ((X4 == 1) ==
                            (!IsBetween(s4, t1, s6) && !IsBetween(s4, t3, s6)))
                            SWAP3(s5, s6, s1, t1, t2, t3, s3, s4, s5);
                        else
                            SWAP4(s5, s6, s1, t1, t2, t4, s3, s4, s5, t2,
                                  t4, t1);
                        if (s8)
                            SWAP1(s7, s8, s1);
                        return Gain;
                    }
                }

                if (Case6 != 0)
                    continue;
                Breadth4 = 0;

                for (Nt4 = t4->PromisedEdgesSet; (t5 = Nt4->To); Nt4++)
                {
                    if (t5 == t4->Pred || t5 == t4->Suc || t5 == t1
                        || t5 == t2)
                        continue;

                    t6 = X4 == 1
                        || IsBetween(u2, t5, u3) ? PRED(t5) : SUC(t5);
                    if ((t5 == s1 && t6 == s2) || (t5 == s2 && t6 == s1)
                        || (t5 == s3 && t6 == s4) || (t5 == s4 && t6 == s3)
                        || FixedOrCommon(t5, t6))
                        continue;
                    G3 = G2 - Nt4->Cost;
                    G4 = G3 + COST(t5, t6);
                    if (!Forbidden(t6, t1) &&
                        (!c || G4 - c(t6, t1) > 0) &&
                        (Gain = G4 - COST(t6, t1)) > 0)
                         {
                        if (X4 == 1)
                            SWAP4(s1, s2, s4, t3, t4, t1, s1, s3, s2, t5,
                                  t6, t1);
                        else
                            SWAP3(t1, t2, t3, s1, s2, s3, t5, t6, t1);
                        return Gain;
                    }
                    if (++Breadth4 > MAXWidth)
                        break;
                    Breadth6 = 0;

                    for (Nt6 = t6->PromisedEdgesSet; (t7 = Nt6->To); Nt6++)
                    {
                        if (t7 == t6->Pred || t7 == t6->Suc)
                            continue;
                        if (X4 == 1)
                            t8 = (IsBetween(u2, t5, t1) ? IsBetween(t5, t7, t1)
                                  : IsBetween(t2, t5, u3) ? IsBetween(u2, t7,
                                                                  t1)
                                  || IsBetween(t5, t7, u3) : IsBetween(SUC(u3),
                                                                   t5,
                                                                   t3) ?
                                  IsBetween(u2, t7, u3)
                                  || IsBetween(t5, t7, t3) : !IsBetween(t4, t7,
                                                                    t6)) ?
                                PRED(t7) : SUC(t7);
                        else
                            t8 = (IsBetween(u2, t5, t1) ?
                                  !IsBetween(u2, t7, t6)
                                  && !IsBetween(t2, t7, u3) : IsBetween(t2, t5,
                                                                    u3) ?
                                  !IsBetween(t2, t7, t6) : IsBetween(SUC(u3),
                                                                 t5,
                                                                 t4) ?
                                  !IsBetween(SUC(u3), t7, t5)
                                  && !IsBetween(t3, t7,
                                              PRED(u2)) : !IsBetween(t3, t7,
                                                                   t5)) ?
                                PRED(t7) : SUC(t7);
                        if (t8 == t1
                            || (t7 == t1 && t8 == t2) || (t7 == t3
                                                          && t8 == t4)
                            || (t7 == t4 && t8 == t3) || (t7 == s1
                                                          && t8 == s2)
                            || (t7 == s2 && t8 == s1) || (t7 == s3
                                                          && t8 == s4)
                            || (t7 == s4 && t8 == s3))
                            continue;
                        if (FixedOrCommon(t7, t8) || Forbidden(t8, t1))
                            continue;
                        G5 = G4 - Nt6->Cost;
                        G6 = G5 + COST(t7, t8);

                        if ((!c || G6 - c(t8, t1) > 0) &&
                            (Gain = G6 - COST(t8, t1)) > 0)
                            {
                            if (X4 == 1)
                                SWAP4(s1, s2, s4, t3, t4, t1, s1, s3, s2,
                                      t5, t6, t1);
                            else
                                SWAP3(t1, t2, t3, s1, s2, s3, t5, t6, t1);
                            SWAP1(t7, t8, t1);
                            return Gain;
                        }
                        if (++Breadth6 > MAXWidth)
                            break;
                    }
                }
            }
        }
    }

    return 0;
}
