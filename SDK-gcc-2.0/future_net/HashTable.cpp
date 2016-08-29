#include "future_net.h"


/*************************************************************************
*  函数名称：HashTable::insert(Road *Rp)
*  功能说明：在每次找到路后就存到HashTable中,减少重算
*  参数说明：Rp为Road指针,通常是this
*  函数返回：无
*  修改时间：2016-5-19
*************************************************************************/
void HashTable::insert(Road *Rp)
{
	if(-1 != whereInTable(Rp->rpoHash))
		return;
	rpoHash[cn]  = Rp->rpoHash;
	VtourCn[cn]  = Rp->VtourCn;
	roadCost[cn] = Rp->roadCost[0];
	
	VtourId[cn] = (int *)malloc(sizeof(int)*Rp->VtourCn);
	Vtour[cn]   = (int *)malloc(sizeof(int)*Rp->VtourCn);
	memcpy(VtourId[cn],Rp->VtourId, sizeof(int)*Rp->VtourCn);
	memcpy(Vtour[cn],  Rp->Vtour,   sizeof(int)*Rp->VtourCn);
	cn++;
	
	//Print("road %d insert : %d\n",Rp->id,cn);
	//printf_1var(Rp->rpoHash,%d);
	//printf_1var(Rp->VtourCn,%d);
	//for(int i = 0;i < Rp->VtourCn-1;i++)
	//	Print("%d,",Rp->VtourId[i]);
	//Print("\n");
	//printf_1var(Rp->roadCost[0],%lld);Print("\n");
}


/*************************************************************************
*  函数名称：HashTable::whereInTable(unsigned int rpoHash)
*  功能说明：判断以rpoHash为标识的路有没有存储过,返回存储位置,没有返回-1
*  参数说明：rpoHash为idRecord的hash
*  函数返回：位置
*  修改时间：2016-5-19
*************************************************************************/
int HashTable::whereInTable(unsigned int hash)
{
	for(int i = 0; i < cn ; i++)
		if(rpoHash[i] == hash)
			return i;
	return -1;
}


/*************************************************************************
*  函数名称：HashTable::insert(Road *Rp)
*  功能说明：如果在HashTable中判断出曾经算过,就直接读取算过的路径信息
*  参数说明：Rp为Road指针,通常是this
*  函数返回：无
*  修改时间：2016-5-19
*************************************************************************/
void HashTable::resumeResult(Road *Rp)
{
	int addr = whereInTable(Rp->rpoHash);
	Rp->VtourCn     = VtourCn[addr];
	Rp->roadCost[0] = roadCost[addr];
	memcpy(Rp->VtourId,this->VtourId[addr],sizeof(int)*(VtourCn[addr]));
	memcpy(Rp->Vtour,  this->Vtour[addr],  sizeof(int)*(VtourCn[addr]));

	//Print("find in HashTable(save time) : \n");
	//printf_1var(VtourCn[addr],%d);
	//for(int i = 0;i < VtourCn[addr] - 1;i++)
	//	Print("%d,",Rp->VtourId[i]);
	//Print("\n");
	//printf_1var(Rp->roadCost[0],%lld);
	//Print("\n");
}
