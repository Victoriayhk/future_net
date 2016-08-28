#include "future_net.h"


/*************************************************************************
*  函数名称：Road::resumeBak()
*  功能说明：恢复使用复制点前的状态
*  参数说明：无
*  函数返回：无
*  修改时间：2016-5-23
*************************************************************************/
void Road::resumeBak()
{
	memcpy(isMust,isMustBak,sizeof(isMust)/sizeof(unsigned char));
	memcpy(list,listBak,sizeof(list)/sizeof(int));
	num = numBak;
}


/*************************************************************************
*  函数名称：Road::examRepeatNode()
*  功能说明：得到VuseCn,检查每个点用了几遍,把重复点加入到V'中
*  参数说明：无
*  函数返回：重复点的个数
*  修改时间：2016-4-26
*************************************************************************/
int Road::examRepeatNode()
{
	int numRecord = num;
	
	//得到每个点使用次数
	memset(VuseCn,0,sizeof(int)*Gp->num);
	for(int i = 0; i < VtourCn; i++)
		VuseCn[Vtour[i]]++;
	
	/********************** 以下是给有重复点的模型用 **********************************/
	//setBak:不管用不用复制点都要这么记录下,主要是为如果没用复制点要恢复状态
	memcpy(isMustBak,isMust,sizeof(isMust)/sizeof(unsigned char));
	memcpy(listBak,list,sizeof(list)/sizeof(int));
	numBak = num;
	
	//得到重复点个数,设置备份变量isMustBak,listBak
	numRepeat = 0;//每次计算numRepeat要清零
	for(int gi = 0; gi < Gp->num; gi++)
		if(isMustBak[gi] == False && VuseCn[gi] >= 2)
		{
			numRepeat++;
			isMustBak[gi] = True;
			listBak[numBak++] = gi;
		}
	
	//如果有重复点就继续补充listBak
	if(numRepeat)//list = {s,必须点,重复点,t,重复点的复制点},num-1存储最后一个点
	{
		listBak[numBak++] = this->t;					//加入终点
		for(int ri = 0;ri < numRepeat;ri++,numBak++)	//加入复制点
			listBak[numBak] = listBak[numBak - numRepeat -1] + MaxV;
	}
	
	/**************************** 以下是给没有重复点的模型用 *******************************/
	//只添加重复点,不复制
	for(int gi = 0; gi < Gp->num; gi++)//检查重复点
		if(isMust[gi] == False && VuseCn[gi] >= 2)
			list[num++] = gi;
	
	//update isMust
	memcpy(isMustOld,isMust,sizeof(isMust)/sizeof(unsigned char));//给setVcost_reduced_SPFA用
	for(int ri = numRecord; ri < num; ri++)
		isMust[list[ri]] = True;

	#if PRINT_ReNode_INFO == 1
		for(int ri = numRecord; ri < num; ri++)
			Print("Add non-must-node %d to must nodes set\n",list[ri]);
		Print("\n");
	#endif
	return numRepeat;
}
