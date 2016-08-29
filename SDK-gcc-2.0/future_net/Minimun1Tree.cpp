#include "future_net.h"


/*******************************************************************************
最小1-tree
用到的值有:
 1. 各节点的pi值
 2. 结点的边集 (用到了所有边, 待改成候选集)
更新的值有:
 1. 结点的dad, 根节点默认为0, 根节点的父节点为其第一个儿子结点
 2. 结点的cost, 根节点默认为0, 根节点的cost为到其第一个儿子结点的cost
 3. 结点的度-2: V
 4. mst_list: 最小生成树链
 5. nsp, esp: Special VVNode, Special Edge
 6. Norm: V的平方和
返回值: 最小1-tree的权值和

作者: 易惠康
日期: 2016-05-04

*********************************************************************************/

void CandidateSolver::check_special(int x, int &sp_cost) {
	int MCost = UnReachCost;
	Edge *c = NULL;

	// 找到不在MST中的最短的邻边
	for (int i = 0; i < numCandi[x]; i++) {
		if (edge[x][i].to == dad[x] || dad[edge[x][i].to] == x) continue;
		if (edge[x][i].cost_pi < MCost) {
			MCost = edge[x][i].cost_pi;
			c = &edge[x][i];
		}
	}

	// 更新x为当前Special VVNode
	if (c != NULL && MCost > sp_cost) {
		sp_cost = MCost;
		nsp = x;
		esp = c;
	}
}

Int64 CandidateSolver::mini_1tree(bool Sparse) {
	// 更新所有边的cost_pi值
	ForAllNodes(x) {
		ForCandisOf(e, x) {
			e->cost_pi = pi[x] + pi[e->to] + e->cost;
		}
	}

	Int64 sum = mst(Sparse);  // 最小生成树

	int sp_cost = -UnReachCost;
	nsp = 0; esp = &edge[0][0];
	for (int x = 1; x < dim; x++) {
		sum -= 2*pi[x];					// 统计1-tree的值在MST中的部分
		check_special(x, sp_cost);		// 特殊结点, 该结点这里约定不是根结点0(但可以与根结点相连)
	}
	sum -= 2 * pi[0];

	sum += esp->cost_pi;
	v[nsp]++;
	v[esp->to]++;

	Norm = 0;
	ForAllNodes(x) Norm += v[x]*v[x];

	return sum;
}
