#include "future_net.h"

CandidateSolver candi;

/*************************************************************************
*  函数名称：Road::creatCandidates()
*  功能说明：初始化Node[]的候选集，candCn和cand[]
*  参数说明：无
*  函数返回：True代表使用ascent直接找到解了,False就是没有找到解
*  修改时间：2016-4-30
*************************************************************************/	
unsigned char Road::creatCandidates(long long &bestCostMin)
{
	candi.init(this);
	unsigned char isFindTour = candi.create_candi();
	
	//求sumPi
	sumPi = 0;
	for (int x = 0; x < num; x++)
		sumPi += Node[x].outPi + Node[x].inPi;
	
	//norm == 0直接出解
	if(isFindTour)
	{
		Print("wanted tour is find in ascent!\n");
		bestCostMin = 0;
		for(int i = 0;i < num;i++)
		{
			bestCostMin += COST(&Node[i],Node[i].bestSucc);
			Node[i].succ = Node[i].bestSucc;	//给TourTable的insert用(bestSucc在create_candi中设置过)
		}
		bestCostMin = (bestCostMin-sumPi)>>7u;
		
		//if(UsedTourTable && bestCostMin < UnReachCost)	tt.insert(this,bestCostMin);
	}
	
	//打印候选集
	#if PRINT_CANDI == 1
		for(int n1 = 0,id;n1 < num;n1++)
		{
			Print("*******************node %3d(%3d) candidate : *******************\n",n1,list[n1]);
			for(int j = 0;j < Node[n1].candCn;j++)
			{
				id = Node[n1].cand[j]->id;
				Print("candidate %dth, id=%3d(%3d), alpha=%4d, cost=%3d\n",j,id,list[id],Node[n1].alpha[j],cost[n1][id]);
			}
			Print("\n");
		}
	#endif
	return isFindTour;
}

/**********************************************************************
生成候选集

按alpha排序, alpha需要先算好

作者: 易惠康
日期: 2016-05-04

***********************************************************************/
void CandidateSolver::gen_candi(int max_candi, long long max_alpha, bool symmetric = false) {
	int *alpha = best_pi;
	heap.init_with_key(best_pi); // 把best_pi当alpha排序用, 借用堆进行排序

	if (num != dim)
		max_candi = GetMin(max_candi + 1, num);	// 自己到自己的复制点不算

	for (int x = 0; x < dim; x++) {
		// 对候选集进行排序
		for (int i = 0; i < numCandi[x]; i++) {
			int y = edge[x][i].to;
			alpha[y] = edge[x][i].alpha;
			cost[y] = edge[x][i].cost;			// 原cost表示结点并入mst的代价, 到这里用不上了.
			heap.push(y);
		}

		for (int i = 0; !heap.empty(); i++) {
			int y = heap.pop();
			edge[x][i].to = y;
			edge[x][i].alpha = alpha[y];
			edge[x][i].cost = cost[y];
		}

		// 截取前max_candi个
		numCandi[x] = GetMin(numCandi[x], max_candi);
	}
}



/**********************************************************************
次梯度优化

本方法将最后得到: 一个最优带最佳pi值的1-tree
	+ weight, nsp, esp, pi, norm

作者: 易惠康
日期: 2016-05-04

***********************************************************************/
Int64 CandidateSolver::ascent() {
	bool InitialPhase = true;
	Int64 W, W0, BestW;
	int BestNorm;

Start:
	MemClear(pi, 0, dim);
	MemClear(best_pi, 0, dim);
	W = mini_1tree(0);
	MemCopy(last_v, v, dim);

	if (Norm == 0) return W;
	gen_candi(AscentCandidates, UnReachCost, 0);

	BestW = W0 = W;
	BestNorm = Norm;
	for (int T = InitialStepSize * Precision, Period = InitialPeriod;
		Period > 0 && T > 0 && Norm != 0; Period /= 2, T /= 2) {
		for (int P = 1; T > 0 && P <= Period && Norm != 0; P++) {
			ForAllNodes(x) {
				if (v[x]) 
				{
					pi[x] += T*(7 * v[x] + 3 * last_v[x]) / 10;
					checkMin(pi[x], UnReachCost >> 2); // 将Pi值约束在[-UnReachCost/4, UnReachCost/4]之间
					checkMax(pi[x], -(UnReachCost >> 2));
				}
				last_v[x] = v[x];
			}

			W = mini_1tree(1);
			//Print("* T = %d, Period = %d, P = %d, W = %0.1f, Norm = %d\n",
            //             T, Period, P, (double) (W) / Precision+3300000, Norm);
			if (Norm == 0) return W;
			if (W > BestW || (W == BestW && Norm < BestNorm)) {
				if (W - W0 > (W0 >= 0 ? W0 : -W0) && AscentCandidates > 0
					&& AscentCandidates < dim) { // 候选集不够大
					W = mini_1tree(0);
					if (W < W0) {
						AscentCandidates *= 2;
						if (AscentCandidates > dim)
							AscentCandidates = dim;
						goto Start;
					}
					W0 = W;
				}

				BestW = W;
				BestNorm = Norm;
				MemCopy(best_pi, pi, dim);

				if (InitialPhase) T *= 2;
				if (P == Period) Period = GetMin(Period * 2, InitialPeriod); // 或者写作Period*2?
			}
			else if (InitialPhase && P > InitialPeriod / 2) {
				InitialPhase = false;
				P = 0;
				T = 3 * T / 4;
			}
		}
	}

	MemCopy(pi, best_pi, dim);
	Int64 ret = mini_1tree(0);
	return ret;
}


/**********************************************************************
由road初始化本类实体所代表的图, FirstNode为结点链表, atsp_tag表示road代表的
图是不是atsp

1. 关于road, 只在这里用一次, 之后不会再被本类使用.
2. 关于FN(结点列表), 这里当FN是空的, 本函数会初始化FN
3. atsp_tag: true表示road代表的图是有向的, false表示road代表的图是无向的,
对于有向图这里会将点double再求候选集

本方法可以多次调用, 但如果图的拓扑结构一致, 只有权不一致, 调用alter_edge_cost
改变权值是更好的方法

复杂度: O(n^2)

作者: 易惠康
日期: 2016-05-04

***********************************************************************/
void CandidateSolver::init(Road *roadp)
{
	num = roadp->num;
	dim = num * 2;
	
	// 结点序列
	node = roadp->Node;
	for (int i = 0; i < dim; i++)
	{
		mst_list[i] = i;	// 最小生成树序列
		numEdge[i] = numCandi[i] = 0;
	}

	// 统计相邻的边的数量
	for (int i = 0; i < num; i++) {
		numEdge[i]++;
		numEdge[i + num]++;
		for (int j = 0; j < num; j++) if (i != j && roadp->cost[i][j] != UnReachCost) {
				numEdge[i]++;
				numEdge[j + num]++;
			}
	}

	// 给边开辟空间
	int cnt = 0;
	for (int i = 0; i < dim; i++) {
		edge[i] = &edge_base[cnt];
		cnt += numEdge[i];
	}

	// 把边的信息填进去
	for (int i = 0; i < num; i++) {
		add_edge(i, i + num, -100000 << 7u);
		for (int j = 0; j < num; j++)
			if (i != j && roadp->cost[i][j] != UnReachCost) {
				add_edge(i, j + num, roadp->cost[i][j] << 7);
			}
	}

	// 初始化参数
	SymmetricCandidates = true;
	InitialPeriod =  num < 100 ? 100 : num;
	InitialStepSize = 1;
	Excess = 1.0 / dim;
	CandidatesNum = 5;
	AscentCandidates = num;
	Precision = 128;
}


inline void CandidateSolver::add_edge(int i, int j, int cost) {
	edge[i][numCandi[i]].to = j;
	edge[i][numCandi[i]].cost = edge[i][numCandi[i]].cost_pi = edge[i][numCandi[i]].alpha = cost;
	numCandi[i]++;

	edge[j][numCandi[j]].to = i;
	edge[j][numCandi[j]].cost = edge[j][numCandi[j]].cost_pi = edge[j][numCandi[j]].alpha = cost;
	numCandi[j]++;
}


/**********************************************************************
计算各边的alpha值

作者: 易惠康
日期: 2016-05-04

***********************************************************************/
void CandidateSolver::calc_alpha() { // 见Page23
	int *beta = best_pi;	// best_pi作beta用
	int *mark = last_v;		// last_v作mark用

	// 考虑不在1-tree中的边
	MemClear(mark, -1, dim);
	for (int i = 0; i < dim; i++) {
		int from = mst_list[i];
		beta[from] = -UnReachCost;
		mark[from] = from;
		for (int to = from; to != 0; to = dad[to]) { // 从from结点向上求beta值
			beta[dad[to]] = GetMax(beta[to], cost[to]);
			mark[dad[to]] = from;
		}

		for (int j = 0; j < dim; j++) { // 向下求beta值
			int to = mst_list[j];
			if (mark[to] != from)
				beta[to] = GetMax(beta[dad[to]], cost[to]);
		}

		for (int j = 0; j < numEdge[from]; j++) {
			int to = edge[from][j].to;
			if (from != nsp && to != nsp) {
				edge[from][j].alpha = edge[from][j].cost_pi - beta[to];
			} else {
				edge[from][j].alpha = GetMax(edge[from][j].cost_pi - esp->cost_pi, 0);
			}
		}
	}
}



/*************************************************
创建候选集
需要的准备工作有:
1. 图
2. 相关参数(最大候选集数等)
输出得到:
1. 各节点的cand, candCn

作者: 易惠康
日期: 2016-05-04

**************************************************/
unsigned char CandidateSolver::create_candi() 
{
	Int64 low_bound = ascent();
	
	// ascent找到一条环路, 将不会更新候选集而直接得到tour解
	if (Norm == 0) 
	{ 
		for (int x = nsp; x != 0; x = dad[x])// 通过dad得到解
			node[x].bestSucc = &node[dad[x]];
		for (int x = esp->to; x != 0; x = dad[x])
			node[dad[x]].bestSucc = &node[x];
		node[esp->to].bestSucc = &node[nsp];
		
		// 点double了, 要去掉复制点
		if (node[0].bestSucc->id == num) {	// 当前的环的反向是反的
			for (int x = 0; x < num; x++)
				node[node[x + num].bestSucc->id].bestSucc = &node[x];
		} 
		else 
		{							// 当前的环的方向是正的
			for (int x = 0; x < num; x++)
				node[x].bestSucc = node[x].bestSucc->bestSucc;
		}
		return True;
	}
	
	Int64 max_alpha = low_bound / Precision;//Excess * Abs(low_bound);
	
	calc_alpha(); // 计算alpha值
	gen_candi(CandidatesNum, max_alpha, SymmetricCandidates);
	
	// 将候选集解复制到node
	for (int x = 0; x < num; x++) 
	{
		node[x].candCn = numCandi[x] - 1;
		for (int i = 0, j = 0; i < node[x].candCn; i++, j++) {
			if (edge[x][j].to == x + num) j++;
			node[x].cand[i] = &node[edge[x][j].to - num];
			node[x].alpha[i] = edge[x][j].alpha;//把alpha赋值到Node里面
		}
		//把pi赋值到Node里面
		node[x].outPi = pi[x];
		node[x].inPi  = pi[x + num];
	}
	return False;
}

