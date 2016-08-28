#include "route.h"
#include "lib_io.h"
#include "lib_time.h"
#include "stdio.h"
#include "time.h"

int main(int argc, char *argv[])
{
	long long start = clock();
    print_time("Begin");
    char *topo[MAX_EDGE_NUM];
    int edge_num;
    char *demand[MAX_DEMAND_NUM];
    int demand_num;

    char *topo_file = argv[1];
    edge_num = read_file(topo, MAX_EDGE_NUM, topo_file);
    if (edge_num == 0)
    {
        printf("Please input valid topo file.\n");
        return -1;
    }
    char *demand_file = argv[2];
    demand_num = read_file(demand, MAX_DEMAND_NUM, demand_file);
    if (demand_num != MAX_DEMAND_NUM)
    {
        printf("Please input valid demand file.\n");
        return -1;
    }

    search_route(topo, edge_num, demand, demand_num);

    char *result_file = argv[3];
    write_result(result_file);
    release_buff(topo, edge_num);
    release_buff(demand, demand_num);

    print_time("End");

	printf("all used time %lldms\n",(clock() - start)/1000);
	return 0;
}

