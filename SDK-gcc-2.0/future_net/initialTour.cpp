#include "future_net.h"

KMTour KM;

/*************************************************************************
*  函数名称：Road::initialTour()
*  功能说明：在每次K_OPT操作时初始化路径,被K_OPT()调用
*  参数说明：无
*  函数返回：初始化路径全权值和
*  修改时间：2016-5-4
*************************************************************************/
long long Road::initialTour(unsigned char hasFindTour, unsigned char &KMFindTour)
{
	static unsigned char isUsed[MaxV2];
	static VVNode *succN[MaxV2];
	long long initTourCost = 0;
	VVNode *predN=NULL, *firstNode, *N=NULL;
	int succNCn, i, j, r;
	
	if(trial==Trials-1 && hasFindTour==false)
	{
		Print("used KM\n");
		KMFindTour = KM.init_tour(num, cost, Node, initTourCost);
		goto initialTourEnd;
	}
	
	//initial first node
	memset(isUsed,False,sizeof(unsigned char)*num);
	r = 0;//rand()%num;
	isUsed[r] = True;
	firstNode = predN = &Node[r];
	
	//add the other num-1 nodes to the tour
	for (i = 1;i < num; i++)
	{
		succNCn = 0;
		//case A : Alpha=0并且出现在BestTour或InNextBestTour的边
		for(j = 0;j < predN->candCn;j++)
		{
			N = predN->cand[j];
			if(!isUsed[N->id]
					&& predN->alpha[j] == 0
					&&(InBestTour(predN,N) || InLastBestTour(predN,N)))
				succN[succNCn++] = N;
		}
		//case B : 不满足case A就随机选择候predN的选集中的未使用的边
		if(succNCn == 0)
		{
			for(j = 0;j < predN->candCn;j++)
			{
				N = predN->cand[j];
				if(!isUsed[N->id])
					succN[succNCn++] = N;
			}
		}
		//case C : 不满足case A、B,随机在剩下没用过的predN发出的可达边中找(如果都不可达,选最后一个不可达点)
		if(succNCn == 0)
		{
			r = rand()%num;
			for (j = r; j < num; j++)	
				if(!isUsed[j])
				{
					N = &Node[j];
					if(cost[predN->id][Node[j].id] < UnReachCost)
						succN[succNCn++] = &Node[j];
				}
			if(j == num)
				for (j = 0; j < r; j++)	
					if(!isUsed[j])
					{
						N = &Node[j];
						if(cost[predN->id][Node[j].id] < UnReachCost)
							succN[succNCn++] = &Node[j];
					}
			//都不可达,选最后一个不可达点
			if(succNCn == 0)
			{
				isUsed[N->id] = True;
				LinkA2B(predN,N);
				initTourCost += COST(predN,N);
				predN = N;
				continue;
			}
		}
		r = rand() % succNCn;
		N = succN[r];
		isUsed[N->id] = True;
		LinkA2B(predN,N);
		initTourCost += COST(predN,N);
		predN = N;
	}
	//link last and first node
	LinkA2B(predN,firstNode);
	initTourCost += COST(predN,firstNode);

initialTourEnd:
	resetOrder();
	
	//if(UsedTourTable && ((initTourCost-sumPi)>>7u) < UnReachCost)	
	//	tt.insert(this,(initTourCost-sumPi)>>7u);
	return initTourCost;
}
