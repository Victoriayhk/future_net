#include "future_net.h"


//initial static variables in class Road
Graph* Road::Gp;
int Road::s = 0, Road::t = 0;
int Road::reIdCn = 0, Road::reIdCnMin = 0;
int Road::reIdList[MaxV] = {0};
int Road::punish = 0;
long long Road::roadCostSum[3] = {UnReachCost,UnReachCost,UnReachCost};
long long Road::roadCostSumMin = UnReachCost;
int Road::sLKHuseCn = 0;
int Road::routeCn = 0;
int Road::iteration = 0,Road::iter[routeCnMax] = {10,10,10,10};
int Road::trial = 0;
int Road::Trials = 0;
unsigned char Road::usedSlowPunish = False;


/*************************************************************************
*  函数名称：Road::initial
*  功能说明：解析输入文件的两行数据，demand为要求通过V'的二维维数组
*  参数说明：filename是路径信息文件名
*  函数返回：无
*  修改时间：2016-4-26
*************************************************************************/
void Road::initial(char *demand,Graph *Gp,int id)
{
	//initial id,Gp
	this->id = id;
	Road::Gp = Gp;
	//initial s,t,list
	int tmp;
	if(3 == sscanf(demand,"%d,%d,%d,%d",&tmp,&this->s,&this->t,list + (++numInit)))
	{	//P' only contains {s,t}
		list[0] = this->s;
		list[1] = this->t;
	}
	else
	{
		list[0] = this->s;
		numInit++;
		//set list={s,V'}
		for(int i = 0,str_len = strlen(demand); i < str_len; i++)
			if(demand[i] == '|')
				if(1 != sscanf(demand+i+1,"%d",list+(numInit++)))
					numInit--;
		list[numInit] = this->t;
	}
	//initial isMustinit
	for(int i = 0; i < numInit; i++)
		isMustInit[list[i]] = True;
	isMustInit[this->t] = True;
	//initial Node[].id
	int nodeNum = sizeof(Node)/sizeof(VVNode);
	for(int i = 0;i < nodeNum;i++)
		Node[i].id = i;
}


/*************************************************************************
*  函数名称：Road::setCost
*  功能说明：得到必经点之间的最短路径
*  参数说明：无
*  函数返回：无
*  修改时间：2016-3-22
*************************************************************************/
void Road::setCost()
{
    if(!isUseCopy)	//不使用加入重复点的方法
	{
		for(int ri = 0; ri < num; ri++) //必经点每两个点的距离,剔除s的入度
			for(int rj = 1; rj < num; rj++)
				cost[ri][rj] = Vcost[list[ri]][list[rj]];
		cost[0][0] = UnReachCost;		//s到自己距离为无穷
		for(int ri = 1; ri < num; ri++) //t的入度(不包括从s来的)作为s的入度
			cost[ri][0] = Vcost[list[ri]][this->t];
	}
	else
	{
		int n = num - numRepeat;				//n={s,必须点,重复点,t}
		//先重设一下点间距离
		for(int ri = 0; ri < num; ri++)
			for(int rj = 0; rj < num; rj++)
				cost[ri][rj] = UnReachCost;
		//集合{s,必须点,重复点,t}内部点的距离
		for(int ri = 0; ri < n-1; ri++) 	//t不要出度
			for(int rj = 1; rj < n; rj++)	//s不要入度
				cost[ri][rj] = Vcost[list[ri]][list[rj]];
		//t的出度:t除了到第一个复制点可达,其它点均不可达
		cost[n-1][n] = 0;
		//所有重复点的复制点(除了最后一个)
		for(int ri = n; ri < num-1; ri++)
		{
			cost[ri][ri+1] = 0;			//相邻两个复制点
			cost[ri][ri-numRepeat-1] = 0;	//复制点到原点
			cost[ri-numRepeat-1][ri+1] = 0;	//原点到下一个复制点
		}
		//最后一个复制点
		cost[num-1][n-2] = 0;			//复制点到原点
		//s的出度:除了最后一个重复点和它的复制点到s可达,其它点均不可达
		cost[n-2][0] = 0;				//最后一个重复点
		cost[num-1][0] = 0;				//最后一个复制点
		//print data
		//for(int ri = 0; ri < num; ri++)
		//{
		//	Print("%2d(%2d)\t",ri,list[ri]);
		//	for(int rj = 0; rj < num; rj++)
		//		if(cost[ri][rj] != UnReachCost)
		//			Print("%2d(%2d)(c=%2d)\t",rj,list[rj],cost[ri][rj]);
		//	Print("\n");
		//}
	}
}


/*************************************************************************
*  函数名称：Road::setTour()
*  功能说明：得到环路中当前必经点顺序,并计算去pi权重
*  参数说明：minBestCost为带pi权重
*  函数返回：无
*  注意事项：tour[num] = this->t;
*  修改时间：2016-4-26
*************************************************************************/
void Road::setTour()
{
	unsigned char isListUse[MaxV2],isFindt = False;
	memset(isListUse,False,sizeof(unsigned char)*MaxV2);
	
	//tsp问题转化回去寻路问题,输出tour
	VVNode *N = &Node[0];
	for(int i = 0;i < num;i++)//此时的num={s,必须点,用到的准必须点,t,未到的准必须点}
	{
		if(!isFindt)
			isListUse[N->id] = True;
		tour[i] = list[N->id];
		if(tour[i] == this->t)
			isFindt = True;
		N = N->bestSucc;
	}

	//reset list[]:{s,最初的必须点,用到的准必须点,t},num下标的地方继续为t
	//撤回没用到的准必须点(isMust恢复False,冲list里面踢出去)
	//升级用到的准必须点(已经在list和must里面设置了,就不用重读设置了)
	if(isUseCopy)
	{
		int listNew[MaxV2],numNew = 0;
		for(int i = 0; i < num - numRepeat - 1; i++)//检查{s,必须点,重复点}是否都用到了
		{
			if(isListUse[i] == True)
				listNew[numNew++] = list[i];
			else
				isMust[list[i]] = False;
		}
		memcpy(list,listNew,sizeof(int)*numNew);
		num = numNew;
		list[num] = this->t;
	}
	tour[num] = this->t;		//必须要设置,因为setVtour用到了
	
#if PRINT_TOUR == 1
	Print("Tour is : \n");
	for(int i = 0;i < num;i++)
		Print("%d,",tour[i]);
	Print("%d\n\n",this->t);
#endif
}


/*************************************************************************
*  函数名称：Road::setVtour
*  功能说明：得到最终序列，点的数量，Vtour包含所有必须点和一些非必须点
   Vtour[]:s->{V',T}->t,T是一些non-must-node的集合
*  参数说明：无
*  函数返回：无
*  注意事项：Vtour[VtourCn-1] = this->t;
*  修改时间：2016-5-8
*************************************************************************/
void Road::setVtour()
{
	int vnext,v2;
	VtourCn = 0;
	for(int i = 0; i < num; i++)//此时num和list已经reset过
    {
        vnext = tour[i];
		v2 = tour[i+1];
        while(vnext != v2)
        {
			Vtour[VtourCn++] = vnext;
            vnext = Vpath[vnext][v2];
        }
    }
	Vtour[VtourCn++] = this->t;
	
#if PRINT_VTOUR == 1
	Print("Vtour is : \n");
	for(int i = 0; i < VtourCn; i++)
	{
		if(isMust[Vtour[i]])
			Print("\n");
		Print("%d,",Vtour[i]);
	}
	Print("\n\n");
#endif
}


/*************************************************************************
*  函数名称：Road::setVtourId
*  功能说明：set VtourId[],要放在swapGraphCostBack()之前,因为此时要用经过交换后的id[][]
*  参数说明：无
*  函数返回：无
*  修改时间：2016-5-25
*************************************************************************/
void Road::setVtourId()
{
	int v1,v2;
	for(int i = 0; i < VtourCn - 1; i++)
	{
		v1 = Vtour[i];
		v2 = Vtour[i+1];
		VtourId[i] = Gp->id[v1][v2];
	}
}

//set roadCost[0],VtourCost[]
void Road::setVtourCost()
{
	int v1,v2;
	roadCost[0] = 0;
	for(int i = 0; i < VtourCn - 1; i++)
	{
		v1 = Vtour[i];
		v2 = Vtour[i+1];
		if(VtourId[i] == Gp->id[v1][v2])
			VtourCost[i] = Gp->cost[v1][v2];
		else
			VtourCost[i] = Gp->subCost[v1][v2];
		roadCost[0] += VtourCost[i];
	}
	
	//set HashTable
	ht.insert(this);
	
	//set TourTable
	//if(UsedTourTable)
	//	tt.lock(this);
}
