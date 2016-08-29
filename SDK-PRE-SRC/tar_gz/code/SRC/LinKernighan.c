#include "Segment.h"
#include "LKH.h"
#include "Hashing.h"
#include "Sequence.h"

/*
 * The LinKernighan function seeks to improve a tour by sequential 
 * and non-sequential edge exchanges.
 *
 * The function returns the cost of the resulting tour. 
 */

GainType LinKernighan()
{
    Node *t1, *t2, *SUCt1;
    GainType Gain, G0, Cost;
    int X2, i, it = 0;
    Candidate *Nt1;
    Segment *S;
    SSegment *SS;

	//初始化Segment
    S = FirstSegment;
    i = 0;
    do {	S->Size = 0;	S->Rank = ++i;	S->Reversed = 0;	S->First = S->Last = 0;	}
    while ((S = S->Suc) != FirstSegment);
	
	//初始化SSegment
    SS = FirstSSegment;
    i = 0;
    do{	SS->Size = 0;	SS->Rank = ++i;	SS->Reversed = 0;	SS->First = SS->Last = 0;}
    while ((SS = SS->Suc) != FirstSSegment);

    
	/* Compute the cost of the initial tour, Cost.
       Compute the corresponding hash value, Hash.
       Initialize the segment list.
       Make all nodes "active" (so that they can be used as t1). */
    FirstActive = LastActive = 0;
	Cost = 0;
    Hash = 0;
    i = 0;
    t1 = FirstNode;
    do 
	{
		//初始化OldSuc，OldPred
        t2 = t1->OldSuc = t1->Suc;
        t1->OldPred = t1->Pred;
        t1->Rank = ++i;//它是tour中第几个
        Cost += (t1->SucCost = t2->PredCost = D(t1, t2)) - t1->Pi - t2->Pi;//原来是C(t1, t2)
		Hash ^= Rand[t1->Id] * Rand[t2->Id];
        
		//得到每个node的best cost（不是前任也不是后继）
		t1->Cost = INT_MAX;
        for (Nt1 = t1->CandidateSet; (t2 = Nt1->To); Nt1++)
            if (t2 != t1->Pred && t2 != t1->Suc && Nt1->Cost < t1->Cost)
                t1->Cost = Nt1->Cost;
        
		//初始化 t1->Parent、Segment
		t1->Parent = S;//第一次S=FirstSegment
        S->Size++;
        if (S->Size == 1)	
			S->First = t1;
        S->Last = t1;
		
		//初始化 SSegment
        if (SS->Size == 0)	
			SS->First = S;
        S->Parent = SS;	SS->Last = S;
        
		//每个Segment满了
		if (S->Size == GroupSize) 
		{
            S = S->Suc;	SS->Size++;
			//每个SSegment满了
            if (SS->Size == SGroupSize)//SGroupSize=Dimension
                SS = SS->Suc;
        }
		//初始化OldPredExcluded、OldSucExcluded
        t1->OldPredExcluded = t1->OldSucExcluded = 0;
        t1->Next = 0;//为Activate queue做准备
        Activate(t1);
    }
    while ((t1 = t1->Suc) != FirstNode);
	
	//最后一个Segment不满也算一个整的Segment，所以SS->Size++
    if (S->Size < GroupSize)	SS->Size++;
	
	
	//k-opt调整路,Loop as long as improvements are found	
	Reversed = 0;
    Swaps = 0;
    Cost /= Precision;
    PredSucCostAvailable = 1;//used in C_FUNCTION,其实对于小规模的case（<5000）没多大卵用
    do
	{  int a=clock(),b;
    	/* Choose t1 as the first "active" node */
        while ((t1 = RemoveFirstActive()))//直到active队列没有元素
		{
            /* t1 is now "passive" */
            SUCt1 = SUC(t1);
            /* Choose t2 as one of t1's two neighbors on the tour */
            for (X2 = 1; X2 <= 2; X2++)//1次还是2次没啥卵用
			{
                t2 = X2 == 1 ? PRED(t1) : SUCt1;
                if (Fixed(t1, t2) || (Near(t1, t2) &&(Trial == 1)))//不去破坏,后一条规则没啥卵用
                    continue;
                G0 = C(t1, t2);
                /* Try to find a tour-improving chain of moves */
                do
                    t2 = BestMove(t1, t2, &G0, &Gain);
                while (t2);
				
                if (Gain > 0) 
				{	/* An improvement has been found */
                    assert(Gain % Precision == 0);
                    Cost -= Gain / Precision;
                    StoreTour();
                    if (HashSearch(HTable, Hash, Cost))
                        goto End_LinKernighan;
                    /* Make t1 "active" again */
                    Activate(t1);
                    break;
                }
                RestoreTour();
            }
        }
        //b= clock();
        // printf("5opt");printf_1var(b-a);
            

        if (HashSearch(HTable, Hash, Cost))
            goto End_LinKernighan;
        HashInsert(HTable, Hash, Cost);
        
		/* Try to find improvements using non-sequential 4/5-opt moves */
        a= clock();
        Gain = 0;
        if ((Gain = Gain23()) > 0) 
		{ 	/* An improvement has been found */
            assert(Gain % Precision == 0);
            Cost -= Gain / Precision;
            StoreTour();
            if (HashSearch(HTable, Hash, Cost))
                goto End_LinKernighan;
        }
        // b= clock();
         // printf("gain23");printf_1var(b-a);
    }
    while (Gain > 0);//当采用Gain23能改进时候继续啊

  End_LinKernighan:
    PredSucCostAvailable = 0;
    NormalizeNodeList();
    NormalizeSegmentList();
    return Cost;
}


/*
 * The NormalizeNodeList function is used to swap the Suc and Pred fields 
 * of nodes in such a way that the list of nodes constitutes a cyclic 
 * two-way list. 
 *
 * A call of the function corrupts the segment list representation.   
 */
//不管正反Reversed，因为每次LinKernighan会Reversed
void NormalizeNodeList()
{
    Node *t1, *t2;

    t1 = FirstNode;
    do
	{
        t2 = SUC(t1);
        t1->Pred = PRED(t1);
        t1->Suc = t2;
    }
    while ((t1 = t2) != FirstNode);
}


/*
 * The NormalizeSegmentList function is used to swap the Suc and Pred fields 
 * of segments in such a way that the list of segments constitutes a cyclic 
 * two-way list. 
 *
 * A call of the function corrupts the tree representation of the tour.   
 *
 * The function is called from LinKernighan.   
 */
void NormalizeSegmentList()
{
    Segment *s1, *s2;

    s1 = FirstSegment;
    do {
        if (!s1->Parent->Reversed)
            s2 = s1->Suc;
        else {
            s2 = s1->Pred;
            s1->Pred = s1->Suc;
            s1->Suc = s2;
        }
    }
    while ((s1 = s2) != FirstSegment);
}
