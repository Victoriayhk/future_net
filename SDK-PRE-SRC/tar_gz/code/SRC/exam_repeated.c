#include "LKH.h"


/*************************************************************************
*  函数名称：exam_isStartEnd
*  功能说明：检查是否是must-node相连边上的所有use>=2的点的起点或者终点
*  参数说明：addr是V中的编号
*  函数返回：True,是StartEnd点，同意把addr加入V';False不同意
*  修改时间：2016-3-17
*************************************************************************/
int exam_isStartEnd(int addr)
{
	int i;
	//from end to start
	int nextMust = Rp0->VtourCn - 1,nextUse2 = Rp0->VtourCn;
	for(i = Rp0->VtourCn - 1; i >= 0; i--)
	{
		if(Rp0->isVV[Rp0->Vtour[i]] == True)
			nextMust = i;
		else
		{
			if(Rp0->Vtour[i]==addr && i<nextMust && nextMust<nextUse2)
				return True;
			if(Rp0->VuseCn[Rp0->Vtour[i]] >= 2 && Rp0->Vtour[i]!=addr)
				nextUse2 = i;
		}
	}
	//from start to end
	int preMust = 0,preUse2 = -1;
	for(i = 0; i < Rp0->VtourCn; i++)
	{
		if(Rp0->isVV[Rp0->Vtour[i]] == True)
			preMust = i;
		else
		{
			if(Rp0->Vtour[i]==addr && preUse2<preMust && preMust<i)
				return True;
			if(Rp0->VuseCn[Rp0->Vtour[i]] >= 2 && Rp0->Vtour[i]!=addr)
				preUse2 = i;	
		}
	}
	return False;
}


/*************************************************************************
*  函数名称：exam_repeated_node
*  功能说明：得到VUse，检查每个点用了几遍，把重复点加入到VV中
*  参数说明：无
*  函数返回：重复点的个数
*  修改时间：2016-3-17
*************************************************************************/
int exam_repeated_node()
{	
	#ifdef MyDebug
	printf("/**************************** start repeated node >>> *******************************/");
	#endif
	
	//initial VUse[i].Cn
	memset(Rp0->VuseCn,0,sizeof(int)*V.num);
	
	//record node's use count
	int i,j,preMust,nowNd,nowCn;
	for(i = 0; i < Rp0->VtourCn; i++)
	{
		nowNd = Rp0->Vtour[i];
		Rp0->VuseCn[nowNd]++;
#ifdef MyDebug
		//record which two must-node ust this non-must-node
		nowCn = Rp0->VuseCn[nowNd];
		if(Rp0->isVV[nowNd] == True)
			preMust = nowNd;
		else
		{
			Rp0->useInfo[nowNd].v1[nowCn] = preMust;
			for(j = i+1; j < Rp0->VtourCn; j++)
				if(Rp0->isVV[Rp0->Vtour[j]] == True)
				{
					Rp0->useInfo[nowNd].v2[nowCn] = Rp0->Vtour[j];
					break;
				}
		}
#endif
	}
	
    //exam non-must-node used >=2,update Rp0->VVlist[]
    int reNdCn = 0,VVnumRec = Rp0->VVnum;
    for(i = 0; i < V.num; i++)
    {
        if(Rp0->isVV[i] == False && Rp0->VuseCn[i] >= 2)//node i is a repeated node,cause cycle
        {
			reNdCn++;
			#ifdef MyDebug
            printf("\nnode %d is used = %d times",i,Rp0->VuseCn[i]);
			//printf("\n%d",i);
			#endif
			if(exam_isStartEnd(i))
			{
				#ifdef MyDebug
				//printf("		Add non-must-node %d to V' in ",i);
				for(j = 1;j <= Rp0->VuseCn[i];j++)
					;//printf("(%d,%d)",Rp0->useInfo[i].v1[j],Rp0->useInfo[i].v2[j]);
				#endif
				Rp0->VVlist[Rp0->VVnum++] = i;
				//break;/********** important *********/
			}
		}
    }
	
	//update V.isVV,Rp0->VVlist
	for(i = VVnumRec; i < Rp0->VVnum; i++)
		Rp0->isVV[Rp0->VVlist[i]] = True;
	Rp0->isVV[V.t] = True;
	Rp0->VVlist[Rp0->VVnum] = V.t;
	
	#ifdef MyDebug
	printf("\n/**************************** >>> stop repeated node *******************************/\n");
	#endif
	printf("BestCost = %lld\n",BestCost);
	return reNdCn;
}
