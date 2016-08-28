#include "future_net.h"

Graph G;
Road Road0,Road1;

void search_route(char *topo[MAX_EDGE_NUM], int edge_num, char *demand[MAX_DEMAND_NUM], int demand_num)
{
	G.initial(topo,edge_num);
	Road0.initial(demand[0], &G, 0);
	Road1.initial(demand[1], &G, 1);
	Road::search_double_route();
	
	//Print("LKH use count: %d\n",Road::sLKHuseCn);
	//printf_3var(G.num,Road0.numInit,Road1.numInit,%d);
	//Print("repeat  sumCost   Road0   Road1\n  %d\t%5lld\t%5lld\t%5lld\n",Road::reIdCn,Road::roadCostSum[0],Road0.roadCost[0],Road1.roadCost[0]);
	//Print("my program end time		=%5ldms\n\n", clock()/1000);
}
