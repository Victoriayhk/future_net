#include "LKH.h"

/*
 * The GenerateCandidates function associates to each node a set of incident 
 * candidate edges. The candidate edges of each node are sorted in increasing
 * order of their Alpha-values.
 *
 * The parameter MaxCandidates specifies the maximum number of candidate edges 
 * allowed for each node, and MaxAlpha puts an upper limit on their 
 * Alpha-values.
 *
 * A non-zero value of Symmetric specifies that the candidate set is to be
 * complemented such that every candidate edge is associated with both its 
 * two end nodes (in this way MaxCandidates may be exceeded). 
 *
 * The candidate edges of each node is kept in an array (CandidatSet) of
 * structures. Each structure (Candidate) holds the following information:
 *
 *      Node *To    : pointer to the other end node of the edge
 *      int Cost    : the cost (length) of the edge
 *      int Alpha   : the alpha-value of the edge
 *
 * The algorithm for computing Alpha-values in time O(n^2) and space O(n) 
 * follows the description in
 *
 *      Keld Helsgaun,
 *      An Effective Implementation of the Lin-Kernighan Traveling 
 *      Salesman Heuristic,
 *      Report, RUC, 1998. 
 */

static int Max(const int a, const int b)
{
    return a > b ? a : b;
}

void GenerateCandidates(int MaxCandidates, GainType MaxAlpha,int Symmetric)
{
    Node *From, *To;
    Candidate *NFrom, *NN;
    int a, d, Count;

	//limit MaxAlpha
    if (MaxAlpha < 0 || MaxAlpha > INT_MAX)
        MaxAlpha = INT_MAX;
	
    /* Free CandidateSet并且From->Mark=0*/
    FreeCandidateSets();
    From = FirstNode;
    do
        From->Mark = 0;
    while ((From = From->Suc) != FirstNode);
	
	//为CandidateSet申请内存（MaxCandidates个）
	do //上一循环最后已经有From = FirstNode;
	{
		assert(From->CandidateSet =(Candidate *) malloc((MaxCandidates + 1)*sizeof(Candidate)));
		From->CandidateSet[0].To = 0;
	}
	while ((From = From->Suc) != FirstNode);

    
	/* Loop for each node, From */
    do //上一循环最后已经有From = FirstNode;
	{
        NFrom = From->CandidateSet;
        if (From != FirstNode) 
		{
            From->Beta = INT_MIN;
            for (To = From; To->Dad != 0; To = To->Dad) 
			{
                To->Dad->Beta = !FixedOrCommon(To, To->Dad) ? Max(To->Beta, To->Cost) : To->Beta;
                To->Dad->Mark = From;
            }
        }
        Count = 0;
        /* Loop for each node, To */
        To = FirstNode;
        do 
		{
            if (To == From)
                continue;
            d = D(From, To);
            if (From == FirstNode)
                a = To == From->Dad ? 0 : d - From->NextCost;
            else if (To == FirstNode)
                a = From == To->Dad ? 0 : d - To->NextCost;
            else 
			{
                if (To->Mark != From)
                    To->Beta = !FixedOrCommon(To, To->Dad) ? Max(To->Dad->Beta, To->Cost) : To->Dad->Beta;
                a = d - To->Beta;
            }
            if (FixedOrCommon(From, To))
                a = INT_MIN;
            else if (Forbidden(From, To))
				continue;
			
            if (a <= MaxAlpha) 
			{
                /* Insert new candidate edge in From->CandidateSet */
                NN = NFrom;
                while (--NN >= From->CandidateSet) 
				{
                    if (a > NN->Alpha || (a == NN->Alpha && d >= NN->Cost))
                        break;
                    *(NN + 1) = *NN;
                }
				//From->CandidateSet[i]，把a插入进去
                NN++;	NN->To = To;	NN->Cost = d;	NN->Alpha = a;
                if (Count < MaxCandidates) { Count++;	NFrom++;}
                NFrom->To = 0;//最后一个要拱出去
            }
        }
        while ((To = To->Suc) != FirstNode);
    }
    while ((From = From->Suc) != FirstNode);

    //AddTourCandidates();//其实没啥用，在GenerateCandidates中该加进去肯定会加进去fixed的边
    if (Symmetric)
        SymmetrizeCandidateSet();
}
