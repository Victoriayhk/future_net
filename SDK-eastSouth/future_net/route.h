#ifndef __ROUTE_H__
#define __ROUTE_H__
#include "lib_io.h"

#define MAX_VEX_NUM 2000            //topo中顶点数量的上限2000

struct EdgeLink
{
	int LinkID;
	int SourceID;
	int DestinationID;
	int Cost;
	int demandID;      //指示当前边属于哪条路径,0表示不属于任何路径,1代表path1,2代表怕path2,3代表属于两条路径
};


typedef struct CostMatrix{
	int  cost;
	int *nodepath;
	int nodenum;
	}CostMatrix;

typedef struct EdgeNode
    {
        int adjvex; //邻接顶点
        int edgeID;  //边的索引号
        int cost;    //边的权值
        struct EdgeNode* next; //指向下一条边
    }EdgeNode;


typedef struct
{
	bool isRequiredNode;   //是否是必须经过的点
	int nodeID;       //顶点编号
	EdgeNode* pFirstEdge; //指示第一条边
	int isRequiredNodeSet;//记录重复边中源节点是nodeID的目的节点，先默认所有重复边中起点不存在相同（后续需要修改）
}VertexNode;

typedef VertexNode AdjList[MAX_VEX_NUM];//邻接表

typedef struct
{
	AdjList adjList; //邻接表
	int VexNum; //顶点个数
	int EdgeNum; //边数
}AdjListGraph;


//迪杰斯特拉算法要求的顶点信息
struct node     //顶点节点，保存id和到源顶点的估算距离，优先队列需要的类型
{
int ID;
int w;     //源顶点id和估算距离
friend bool operator<(node a, node b)   //因要实现最小堆，按升序排列，因而需要重载运算符，重定义优先级，以小为先
 {
   return a.w > b.w;
  }//声明算法调用的接口函数
  };

int FindOptimalPath(int *NodePath,  int  NodeNum, int **WeightMatrix);
void search_route(char *topo[MAX_EDGE_NUM], int edge_num, char *demand[MAX_DEMAND_NUM], int demand_num);


#endif
