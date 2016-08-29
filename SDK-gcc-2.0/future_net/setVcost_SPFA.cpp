#include "future_net.h"


/*************************************************************************
*  函数名称：Road::setVcost_SPFA
*  功能说明：得到任意必经点（must点）u,v间的距离Vcost，此时其它点间的cost没求，想要的话看Gp->cost（思路：不拿必经点去做更新）
*  参数说明：无
*  函数返回：无
*  修改时间：2016-4-28
*************************************************************************/
static unsigned char vis[MaxV];
static Queue que;
void Road::setVcost_SPFA()
{
	int u, v;
	for (int j = 0; j < Gp->num; j++) 
	{
		if (isMust[j] == False) 
			continue;
		for (int i = 0; i < Gp->num; i++)	
			Vcost[i][j] = UnReachCost;
		Vcost[j][j] = 0;
		que.clear();
		que.push(j); vis[j] = true;
		while (!que.empty())
		{
			v = que.pop(); vis[v] = false;
			if(rand() & 1)
			{
				for (int i = 0; i < Gp->Node[v].predCn; i++)
				{
					u = Gp->Node[v].predList[i];
					if (Gp->cost[u][v] + Vcost[v][j] < Vcost[u][j]) 
					{
						Vcost[u][j] = Gp->cost[u][v] + Vcost[v][j];
						Vpath[u][j] = v;
						if (!vis[u] && !isMust[u]) {
							que.push(u);
							vis[u] = true;
						}
					}
				}
			} 
			else
			{
				for (int i = Gp->Node[v].predCn - 1 ; i >= 0; i--) {
					u = Gp->Node[v].predList[i];
					if (Gp->cost[u][v] + Vcost[v][j] < Vcost[u][j]) {
						Vcost[u][j] = Gp->cost[u][v] + Vcost[v][j];
						Vpath[u][j] = v;
						if (!vis[u] && !isMust[u]) {
							que.push(u);
							vis[u] = true;
						}
					}
				}
			}
		}
	}
    for(int i = 0; i < Gp->num; i++)
        Vcost[i][i] = UnReachCost;
}




//省时间版本(用于solveTspCn>1)
void Road::setVcost_reduced_SPFA()
{
	unsigned char isMustNew[MaxV] = {0};
	for(int i = 0;i < Gp->num;i++)
		if(!isMustOld[i] && isMust[i])
			isMustNew[i] = True;
	
	int u, v;
	for (int j = 0; j < Gp->num; j++)
	{
		bool update_required = isMustNew[j]; // 当前结点是否需要作为终点跑SPFA, 本身是新的必经节点需要更新
		if (isMustOld[j]) 
		{
			for (v = 0; v < Gp->num && !update_required; v++) 
			{
				if(isMustOld[v]==False || v==j)
					continue;
				if (Vcost[v][j] != UnReachCost) 
				{
					for (int k = Vpath[v][j]; k != j; k = Vpath[k][j]) 
					{
						if (isMustNew[k]) 
						{
							update_required = true;	// 存在路径经过了新的必经点, 需要更新
							break;
						}
					}
				}
			}
		}
		if (update_required) 
		{
			for (int i = 0; i < Gp->num; i++) Vcost[i][j] = UnReachCost;
			Vcost[j][j] = 0;

			que.clear();
			que.push(j); vis[j] = true;
			while (!que.empty()) {
				v = que.pop(); vis[v] = false;
	            if(rand() & 1) {
					for (int i = 0; i < Gp->Node[v].predCn; i++) {
						u = Gp->Node[v].predList[i];
						if (Gp->cost[u][v] + Vcost[v][j] < Vcost[u][j]) {
							Vcost[u][j] = Gp->cost[u][v] + Vcost[v][j];
							Vpath[u][j] = v;
							if (!vis[u] && !isMustOld[u] && !isMustNew[u]) {
								que.push(u);
								vis[u] = true;
							}
						}
					}
				} else {
					for (int i = Gp->Node[v].predCn - 1 ; i >= 0; i--) {
						u = Gp->Node[v].predList[i];
						if (Gp->cost[u][v] + Vcost[v][j] < Vcost[u][j]) {
							Vcost[u][j] = Gp->cost[u][v] + Vcost[v][j];
							Vpath[u][j] = v;
							if (!vis[u] && !isMustOld[u] && !isMustNew[u]) {
								que.push(u);
								vis[u] = true;
							}
						}
					}
				}
	        }
		}
	}
	//Print("Time : %s() used time %lld\n\n",__FUNCTION__,clock() - timeStart);
}

/*************************************************************************
*  函数名称：Road::setVcost_DIJ
*  功能说明：得到任意必经点（must点）u,v间的距离Vcost，此时其它点间的cost没求，想要的话看Gp->cost（思路：不拿必经点去做更新）
*  参数说明：无
*  函数返回：无
*  修改时间：2016-4-28
*************************************************************************/
//int dij[MaxV];
//void Road::setVcost_DIJ() 
//{
//	static long long usedtime=0;
//	long long timeStart = clock();
//	//run Gp->num times SPFA
//	int u, v;

//	for (int i = 0; i < Gp->num; i++) Vcost[0][i] = UnReachCost;
//	for (int i = 1; i < Gp->num; i++) MemCopy(Vcost[i], Vcost[0], Gp->num);

//	heap.init_with_key(dij);
//	for (int j = 0; j < Gp->num; j++) if (isMust[j]) {
//		dij[j] = Vcost[j][j] = 0;

//		while (!heap.empty()) heap.pop();
//		heap.push(j); vis[j] = false;
//		for (int i = 0; i < Gp->num; i++) if (i != j) {
//			dij[i] = UnReachCost;
//			heap.push(i);
//			vis[i] = false;
//		}
//		while (!heap.empty()) {
//			v = heap.pop();
//			if (vis[v]) continue;
//			vis[v] = true;
//			if (v != j && isMust[v]) continue;

//            if(rand() % 2) {
//				for (int i = 0; i < Gp->Node[v].predCn; i++) {
//					u = Gp->Node[v].predList[i];
//					if (Gp->cost[u][v] + Vcost[v][j] < Vcost[u][j]) {
//						dij[u] = Vcost[u][j] = Gp->cost[u][v] + Vcost[v][j];
//						Vpath[u][j] = v;
//						heap.change(u);
//					}
//				}
//			} else {
//				for (int i = Gp->Node[v].predCn - 1 ; i >= 0; i--) {
//					u = Gp->Node[v].predList[i];
//					if (Gp->cost[u][v] + Vcost[v][j] < Vcost[u][j]) {
//						dij[u] = Vcost[u][j] = Gp->cost[u][v] + Vcost[v][j];
//						Vpath[u][j] = v;
//						heap.change(u);
//					}
//				}
//			}
//        }
//    }
//    for(int i = 0; i < Gp->num; i++)
//        Vcost[i][i] = UnReachCost;
//	//Print("Time : %s() used time %lld\n",__FUNCTION__,clock() - timeStart);
//	usedtime+=clock() - timeStart;
//	Print("usedtime = %lld\n",usedtime);
//}

