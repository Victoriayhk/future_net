#ifndef __ROUTE_H__
#define __ROUTE_H__

//route.c
void search_double_route();
void search_single_route(Road *,Road *);
void set_repeated_id();

void swap_Vcost_VsubCost(int direction);
void set_VVcost_LKH();
void set_VVcost_BFS();
void set_VVtour();
void set_Vtour();
void set_VtourCost();

//RouteAlgorithm.c
void LHK_algorithm();
void BFS_algorithm();

//ShortestPathAlgorithm.c
//void DIJ_algorithm();
//void Floyd_algorithm();
void SPFA_algorithm();

//exam_repeated.c
int exam_isStartEnd(int addr);
int exam_repeated_node();

#endif
