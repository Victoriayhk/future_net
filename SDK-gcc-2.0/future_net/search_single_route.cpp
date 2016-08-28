#include "future_net.h"


/*************************************************************************
*  函数名称：Road::search_single_route
*  功能说明：得到{s,V',t}的最短无环路径
*  参数说明：Rpo是另外一条路
*  函数返回：虽然无返回值，但是roadCost[0]和VtourCn标志有没有路径（无路径时候roadCost[0] >= UnReachCost)
*  修改时间：2016-5-23
*************************************************************************/
void Road::search_single_route(Road *Rpo)
{
	long long timeStart = clock();
	
	//如果计算过就直接从hash里面取出来
	swapGraphCost(Rpo);
	if(-1 != ht.whereInTable(rpoHash))
	{
		ht.resumeResult(this);
		swapGraphCostBack(Rpo);
		return ;
	}
	
	//initial isMust、num、list、roadCost、solveTspCn、isUseCopy、cost
	memcpy(isMust,isMustInit,sizeof(unsigned char)*Gp->num);
	num = numInit;
	list[num] = this->t;
	roadCost[2] = roadCost[1];	roadCost[1] = roadCost[0];	roadCost[0] = UnReachCost;
	solveTspCn = 0;
	isUseCopy = False;
	
	//转化为tsp问题,每次都是新的tsp
	while(1)
	{
		solveTspCn++;		//printf_2var(solveTspCn,isUseCopy,%d);
		//if(UsedTourTable)	tt.reset();
		setVcost_SPFA();	//find shortest path and record path in V
		
		if(num == 1)		//num=1 too easy
		{
			roadCost[0] = Vcost[this->s][this->t];
			tour[0] = this->s;	tour[1] = this->t;
		}
		else if(num == 2)	//num=2 too easy
		{
			roadCost[0] = Vcost[s][list[1]] + Vcost[list[1]][this->t];
			tour[0] = this->s;	tour[1] = list[1];	tour[2] = this->t;
		}
		else				//num>3 difficult
		{
			setCost();		//Vcost到cost值的转换
			roadCost[0] = LKH();
			if(roadCost[0] < UnReachCost) 	setTour();
		}
		//没有找到路径
		if(roadCost[0] >= UnReachCost)
		{
			if(solveTspCn == 1)	break;	//case A:第一次就没求出路径直接退出
			if(!isUseCopy)				//case B:上次没使用复制点导致没有解,恢复有复制点的状态
			{
				isUseCopy = True;
				resumeBak();
				continue;
			}
			else break;					//case C:上次使用复制点了仍然没有求出解(其实不太可能)就跳出吧
		}
		else
		{
			isUseCopy = False;
			//有路径条件下：记录最终路径，包含所有must-node和部分non-must-node
			setVtour();
			//有路径条件下：没有重复点则跳出，正常输出路径；有重复点则把重复点加入VV，继续找
			if(examRepeatNode() == 0)	break;
			//还有重复点,但是time out,then break,output NA
			if(clock() > TIME_OUT)	{roadCost[0] = UnReachCost;break;}
		}
	}
	if(roadCost[0] < UnReachCost)
		setVtourId();
	swapGraphCostBack(Rpo);
	if(roadCost[0] < UnReachCost)
		setVtourCost();
	singleRouteTime = clock() - timeStart;
}
