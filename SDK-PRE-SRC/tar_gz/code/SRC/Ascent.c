#include "LKH.h"

/* 
 * The Ascent function computes a lower bound on the optimal tour length 
 * using subgradient optimization. The function also transforms the original 
 * problem into a problem in which the Alpha-values reflect the likelihood 
 * of edges being optimal.
 *
 * The function attempts to find penalties (Pi-values) that maximizes the 
 * lower bound L(T(Pi)) - 2*PiSum, where L(T(Pi)) denotes the length of the 
 * minimum spanning 1-tree computed from the transformed distances, and PiSum 
 * denotes the sum of Pi-values. If C(i,j) denotes the length of an edge 
 * (i,j), then the transformed distance D(i,j) of an edge is 
 * C(i,j) + Pi(i) + Pi(j).
 *
 * The Minimum1TreeCost function is used to compute the cost of a minimum 
 * 1-tree.The Generatecandidates function is called in order to generate 
 * candidate sets. Minimum 1-trees are then computed in the corresponding 
 * sparse graph.         
 */

GainType Ascent()
{
    Node *t;
    GainType BestW, W, W0, Alpha, MaxAlpha = INT_MAX;
    int T, Period, P, InitialPhase, BestNorm;

  Start:
    /* 所有Node的pi赋值为0，Initialize Pi and BestPi */
    t = FirstNode;
    do
        t->Pi = t->BestPi = 0;
    while ((t = t->Suc) != FirstNode);
    //AddTourCandidates();//have read

    /* Compute the cost of a minimum 1-tree，0:Sparse,1:dense */
    W = Minimum1TreeCost(0);//have read
	if (!Norm)	return W;

    /* Generate symmetric candididate sets for all nodes */
	GenerateCandidates(AscentCandidates, MaxAlpha, 1);//have read

    /* Set LastV of every node to V (the node's degree in the 1-tree) */
    t = FirstNode;
    do
        t->LastV = t->V;
    while ((t = t->Suc) != FirstNode);

	//次梯度优化开始
    BestW = W0 = W;
    BestNorm = Norm;
    InitialPhase = 1;
	
    /* Perform subradient optimization with decreasing period length and decreasing step size */
    for (Period = InitialPeriod, T = InitialStepSize * Precision;//InitialPeriod=100,InitialStepSize=1,Precision=100
         Period > 0 && T > 0 && Norm != 0; Period /= 2, T /= 2) /* Period and step size are halved at each iteration */
	{
        for (P = 1; T && P <= Period && Norm != 0; P++) //100次，50次，25次，12次，6次...
		{
            /* Adjust the Pi-values */
            t = FirstNode;
            do 
			{
                if (t->V != 0) 
				{
                    t->Pi += T * (7 * t->V + 3 * t->LastV) / 10;//T是步长，7比3的互补滤波
                    if (t->Pi > INT_MAX / 4)	t->Pi = INT_MAX / 4;	//对于pi的限幅
                    else if (t->Pi < -INT_MAX / 4)	t->Pi = -INT_MAX / 4;
                }
                t->LastV = t->V;
            }
            while ((t = t->Suc) != FirstNode);
			
            /* Compute a minimum 1-tree in the sparse graph */
            W = Minimum1TreeCost(1);//由于运行很多次，所以用稀疏图比较好
			//printf("* T = %d, Period = %d, P = %d, BestW = %0.1f, Norm = %d\n",
              //           T, Period, P, (double) BestW / Precision, Norm);

            /* Test if an improvement has been found */
            if (W > BestW || (W == BestW && Norm < BestNorm)) 
			{
                /* If the lower bound becomes greater than twice its initial value 
				it is taken as a sign that the graph might be too sparse */
                if (W - W0 > (W0 >= 0 ? W0 : -W0) && AscentCandidates > 0&& AscentCandidates < Dimension) 
				{
                    W = Minimum1TreeCost(0);//dense
                    if (W < W0) 
					{
                        /* Double the number of candidate edges and start all over again */
                        if ((AscentCandidates *= 2) > Dimension)
                            AscentCandidates = Dimension;
                        goto Start;
                    }
                    W0 = W;
                }
                BestW = W;
                BestNorm = Norm;
				
                /* Update the BestPi-values */
                t = FirstNode;
                do
                    t->BestPi = t->Pi;
                while ((t = t->Suc) != FirstNode);
                // printf("* T = %d, Period = %d, P = %d, BestW = %0.1f, Norm = %d\n",
                //          T, Period, P, (double) BestW / Precision, Norm);
				
                /* If in the initial phase, the step size is doubled */
                if (InitialPhase &&  T*sqrt((double) Norm) > 0)
                    T *= 2;
                /* If the improvement was found at the last iteration of the current period, then double the period */
                if (P == Period && (Period *= 2) > InitialPeriod)
                    Period = InitialPeriod;
            } 
			else if (InitialPhase && P > Period / 2) 
			{
                // printf("  T = %d, Period = %d, P = %d, W = %0.1f, Norm = %d\n",
                //          T, Period, P, (double) W / Precision, Norm);
				/* Conclude the initial phase */
				InitialPhase = 0;
				P = 0;
				T = 3 * T / 4;
			}
        }
    }
	
	/* Update the Pi using BestPi-values */
    t = FirstNode;
    do 
	{
        t->Pi = t->BestPi;
        t->BestPi = 0;
    } while ((t = t->Suc) != FirstNode);

    /* Compute a minimum 1-tree using BestPi-values*/
    W = BestW = Minimum1TreeCost(0);//dense
    return W;
}
