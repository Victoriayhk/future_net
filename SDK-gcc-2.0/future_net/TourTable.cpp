#include "future_net.h"


//bestCost是去掉pi的值,但是没有去掉SubUnReachCost
void TourTable::insert(Road *Rp,int bestCost)
{
	if(num >= cnMax)
		return;
	
	//set roadCost
	roadCost[num] = bestCost%SubUnReachCost;

	//set tour
	if(!isMalloc[num])
	{
		Vtour[num] = (int *)malloc(sizeof(int)*Rp->Gp->num);
		VtourId[num] = (int *)malloc(sizeof(int)*Rp->Gp->num);
		isMalloc[num] = True;
	}
	VVNode *N = Rp->Node;
	for(int i = 0;i < Rp->num;i++)
	{
		Vtour[num][i] = Rp->list[N->id];
		if(Vtour[num][i] == Rp->t)
		{
			tourCn[num] = i;
			break;
		}
		N = N->succ;
	}
	if(!Rp->isUseCopy)
	{
		tourCn[num] = Rp->num;
		Vtour[num][Rp->num] = Rp->t;
	}
	//Print("insert Tour is : %d\n",roadCost[num]);
	//for(int i = 0;i < tourCn[num];i++)
	//	Print("%d,",Vtour[num][i]);
	//Print("road %d insert success\n\n",Rp->id);
	//插入tour成功,计数器加一
	num++;
}

void TourTable::lock(Road *Rp)
{
	unsigned int hash;
	int vnext1,vnext2,v2,idCn;
	for(int r = validNum;r < num;r++)
	{
		//检查重复点用
		static int VuseCn[MaxV];
		memset(VuseCn,0,sizeof(int)*Rp->Gp->num);
		
		//set VtourCnTmp、VtourId
		idCn = 0;
		for(int i = 0; i < tourCn[r]; i++)
		{
			vnext1 = vnext2 = Vtour[r][i];
			v2 = Vtour[r][i+1];
			while((vnext1=vnext2) != v2)
			{
				vnext2 = Rp->Vpath[vnext1][v2];
				VtourId[validNum][idCn] = Rp->Gp->id[vnext1][vnext2];
				idCn++;
				if(++VuseCn[vnext2] >= 2)
				{
					//printf("hehe repeat\n");
					goto firstForEnd;
				}
			}
		}
		
		//计算rpHash,判断是否在table中,不在的话就插入rpHash、roadCost、VtourCn(VtourId插人过了)(Vtour不需要插入了)
		hash = Road::SDBMHash(idCn,VtourId[validNum]);
		if(!isInTable(hash))
		{
			rpHash[validNum] = hash;
			roadCost[validNum] = roadCost[r];
			VtourCn[validNum] = idCn + 1;
			validNum++;
		}
		//else
		//{
		//	Print("VtourId is : \n");
		//	for(int i = 0; i < Cn-1; i++)
		//		Print("%d,",VtourId[validCn][i]);
		//	Print("\n\n");
		//}
firstForEnd:;
	}
}


unsigned char TourTable::isInTable(unsigned int hash)
{
	for(int i = 0; i < validNum ; i++)
		if(rpHash[i] == hash)
			return True;
	return False;
}


void TourTable::reset()
{
	//printf_2var(num,validNum,%d);
	num = validNum;
	//printf_2var(num,validNum,%d);
}


//遍历Road0和Road1所有路径的组合
//if(!UsedTourTable)	return;
//printf_2var(Road0.tt.validNum,Road1.tt.validNum,%d);
//TourTable *tt0 = &Road0.tt,*tt1 = &Road1.tt;
//static int usedLabel[MaxEdge],label = 0;
//for(int m = 0; m < tt0->validNum ; m++)
//	for(int n = 0; n < tt1->validNum ; n++)
//	{
//		int *VtourId0 = tt0->VtourId[m],*VtourId1 = tt1->VtourId[n];
//		roadCostSum[0] = tt0->roadCost[m] + tt1->roadCost[n];
//		label++;
//		reIdCn = 0;
//		for(int i = 0;i < tt0->VtourCn[m] - 1;i++)
//			usedLabel[VtourId0[i]] = label;
//		for(int i = 0;i < tt1->VtourCn[n] - 1;i++)
//			if(usedLabel[VtourId1[i]] == label)
//				reIdCn++;
//		//printf_1var(reIdCn,%d);
//		//printf_3var(roadCostSum[0],tt0->roadCost[m],tt1->roadCost[n],%lld);
//		if(findBetterResult)	reWriteResult(tt0->VtourCn[m],VtourId0,tt1->VtourCn[n],VtourId1);
//	}
