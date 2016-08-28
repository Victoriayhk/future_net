#include "Algorthm.h"
#include "Structure.h"

/*
 * ReadPathMatrix函数读取最短路径信息,并存储在对应的数据结构中
 */
static void BuildNodes(void);     //创建一个节点
static void SaveMatrixInfo(int **WeightMatrix);  //存储权值矩阵信息
int C_FUNCTION(Node * Na, Node * Nb);
int D_FUNCTION(Node * Na, Node * Nb);
int Distance(Node * Na, Node * Nb);     //获取两个节点之间权值
void ReadPathMatrix(int **WeightMatrix)
{
    ReleaseAll();  //初始化
    FirstNode = 0;        //初始化头节点
    SavedDim = Dim ;  //保存矩阵维数
    SaveMatrixInfo(WeightMatrix);

    COST =  C_FUNCTION ;
    DISTANCE=  D_FUNCTION;
 //根据问题规模调整参数,防止越界
   if (SelectedEdgesNum > Dim - 1)
           SelectedEdgesNum = Dim - 1;
 }



  //创建节点
static void BuildNodes()
{
    Node *Prev = NULL, *N = NULL;


    if (Dim <= 0)
   {
       printf("DIMENSION should be a positive integer!");
        exit(0);
   }

    Dim *= 2;  //将ATSP转化为STSP时维数加倍
    assert(NodeSet = (Node *) calloc(Dim + 1, sizeof(Node))); //申请内存
    for (int i = 1; i <= Dim; i++, Prev = N) {
        N = &NodeSet[i];
        if (i == 1)
            FirstNode = N;
        else
            Link(Prev, N);
        N->Id = i;

    }
    Link(N, FirstNode);
}
//存储路径矩阵信息
static void SaveMatrixInfo(int **W)
{
    Node *Nodei, *Nodej;


      FirstNode = NULL;
     if (!FirstNode)
        BuildNodes();   //创建头结点

        int n = Dim / 2;

       assert(CostMatrix = (int *) calloc((size_t) n * n, sizeof(int)));

       for (Nodei = FirstNode; Nodei->Id <= n; Nodei = Nodei->Suc)

            Nodei->Row = &CostMatrix[(size_t) (Nodei->Id - 1) * n] - 1;

            n = Dim / 2;
            for (int i = 1; i <= n; i++) {
                Nodei = &NodeSet[i];
                for (int j = 1; j <= n; j++) {

                    Nodei->Row[j] = W[i-1][j-1];
                    if (i != j && W [i-1][j-1]> M)
                        M = W[i-1][j-1];
                }
                Nodej = &NodeSet[i + n];
                if (!Nodei->PointTo1)
                    Nodei->PointTo1 = Nodej;
                else if (!Nodei->FixedTo2)
                    Nodei->FixedTo2 = Nodej;
                if (!Nodej->PointTo1)
                    Nodej->PointTo1 = Nodei;
                else if (!Nodej->FixedTo2)
                    Nodej->FixedTo2 = Nodei;
            }
}


/*
 *查找两点之间的权值函数
 */

int C_FUNCTION(Node * Na, Node * Nb)
{
    Node *Nc;
    PromisedEdges *pEdge;
    int Index, i, j;

    if (IsCostAvailable) {
        if (Na->Suc == Nb)
            return Na->SucCost;
        if (Na->Pred == Nb)
            return Na->PredCost;
    }
    if ((pEdge = Na->PromisedEdgesSet))
        for (; (Nc = pEdge->To); pEdge++)
            if (Nc == Nb)
                return pEdge->Cost;

    if (SavedFlag == 0)
        return DISTANCE(Na, Nb);
    i = Na->Id;
    j = Nb->Id;
    if (i > j) {
        int k = i;
        i = j;
        j = k;
    }
    Index = ((i << 8) + i + j) & CacheMask;
    if (SavedFlag[Index] == i)
        return CacheVal[Index];
    SavedFlag[Index] = i;
    return (CacheVal[Index] = DISTANCE(Na, Nb));
}

int D_FUNCTION(Node * Na, Node * Nb)
{
    return (Fixed(Na, Nb) ? 0 : Distance(Na, Nb) * Accuracy) + Na->Pi +  Nb->Pi;
}

//从矩阵中读取权值
int Distance(Node * Na, Node * Nb)
{
    int n = SavedDim;
    if ((Na->Id <= n) == (Nb->Id <= n))
        return M;
    if (abs(Na->Id - Nb->Id) == n)
        return 0;
    return Na->Id < Nb->Id ? Na->Row[Nb->Id - n] : Nb->Row[Na->Id - n];
}


/*
 * InitParameter函数用于初始化启发式算法的相关参数
 */
void InitParameters()
{
    ExchangeType = 5;
    OptimalCost = MINUS_INFINITY;   //初始化最优解
    Accuracy = 100;               // 转化后的内部距离精度,100对应两位小数
    RunTimes = 1;       //运行次数
    RandNum = 1;                //随机数生成器的初始种子
    ExchangeTimes = 0;              //交换次数
    SelectedEdgesNum =50;    //候选边的数量
    Ex = 1.0 / Dim;      //候选边最大的alpha值
    InitIterationTimes = Dim / 2;   //子梯度上升算法中初始的迭代次数
    if (InitIterationTimes < 100)
         InitIterationTimes = 100;
    StepLength = 1;          //初始迭代步长
    MAXWidth = INT_MAX;   //搜索宽度
    MAXSelectedEdgesNum = 5;             //每个节点的最大候选边数量
    MAXExchanges = Dim;   //寻求改进Tour的过程中的最大可交换次数
    MAXTests =Dim;      //程序运行一次允许的最大可交换次数
}
