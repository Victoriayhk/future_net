#include "LKH.h"

/*
 * The ChooseInitialTour function generates a pseudo-random initial tour. 
 * The algorithm constructs a tour as follows. 
 *
 * First, a random node N is chosen.
 *
 * Then, as long as no all nodes have been chosen, choose the next node to 
 * follow N in the tour, NextN, and set N equal to NextN.
 *
 * NextN is chosen as follows:
 *  
 *  (A) If possible, choose NextN such that (N,NextN) is a fixed edge, or
 *      is common to two or more tours to be merged.
 *  (B) Otherwise, if possible, and Trial = 1, choose NextN such that 
 *      (N,NextN) is an edge of a given initial tour.
 *  (C) Otherwise, if possible, choose NextN so that (N,NextN) is a 
 *      candidate edge, the alpha-value of (N,NextN) is zero, and (N,NextN) 
 *      belongs to the current best or next best tour.
 *  (D) Otherwise, if possible, choose NextN such that (N,NextN) is a 
 *      candidate edge.
 *  (E) Otherwise, choose NextN at random among those nodes not already 
 *      chosen.
 *
 *  When more than one node may be chosen, the node is chosen at random 
 *  among the alternatives (a one-way list of nodes). 
 *
 *  The sequence of chosen nodes constitutes the initial tour.
 */
void ChooseInitialTour()
{
    Node *N, *NextN, *FirstAlternative, *Last;
    Candidate *NN;
    int Alternatives, Count, i;

    if (Trial == 1)
	{
        if (InitialTourAlgorithm == BORUVKA ||
            InitialTourAlgorithm == GREEDY ||
            InitialTourAlgorithm == MOORE ||
            InitialTourAlgorithm == NEAREST_NEIGHBOR ||
            InitialTourAlgorithm == QUICK_BORUVKA ||
            InitialTourAlgorithm == SIERPINSKI) 
		{
            GainType Cost = InitialTourAlgorithm == MOORE || InitialTourAlgorithm == SIERPINSKI ? 
							SFCTour(InitialTourAlgorithm) : GreedyTour();
            return;
        }
    }

Start:
    /* Mark all nodes as "not chosen" by setting their V field to zero */
    N = FirstNode;
    do
        N->V = 0;
    while ((N = N->Suc) != FirstNode);
    Count = 0;

    /* Mark FirstNode as chosen */
    FirstNode->V = 1;
    N = FirstNode;

    /* Loop as long as not all nodes have been chosen */
    while (N->Suc != FirstNode) 
	{
        FirstAlternative = 0;
        Alternatives = 0;
        Count++;

        /* Case A */
        for (NN = N->CandidateSet; (NextN = NN->To); NN++) 
		{
            if (!NextN->V && Fixed(N, NextN)) 
			{
                Alternatives++;
                NextN->Next = FirstAlternative;
                FirstAlternative = NextN;
				break;
            }
        }
		//没有Fixed的点，而且不是第一次选初始路
        if (Alternatives == 0 && Trial > 1)
		{
            /* Case C */
            for (NN = N->CandidateSet; (NextN = NN->To); NN++) 
			{
                if (!NextN->V && NN->Alpha == 0 && 
						(InBestTour(N, NextN) || InNextBestTour(N, NextN))) 
				{
                    Alternatives++;
                    NextN->Next = FirstAlternative;
                    FirstAlternative = NextN;
                }
            }
        }
        if (Alternatives == 0) 
		{
            /* Case D */
            for (NN = N->CandidateSet; (NextN = NN->To); NN++) 
			{
                if (!NextN->V)
				{
                    Alternatives++;
                    NextN->Next = FirstAlternative;
                    FirstAlternative = NextN;
                }
            }
        }
        if (Alternatives == 0) //有点难，找到从现在这个位置找到一个不禁止连接的点
		{
            /* Case E (actually not really a random choice) */
            NextN = N->Suc;
            while (Forbidden(N, NextN) && NextN->Suc != FirstNode)
                NextN = NextN->Suc;
            if (Forbidden(N, NextN))//此时NextN->Suc==FirstNode，已经到头了，只能从头找了
			{
                FirstNode = FirstNode->Suc;
                goto Start;
            }
        }
		else 
		{
            NextN = FirstAlternative;
            if (Alternatives > 1) 
			{
                /* Select NextN at random among the alternatives */
                i = Random() % Alternatives;
                while (i--)
                    NextN = NextN->Next;
            }
        }
        /* Include NextN as the successor of N */
        Follow(NextN, N);
        N = NextN;
        N->V = 1;
    }
    if (Forbidden(N, N->Suc)) 
	{
        FirstNode = FirstNode->Suc;
        goto Start;
    }
}
