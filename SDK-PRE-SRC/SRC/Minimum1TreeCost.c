#include "LKH.h"

/*
 * The Minimum1TreeCost function returns the cost of a minimum 1-tree.
 *
 * The minimum 1-tre is found by determining the minimum spanning tree and 
 * then adding an edge corresponding to the second nearest neighbor of one 
 * of the leaves of the tree (any node which has degree 1). The leaf chosen
 * is the one that has the longest second nearest neighbor distance.
 *
 * The V-value of a node is its degree minus 2. Therefore, Norm being the 
 * sum of squares of all V-values, is a measure of a minimum 1-tree/s 
 * discrepancy from a tour. If Norm is zero, then the 1-tree constitutes a 
 * tour, and an optimal tour has been found.
 */

GainType Minimum1TreeCost(int Sparse)
{
    Node *N, *N1 = 0;
    GainType Sum = 0;

	//求最小1Tree的第一步是求最小生成树
    MinimumSpanningTree(Sparse);//N->Cost是原始权值*Precision+Na->Pi+Nb->Pi
	
	//每个点的度先减去2，并且计算每个点的Pi的Sum
    N = FirstNode;
    do{	N->V = -2;	Sum += N->Pi;}
    while ((N = N->Suc) != FirstNode);
	
	//最小生成树除了根节点以外的，每个点和父亲节点各自度+1,并且计算最小生成树的权值Sum（已经减去了2*Σpi）
    Sum *= -2;
    while ((N = N->Suc) != FirstNode)//上一循环最后已经有N = FirstNode;
	{
        N->V++;
        N->Dad->V++;
        Sum += N->Cost;
        N->Next = 0;//辅助用的
    }
	
	//增加根节点到FirstNode->Suc的边，形成个环
    FirstNode->Dad = FirstNode->Suc;//？？？
    FirstNode->Cost = FirstNode->Suc->Cost;//？？？
	int Max = INT_MIN;
    do //上一循环最后已经有N = FirstNode;
	{
        if (N->V == -1) //度只有1，最小生成树中的叶子节点或者根节点
		{
			//从N发射出去的最短edge（但是不包括在最小生成树中）
			//（是fixed的就返回fixed，不是的话找最小）,N->Next记录指向的节点
            Connect(N, Max, Sparse);
            if (N->NextCost > Max) //记录最大的 "最短edge"
			{
                N1 = N;				//N1用来记录这个点
                Max = N->NextCost;
            }
        }
    }
    while ((N = N->Suc) != FirstNode);
	
	//增加从N1到N1->Next的边，两者度+1
    N1->Next->V++;
    N1->V++;
    Sum += N1->NextCost;//Sum也加N1->NextCost，也就是最小1-tree的值（当然*Precision了）
    
	//计算度-2的平方和Norm
	Norm = 0;
    do//上一循环最后已经有N = FirstNode;
        Norm += N->V * N->V;
    while ((N = N->Suc) != FirstNode);
	
	//让N1作为FirstNode，第二个为root（Dad = 0）
    if (N1 == FirstNode)
        N1->Suc->Dad = 0;
    else 
	{
        FirstNode->Dad = 0;
        Precede(N1, FirstNode);
        FirstNode = N1;
    }
	
	//恰好Norm==0，最小1-tree是一个tour
    if (Norm == 0) 
	{
        for (N = FirstNode->Dad; N; N1 = N, N = N->Dad)
            Follow(N, N1);
        for (N = FirstNode->Suc; N != FirstNode; N = N->Suc)
            N->Dad = N->Pred;
        FirstNode->Suc->Dad = 0;
    }
    return Sum;
}
