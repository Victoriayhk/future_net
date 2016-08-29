#include "future_net.h"

#define NaBreak			{Print("NA :%d\n",Rp->id);	break;}
#define breakAvailable	   ((clock() > TIME_OUT) \
						|| (!usedSlowPunish && routeCn<2 && iteration==1 && reIdCn==0) \
						|| (Road0.roadCost[0] == Road0.roadCost[1] && Road1.roadCost[0] == Road1.roadCost[1]) \
						|| (Road0.roadCost[0] == Road0.roadCost[2] && Road1.roadCost[0] == Road1.roadCost[2]))
#define SWAP(Rp,Rpo)	{Road *r = Rp;	Rp = Rpo;	Rpo = r;} 


/*************************************************************************
*  函数名称：Road::search_double_route
*  功能说明：得到{s,V',t}和{s,V'',t}两条无环路径，重合边尽量少，然后路径和尽量短
*  参数说明：无
*  函数返回：虽然无返回值，但是roadCostSum[0]标志有没有路径（无路径时候roadCostSum[0] >= UnReachCost)
*  修改时间：2016-3-22
*************************************************************************/
void Road::search_double_route()
{	
	Road *Rp,*Rpo;
	reIdCnMin = MaxV + 1;					//两条路的重复边个数,的最好值
	roadCostSumMin = UnReachCost;			//两条路的路径和,的最好值
	setPunishMethod();

	//try to find minreIdCn and minRoadCostSum(先从Road0或先从Road1都找)
	for(routeCn = 0; routeCn < routeCnMax; routeCn++)
	{
		Road0.resetRoad();		Road1.resetRoad();
		if(routeCn%2==1)		{Rp = &Road0;	Rpo = &Road1;}
		else 					{Rp = &Road1;	Rpo = &Road0;}
		
		if(usedSlowPunish)	punish = 0;
		for(iteration = 0; iteration < iter[routeCn] ;iteration++)
		{
			//Print("*********************************");printf_3var(Rp->id,routeCn,iteration,%d);
			Rp->search_single_route(Rpo);			
			if(Rp->roadCost[0] >= UnReachCost)
				NaBreak;
			setReIdList(&Road0,&Road1);
			if(iteration>=1 && findBetterResult)//如果找到更好的解就重写结果字符串
				reWriteResult(Road0.VtourCn,Road0.VtourId,Road1.VtourCn,Road1.VtourId);
			if(breakAvailable)	//满足(超时、无重边、和上次迭代路权值一样、和上上次迭代路权值一样)则输出、跳出,否则继续迭代
				break;
			SWAP(Rp,Rpo);
		}
		if(clock() > TIME_OUT)	break;
	}
}


/*************************************************************************
*  函数名称：Road::setReIdList
*  功能说明：记录Road[0]和Road[1]重复的边数个数和id,同时计算roadCostSum
*  参数说明：无
*  函数返回：无
*  修改时间：2016-4-26
*************************************************************************/
void Road::setReIdList(Road *Rp,Road *Rpo)
{
	static int usedLabel[MaxEdge],label = 0;
	label++;
	
	//计算reIdCn、reIdList
	reIdCn = 0;
	for(int i = 0;i < Rp->VtourCn - 1;i++)
		usedLabel[Rp->VtourId[i]] = label;
	for(int i = 0;i < Rpo->VtourCn - 1;i++)
		if(usedLabel[Rpo->VtourId[i]] == label)
			reIdList[reIdCn++] = Rpo->VtourId[i];
	
	//计算roadCostSum
	roadCostSum[2] = roadCostSum[1];
	roadCostSum[1] = roadCostSum[0];
	roadCostSum[0] = Rp->roadCost[0] + Rpo->roadCost[0];
	
	//printf info
	//Print("Road now  repeat cost =%5d,%9lld,\t\t\t%9lld,%9lld\n",reIdCn,roadCostSum[0],Road0.roadCost[0],Road1.roadCost[0]);
	//Print("Road last repeat cost =%5d,%9lld,\t\t\t%9lld,%9lld\n",reIdCn,roadCostSum[1],Road0.roadCost[1],Road1.roadCost[1]);
	//Print("Road lasttrepeat cost =%5d,%9lld,\t\t\t%9lld,%9lld\n",reIdCn,roadCostSum[2],Road0.roadCost[2],Road1.roadCost[2]);
}


/*************************************************************************
*  函数名称：Road::reWriteResult
*  功能说明：写入到输出字符串中
*  参数说明：两个VtourCn和Vtour
*  函数返回：无
*  修改时间：2016-5-22
*************************************************************************/
void Road::reWriteResult(int VtourCn0,int VtourId0[],int VtourCn1,int VtourId1[])
{
	printf_2var(routeCn,iteration,%d);
	reIdCnMin = reIdCn;
	roadCostSumMin = roadCostSum[0];
	
	clear_result();
	for(int i = 0; i < VtourCn0 - 1; i++)	record_result(WORK_PATH, VtourId0[i]);
	for(int i = 0; i < VtourCn1 - 1; i++)	record_result(BACK_PATH, VtourId1[i]);
}
