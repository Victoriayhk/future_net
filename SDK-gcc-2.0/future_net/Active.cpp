#include "future_net.h"


/*************************************************************************
*  函数名称：Road::resetActive()
*  功能说明：reActive all nodes
*  参数说明：无
*  函数返回：无
*  修改时间：2016-5-23
*************************************************************************/
void Road::resetActive()
{
    firstActive = lastActive = NULL;
	for (int i = 0; i < num; i++)
	{
		Node[i].next = NULL;
		Active(&Node[i]);
    }
}


/*************************************************************************
*  函数名称：Road::Active(VVNode *N)
*  功能说明：Active节点N
*  参数说明：N为节点指针
*  函数返回：无
*  修改时间：2016-4-30
*************************************************************************/
void Road::Active(VVNode *N)
{
    if (N->next != NULL)			//在队列里面
        return;
    if (firstActive == 0)			//队列为空
        firstActive = lastActive = N;
    else							//队列不为空
        lastActive = lastActive->next = N;
    lastActive->next = firstActive;	//队尾指向队首、循环队列
}


/*************************************************************************
*  函数名称：VVNode * RemoveFirstActive
*  功能说明：取出active队列第一个元素，队列空的话返回NULL
*  参数说明：无
*  函数返回：N为出队列的节点指针
*  修改时间：2016-4-30
*************************************************************************/
VVNode * Road::RemoveFirstActive()
{
    VVNode *N = firstActive;
    if (firstActive == lastActive)	//队列只有一个元素
        firstActive = lastActive = NULL;
    else
        lastActive->next = firstActive = firstActive->next;
    if(N)
        N->next = NULL;				//表示不再队列里面
    return N;
}


/*************************************************************************
*  函数名称：Road::resetOrder()
*  功能说明：每次更新tour时调用,order为:0、1、2、...、num-1
*  参数说明：无
*  函数返回：无
*  修改时间：2016-4-30
*************************************************************************/
void Road::resetOrder()
{
	int order = 0;
	VVNode *N = &Node[0];
	do
		N->order = order++;
	while((N=N->succ) != &Node[0]);
}
