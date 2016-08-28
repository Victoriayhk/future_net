#include "LKH.h"

/* 
 * Functions for computing the transformed distance of an edge (Na,Nb). 
 */


/*
 * The C_FUNCTION function is used when the distance is defined by a
 * function (e.g. the Euclidean distance function). In order to speed
 * up the computations the following algorithm used:
 *
 *  (1) If (Na,Nb) is an edge on the current tour, then its distance 
 *      is available in either the field PredCost or SucCost.
 *	   
 *  (2) A hash table (CacheVal) is consulted to see if the distance has
 *      been stored. 
 *	    
 *  (3) Otherwise the distance function is called and the distance computed
 *      is stored in the hash table.	    	      
 */

//report45业中说超过5000个点才用
int C_FUNCTION(Node * Na, Node * Nb)
{
    Node *Nc;
    Candidate *Cand;
    int Index, i, j;

    if (PredSucCostAvailable) 
	{
        if (Na->Suc == Nb)
            return Na->SucCost;
        if (Na->Pred == Nb)
            return Na->PredCost;
    }
    if ((Cand = Na->CandidateSet))
        for (; (Nc = Cand->To); Cand++)
            if (Nc == Nb)
                return Cand->Cost;
    if ((Cand = Na->BackboneCandidateSet))
        for (; (Nc = Cand->To); Cand++)
            if (Nc == Nb)
                return Cand->Cost;
    if (CacheSig == 0)
        return D(Na, Nb);
    i = Na->Id;
    j = Nb->Id;
    if (i > j) //i大的话就交换次序，让i小
	{
        int k = i;
        i = j;
        j = k;
    }
    Index = ((i << 8) + i + j) & CacheMask;//???
    if (CacheSig[Index] == i)
        return CacheVal[Index];
    CacheSig[Index] = i;
    return (CacheVal[Index] = D(Na, Nb));
}

//是Fixed就返回Na->Pi + Nb->Pi
//不是的话就Distance_ATSP*Precision+Na->Pi + Nb->Pi
int D_FUNCTION(Node * Na, Node * Nb)
{
    return (Fixed(Na, Nb) ? 0 : Distance_ATSP(Na, Nb) * Precision)
		   + Na->Pi + Nb->Pi;
}

/* 
 * Functions for computing distances (see TSPLIB).
 *
 * The appropriate function is referenced by the function pointer Distance.
 */
//总的原则：原点ABC是出度，复制点A'B'C'是入度
//都是原始点或都是复制点就返回最大值M
//其中一个是另一个的复制点就返回0
//一个原始点一个复制点正常返回atsp的权值
int Distance_ATSP(Node * Na, Node * Nb)
{
    int n = DimensionSaved;
    if ((Na->Id <= n) == (Nb->Id <= n))
        return M;//???为什么不更大？
    if (abs(Na->Id - Nb->Id) == n)
        return 0;
    return Na->Id < Nb->Id ? Na->C[Nb->Id - n] : Nb->C[Na->Id - n];
}


/* 
 * The Forbidden function is used to test if an edge, (ta,tb), 
 * is one of the forbidden edges (C(ta, tb) == M) in a solution of
 * an asymmetric traveling saleman problem. 
 * If the edge is forbidden, the function returns 1; otherwise 0.
 */

//同时小于或大于n就禁止，返回1
int Forbidden(const Node * ta, const Node * tb)
{
    return (ta->Id <= DimensionSaved) == (tb->Id <= DimensionSaved);
}

