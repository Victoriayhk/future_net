#include "LKH.h"

/*
 * The AddTourCandidates function extends the candidate set with tour
 * edges given in the tour files.
 *   
 * The function is called from GenerateCandidateSet and OrderCandidateSet.  
*/
//加入互为fixed节点之间的边
void AddTourCandidates()
{
    Node *Na, *Nb;

    /* Add fixed edges */
    Na = FirstNode;
    do 
	{
        if (Na->FixedTo1)
            AddCandidate(Na, Na->FixedTo1, D(Na, Na->FixedTo1), 0);
    }
    while ((Na = Na->Suc) != FirstNode);
}
