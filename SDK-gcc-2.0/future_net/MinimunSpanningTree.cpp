#include "future_net.h"


/*******************************************************************************
最小生成树
用到的值有:
 1. 结点的cost_pi值, 即cost(i, j) + pi(i) + pi(j)
 2. 结点的边集 (用到了所有边, 待改成候选集)
更新的值有:
 1. 结点的dad, 根节点默认为0, 根节点的父节点为其第一个儿子结点
 2. 结点的cost, 根节点默认为0, 根节点的cost为到其第一个儿子结点的cost
 3. 结点的度-2: V
 4. mst_list: 最小生成树链
返回值: 最小生成树的权值和

作者: 易惠康
日期: 2016-05-04
*********************************************************************************/

Int64 CandidateSolver::mst(bool Sparse) {
	Int64 sum = 0;

	// 初始化相关权值
	ForAllNodes(x) {
		v[x] = -2;					// 度-2
		cost[x] = UnReachCost;		// 权
	}

	cost[0] = 0;
	mst_list[0] = 0;
	if (Sparse) {					// 稀疏图
		heap.init_with_key(cost);
		for (int i = 1; i < dim; i++)
			heap.push(i);

		// 根节点
		for (int i = 0; i < numCandi[0]; i++) {
			cost[edge[0][i].to] = edge[0][i].cost_pi;
			dad[edge[0][i].to] = 0;
			heap.change(edge[0][i].to);
		}

		// 其它结点
		for (int i = 1; i < dim; i++) {
			int x = heap.pop();
			mst_list[i] = x;

			sum += cost[x];
			v[x]++; v[dad[x]]++;

			for (int j = 0; j < numCandi[x]; j++) {
				int y = edge[x][j].to;
				if (heap.has(y) && edge[x][j].cost_pi < cost[y]) {
					cost[y] = edge[x][j].cost_pi;
					dad[y] = x;
					heap.change(y);
				}
			}
		}
	}
	else {	// 稠密图
		MemClear(in_mst, false, dim);
		in_mst[0] = true;

		int mcost = UnReachCost + 1, blue = 0, p = 0;
		for (int i = 1; i < dim; i++) {
			for (int j = 0; j < numCandi[blue]; j++) {
				int x = edge[blue][j].to;
				if (!in_mst[x] && checkMin(cost[x], edge[blue][j].cost_pi)) dad[x] = blue;
			}
			for (int j = i; j < dim; j++)
				if (checkMin(mcost, cost[mst_list[j]])) p = j;

			blue = mst_list[p];
			sum += cost[blue];
			if (p != i) {
				mst_list[p] = mst_list[i];
				mst_list[i] = blue;
			}
			in_mst[blue] = true;
			v[blue]++;
			v[dad[blue]]++;
			mcost = UnReachCost + 1;
		}
	}
	dad[0] = mst_list[1];
	cost[0] = cost[dad[0]];

	return sum;
}
