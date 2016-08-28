#include "LKH.h"


/*************************************************************************
*  函数名称：search_double_route
*  功能说明：得到{s,V',t}和{s,V'',t}两条无环路径，重合边尽量少，然后路径和尽量短
*  参数说明：无
*  函数返回：无
*  修改时间：2016-3-22
*************************************************************************/
void search_double_route()
{
	static int order = 0,cn = 5;
	//Rd[0].cost = Rd[1].cost = MaxECost+1;	
	while(cn--)
	{
		search_single_route(Rp0=&Rd[order],Rp1=&Rd[1-order]);
		search_single_route(Rp0=&Rd[1-order],Rp1=&Rd[order]);
		
		printf("Road cost 		=%5lld,%5lld,%5lld		",V.RCostSum,Rp0->cost,Rp1->cost);
		printf("Road cost 		=%5lld,%5lld,%5lld\n",V.RCostSum,Rp0->lastCost,Rp1->lastCost);
		
		if(Rp0->cost >= MaxECost)	break;
		if((Rp0->cost == Rp0->lastCost) && (Rp1->cost == Rp1->lastCost))	{set_repeated_id();	break;}
		if(clock() > TIME_OUT)	{V.RCostSum = MaxECost;	break;}
	}
}

/*************************************************************************
*  函数名称：search_single_route
*  功能说明：得到{s,V',t}的最短无环路径
*  参数说明：无
*  函数返回：这条路的长度
*  修改时间：2016-3-22
*************************************************************************/
void search_single_route(Road *noUse1,Road *noUse2)
{
	//initial all
	Rp0->LKHuseCn = Rp0->LKHuseCnOld = 0;
	memcpy(Rp0->isVV,Rp0->isVVinit,sizeof(unsigned char)*V.num);
	Rp0->VVnum = Rp0->VVnumInit;	Rp0->VVlist[Rp0->VVnum] = V.t;
	swap_Vcost_VsubCost(0);

	while(1)
	{
		SPFA_algorithm();//find shortest path and record path in V（Floyd_algorithm();）
		if(Rp0->VVnum == 1)
		{
			BestCost = Rp0->Vcost[V.s][V.t];
			Rp0->VVtour[0] = V.s;	Rp0->VVtour[1] = V.t;
		}
		else if(Rp0->VVnum == 2)//special case:LKH can't handle Rp0->VVnum=2,process it by hand
		{
			BestCost = Rp0->Vcost[V.s][Rp0->VVlist[1]] + Rp0->Vcost[Rp0->VVlist[1]][V.t];
			Rp0->VVtour[0] = V.s;	Rp0->VVtour[1] = Rp0->VVlist[1];	Rp0->VVtour[2] = V.t;
		}
		/*else if(Rp0->VVnum < 10 && V.num < 100)
		{
			set_VVcost_BFS();
			BFS_algorithm();
			//set_VVtour() is already executed in BFS_algorithm()
		}*/
		else
		{
			set_VVcost_LKH();	//write_VVcost2file("cost.txt");
			LHK_algorithm();
			set_VVtour();
		}
		//没有路径则跳出，输出NA
		if(BestCost >= MaxECost)	break;
		//有路径条件下：记录最终路径，包含所有must-node和部分non-must-node
		set_Vtour();
		//有路径条件下：没有重复点则跳出，正常输出路径；有重复点则把重复点加入VV，继续找
		if(exam_repeated_node() == 0)	break;
		//time out then break,output NA
		if(clock() > TIME_OUT)	{BestCost = MaxECost;	break;}
	}
	set_VtourCost();
	swap_Vcost_VsubCost(1);
}

/*************************************************************************
*  函数名称：swap_Vcost_VsubCost
*  功能说明：交换 V.cost and V.subCost部分空间（上次single route的Road经过的路）
*  参数说明：direction:0,V.subCost->V.cost;1,V.cost->V.subCost(0和1分别代表换入和换出)
*  函数返回：无
*  修改时间：2016-3-22
*************************************************************************/
static void swap(int *a,int *b)
{
	static int c;
	 c = *a;
	*a = *b;
	*b =  c;
}
void swap_Vcost_VsubCost(int direction)
{
    int i,tmp,v1,v2;
	int *const tour = Rp1->Vtour;
	if(direction == 1)
		goto Vcost2VsubCost;
	//do swap V.cost and V.subCost
	for(i = 0; i < Rp1->VtourCn-1; i++)
	{
		v1 = tour[i],v2 = tour[i+1];
		//如果另外一条路线没用最短路id，用的是次短路subID，就continue，因为正好把id留给要求的Rp0
		if(Rp1->VtourId[i] != V.id[v1][v2])
			continue;
		if(V.subCost[v1][v2] == MaxECost)
		{
			V.subCost[v1][v2] = V.cost[v1][v2];
			V.cost[v1][v2] = SubMaxECost + V.subCost[v1][v2];
		}
		else
		{
			swap(&V.cost[v1][v2],&V.subCost[v1][v2]);
			swap(&V.id[v1][v2],&V.subId[v1][v2]);
		}
	}
	return ;
	//undo swap V.cost and V.subCost
Vcost2VsubCost:
	for(i = 0; i < Rp1->VtourCn-1; i++)
	{
		v1 = tour[i],v2 = tour[i+1];
		//如果另外一条路线没用最短路id，用的是次短路subID，就continue，因为正好把id留给要求的Rp0
		if(Rp1->VtourId[i] != V.id[v1][v2])
			continue;
		if(V.cost[v1][v2] == SubMaxECost + V.subCost[v1][v2])
		{
			V.cost[v1][v2] = V.subCost[v1][v2];
			V.subCost[v1][v2] = MaxECost;
		}
		else
		{
			swap(&V.cost[v1][v2],&V.subCost[v1][v2]);
			swap(&V.id[v1][v2],&V.subId[v1][v2]);
		}
	}
}	

/*************************************************************************
*  函数名称：set_VVcost_LKH(),set_VVcost_BFS()
*  功能说明：为不同算法得到VV.cost
*  参数说明：无
*  函数返回：无
*  修改时间：2016-3-22
*************************************************************************/
void set_VVcost_LKH()
{
    int i,j;
    for(i = 0; i < Rp0->VVnum; i++) //rows and rolumns
        for(j = 0; j < Rp0->VVnum; j++)
			Rp0->VVcost[i][j] = Rp0->Vcost[Rp0->VVlist[i]][Rp0->VVlist[j]];
    for(i = 1; i < Rp0->VVnum; i++) //first rolumn
        Rp0->VVcost[i][0] = Rp0->Vcost[Rp0->VVlist[i]][V.t];
}
void set_VVcost_BFS()
{
    int i,j;
	for(i = 0; i < Rp0->VVnum + 1; i++) //rows and rolumns
        for(j = 0; j < Rp0->VVnum + 1; j++)
			Rp0->VVcost[i][j] = Rp0->Vcost[Rp0->VVlist[i]][Rp0->VVlist[j]];
}

/*************************************************************************
*  函数名称：set_VVtour
*  功能说明：得到Road.VVtour
*  参数说明：无
*  函数返回：无
*  修改时间：2016-4-14
*************************************************************************/
void set_VVtour()
{
	 int cnn =13 ,cn = 0;
	 printf("Tour is :\n");

	int i,j;
    int n = Dimension / 2;
    for (i = 1; i < n && BestTour[i] != 1; i++);
    for (j = 1; j <= n; j++)
    {
        Rp0->VVtour[j-1] = Rp0->VVlist[BestTour[i]-1];
        printf("%4d,",BestTour[i]-1);
        if (++i > n)
			i = 1;

		if(cn++ == cnn)
		{
			cn=0;
			printf("\n");
		}
    }
    Rp0->VVtour[Rp0->VVnum] = V.t;

    printf("\n");
}

/*************************************************************************
*  函数名称：set_Vtour()
*  功能说明：得到最终序列，点的数量，tour包含所有必须点和一些非必须点
			tour[]:s->{V',T}->t,T是一些non-must-node的集合
*  参数说明：无
*  函数返回：无
*  修改时间：2016-4-14
*************************************************************************/
void set_Vtour()
{
	int i,vnext,v2;
	Rp0->VtourCn = 0;
	for(i = 0; i < Rp0->VVnum; i++)
    {
        vnext = Rp0->VVtour[i];
        v2 = Rp0->VVtour[i+1];
        while(vnext != v2)
        {
			Rp0->Vtour[Rp0->VtourCn++] = vnext;
            vnext = Rp0->Vpath[vnext][v2];
        }
    }
	Rp0->Vtour[Rp0->VtourCn++] = V.t;
}

/*************************************************************************
*  函数名称：set_VtourCost()
*  功能说明：记录Vtour这条最终路中每段的长度
*  参数说明：无
*  函数返回：无
*  修改时间：2016-4-14
*************************************************************************/
void set_VtourCost()
{
	int i,v1,v2;
	if(BestCost < MaxECost)
	{	
		for(i = 0; i < Rp0->VtourCn - 1; i++)
		{
			v1 = Rp0->Vtour[i],v2 = Rp0->Vtour[i+1];
			Rp0->VtourCost[i] = V.cost[v1][v2]%SubMaxECost;;
			Rp0->VtourId[i] = V.id[v1][v2];
		}
	}
	Rp0->lastCost = Rp0->cost;
	Rp0->cost = BestCost;
	if(BestCost < MaxECost)
		Rp0->cost %= SubMaxECost;
	V.RCostSum = Rp0->cost + Rp1->cost;
}

/*************************************************************************
*  函数名称：set_repeated_id
*  功能说明：记录tour1和tour2重复的边数和id
*  参数说明：无
*  函数返回：无
*  修改时间：2016-4-19
*************************************************************************/
void set_repeated_id()
{
	int i,j;
	V.reIdCn = 0;
	for(i = 0;i < Rp0->VtourCn - 1;i++)
		for(j = 0;j < Rp1->VtourCn - 1;j++)
			if(Rp0->VtourId[i] == Rp1->VtourId[j])
			{
				V.reIdList[V.reIdCn++] = Rp0->VtourId[i];
				break;
			}
}
