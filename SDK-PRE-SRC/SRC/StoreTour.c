#include "Segment.h"
#include "LKH.h"

/* 
 * The StoreTour function is called each time the tour has been improved by 
 * the LinKernighan function.
 *
 * The function "activates" all nodes involved in the current sequence of moves.
 *
 * It sets OldPred to Pred and OldSuc to Suc for each of these nodes. In this
 * way it can always be determined whether an edge belongs to current starting
 * tour. This is used by the BestMove function to determine whether an edge is
 * excludable.
 *
 * Finally, for each of these nodes the function updates their Cost field.
 * The Cost field contains for each node its minimum cost of candidate edges 
 * not on the tour. The value is used by the BestMove function to decide 
 * whether a tentative（暂定的） non-gainful move should be considered. 
 */

void StoreTour()
{
    Node *t, *u;
    Candidate *Nt;
    int i;

    while (Swaps > 0) 
	{
        Swaps--;
        for (i = 1; i <= 4; i++)//取出stack顶
		{
            t = i == 1 ? SwapStack[Swaps].t1 :
                i == 2 ? SwapStack[Swaps].t2 :
                i == 3 ? SwapStack[Swaps].t3 : SwapStack[Swaps].t4;
            Activate(t);
            t->OldPred = t->Pred;
            t->OldSuc = t->Suc;
            t->OldPredExcluded = t->OldSucExcluded = 0;
            t->Cost = INT_MAX;
            for (Nt = t->CandidateSet; (u = Nt->To); Nt++)
                if (u != t->Pred && u != t->Suc && Nt->Cost < t->Cost)
                    t->Cost = Nt->Cost;
        }
    }
}


/*
 * The RestoreTour function is used to undo a series of moves. The function 
 * restores the tour from SwapStack, the stack of 2-opt moves. A bad sequence 
 * of moves is undone by unstacking the 2-opt moves and making the inverse 
 * 2-opt moves in this reversed sequence.
 */

void RestoreTour()
{
    Node *t1, *t2, *t3, *t4;

    /* Loop as long as the stack is not empty */
    while (Swaps > 0) {
        /* Undo topmost 2-opt move */
        Swaps--;
        t1 = SwapStack[Swaps].t1;
        t2 = SwapStack[Swaps].t2;
        t3 = SwapStack[Swaps].t3;
        t4 = SwapStack[Swaps].t4;
        Swap1(t3, t2, t1);
        Swaps--;
        /* Make edges (t1,t2) and (t2,t3) excludable again */
        t1->OldPredExcluded = t1->OldSucExcluded = 0;
        t2->OldPredExcluded = t2->OldSucExcluded = 0;
        t3->OldPredExcluded = t3->OldSucExcluded = 0;
        t4->OldPredExcluded = t4->OldSucExcluded = 0;
    }
}
