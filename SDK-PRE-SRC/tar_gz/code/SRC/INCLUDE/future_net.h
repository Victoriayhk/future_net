#ifndef _FUTURE_NET_H
#define _FUTURE_NET_H

//future_net macro***************************************************
//#define PRELIMINARY
#ifdef PRELIMINARY
	#define RW_LINE_NUM 1
#else
	#define RW_LINE_NUM 2
	#define SEMI_FINAL
	//#define FINAL
#endif
#define MyDebug
//#define EXAM_RESULT

#define MaxV 2000				//所有node的数量上限
#define MaxVV 100				//必须node的数量上限
#define MaxEOut 20				//node的出度上限
#define MaxE MaxV*MaxEOut		//可达边的数量上限
#define IN_MAX_LINE_LEN  12000		//输入文件中一行的字符数上限(char number<103*4=412)
#define OUT_MAX_LINE_LEN 12000		//输出文件中一行的字符数上限(char number<1000*4=4000)
#define SPFA_QUEUE_NUM (1<<20u)		//SPFA时候queue大小52w
#define TIME_OUT 9000000			//超时的时间9000ms
#define GetMax(a,b) (a)>(b)?(a):(b)
#define str(var) #var" = %d"
#define printf_1var(v0) 		printf(str(v0)"\n",v0)
#define printf_2var(v0,v1) 		printf(str(v0)","str(v1)"\n",v0,v1)
#define printf_3var(v0,v1,v2) 	printf(str(v0)","str(v1)","str(v2)"\n",v0,v1,v2)
#define False 0
#define True 1

long long MaxReachECost;			//=max(V.cost),可达边的最大权
long long SubMaxECost;				//=MaxReachECost*(V.num+1),可达边构造成的-假的不可达边权重
long long MaxECost;					//=SubMaxECost*(V.num+1)//不可达边的权重20w或4ww,int:2^31=21ww


//future_net struct***************************************************
struct								//use for SPFA_algorithm
{
    int preNd[MaxV];
	int	preCn;
}NODE[MaxV];

#ifdef MyDebug
struct twoNodeList				//use for exam_repeated_node
{	
	int v1[MaxV];				//坐标是V.useCn[i]
	int v2[MaxV];				//坐标是V.useCn[i]
};
#endif

struct
{
	int cost[MaxV][MaxV];		//两点之间最短路径
	int id[MaxV][MaxV];			//两点之间最短路径ID
	int subCost[MaxV][MaxV];	//两点之间第二短路径
	int subId[MaxV][MaxV];		//两点之间第二短路径ID
	int num;					//number of nodes in V
	int s,t;					//source,destination
	long long RCostSum,lastRCostSum;
	int LKHuseCnSum;
	int reIdCn,reIdList[MaxV];	//两条边重复边个数
}V;

typedef struct Road Road;
struct Road
{
	int VVnum,VVnumInit;
	int VVlist[MaxV];
	int VVcost[MaxV][MaxV];
	int VVtour[MaxV];
	unsigned char isVV[MaxV],isVVinit[MaxV];
	
	int Vcost[MaxV][MaxV];
	int Vpath[MaxV][MaxV];
	int VuseCn[MaxV];
	
	int Vtour[MaxV],VtourCn;
	int VtourCost[MaxV];		//tour坐标是[0,tourCn-1]
	int VtourId[MaxV];			//tour坐标是[0,tourCn-1]
	long long lastCost,cost;
	
	#ifdef MyDebug
	struct twoNodeList useInfo[MaxV];
	#endif
	
	int LKHuseCn,LKHuseCnOld;
};
Road Rd[2],*Rp,*Rp0,*Rp1;		//Rp、Rp0、Rp1都是临时变量


//future_net include**************************************************
#include <sys/timeb.h>
#include "route.h"
#include "read_info.h"
#include "write_result.h"

#endif
