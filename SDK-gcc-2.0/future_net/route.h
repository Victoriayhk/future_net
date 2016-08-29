#ifndef __ROUTE_H__
#define __ROUTE_H__

#include "lib_io.h"
void search_route(char *graph[MAX_EDGE_NUM], int edge_num, char *condition[MAX_DEMAND_NUM], int demand_num);
void setIter(int iter[]);

#endif
