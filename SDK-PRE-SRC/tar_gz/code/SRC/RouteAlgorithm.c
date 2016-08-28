#include "LKH.h"

/*************************************************************************
*  函数名称：BFS_aglorithm
*  功能说明：BFS状态压缩算法计算must-node的atsp最短路
*  参数说明：无
*  函数返回：无
*  修改时间：2016-3-22
*  有待优化：优先队列，或者别的算法
*************************************************************************/
int dp[16][1<<15u];

struct
{
	int nowNd;
	unsigned short state;
	unsigned long father;
	int cost;
}StateQ[1000000];//100w

struct
{
    int nextNd[MaxV];
	int	nextCn;
}mNODE[MaxV];

static __inline setStateQ(unsigned long i,
						  int nowNd,
						  unsigned short state,
						  unsigned long father,
						  int cost)
{
	StateQ[i].nowNd = nowNd;
	StateQ[i].state = state;
	StateQ[i].father = father;
	StateQ[i].cost = cost;
}

void BFS_algorithm()
{
	int i,j;
	
	//initial mNODE
	for(i = 0;i < Rp0->VVnum;i++)
		for(j = 0;j < Rp0->VVnum;j++)
			if(Rp0->VVcost[i][j] < MaxECost)
				mNODE[i].nextNd[mNODE[i].nextCn++] = j;
	//initial dp
	int tmpnum = (1<<Rp0->VVnum);
	for(i = 0;i < Rp0->VVnum;i++)
		for(j = 0;j < tmpnum;j++)
			dp[i][j] = MaxECost;
	//BFS initial
	unsigned long pre = 0,end = 1,minadd = 0;
	int nowNd,cost,nextCn,nextNd;
	int mincost = MaxECost;
	unsigned short state;
	//BFS
	setStateQ(0,0,1,0,0);
	while(pre < end)
	{	
		//pop
		state = StateQ[pre].state;
		nowNd = StateQ[pre].nowNd;	
		cost  = StateQ[pre].cost;
		pre++;
		//all nodes is visited
		if(state == (1<<Rp0->VVnum) - 1)
		{
			if(mincost > cost + Rp0->VVcost[nowNd][Rp0->VVnum])
			{
				mincost = cost + Rp0->VVcost[nowNd][Rp0->VVnum];
				minadd = pre - 1;
			}
			continue;
		}
		//add next nodes
		nextCn = mNODE[nowNd].nextCn;
		for(i = 0;i < nextCn;i++)
		{
			nextNd = mNODE[nowNd].nextNd[i];
			if(!(state & (1<<nextNd)) 
					&& cost + Rp0->VVcost[nowNd][nextNd] < dp[nextNd][state|(1<<nextNd)] 
					&& cost + Rp0->VVcost[nowNd][nextNd] < mincost)
			{
				//printf("address=%d,nowNd=%d,state=%d,father=%ld,cost=%d\n",
				//	   end,nextNd,state|(1<<nextNd),pre-1,cost+Rp0->VVcost[nowNd][nextNd]);
				dp[nextNd][state|(1<<nextNd)] = cost + Rp0->VVcost[nowNd][nextNd];
				setStateQ(end++,nextNd,state|(1<<nextNd),pre-1,cost+Rp0->VVcost[nowNd][nextNd]);
			}
		}
	}
	BestCost = mincost;
	//no road then return
	if(BestCost >= MaxECost)	
		return ;
	//set_VVlistRst
	unsigned long father = minadd;
	Rp0->VVtour[Rp0->VVnum] = Rp0->VVlist[V.t];
	for (i = Rp0->VVnum - 1; i >= 0; i--)
	{
		Rp0->VVtour[i] = Rp0->VVlist[StateQ[father].nowNd];
		father = StateQ[father].father;
	}
	Rp0->VVtour[Rp0->VVnum] = V.t;
}


/*************************************************************************
*  函数名称：LHK_algorithm
*  功能说明：LKH算法计算must-node的atsp最短路
*  参数说明：无
*  函数返回：无
*  修改时间：2016-3-10
*************************************************************************/
void LHK_algorithm()
{
	V.LKHuseCnSum++;
	Rp0->LKHuseCn++;
	
    /* Read the specification of the problem */
    ReadParameters();		//have read，读进入参数
	ReadProblem();			//have read，读入问题权重啥的，申请Node
    AllocateStructures();	//have read，释放和申请各种变量
    
    
 //    long long timeStart = clock();
	// printf("start run %s() *********************************************\n",__FUNCTION__);
	CreateCandidateSet();	//reading，创建每个点的侯选集
	// printf("Time : %s() used time %lld\n\n",__FUNCTION__,clock() - timeStart);

    if (Norm == 0)
    {
        BestCost = (GainType) LowerBound;
        RecordBetterTour();
		RecordBestTour();
        //WriteTour(TourFileName, BestTour, BestCost);
    }
	else
	{
		BestCost = PLUS_INFINITY;
		BestCost = FindTour();      /* using the Lin-Kernighan heuristic */
		RecordBetterTour();
		RecordBestTour();
		//WriteTour(TourFileName, BestTour, BestCost);
	}
}
