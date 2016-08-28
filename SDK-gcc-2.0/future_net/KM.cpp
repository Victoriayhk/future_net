#include "future_net.h"


/*************************************************************************
使用指派方式初始化一个解
参数:
    1. num: 结点数
    2. _cost: 结点间的cost, 实际使用的加上inPi, outPi后的修正权值
    3. node: 结点(需要用到里面的inPi, outPi值)
    4. cost_sum: 计算解的边权和
得到:
    1. node中的succ, pred属性连接构成一个循环双向链表, 即解
    2. cost_sum
返回值:
    true: 当前解就是最优解(单个圈), 无需迭代求优
    false: 当前解还不是最优解(多个圈)
算法复杂度:
    KM算法, O(n^3)
*************************************************************************/
bool KMTour::init_tour(int num, int _cost[][MaxV2], VVNode node[], long long &cost_sum) {
    n = num; cost = _cost; Node = node;
    run_km();

    int cnt = 0;
    int *medge = slack;
    MemClear(vx, 0, n);
    for (int i = 0; i < n; i++) 
	{
        LinkA2B(&node[match[i]], &node[i]);
        if (!vx[i]) {
            vx[i] = true;
            int mcost = cost[match[i]][i], k = i;
            // long long mcost = iCOST(match[i], i);
            // int k = i;
            for (int j = match[i]; j != i; j = match[j]) {
                vx[j] = true;
                if (checkMax(mcost, cost[match[j]][j])) k = j;
                // if (checkMax(mcost, iCOST(match[j], j))) k = j;
            }
            medge[cnt++] = k;
        }
    }

    medge[cnt] = medge[0];
    for (int i = 0; i < cnt; i++) { // 多个圈, 合并圈圈
        int x = medge[i], y = medge[i + 1];
        LinkA2B(&node[match[x]], &node[y]);
    }

    cost_sum = 0;       // 路径和
    for (int i = 0; i < num; i++) cost_sum += COST(&node[i], node[i].succ);

    return cnt == 1;    // cnt==1表示当前指派问题形成一个圈, 解已经找到, 否则还没有
}

void KMTour::run_km() {
    MemClear(match, -1, n);
    MemClear(lx, false, n);
    MemClear(ly, false, n);

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) checkMax(lx[i], -cost[i][j]);

    for (int x = 0; x < n; x++) {
        for (int i = 0; i < n; i++) slack[i] = UnReachCost;

        while (1) {
            MemClear(vx, 0, n);
            MemClear(vy, 0, n);

            if (dfs(x)) break;

            int d = UnReachCost;
            for (int i = 0; i < n; i++)
                if (!vy[i] && d > slack[i]) d = slack[i];

            for (int i = 0; i < n; i++) {
                if (vx[i]) lx[i] -= d;
                if (vy[i]) ly[i] += d; else slack[i] -= d;
            }
        }
    }
}

bool KMTour::dfs(int u) {
    vx[u] = true;
    for (int v = 0; v < n; v++) {
        if (vy[v]) continue;
        int t = lx[u] + ly[v] + cost[u][v];
        if (t == 0) {
            vy[v] = true;
            if (match[v] == -1 || dfs(match[v])) {
                match[v] = u;
                return true;
            }
        }
        else if (slack[v] > t) slack[v] = t;
    }
    return false;
}
