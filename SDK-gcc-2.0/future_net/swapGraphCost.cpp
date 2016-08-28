#include "future_net.h"


/*************************************************************************
*  函数名称：swapGraphCost
*  功能说明：交换 cost and subCost部分空间（上次single route的Road经过的路）
*  参数说明：direction:1和0分别代表swap和undoSwap
*  函数返回：无
*  修改时间：2016-3-22
*************************************************************************/
static unsigned char isEdgeChanged[MaxV];
static int costRecord[MaxV],idRecord[MaxV];
void Road::swapGraphCost(Road *Rpo)
{	
	if(Rpo == this || Rpo->VtourCn < 2)//如果只是不加限制求自己或者Rpo解不正确、就不交换
	{
		rpoHash = 23;
		return ;
	}
	
	//设置punish
	setPunish();
	
	//在cost上设置阻碍
	memset(isEdgeChanged,False,sizeof(unsigned char)*Rpo->VtourCn);
	memset(idRecord,	 0,    sizeof(int)*Rpo->VtourCn);
	for(int i = 0; i < Rpo->VtourCn - 1; i++)
	{
		int v1 = Rpo->Vtour[i], v2 = Rpo->Vtour[i+1];
		
		//如果另外一条路没用最短边id,用的是次短边subID,就continue,因为正好把最短边id留给要求的Rp
		if(Rpo->VtourId[i] == Gp->subId[v1][v2])
			continue;
		
		//important:Rpo使用了Rp中的必须点,不能给Rp设置阻碍
		if(    (routeCn==2 || routeCn==3)
				&& (Gp->subCost[v1][v2] == UnReachCost)
				&& (reIdCn <= reIdCnMin)
				&& (isMustInit[v1] || isMustInit[v2]))
			continue;
		
		//store and swap
		costRecord[i] = Gp->cost[v1][v2];
		idRecord[i]   = Gp->id[v1][v2];
		if(Gp->subCost[v1][v2] == UnReachCost)
			Gp->cost[v1][v2] += punish;
		else	//有备用边
		{
			Gp->cost[v1][v2] = Gp->subCost[v1][v2];
			Gp->id[v1][v2] = Gp->subId[v1][v2];
		}
		isEdgeChanged[i] = True;
	}
	//计算rpoHashBak,用于存储结果
	rpoHash = SDBMHash(Rpo->VtourCn - 1,idRecord);
}


//undo swap cost to subCost
void Road::swapGraphCostBack(Road *Rpo)
{
	if(Rpo == this || Rpo->VtourCn < 2)//如果只是不加限制求自己或者Rpo解不正确、就不交换
		return ;
	int v1,v2;
	for(int i = 0; i < Rpo->VtourCn - 1; i++)
	{
		if(isEdgeChanged[i] == True)
		{
			v1 = Rpo->Vtour[i];
			v2 = Rpo->Vtour[i+1];
			Gp->cost[v1][v2] = costRecord[i];
			Gp->id[v1][v2] = idRecord[i];
		}
	}
}


/*************************************************************************
*  函数名称：Road::SDBMHash
*  功能说明：使用SDBMHash方法计算hash值(使用的是arr下标为[0,cn-1])
*  参数说明：无
*  函数返回：无
*  修改时间：2016-5-17
*************************************************************************/
unsigned int Road::SDBMHash(int cn,int arr[])
{
	if(cn < 1)
		return 23;
	static char str[sizeof(int)*MaxV];
	
	unsigned int hash = 0;
	int strLen = sizeof(int)*cn;
	memcpy(str, arr, strLen);
	for(int i = 0; i < strLen ; i++)
		hash = str[i] + (hash << 6) + (hash << 16) - hash;
	return (hash & 0x7FFFFFFF);
}
