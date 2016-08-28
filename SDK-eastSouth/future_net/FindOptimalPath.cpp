#include "Algorthm.h"       //算法所需的头文件声明

/*
 * 启发式搜索算法的主程序函数,被search_route函数调用
 *
 * 输入:(1)ATSP问题维数Dim;(2)任意两点之间的权值矩阵WeightMatrix[][]
 * 输出:存储输出路径的数组NodePath,返回最短路径权值
 */
int SelectedEdgesNum;   //每个节点的候选边的数量
CostType MinimumCost;      //最好路径的权值
int *BestPath;  //当前找到的最好路径
CostType BetterCost;    //较好路径的权值
int *BetterPath;    // 较好的路径
int CacheMask;
int *CacheVal;
int *SavedFlag;

int *CostMatrix;   //权值矩阵
int Dim;                //节点个数
int SavedDim;    //保存的节点个数
double Ex;
Node *FirstActive, *LastActive; //激活节点集合的第一个和最后一个顶点
Node *FirstNode;      //链表中的第一个节点
Segment *FirstSegment;  //指向第一个段的指针
SSegment *FirstSSegment;    //指向第一个大段的指针

int SegSize;
int SSegmentSize;
int SegmentNums;    //段的数量
int SSegmentNums;   //大段的数量
unsigned KeyValue;  //当前路径的哈希值
Node **Heap;    //用于计算最小生成树的堆
HashTable *HTable;      //哈希表用于存储路径信息
int InitIterationTimes;      //子梯度上升算法的初始迭代次数
int StepLength;    //子梯度上升算法的初始步长

double LB;      //子梯度上升算法得到的下界
int M;      //将ATSP问题转化为STSP时使用的参数
int MAXWidth; //为进行一次有效边交换的最大搜索次数
int MAXSelectedEdgesNum;    //每个节点允许的最多节点数
int MAXExchanges;   //最大可交换次数
int MAXTests;  //算法一次运行的最多尝试次数
int ExchangeType;   //定义使用的边交换类型
Node *NodeSet;  //节点数组
int Discrepancy;    //衡量最小1树的"环度",等于0表示其为1个环,不需再进行下面的边交换操作
CostType OptimalCost;     //已知的最小权值
int Accuracy;  //内部计算精度
int IsCostAvailable;
unsigned *RandomNumSet; //存放随机数的数组
short IsPathReversed;
int Run; /* 当前运行次数 */
int RunTimes;       /* 运行总次数 */
unsigned RandNum;  /* 生成随机数的种子*/
RecordOfExchange *SwapStack;  /* 存储边交换记录的栈 */
int ExchangeTimes;      /* 边交换次数 */
int Test;      /* 当前尝试的次数 */

CostFunction  DISTANCE, COST, c;   //查找权值函数的指针





int FindOptimalPath(int *NodePath,  int  NodeNum, int **WeightMatrix)
{

//     for(int i = 0; i < NodeNum; i++)
//     {
//            for(int j = 0; j < NodeNum; j++)
//            {
//                printf("%d  ",WeightMatrix[i][j]);
//            }
//        printf("\n");
//        }

      CostType Cost;
      int i,j = 0;
     Dim = NodeNum ;                    //Dim为全局变量,在头文件中定义,此处赋值之后传给readProblem()函数
     InitParameters();                      //      算法参数初始化
     ReadPathMatrix(WeightMatrix);     //读取路径权值矩阵,并存储
     BuildDataStructure();                   //  给算法所需的数据结构分配存储空间
     FindPromisingEdges();               //为每个节点创建候选边集合

    if (Discrepancy != 0)
        MinimumCost = INFINITY;    //若没找到最优解,置最小权值为INFINITY
    else {
        /* 最小1树的子梯度上升算法已找到最优路径,直接保存结果 */
        OptimalCost = MinimumCost = (CostType) LB;
        SaveBetterResult();
        SaveBestResult();
        RunTimes = 0;                  //不再需要进行边交换,置边交换算法的执行次数为0
    }


  //  运行边交换算法(RunTimes)次,找局部最优解
    for (Run = 1; Run <= RunTimes; Run++) {
        Cost = FindTour();            /* 使用边交换的启发式算法来优化初始的Tour */
       if (Run > 1)
            Cost = CombineCurrentPathWithBestPath();
        if (Cost < MinimumCost) {
            MinimumCost = Cost;
            SaveBetterResult();
            SaveBestResult();
        }

        if (Cost < OptimalCost) {
            OptimalCost = Cost;
        }

        srand(++RandNum);
    }

  //保存最短路径到NodePath中
    for (i = 1; i <  NodeNum && BestPath[i] != 1; i++);  //找到起点
    for (j =  0; j < NodeNum -1; j++) {
          NodePath[ j] = BestPath[i] -1;
            if (++i >  NodeNum)
                i = 1;
    }
    return  MinimumCost;    //返回最小权值
}
