#include "LKH.h"

/*
 * The CreateCandidateSet function determines for each node its set of incident
 * candidate edges.
 *
 * The Ascent function is called to determine a lower bound on the optimal tour 
 * using subgradient optimization. But only if the penalties (the Pi-values) is
 * not available on file. In the latter case, the penalties is read from the 
 * file, and the lower bound is computed from a minimum 1-tree.      
 *
 * The function GenerateCandidates is called to compute the Alpha-values and to 
 * associate to each node a set of incident candidate edges.  
 *
 * The CreateCandidateSet function itself is called from LKHmain.
 */

void CreateCandidateSet()
{
    GainType Cost, MaxAlpha, A;
    Node *Na;
	
	//次梯度优化
	Cost = Ascent();//have read
	
	//为每个节点创建5个非对称的Candidates
    LowerBound = (double) Cost / Precision;
    MaxAlpha = (GainType) fabs(Excess * Cost);
    FreeCandidateSets();
	GenerateCandidates(MaxCandidates, MaxAlpha, CandidateSetSymmetric);//5,max,0
    ResetCandidateSet();
	
	//******************************** my code ***************************************
	//printf CandidateSet;
	// printf_1var(V.num);
	// int n=0;
	// for(n=0;n<84;n++)
	// 	printf("%d|",n);

	// Node *From = FirstNode;
	// do{
	// 	int cn = 0;
	// 	Candidate *NFrom = From->CandidateSet;
		
	// 	if(From->Id - 1 >= Dimension/2)
	// 		continue;
	// 	printf("{%3d",Rp0->VVlist[From->Id - 1]);		//LKH源码下标从1开始，自己的从0开始
		
	// 	//获得数量
	// 	while(NFrom->To != 0)
	// 	{
	// 		NFrom++;
	// 		cn++;
	// 	}
		
	// 	printf(",%2d",cn);
	// 	NFrom = From->CandidateSet;
	// 	while(NFrom->To != 0)
	// 	{
	// 		printf(",%3d",Rp0->VVlist[NFrom->To->Id - 1-Dimension/2]);//LKH源码下标从1开始，自己的从0开始
	// 		NFrom++;
	// 	}
	// 	printf("},\n");
 //    }
 //    while ((From = From->Suc) != FirstNode);
 //    
	/*
	//printf must-node topo for 易慧康
	printf("\n\n");
	int i,j,edgeCn=0;
	for(i=0;i < Rp0->VVnum;i++)
		for(j=0;j < Rp0->VVnum;j++)
			if(Rp0->VVcost[i][j] != MaxECost)
				printf("%d,%d,%d,%d\n",edgeCn++,i,j,Rp0->VVcost[i][j]);
	*/
}
