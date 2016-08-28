#include "LKH.h"


/*************************************************************************
*  函数名称：Floyd_algorithm
*  功能说明：得到任意u,v间的距离（更新时不能过VV中的点）
*  参数说明：无
*  函数返回：无
*  修改时间：2016-3-10
*  有待优化：改成别的算法求ij最短路
*************************************************************************/
//void Floyd_algorithm()
//{
//    int i,j,k;
//	
//	//initial V.cost,V.path
//	for(j = 0; j < V.num; j++)	V.path[0][j] = j;
//	memcpy(V.cost[0],V.costInit[0],sizeof(int)*V.num);
//	for(i = 1; i < V.num; i++)
//	{
//		memcpy(V.cost[i],V.costInit[i],sizeof(int)*V.num);
//        memcpy(V.path[i],V.path[0],sizeof(int)*V.num);
//	}
//	
//	//floyd
//    for(k = 0; k < V.num; k++)
//        if(V.isVV[k] == False)
//            for(i = 0; i < V.num; i++)
//                for(j = 0; j < V.num; j++)
//                    if(V.cost[i][j] > V.cost[i][k] + V.cost[k][j])
//                    {
//                        V.cost[i][j] = V.cost[i][k] + V.cost[k][j];
//                        V.path[i][j] = V.path[i][k];
//                    }
//    for(i = 0; i < V.num; i++)
//        V.cost[i][i] = MaxECost;
//}


/*************************************************************************
*  SPFA_algorithm
*  功能说明：得到任意u,v间的距离（更新时不能过VV中的点）
*  参数说明：无
*  函数返回：无
*  修改时间：2016-4-14
*************************************************************************/
static int queue[SPFA_QUEUE_NUM];
void SPFA_algorithm()
{
    int i, j;
	//initial Road.Vcost
	for(i = 0; i < V.num; i++)	
		memcpy(Rp0->Vcost[i],V.cost[i],sizeof(int)*V.num);
	
	//initial V.path
	for(j = 0; j < V.num; j++)	Rp0->Vpath[0][j] = j;
	for(i = 1; i < V.num; i++)	memcpy(Rp0->Vpath[i],Rp0->Vpath[0],sizeof(int)*V.num);
	
	// initial SPFA: quene, vis:malloc&memset
    unsigned char *vis  = (unsigned char*)malloc(sizeof(unsigned char)*V.num);
	memset(vis, False, sizeof(unsigned char)*V.num);
	
	//run SPFA V.num times for each Node
	for (j = 0; j < V.num; j++)
	{
		if(Rp0->isVV[j] == False)	continue;
        int top = 0, u, v, pri, end;
		
		//initial V.cost[:][j]
		for (i = 0; i < V.num; i++)	Rp0->Vcost[i][j] = MaxECost;
		Rp0->Vcost[j][j] = 0;
		
		//initial quene
		queue[0] = j; pri = 0; end = 1; vis[j] = True;
		while (pri < end)
		{
			v = queue[pri];	vis[v] = False;	pri++;
            for (i = 0; i < NODE[v].preCn; i++)
			{
                u = NODE[v].preNd[i];
                if (V.cost[u][v] + Rp0->Vcost[v][j] < Rp0->Vcost[u][j]) 
				{
                    Rp0->Vcost[u][j] = V.cost[u][v] + Rp0->Vcost[v][j];
					Rp0->Vpath[u][j] = v;//V.path[u][v];
					if (vis[u] == False && Rp0->isVV[u] == False)
					{
						queue[end] = u;	
						vis[u] = True;	
						end++;
					}
                }
            }
        }
    }
    for(i = 0; i < V.num; i++)
        Rp0->Vcost[i][i] = MaxECost;
}



/*************************************************************************
*  HNode,Heap,heap_init,heap_pop,heap_insert
*  功能说明：自己实现的heap
*  修改时间：2016-3-21
*************************************************************************/
/*struct HNode 
{
 int x;
 int dis;
};
struct Heap 
{
 struct HNode **list;
 int num;
} heap;

void heap_init(int num) 
{
 heap.list = (struct HNode **)malloc(sizeof(struct HNode *) * (num + 1));
 heap.num = 0;
}

int heap_pop() 
{
 int loc = 1, child;
 struct HNode *rmove, *node;
 rmove = heap.list[1];
 node = heap.list[heap.num--];

 while (loc <= (heap.num >> 1)) 
 {
  child = loc << 1;
  if (child < heap.num && heap.list[child+1]->dis < heap.list[child]->dis)
   child ++;
  if (node->dis <= heap.list[child]->dis)
   break;
  heap.list[loc] = heap.list[child];
  loc = child;
 }
 heap.list[loc] = node;
 return rmove->x;
}

void heap_insert(int x, int dis) 
{
 struct HNode *node = (struct HNode *)malloc(sizeof(struct HNode));
 node->x = x; node->dis = dis;

 int loc = ++ heap.num, parent = loc >> 1;
 while (parent && node->dis < heap.list[parent]->dis) 
 {
  heap.list[loc] = heap.list[parent];
  loc = parent;
  parent >>= 1;
 }
 heap.list[loc] = node;
}*/


/*************************************************************************
*  DIJ_algorithm
*  功能说明：得到任意u,v间的距离（更新时不能过VV中的点）
*  参数说明：无
*  函数返回：无
*  修改时间：2016-3-18
*  有待优化：改成别的算法求ij最短路
*************************************************************************/
/*void DIJ_algorithm() 
{
 int i, j, u, v;
  
 //initial V.path
 for(j = 0; j < V.num; j++)	V.path[0][j] = j;
 for(i = 1; i < V.num; i++)	memcpy(V.path[i],V.path[0],sizeof(int)*V.num);
  
 // initial SPFA: quene, vis:malloc&memset
    unsigned char *vis  = (unsigned char*)malloc(sizeof(unsigned char)*V.num);
 memset(vis, False, sizeof(unsigned char)*V.num);

 heap_init(V.num * V.num);
 for (j = 0; j < V.num; j++) 
 {
  if (V.isVV[j] == False) continue;

  for (i = 0; i < V.num; i++) 
  {
   V.cost[i][j] = MaxECost;
   vis[i] = False;
  }
  V.cost[j][j] = 0;

  heap_insert(j, 0);
  while (heap.num > 0) 
  {
   v = heap_pop();
   if (vis[v] == True) continue;
   vis[v] = True;

   for (i = 0; i < NODE[v].preCn; i++) 
   {
	u = NODE[v].preNd[i];
	if (V.costInit[u][v] + V.cost[v][j] < V.cost[u][j]) 
	{
	 V.cost[u][j] = V.costInit[u][v] + V.cost[v][j];
	 V.path[u][j] = v;
	 if (V.isVV[u] == False) heap_insert(u, V.cost[u][j]);
	 }
   }
  }
 }
 for(i = 0; i < V.num; i++)
  V.cost[i][i] = MaxECost;
}*/
