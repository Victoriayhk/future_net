#ifndef _FUTURE_NET_H
#define _FUTURE_NET_H

/********************************** include ********************************/
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lib_record.h"
#include "lib_io.h"
#include "route.h"


/********************************** macro ********************************/
#define MaxV 				2000						//所有node的数量上限
#define MaxV2 				MaxV*2						//所有node的数量上限
#define MaxEOut 			20							//node的出度上限
#define MaxEdge 			MaxV*MaxEOut				//可达边的数量上限
#define MaxReachCost		100							//=max(V.cost[][]),可达边的最大权			
#define SubUnReachCost		(MaxReachCost*MaxV + 1)		//=MaxReachCost*(V.num+1),可达边构造成的-假的不可达边权重		
#define UnReachCost	 		(SubUnReachCost*(MaxV + 1))	//=SubUnReachCost*(V.num+1)4亿,int:2^31=21ww

#define routeCnMax			4							//交替寻路的轮数
#define iterationMax		10							//每轮single-route次数上限
#define UsedTourTable		False						//使用或不使用TourTbable,False True
#define findBetterResult	((roadCostSum[0]<UnReachCost) && (reIdCn<reIdCnMin || (reIdCn==reIdCnMin&&roadCostSum[0]<roadCostSumMin)))

#define TIME_OUT 			9000000						//超时时间=9000ms
#define False 				0
#define True  				1

#define PRINT_TOUR			0
#define PRINT_VTOUR			0
#define PRINT_CANDI			0

#define PRINT_OPT_INFO		0
#define PRINT_ReNode_INFO	0
#define PRINT_TIME_INFO		0

//c++11:字符串跟变量连接的时候,必须增加一个空格才行(用于printf_2var和printf_3var)
#define Print(...) //						printf		//(...)//
#define str(var,type) 				#var" = "#type
#define printf_1var(v0,type) 		Print(str(v0,type)"\n",v0)
#define printf_2var(v0,v1,type) 	Print(str(v0,type)"," str(v1,type)"\n",v0,v1)
#define printf_3var(v0,v1,v2,type) 	Print(str(v0,type)"," str(v1,type)"," str(v2,type)"\n",v0,v1,v2)
#define PRINT_ERR(format, ...)		{Print( format, ##__VA_ARGS__); exit(0);}
#define GetMin(a, b) 				((a) < (b) ? (a):(b))
#define GetMax(a, b) 				((a) > (b) ? (a):(b))

//used for creat candidates下列宏定义中的参数不要使用函数, 应该先计算出函数值再用
#define For(x, n) 					for (int x = 0; x < (n); x++)
#define MemClear(x, val, n) 		memset((x), (val), sizeof((x)[0])*(n))
#define MemCopy(x, y, n) 			memcpy((x), (y), sizeof(x[0])*n)
#define SelfAddIf(a, condi) 		{ if (condi) (a)++;}
#define ForAllNodes(x) 				for (int x = 0; x < dim; x++)
#define ForEdgesOf(e, x) 			for (Edge *e = edge[x], *e_end = edge[x]+numEdge[x]; e != e_end; e++)
#define ForCandisOf(e, x) 			for (Edge *e = edge[x], *e_end = edge[x]+numCandi[x]; e != e_end; e++)

//used for k-opt
#define MaxCandidates 				5						//每个点候选集最大个数
#define COST(A,B) 					((((long long)cost[(A)->id][(B)->id])<<7u) + (A)->outPi + (B)->inPi)
#define LinkA2B(A,B)				{(A)->succ = (B);	(B)->pred = (A);}
//test if b is in (a,c]
#define isInSegment(b,a,c)			((a->order<c->order && (a->order<b->order  && b->order<=c->order)) \
									|| (c->order<a->order && (b->order<=c->order || a->order<b->order)))
#define InBestTour(a, b) 			((a)->bestSucc == (b))
#define InLastBestTour(a, b) 		((a)->lastBestSucc == (b))
//test if isShouldBe is true(说白了就是：条件的地方放应该发生的事情,如果应该发生的事未发生就continue)
#define AssertContinue(isShouldBe) 	{if(!(isShouldBe))	continue;}
#define RecordT(num) 				T##num=t##num


/********************************** 外部声明 ********************************/
typedef long long Int64;
class Edge;
class Queue;
class Heap;
class CandidateSolver;
class KMTour;
class TourTable;
class HashTable;
class VNode;
class VVNode;
class Graph;
class Road;
extern Graph G;
extern Road Road0,Road1;
template <class T> inline bool checkMin(T& a, T b)
{ 
	if (b < a) { a = b; return true; }
	return false;
}
template <class T> inline bool checkMax(T& a, T b) 
{ 
	if (b > a) { a = b; return true; }
	return false;
}


/********************************** class definition ********************************/
class Edge 			// 候选(边)集中的结点
{
public:
	int to;						// 边的端点编号
	int cost;					// 边的cost
	int cost_pi;				// 带pi的cost值
	int alpha;					// alpha值
};

class Heap			// 二叉堆
{
	int h[MaxV2*2 + 1];				// 堆值id
	int sum, pos[MaxV2*2], *key;	// 堆的当前容量, 堆元素值的位置下标, 排序关键字
	inline void lazy_insert(int);	// 末尾加新元素id, 不检查是否重复加入, O(1)
	void shift_up(int);				// 上调某节点, 参数为id, O(logn)
	void shift_down(int);			// 下调某节点, 参数为id, O(logn)
public:
	Heap();							// 构造函数, O(n)
	bool empty();					// 是否为空, O(1)
	int size();						// 当前堆中的的元素个数
	bool has(int);					// 检查是否包含某个元素, O(1)
	void init_with_key(int []);		// 指定排序关键字key, key为引用, 不开辟新空间, 不会初始化pos, O(1)
	void push(int);					// 插入新元素, 不查重, 重复push会push进去多次, 会出错, O(logn)
	void change(int);				// key值已改, 需要上调(理论上要考虑下调, 这里特殊情况, 只要上调), O(logn)
	int pop();						// 弹出堆顶元素, O(logn)
};

class Queue 
{
	int front, tail, capicity, *q;
public:
	Queue() { capicity = MaxV; front = tail = 0; q = new int[MaxV]; }
	Queue(int _size) { capicity = _size, front = tail = 0; q = new int[capicity]; }
	~Queue() { delete q; }
	void clear() {front = tail = 0;}
	bool empty() {return front == tail; };
	void push(int x) {q[tail++] = x; if (tail == capicity) tail = 0; }
	int pop() {int ret = q[front++]; if (front == capicity) front = 0; return ret; }
};

class CandidateSolver
{
public:
	int num, dim;						// 结点数, 如果是atsp问题有dim = 2*num, tsp问题有dim=num
	int numEdge[MaxV2*2];				// 各节点邻边数;
	int numCandi[MaxV2*2];				// 候选集数(<=边数)
	Edge *edge[MaxV2*2];				// 边的邻接表
	Edge edge_base[MaxV2*(MaxEOut+2)];	// 边的邻接表存储在一个一维列表中
	VVNode *node;						// 结点链表
	Heap heap;							// 二叉堆

	int nsp;							// 1-tree特殊点
	Edge *esp;							// 1-tree特殊边
	bool in_mst[MaxV2*2];
	int mst_list[MaxV2*2];				// 最小生成树序列(默认根为0)
	int dad[MaxV2*2];					// mst中的父节点
	int cost[MaxV2*2];					// 连接到mst的cost
	int v[MaxV2*2], last_v[MaxV2*2];	// 结点在1-tree中的度-2
	int pi[MaxV2*2], best_pi[MaxV2*2];	// 结点Pi值

	int CandidatesNum;
	int Precision;						// 精度
	int AscentCandidates;				// Ascent用
	bool SymmetricCandidates;			// P34, GenerateCandidateSet用
	int Norm;							// Ascent用, Norm为v的平方和
	int InitialPeriod, InitialStepSize;	// Ascent用的两个控制迭代次数和步长的变量
	double Excess;						// CreateCandidateSet用, P32

	void check_special(int, int&);		// 检查当前结点是否可以构成1-tree的SpecialNode, O(n)
	inline void add_edge(int, int, int);// 在已分配空间的情况下, 加一条双向边, 边的下标为numCandi[x], O(1)

	void calc_alpha();					// 计算alpha值, 依赖于边的cost_pi值, O(n^2)
	Int64 mst(bool);					// 求最小生成树, 依赖于边的cost_pi值, O(n^2)
	Int64 mini_1tree(bool);				// 求最小1-tree, 依赖于pi值, O(n^2)
	Int64 ascent();						// 次梯度优化, O(iteration * n^2)
	void gen_candi(int, Int64, bool);	// 生成候选集, 依赖于alpha值, O(n*n*log(n))
	void init(Road *roadp);				// O(n^2)
	unsigned char create_candi();		// O(max(ascent, gen_candi))
};

class VNode						//used in Graph(),setVcost_SPFA()
{
public:
    int predList[MaxV];
	int	predCn;
	
	VNode(){};
	void addPred(int NodeId) { predList[predCn++] = NodeId;}//类中函数编译时候自动加inline
};

class VVNode					//used in LKH()
{
public:
	int id;						//node的id,是必经点经过映射后的id,也就是demand.csv中出现的顺序
    VVNode *pred,*succ;			//路径中后继节点和前驱节点a
	VVNode *bestSucc;			//后继:一次findTour中,最好路径的记录
	VVNode *lastBestSucc;		//后继:一次findTour中,次最好路径
	int inPi,outPi;
	VVNode *cand[MaxCandidates];//候选后继数组
	int alpha[MaxCandidates];	//候选后继点的Alpha距离(在LKH论文中写的)
	int candCn;					//候选后继数量
	VVNode *next;				//指向下一个active node,是空的话代表未激活(就是不在循环队列里)
	int order;					//tour中出现的顺序
	
	VVNode(){};
	void addCand(VVNode *N) { cand[candCn++] = N;}
};

class KMTour {
	VVNode *Node;
	int n, match[MaxV2];
	int lx[MaxV2], ly[MaxV2], slack[MaxV2];
	int (*cost)[MaxV2];
	bool vx[MaxV2], vy[MaxV2];

	bool dfs(int u);
	void run_km();

public:
	bool init_tour(int, int [][MaxV2], VVNode[], long long&);
};


const int cnMax = 10000;
class TourTable
{
public:
	int num,validNum;//,cnMax;
	unsigned char isMalloc[cnMax];
	unsigned int rpHash[cnMax];
	int roadCost[cnMax];
	int tourCn[cnMax];
	int VtourCn[cnMax];
	int *Vtour[cnMax];
	int *VtourId[cnMax];
	
	TourTable()
	{
		num = validNum = 0;
		memset(isMalloc,False,cnMax);
	};
	void reset();
	void lock(Road *Rp);
	void insert(Road *Rp,int bestCost);
	unsigned char isInTable(unsigned int hash);
	//void resumeResult(Road *Rp);
};

class HashTable
{
public:
	int cn;
	unsigned int rpoHash[routeCnMax*iterationMax];
	int VtourCn[routeCnMax*iterationMax];
	int *VtourId[routeCnMax*iterationMax];
	int *Vtour[routeCnMax*iterationMax];
	long long roadCost[routeCnMax*iterationMax];
	
	HashTable()	{cn = 0;};
	void reset(){cn = 0;};
	void insert(Road *Rp);
	int whereInTable(unsigned int hash);
	void resumeResult(Road *Rp);
};

class Graph
{
private:
	void setCost();				//used in initial()
public:
	int cost[MaxV][MaxV];		//两点之间最短路径
	int id[MaxV][MaxV];			//两点之间最短路径ID
	int subCost[MaxV][MaxV];	//两点之间第二短路径
	int subId[MaxV][MaxV];		//两点之间第二短路径ID
	VNode Node[MaxV];			//所有点的结构体,used in SPFA
	int num;					//number of nodes in V
	
	Graph(){};													//used in route()
	void initial(char *topo[MAX_EDGE_NUM], int edge_num);		//used in route()
};

class Road
{
private:
	//all only used in search_single_route()
	void swapGraphCost(Road *Rpo);
	void swapGraphCostBack(Road *Rpo);
	void setPunish();
	static void setPunishMethod();
	
	void setVcost_SPFA();
	void setVcost_reduced_SPFA();
	void setCost();
	void setTour();
	void setVtour();
	void setVtourId();
	void setVtourCost();
	
	long long LKH();							//used in search_single_route()
	void resetBestSucc();						//used in LKH()
	long long getBestTourCost();				//used in LKH()
	long long findTour(unsigned char );			//used in LKH()
	void storeBestSucc();						//used in findTour()
	long long initialTour(unsigned char,unsigned char &);//used in findTour()
	void resetOrder();							//used in initialTour(),_X_OPT()
	void resetActive();							//used in _X_OPT()
	void Active(VVNode *N);						//used in _X_OPT()
	VVNode *RemoveFirstActive();				//used in _X_OPT()
	
	//definition in candidate.cpp
	unsigned char creatCandidates(long long &minBestCost);//used in LKH()
	
	//definition in _X_OPT.cpp
	unsigned char _3_OPT(long long &bestCost);	//used in findTour()
	unsigned char _4_OPT(long long &bestCost);	//used in findTour()
	unsigned char _5_OPT(long long &bestCost);	//used in findTour()
	
	//definition in exam_repeat.cpp
	int examRepeatNode();						//used in search_single_route()
	void resumeBak();							//used in search_single_route()、examRepeatNode()
	
public:
	static Graph *Gp;					//两条路共用的Graph的地址
	static int s,t;						//两条路共用的source,destination
	static int sLKHuseCn;				//寻路总共使用LKH的次数
	
	static long long roadCostSum[3];	//两条路的路径和,[0][1]是最新和上一次
	static long long roadCostSumMin;	//两条路的路径和最优值(保证reIdCn最小的情况下)
	
	static int reIdCn,reIdCnMin;		//两条路重复边个数的当前值和最好值
	static int reIdList[MaxV];			//[0][1]是最新和上一次,reIdList是重复的边的id数组
	static int punish;					//punish是惩罚,reCostAvg是重复边的权值平均
	static unsigned char usedSlowPunish;
	
	static int routeCn;					//route的轮数计数器
	static int iteration,iter[routeCnMax];//每轮中single_route的计数器,和上限
	static int trial,Trials;			//使用findTour次数计数器,和次数上限
	
	//for tsp
	VVNode Node[MaxV2];					//必经点的集合
	VVNode *firstActive,*lastActive;	//指向第一个active node,指向最后一个active node
	int num,numInit,numRepeat,numBak;	//环路中当前必经点的数量,和初始化必经点数量,tsp每次的重复点数,加入重复点时备份变量
	int list[MaxV2],listBak[MaxV2];		//环路中当前必经点的id数组,加入重复点时备份
	int cost[MaxV2][MaxV2];				//环路中当前必经点间最短路径
	int tour[MaxV2];					//环路中当前必经点顺序
	unsigned int rpoHash;				//另外一条路设置的阻碍,得到的hash值
	HashTable ht;						//存储single_route的结果
	TourTable tt;						//存储findTour中的中间结果
	long long sumPi;					//节点outPi和inPi的和
	
	//for HuaWei road
	int id;								//路的编号,0,1
	int Vcost[MaxV][MaxV];				//环路用到的所有点间最短路径(set in Graph::SPFA())
	int Vpath[MaxV][MaxV];				//环路用到的所有点间路径(set in Graph::SPFA())
	int VuseCn[MaxV];					//环路用到的所有点用的次数
	unsigned char isMustOld[MaxV];		//所有点是否是must点的flag,在加入非必须点之前的状态
	unsigned char isMust[MaxV];			//所有点是否是must点的flag,当前值
	unsigned char isMustBak[MaxV];		//加入重复点时备份
	unsigned char isMustInit[MaxV];		//所有点是否是must点的初始化flag
	int Vtour[MaxV],VtourCn;			//s->t路径中当前所有点(必经+非必经)顺序和数量
	int VtourCost[MaxV];				//s->t路径中当前所有点(必经+非必经)向后权值(最后点t没有)
	int VtourId[MaxV];					//s->t路径中当前所有点(必经+非必经)向后边ID(最后点t没有)
	long long roadCost[3];				//s->t路径和,[0][1]是最新和上一次
	int solveTspCn;						//单条路求tsp次数计数器
	unsigned char isUseCopy;			//是否使用copy点策略
	long long singleRouteTime;			//单次使用search_single_route的时间
	
	//definition in Road.cpp
	Road(){resetRoad();}				//used in route();
	void resetRoad()
	{
		roadCost[0] = roadCost[1] = roadCost[2] = UnReachCost;
		VtourCn = 0;
	};
	void initial(char *demand,Graph *Gp,int id);		//used in route();
	static unsigned int SDBMHash(int cn,int arr[]);		//used in swapGraphCost()
	
	//definition in route.cpp(出现在类体外的函数定义不能指定关键字static,只能出现在类内)
	static void search_double_route();					//used in route();
	void search_single_route(Road *Rpo);				//used in search_double_route();
	static void setReIdList(Road *Rp,Road *Rpo);		//used in search_double_route();
	static void reWriteResult(int VtourCn0,int VtourId0[],int VtourCn1,int VtourId1[]);//used in search_double_route();
};

#endif
