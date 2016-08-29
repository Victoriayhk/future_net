#include "future_net.h"


/*************************************************************************
*  函数名称：Road::setPunishMethod
*  功能说明：设置usedSlowPunish和单轮迭代次数iter
*  参数说明：无
*  函数返回：无
*  修改时间：2016-5-25
*************************************************************************/
void Road::setPunishMethod()
{
	//第一步:先求Road0,在给Road1设置阻碍,粗略计算重边数
	//运行完:reIdCnMin==MaxV + 1:"难用例"		reIdCnMin<MaxV + 1:"简单用例"
	usedSlowPunish = False;
	Road0.search_single_route(&Road0);//裸求Road0
	Road1.search_single_route(&Road0);//用Road0走过的限制求Road1
	setReIdList(&Road0,&Road1);
	if(findBetterResult)
		reWriteResult(Road0.VtourCn,Road0.VtourId,Road1.VtourCn,Road1.VtourId);
	
	//第二步:如果上一步运行完判断出是个"难用例":先求Road1,在给Road0设置阻碍,再次计算重边数
	//运行完:reIdCnMin==MaxV + 1:"超难用例"		reIdCnMin<MaxV + 1:"难用例"
	if(reIdCnMin == MaxV + 1)
	{
		usedSlowPunish = False;
		Road1.search_single_route(&Road1);//裸求Road1
		Road0.search_single_route(&Road1);//用Road1走过的限制求Road0
		setReIdList(&Road0,&Road1);
		if(findBetterResult)
			reWriteResult(Road0.VtourCn,Road0.VtourId,Road1.VtourCn,Road1.VtourId);
	}
	
	//第三步:设置惩罚方式
	srand(0);
	Road0.ht.reset();
	Road1.ht.reset();
	if(reIdCnMin == 0)//"简单用例"或"难用例"中的"超简单用例"(reIdCnMin==0)
	{
		usedSlowPunish = True;
		printf("used smart punish!!!\n");
		printf_1var(Road0.singleRouteTime + Road1.singleRouteTime,%lld);
		if(Road0.singleRouteTime + Road1.singleRouteTime < 500*1000)//小用例
		{
			iter[0]=7; iter[1]=0;	iter[2]=7;	iter[3]=0;
			printf("small case\n");
		}
		else//大用例
		{
			iter[0] = iter[1] = iter[2] = iter[3] = 10;
			printf("large case\n");
		}
	}
	else if(reIdCnMin == MaxV + 1)//"超难用例"
	{
		usedSlowPunish = True;
		iter[0] = iter[1] = iter[2] = iter[3] = 10;
		printf("super complex case\n");
	}
	else
	{
		usedSlowPunish = False;
		iter[0]=10; iter[1]=10;iter[2]=10;	iter[3]=10;
		printf("normal case\n");
	}
}


/*************************************************************************
*  函数名称：Road::setPunish
*  功能说明：设置每轮的punish
*  参数说明：无
*  函数返回：无
*  修改时间：2016-5-25
*************************************************************************/
void Road::setPunish()
{
	if(!usedSlowPunish)
	{
		punish = SubUnReachCost;
		//printf_1var(reIdCnMin,%d);
		if(reIdCnMin == 0)
		{
			usedSlowPunish = True;
			punish = 10*iteration;
		}
	}
	else
	{
		punish += 10;
	}
}
