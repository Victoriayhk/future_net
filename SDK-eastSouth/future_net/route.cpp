#include "route.h"
#include "lib_record.h"
#include "lib_io.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include <iostream>
#include <string>
#include <queue>
#include <time.h>
using namespace std;

//****************************************相关宏定义****************************************//
#define MAXEDGENUM 2000     //一条路径上的最多边数,由于顶点上限为2000,故边数上限为1999
#define INF  9999                           //为了保证路径不经过某条边,可将其权重暂时置为INF
#define INFINITY  999999       //两点之间不存在直接路径时,其距离可表示为INFINITY
#define MAX_PASSNODE_NUM  120     //一条路径上的必经点数量,上限为102,这里设为120留有一定余量
#define MAXRUNTIMES 50                   //程序最多可迭代的次数
#define TIMELIMIT 9800       //程序最长可运行时间,单位为ms,考虑到数据读取和写入文件的时间,这里将其设置为略小于10000
//**********************************全局变量定义************************************************//
EdgeLink TotalLink[MAX_EDGE_NUM]= {0};              //用于存储topo中边的信息的结构数组
bool Visited[MAX_VEX_NUM];                                      //全局数组，记录结点是否已补访问,访问过为true,否则为false
int Par[MAX_VEX_NUM];                                                //每个顶点的父亲节点，可以用于还原最短路径树
node dis[MAX_VEX_NUM];                                            //源点到每个顶点估算距离，最后结果为源点到所有顶点的最短路。
int inbetweenodenum[MAX_VEX_NUM];              //源点到每个顶点的最短路径上经过的中间节点数
priority_queue<node> que;                                   //优先队列,stl实现,用于计算最短路径

int DestinationID;         //终点ID
int SourceID;                 //起点ID
int STEP = 0;                    //调整路径权值时的参数,初步测试该值越小得到的结果越好,但迭代的次数越多,耗时越长
int MAXruntimes = 0;    //程序可运行次数
//**************************************Local function declaration****************************************************//
//基于Dijkstra计算最短路径函数
static void Dijkstra(int source,int *requiredNodeset,int requiredNodeNum, CostMatrix **costmatrix, AdjListGraph G);
//检查单条路径中的重复点
static int checkRepeatedNode(int *Nodeset, int nodeNum, int *repeatedNodeset);
//获取topo图中边的信息
static int  GetTotalLink(char *topo[MAX_EDGE_NUM], int edge_num);
//输入必经点集合(包含起点和终点),求一条经过所有必经点的最短路径
static int FindRoute(AdjListGraph G, int *requiredNodeSet, int requiredNodeNum,  EdgeLink *FinalEdgeID,int *EdgeNum);
//检测两条路径上的重复边,并将其保存在RepeatedEdges集合中
static int CheckRepeatedEdges(EdgeLink *Path[2], int EdgeNum[2], EdgeLink *RepeatedEdges);
//更加通用的检测重复边的函数
static int CheckConflictedEdges(EdgeLink *edgeset1, int EdgeNum1,EdgeLink *edgeset2, int EdgeNum2, EdgeLink *RepeatedEdges);
//调整拓扑图中重复边的权值,逐步增加
static int AdjustTopo(AdjListGraph G, EdgeLink *ConflictedlEdgeSet, int EdgesNum );
//改变拓扑图中边的权值为INF
static int ChangeTopo(AdjListGraph G, EdgeLink *ConflictedlEdgeSet, int EdgesNum );
//恢复topo图的信息
static int ResetTopo(AdjListGraph G, EdgeLink *ConflictedlEdgeSet, int EdgesNum );
//获得重复边权值的平均值
static int getmean(EdgeLink *edge,int edgeNum);

//********************************************print information function for debug*******************************************//
//以邻接表的形式打印topo图的数据
static void printGraph( AdjListGraph G);
//打印必经点信息
static void printDemandSet(int *RequiredNodeSet[2], int *RequiredNodeNum);
//打印边的索引信息
static void printEdgesInfo(char *str, EdgeLink *repeatededgeset, int edgenum);
//打印路径信息
static void printPath(int pathId,EdgeLink *edgearray, int num);

//***********************************************************************************************************************//


void search_route(char *topo[MAX_EDGE_NUM], int edge_num, char *demand[MAX_DEMAND_NUM], int demand_num)
{
    clock_t start_find_time = clock(); //开始计时

    //变量定义
    AdjListGraph G;             //邻接表用于存储图的信息
   int curMinTotalCost = INFINITY;  //当前两条路径的最小权值
   int curMinConflictedEdgesNum = INF; //当前两条路径的最小重复边
    //获取取topo图的信息并存储
    G.VexNum = GetTotalLink(topo, edge_num);  //VexNum表示可能的最大顶点ID,并不一定等于顶点的个数
    G.EdgeNum =  edge_num;            //topo图中的边数

    //邻接表G初始化
    for ( int i = 0; i <=G.VexNum; ++i )
    {
        G.adjList[i].pFirstEdge = NULL;
        G.adjList[i].isRequiredNode = false;
    }

    //将图的信息存入邻接表中
    for(int i = 0; i < edge_num; ++i)
    {
        EdgeNode* pEdgeNode = new EdgeNode;
        pEdgeNode ->adjvex =  TotalLink[i].DestinationID;
        pEdgeNode->cost = TotalLink[i].Cost;
        pEdgeNode->edgeID =  TotalLink[i].LinkID;

        //表头插入法,无法去除重边,当两个顶点之间存在多条直接路径时会出现bug
        pEdgeNode->next = G.adjList[TotalLink[i].SourceID].pFirstEdge ;
        G.adjList[TotalLink[i].SourceID].nodeID = TotalLink[i].SourceID;
        G.adjList[TotalLink[i].SourceID].pFirstEdge  = pEdgeNode;
    }


    //printGraph(G);           //以邻接表的形式打印topo图信息




    //读取必经点信息并存储,需要注意的是,可能路径上没有必经点要求,输入为NA
    int DemandID[2] = {0};           //存储路径编号
    int *RequiredNodeSet[2] = {0};
    int RequiredNodeNum[2] = {0};
    for(int i = 0 ; i < 2; i++)
    {
        int j = 0;
        DemandID[i] = atoi(strtok(demand[i],","));
        RequiredNodeSet[i]  = new int[MAX_PASSNODE_NUM];
        RequiredNodeSet[i][j++] = atoi(strtok(NULL,", "));   //必经点集合下标为0的为起点
        SourceID = RequiredNodeSet[i][0];
        DestinationID = atoi(strtok(NULL,", "));

        char *str;
        char cmpstr[] = {78,65,10};  //"NA\n"
        //char cmpstr1[] = {78,65,13,10};
       // char cmpstr2[] = {78,65,13};
       // char cmpstr3[] = {78,65,10,13};
        str = strtok(NULL,"|,");
//        for(int i = 0; i < strlen(str); i++)
//            printf("%d",str[i]);
//      printf("\n");
        if(strncmp(str,cmpstr,2) == 0)
            //判断是否存在必经点要求
        {
            //printf("hello world\n");
            RequiredNodeNum[i] = 2;
            RequiredNodeSet[i][1] = DestinationID;
        }
        else
        {
            RequiredNodeSet[i][j++] = atoi(str);
            while(str = strtok(NULL,"|, "))
            {
                int VexID = atoi(str);
                RequiredNodeSet[i][j++] = VexID;
            }
            RequiredNodeSet[i][j++] = DestinationID;       //必经点集合最后一个为终点
            RequiredNodeNum[i] = j ;                         //必经顶点的数量包括顶点和终点
        }




//            if((RequiredNodeSet[0][0] != RequiredNodeSet[1][0]) ||
//            (RequiredNodeSet[0][RequiredNodeNum[0]-1] != RequiredNodeSet[1][RequiredNodeNum[1]-1]) )
//            {
//                printf("Error:两条路径必须要有相同的起点和终点!");
//                exit(0);
//            }

    }
    //  printGraph(G);
  //  printDemandSet(RequiredNodeSet,RequiredNodeNum);    //打印必经节点信息



    //变量定义
    EdgeLink *RepeatedEdges = new  EdgeLink[2000];    //一条路径上最多有2000个顶点,因而重复边最多也就是2000条(实际上达不到)
    int RepeatedEdgesNum  = 0;         //重复边的数量
    int *cost = new int[2];                    //两条路径的权值分别为cost[0],cost[1]
    EdgeLink *FinalEdgeID[2];              //保存最终的路径信息
    for(int i = 0; i < 2; i++)
    {
        FinalEdgeID[i] = new EdgeLink[MAXEDGENUM];
    }
    int EdgeNum[2] = {0};                  //每条路径上的边数初始化

    //根据两条路径上上的必经点要求的不同情况采用不同的方法处理
    if((RequiredNodeNum[0] ==2)&&(RequiredNodeNum[1] == 2) )                //两条路径均没有必经点要求
    {
        int Path1cost = FindRoute(G, RequiredNodeSet[0], RequiredNodeNum[0], FinalEdgeID[0],&EdgeNum[0]);  //寻找一条最短路径,之后保证不经过这条路径上所有边的基础上再找一条最短路径
        ChangeTopo(G,FinalEdgeID[0],EdgeNum[0]);
        int Path2cost = FindRoute(G, RequiredNodeSet[1], RequiredNodeNum[1], FinalEdgeID[1],&EdgeNum[1]);
        for(int j = 0; j < EdgeNum[0]; j++)
            record_result(WORK_PATH,FinalEdgeID[0][j].LinkID);
        for(int j = 0; j < EdgeNum[1]; j++)
            record_result(BACK_PATH,FinalEdgeID[1][j].LinkID);
    }
    else
    {

/*************************************************第一步****************************************************
    先在一条路径最优的情况下,保证另一条路径不经过最优路径上的所有边,在此基础上计算另一条
    路径,由于最优路径可以选择第一条或是第二条,因而就会出现两种情况,可以选择结果较优一种情况输出.
    这样做的的好处时在不增加迭代次数的基础上既能够找出必经重复边,又能够得到一种重复边最少的解,
    保证在大规模例子计算耗时较长时为防止超时直接跳出下面第二步的迭代计算,保证在任何情况下都能有解输出
************************************************************************************************************/
            EdgeLink *FinalEdgeID1[2];    //保存第一种情况得到的路径信息
            EdgeLink *FinalEdgeID2[2];    //保存第二种情况得到的路径信息
            EdgeLink *repeatededgsset[2]; //保存两种情况下得到重复边,这些重复边分别是path2和path1的必经边
            int repeatededgenum[2] = {0};  //保存两种情况下的重复边的数量
            for(int i = 0; i < 2; i++)
            {
                repeatededgsset[i] = new  EdgeLink[MAXEDGENUM];
                FinalEdgeID1[i] = new EdgeLink[MAXEDGENUM];
                FinalEdgeID2[i] = new EdgeLink[MAXEDGENUM];
            }
            int EdgeNum1[2] = {0};
            int EdgeNum2[2] = {0};


//***************************第一种情况:计算path1时没有约束,计算path2时,保证尽可能少地不经过path1中的边**************************//
            //计算路径1
            int Path1cost1 = FindRoute(G, RequiredNodeSet[0], RequiredNodeNum[0], FinalEdgeID1[0],&EdgeNum1[0]);
            //printf("%d\n",Path1cost1);
            int Path2cost1 = FindRoute(G, RequiredNodeSet[1], RequiredNodeNum[1], FinalEdgeID2[1],&EdgeNum2[1]);
            int CriticalEdgesNum = CheckConflictedEdges(FinalEdgeID1[0],EdgeNum1[0],FinalEdgeID2[1],EdgeNum2[1],RepeatedEdges);
         //   printEdgesInfo("初始重复边集合:",RepeatedEdges,CriticalEdgesNum);
            if(CriticalEdgesNum != 0)              //算法得到两条路径后,如果无重边直接输出
            {


            //将路径1上所有边的权值加上INF
            ChangeTopo(G,FinalEdgeID1[0],EdgeNum1[0]);
            //计算路径2
            int Path2cost2 = FindRoute(G, RequiredNodeSet[1], RequiredNodeNum[1], FinalEdgeID1[1],&EdgeNum1[1]);
            //恢复拓扑图
            ResetTopo(G,FinalEdgeID1[0],EdgeNum1[0]);
            //检测结果中的重复边,这些边也就是path2上的必经边
             repeatededgenum[0] = CheckRepeatedEdges(FinalEdgeID1,EdgeNum1,repeatededgsset[0]);
             //计算该种情况下的总权值totalcost1,只需要减去INF*重复边条数即可
            int totalcost1 = Path1cost1 + Path2cost2;
           // printEdgesInfo("path2上的必经边:",repeatededgsset[0],repeatededgenum[0]);

//****************************第二种情况:计算path2时没有约束,计算path1时,保证尽可能少地不经过path2中的边************************//
            //计算路径2

            //将路径2上所有的边的权值加上INF
            ChangeTopo(G,FinalEdgeID2[1],EdgeNum2[1]);
            //计算路径1
            int Path1cost2 = FindRoute(G, RequiredNodeSet[0], RequiredNodeNum[0], FinalEdgeID2[0],&EdgeNum2[0]);
            //恢复拓扑图
            ResetTopo(G,FinalEdgeID2[1],EdgeNum2[1]);
            //ResetTopo(G,FinalEdgeID1[1],EdgeNum2[1]);
            //检测结果中的重复边,这些边也就是path1上的必经边
             repeatededgenum[1] = CheckRepeatedEdges(FinalEdgeID2,EdgeNum2,repeatededgsset[1]);
             //计算该种情况下的总权值totalcost1,只需要减去INF*重复边条数即可
            int totalcost2 = Path2cost1 + Path1cost2 ;
          //  printEdgesInfo("path1上的必经边:",repeatededgsset[1],repeatededgenum[1]);


            //输出两种情况下的路径情况对比
//            printf("不考虑重复边情况时两条路径的最小权值: %d\n",Path1cost1 + Path2cost1);
//            printf("路径1优先时两条路径总权值: %d\nrepeatededgesnum:%d\n",totalcost1,repeatededgenum[0]);
//            printf("路径2优先时两条路径总权值: %d\nrepeatededgesnum:%d\n",totalcost2,repeatededgenum[1]);

            //比较两种情况下的结果优劣,暂时选取较好的一种保存结果
        EdgeLink **EdgeID;
        int *Num;
        if(repeatededgenum[0] == repeatededgenum[1])
        {
            curMinConflictedEdgesNum = repeatededgenum[0];
            if(totalcost1> totalcost2)
            {
                curMinTotalCost = totalcost2;
          //      printf("second is better\n");
                EdgeID = FinalEdgeID2;
                Num = EdgeNum2;
            }
            else
            {
                curMinTotalCost = totalcost1;
         //       printf("first is better\n");
                EdgeID = FinalEdgeID1;
                Num = EdgeNum1;
            }
        }
        else if(repeatededgenum[0] < repeatededgenum[1])
        {
             curMinTotalCost = totalcost1;
             curMinConflictedEdgesNum = repeatededgenum[0];
         //     printf("first is better\n");
                EdgeID = FinalEdgeID1;
                Num = EdgeNum1;
        }
        else
        {
            curMinTotalCost = totalcost2;
            curMinConflictedEdgesNum = repeatededgenum[1];
      //       printf("second is better\n");
                EdgeID = FinalEdgeID2;
                Num = EdgeNum2;
        }
            //保存结果到result.csv中
            for(int j = 0; j < Num[0]; j++)
                record_result(WORK_PATH,EdgeID[0][j].LinkID);
            for(int j = 0; j < Num[1]; j++)
                record_result(BACK_PATH,EdgeID[1][j].LinkID);


/****************************************第一步结束*****************************************************/


/************************************************第二步************************************************
在第一步的基础上,先将必经重复边排除在重复边集合,对非必经重复边进行分类,找出他们所属的路径
*******************************************************************************************************/


        int *cost = new int[2];              //存储路径权值

    //    CriticalEdgesNum = CheckConflictedEdges(FinalEdgeID1[0],EdgeNum1[0],FinalEdgeID2[1],EdgeNum2[1],RepeatedEdges);
    //    if(CriticalEdgesNum != 0)         //如果结果有重边,继续下面的计算,否则直接输出结果

            EdgeLink *CriticalEdges = new  EdgeLink[CriticalEdgesNum];         //初始的重复边集合
      //      printf("初始重复边:");
            for(int i = 0; i <CriticalEdgesNum; i++)
            {
                CriticalEdges[i] = RepeatedEdges[i] ;
            //    printf("%d  ",CriticalEdges[i].LinkID);
            }
      //      printf("\n");


/**************************确定必经重复边,并将其交换至重复边末尾,置其demandID为相应的PathID**************************/
            int vitaledgescount = 0;  //必经重复边的条数
            for(int i = 0; i <2; i++)
            {
                for(int j = 0; j < repeatededgenum[i]; j++)
                {
                    for(int k = 0; k < CriticalEdgesNum; k++)
                    {
                        if(repeatededgsset[i][j].LinkID == CriticalEdges[k].LinkID)
                        {
                            if(CriticalEdges[k].demandID == 0)
                            {
                                //交换必经边至末尾
                                CriticalEdges[k].demandID = i +1;
                                EdgeLink temp;
                                temp = CriticalEdges[k];
                                CriticalEdges[k] = CriticalEdges[CriticalEdgesNum-1-vitaledgescount];
                                CriticalEdges[CriticalEdgesNum-1-vitaledgescount] = temp;
                                vitaledgescount++;
                                //   printf("%d  ",FinalEdgeID[i][j].LinkID)
                            }                                                                   //如果该重复边既是path1的必经边,又是path2的必经边,此时虽然我们将其归为
                            //  path2,但由于其为必经边,因而将其权值置为9999后,path1仍然会通过,对最终的结果无影响

                        }
                    }
                }

            }
//            printf("不包括必经重复边的重复边集合:");
//            for(int i = 0; i <CriticalEdgesNum - vitaledgescount; i++)
//            {
//                printf("%d  ",CriticalEdges[i].LinkID);
//                RepeatedEdges[i] = CriticalEdges[i];
//            }
//            printf("\n");


        clock_t prefind_end_time = clock();
        double prefindtime = static_cast<double>(prefind_end_time - start_find_time)/CLOCKS_PER_SEC*1000;
//         printf("第一步处理结束运行时间:%f ms\n",prefindtime);


         //超时控制,确定最大可循环次数
         if((TIMELIMIT - prefindtime*2) < 0)
             goto END_FIND;
        else
        {
            MAXruntimes = int((TIMELIMIT - prefindtime*2)*2/prefindtime);
 //           printf("最大可循环次数%d\n",MAXruntimes);
            if(MAXruntimes == 0)
                MAXruntimes = 1;
        }

            //确定非必经重复边分属于哪条路径
            CriticalEdgesNum = CriticalEdgesNum - vitaledgescount;
            /*************************************************************************************************/
            int runtimes = 0;  //当前循环次数
            RepeatedEdgesNum = CriticalEdgesNum;
            STEP = 0;
            int k = 0;
            while(RepeatedEdgesNum&&(runtimes < MAXruntimes))
            {
                runtimes++;
                STEP += getmean(RepeatedEdges,RepeatedEdgesNum) ;
              //   STEP += 1;
       //         printf("STEP = %d\n",STEP);
                k = 0;
                AdjustTopo(G,CriticalEdges,CriticalEdgesNum);
                //       AdjustTopo(G,RepeatedEdges,RepeatedEdgesNum);
                for(int i = 0; i < 2; i++)
                    cost[i] = FindRoute(G, RequiredNodeSet[i], RequiredNodeNum[i], FinalEdgeID[i],&EdgeNum[i]);
                RepeatedEdgesNum = CheckRepeatedEdges(FinalEdgeID, EdgeNum, RepeatedEdges);
                for(int t = 0; t < 2; t++)
                {
                    for(int i = 0; i < EdgeNum[t]; i++)
                    {

                        for(int j = 0; j < CriticalEdgesNum; j++)
                        {
                            if(FinalEdgeID[t][i].LinkID == CriticalEdges[j].LinkID)
                            {
                                //k++;
                                CriticalEdges[j].demandID = t+1;                   //重复边出现在哪条路径上,就将该重复边的DemandID置为对应值,1 for path1,2 for path2

                                //	printf("%d  ",RepeatedEdges[i].LinkID);
                            }
                        }
                    }
                }

      //          printf("更新的重复边集合:");
                for(int i = 0; i < RepeatedEdgesNum; i++)
                {

                    for(int j = 0; j < CriticalEdgesNum; j++)
                    {
                        if(RepeatedEdges[i].LinkID == CriticalEdges[j].LinkID)
                        {
                            k++;
                            CriticalEdges[j].demandID = 3;              //如果该重复边出现在两条路径上,将其置为3,表示其仍为重复边
      //                      printf("%d  ",RepeatedEdges[i].LinkID);
                        }
                    }
                }
     //           printf("\n");

                RepeatedEdgesNum = k;

            }

            CriticalEdgesNum = CriticalEdgesNum + vitaledgescount;
            EdgeLink *edgsBelongtoPath1 = new EdgeLink[CriticalEdgesNum];
            EdgeLink *edgsBelongtoPath2 = new EdgeLink[CriticalEdgesNum];
            int path1num = 0,path2num = 0;


            //将重复边分配到两条路径上

            for(int j = 0; j < CriticalEdgesNum; j++)
            {
                if(CriticalEdges[j].demandID == 1)
                {
   //                 printf("egde %d belong to path1\n",CriticalEdges[j].LinkID);
                    edgsBelongtoPath1[path1num++] = CriticalEdges[j];
                }

                else if(CriticalEdges[j].demandID == 2)
                {
  //                  printf("egde %d belong to path2\n",CriticalEdges[j].LinkID);
                    edgsBelongtoPath2[path2num++] = CriticalEdges[j];
                }

//                else
//                    printf("edge %d can be on either path\n",CriticalEdges[j].LinkID);
            }

            //跳出循环后再次计算剩余时间
            clock_t finish_loop_time = clock();
             double looptime = static_cast<double>(finish_loop_time - start_find_time)/CLOCKS_PER_SEC*1000;
 //           printf("结束循环时程序运行时间:%f ms\n",looptime);
            double avai_time = TIMELIMIT - looptime;
            if(avai_time < prefindtime/2)
                goto END_FIND;
//按两种方式计算最终得到的路径
            //必须先要还原topo图数据
            ResetTopo(G,CriticalEdges,CriticalEdgesNum);
//方法一:优先计算路径2,之后计算路径1

            ChangeTopo(G,edgsBelongtoPath1,path1num);
            //   printGraph(G);
            int path2cost1 = (FindRoute(G,RequiredNodeSet[1],RequiredNodeNum[1],FinalEdgeID[1],&EdgeNum[1]));
            ResetTopo(G,edgsBelongtoPath1,path1num);
            ChangeTopo(G,FinalEdgeID[1],EdgeNum[1]);
            int path1cost1 = FindRoute(G,RequiredNodeSet[0],RequiredNodeNum[0],FinalEdgeID[0],&EdgeNum[0]);
            ResetTopo(G,FinalEdgeID[1],EdgeNum[1]);
            int conflictededgesNum1 = CheckRepeatedEdges(FinalEdgeID, EdgeNum, RepeatedEdges);
//            printf("第1种情况下重复边条数= %d\n",conflictededgesNum1);
            int newtotalcost1 = path2cost1 + path1cost1;
//            printf("第1种情况下权值: %d + %d =%d\n",path2cost1,path1cost1,newtotalcost1);
//            for(int i = 0; i < 2; i++)
//                printPath(i+1,FinalEdgeID[i],EdgeNum[i]);
            //保存路径信息&&(curMinTotalCost >= newtotalcost1)
            if((conflictededgesNum1 < curMinConflictedEdgesNum) )
            {
            curMinConflictedEdgesNum = conflictededgesNum1;
            curMinTotalCost = newtotalcost1;
             clear_result();
            for(int j = 0; j < EdgeNum[0]; j++)
                record_result(WORK_PATH,FinalEdgeID[0][j].LinkID);
            for(int j = 0; j < EdgeNum[1]; j++)
                record_result(BACK_PATH,FinalEdgeID[1][j].LinkID);
            }
            else if(conflictededgesNum1 == curMinConflictedEdgesNum)
            {
                if(curMinTotalCost > newtotalcost1)
                {
                    curMinTotalCost = newtotalcost1;
                    clear_result();
                    for(int j = 0; j < EdgeNum[0]; j++)
                        record_result(WORK_PATH,FinalEdgeID[0][j].LinkID);
                    for(int j = 0; j < EdgeNum[1]; j++)
                        record_result(BACK_PATH,FinalEdgeID[1][j].LinkID);
                }

            }


//方法二:优先计算路径1,之后计算路径2
             clock_t current_time = clock();
             double sparetime = static_cast<double>(current_time - start_find_time)/CLOCKS_PER_SEC*1000;
             sparetime = TIMELIMIT - sparetime;
  //          printf("程序剩余可执行时间:%f ms\n",sparetime);
            if(sparetime > prefindtime/2)
          {
            ChangeTopo(G,edgsBelongtoPath2,path2num);
            int path2cost2 = (FindRoute(G,RequiredNodeSet[0],RequiredNodeNum[0],FinalEdgeID[0],&EdgeNum[0]));
            ResetTopo(G,edgsBelongtoPath2,path2num);
            ChangeTopo(G,FinalEdgeID[0],EdgeNum[0]);
            int path1cost2 = (FindRoute(G,RequiredNodeSet[1],RequiredNodeNum[1],FinalEdgeID[1],&EdgeNum[1]));
            ResetTopo(G,FinalEdgeID[0],EdgeNum[0]);
            int conflictededgesNum2 = CheckRepeatedEdges(FinalEdgeID, EdgeNum, RepeatedEdges);
   //         printf("第2种情况下的重复边条数 = %d\n",conflictededgesNum2);
            int newtotalcost2 = path2cost2 + path1cost2;
   //         printf("第2种情况下的权值 %d + %d = %d\n",path2cost2,path1cost2,newtotalcost2);
//            for(int i = 0; i < 2; i++)
//                printPath(i+1,FinalEdgeID[i],EdgeNum[i]);
            //保存路径信息
            if((conflictededgesNum2 < curMinConflictedEdgesNum) )
            {
            curMinConflictedEdgesNum = conflictededgesNum2;
            curMinTotalCost = newtotalcost2;
             clear_result();
            for(int j = 0; j < EdgeNum[0]; j++)
                record_result(WORK_PATH,FinalEdgeID[0][j].LinkID);
            for(int j = 0; j < EdgeNum[1]; j++)
                record_result(BACK_PATH,FinalEdgeID[1][j].LinkID);
            }
            else if(conflictededgesNum2== curMinConflictedEdgesNum)
            {
                if(curMinTotalCost > newtotalcost2)
                {
                    curMinTotalCost = newtotalcost2;
                    clear_result();
                    for(int j = 0; j < EdgeNum[0]; j++)
                        record_result(WORK_PATH,FinalEdgeID[0][j].LinkID);
                    for(int j = 0; j < EdgeNum[1]; j++)
                        record_result(BACK_PATH,FinalEdgeID[1][j].LinkID);
                }

            }

//如果第二种情况下的权值更小,清除第一次的结果,保存第二种情况下的路径信息
            if(conflictededgesNum2 <= conflictededgesNum1)
            {
                if((path2cost2 + path1cost2) < (path2cost1 + path1cost1))
                {
                    clear_result();
                    for(int j = 0; j < EdgeNum[0]; j++)
                        record_result(WORK_PATH,FinalEdgeID[0][j].LinkID);
                    for(int j = 0; j < EdgeNum[1]; j++)
                        record_result(BACK_PATH,FinalEdgeID[1][j].LinkID);
                }
            }
          }

            /************************************************************************************************/
        }
        else
        {
         //     printf("不考虑重复边情况时两条路径的最小权值: %d\n",Path1cost1 + Path2cost1);
            clear_result();
            for(int j = 0; j < EdgeNum1[0]; j++)
                record_result(WORK_PATH,FinalEdgeID1[0][j].LinkID);
            for(int j = 0; j < EdgeNum2[1]; j++)
                record_result(BACK_PATH,FinalEdgeID2[1][j].LinkID);
        }

    }
END_FIND:
          clock_t finish_find_time = clock();
          double findtime = static_cast<double>(finish_find_time - start_find_time)/CLOCKS_PER_SEC*1000;
         printf("search_route函数运行总时间:%f ms\n",findtime);

             printf("totalcost:%d\n",curMinTotalCost);
             printf("repeatededgesnum:%d\n",curMinConflictedEdgesNum);


}


//读取topo图信息,存储在TotalLink数组中
static int GetTotalLink(char *topo[MAX_EDGE_NUM], int edge_num)//已经修改过
{
    int maxNodeID = 0;             //有出度的顶点的最大ID
    for (int m=0; m<edge_num; m++)
    {
        TotalLink[m].LinkID=atoi(strtok(topo[m],", "));         //有向边索引号
        TotalLink[m].SourceID=atoi(strtok(NULL,", "));            //有向边起点
        TotalLink[m].DestinationID=atoi(strtok(NULL,", "));    //有向边终点
        TotalLink[m].demandID = 0;                                 //初始状态有向边不属于任何路径

        //判断当前边是否和法,即起点是否等于终点,不合法输出提示信息,最后可以去掉
        if( TotalLink[m].SourceID ==TotalLink[m].DestinationID)
              printf("edge[%d] is illegal!\n",m);
        TotalLink[m].Cost=atoi(strtok(NULL,", "));      //有向边权值

        //找到最大的顶点ID
        if(TotalLink[m].SourceID > maxNodeID)
            maxNodeID =  TotalLink[m].SourceID;
        if(TotalLink[m].DestinationID > maxNodeID)
            maxNodeID =  TotalLink[m].DestinationID;
    }
    return maxNodeID;
}



//Dijkstra算法，传入源顶点,图的邻接表存储,必经点的集合,必经点的数目,返回路径及权值信息

static void Dijkstra(int source,int *requiredNodeset,int requiredNodeNum, CostMatrix **costmatrix, AdjListGraph G)
{
    //在邻接表中设置必经点标志,遍历时禁止这些顶点
    for(int i = 0; i < requiredNodeNum; ++i)
    {
        G.adjList[requiredNodeset[i]].isRequiredNode  = true;
    }
    G.adjList[source].isRequiredNode = false;
    for(int i = 0; i <=G.VexNum ; i++) //初始化   ,后续需要修改
    {
        dis[i].ID = G.adjList[i].nodeID;
        dis[i].w = INFINITY;           //估算距离置INF
        Par[i] = -1;         //每个顶点都无父亲节点
        Visited[i] = false;     //都未找到最短路
        inbetweenodenum[i] = 0; //源点到每个顶点的中间节点数均为0
    }
    dis[source].w = 0;                 //源点到源点最短路权值为0
    que.push(dis[source]);               //压入队列中

    while(!que.empty())           //算法的核心，队列空说明完成了操作
    {
        node CurrentNode = que.top();      //取最小估算距离顶点
        que.pop();
        int u = CurrentNode.ID;
        if(Visited[u])                 //若访问过,结束本次循环
            continue;
        Visited[u] = true;
        EdgeNode * p = G.adjList[u].pFirstEdge;
        if(G.adjList[u].isRequiredNode) //如果当前点是必经点,不再访问该节点的邻接顶点
        {
            p = NULL;
        }

        //松弛操作
        while((p != NULL) )    //找所有与他相邻的顶点，进行松弛操作，更新估算距离，压入队列。
        {
            int v = p->adjvex;
            if((!Visited[v] ) && (dis[v].w > dis[u].w+p->cost))
            {
                dis[v].w = dis[u].w+p->cost;
                Par[v] = u;
                inbetweenodenum[v] = inbetweenodenum[u] + 1;   //u是v的父节点,故源点到v的中间节点数比到u的中间节点数多1
                que.push(dis[v]);
            }
            p = p->next;
        }
    }
}

//给定一个数组检查数组中是否有重复的元素,并将重复的元素保存到repeatedNodeset中,返回值为重复元素的个数
int checkRepeatedNode(int *Nodeset, int nodeNum, int *repeatedNodeset)
{
    bool repeatedNode = false; //是否重复标志
    int count = 0; //记录重复的点数
    int *checkNodeset  = new int[nodeNum];
    //将Nodeset拷贝至checkNode中
    for(int  i = 0; i < nodeNum  ; i++)
    {
        checkNodeset[i]  = Nodeset[i];
        //     printf("%d  ",Nodeset[i]);
    }
//printf("\n");
    //查找重复的点
    for(int  i = 0; i < nodeNum -1; i++)
    {
        for(int j = i+1; j < nodeNum ; j++)
        {
            if((checkNodeset[i]  == checkNodeset[j] ) &&(checkNodeset[i] != -1))
            {
                repeatedNode = true;
                checkNodeset[j] = -1;
            }
        }
        if(repeatedNode)
        {
            repeatedNodeset[count++] =  checkNodeset[i];
            repeatedNode = false;
        }
    }
    delete []checkNodeset;
    return count;
}

//给定必经点集合找最短路径
static int FindRoute(AdjListGraph G, int *requiredNodeSet, int requiredNodeNum, EdgeLink *FinalEdgeID,int *EdgeNum)
{
  //  clock_t start_time = clock();

    int TotalCost = 0;                       //路径总权值
    int repeatedNodenum = 0;  //重复顶点的数量
    int totalNodeNum = 1;  //最终路径经过的点数,考虑到计算时未考虑终点,此处应初始化为1
    int *finalNodePath = NULL ;   //保存最终的路径信息
    int *repeatedNodeset = NULL; //保存重复节点的数组
    int *oldRequiredNodeset = NULL;

    //将必经点信息拷贝至newRequiredNodeset数组中
    int *newRequiredNodeset  =  new int[requiredNodeNum];
    for(int i = 0; i < requiredNodeNum; i++)
        newRequiredNodeset[i] = requiredNodeSet[i];



    do
    {
        //初始化过程
        totalNodeNum = 1;
        CostMatrix  **costmatrix = new CostMatrix *[requiredNodeNum +1];  //矩阵维数为(requiredNodeNum +1)X(requiredNodeNum +1)维,需要加入虚拟点
        for(int i= 0; i <= requiredNodeNum; i++)
        {
            costmatrix[i] =  new CostMatrix [requiredNodeNum +1];
            for(int j = 0; j <= requiredNodeNum; j++)
            {
                costmatrix[i][j].cost  =  INFINITY;
                costmatrix[i][j].nodenum = 0;
                //       costmatrix[i][j].nodepath = new int [50];          //可能会有问题的
            }

        }

        for(int i = 0; i < requiredNodeNum ; i ++)
        {
            Dijkstra(newRequiredNodeset[i],newRequiredNodeset,requiredNodeNum, costmatrix,  G);
            for(int j= 0; j< requiredNodeNum; j++)
            {
                costmatrix[i][j].cost = dis[newRequiredNodeset[j]].w;
                //      costmatrix[i][j].nodenum = inbetweenodenum[requiredNodeSet[j]];
                costmatrix[i][j].nodepath = new int[inbetweenodenum[newRequiredNodeset[j]]];   //根据中间节点的数目分配内存

                //获取节点requiredNodeset[i] requiredNodeset[j]最短路径上的中间节点,即以顶点表示的路径信息
                int  par = Par[newRequiredNodeset[j]];
                while(par != -1)
                {
                    costmatrix[i][j].nodepath[costmatrix[i][j].nodenum++]  = par;
                    par = Par[par];

                }                                     //反向输出路径,包括起点,不包括终点
                //      printf("%d:%d   ",costmatrix[i][j].nodenum,inbetweenodenum[newRequiredNodeset[j]]);
            }
            //     printf("\n");
        }
        //加入虚拟点的路径信息,虚拟点到起点距离为0,终点到虚拟点距离为0,其它均设为INFINITY
        for(int i =0; i<= requiredNodeNum; ++i)
        {
            costmatrix[i][requiredNodeNum].cost  =  INFINITY;
            costmatrix[i][requiredNodeNum].nodenum = 0;
            costmatrix[i][requiredNodeNum].nodepath = NULL;
        }
        for(int i =0; i<= requiredNodeNum; ++i)
        {
            costmatrix[requiredNodeNum][i].cost  =  INFINITY;
            costmatrix[requiredNodeNum][i].nodenum = 0;
            costmatrix[requiredNodeNum][i].nodepath = NULL;
        }
        costmatrix[requiredNodeNum][0].cost  = 0;
        costmatrix[requiredNodeNum-1][requiredNodeNum].cost  = 0;


        int   **weightmatrix = new int *[requiredNodeNum+1];
        for(int i = 0 ; i <= requiredNodeNum; i ++)
        {
            weightmatrix[i]  = new int[requiredNodeNum+1];
            for(int j = 0; j <= requiredNodeNum; j++)
            {
                weightmatrix[i][j] =costmatrix[i][j].cost;
              //  printf("%8d",weightmatrix[i][j]);
            }
           //     printf("\n");
        }
        int  *NodePath = new int[requiredNodeNum+1];   //申请一个一维数组,用来存储LKHmain得到的路径信息,不存储虚拟点,第一个对应起点,最后一个对应终点

        //      printf("dijastra Running time is:%f ms\n",runtime);
        if(requiredNodeNum != 2)
        {
            TotalCost =FindOptimalPath(NodePath,requiredNodeNum +1, weightmatrix);
        }
        else
        {
            for(int i = 0; i < 3; i++)
                NodePath[i] = i;
            TotalCost = weightmatrix[0][1];
        }




       if(TotalCost > INFINITY)
                  break;
        for(int i = 0; i < requiredNodeNum -1; i++)
        {
            totalNodeNum += costmatrix[NodePath[i]][NodePath[i+1]].nodenum;

        }
//      printf("nodepath is:");
//       for(int i = 0; i < requiredNodeNum; i++)
//            printf("NodePath[%d]:%d->",NodePath[i],requiredNodeSet[NodePath[i]]);
//            printf("requiredNodeset[%d]:%d  ",i,newRequiredNodeset[i]);
//       printf("\n");
//       printf("requiredNodenum:%d\n",requiredNodeNum);
        finalNodePath = new int [totalNodeNum];
        int k =0;
        for(int i = 0; i < requiredNodeNum -1; ++i)
        {
            for(int j = (costmatrix[NodePath[i]][NodePath[i+1]].nodenum - 1) ; j >= 0; j--)
            {
                finalNodePath[k++]  = costmatrix[NodePath[i]][NodePath[i+1]].nodepath[j];

            }
        }
        finalNodePath[k]  = DestinationID;
//       printf("nodepath is:");
//       for(int i = 0; i < totalNodeNum; i++)
//            printf("%d->",finalNodePath[i]);
//       printf("\n");
        //检查是否有重复经过的点,如果有,将其加入到requiredNodeset中继续循环迭代,如果没有则输出结果
        repeatedNodeset  =  new int[totalNodeNum/2];    //申请数组用于存储重复点
        repeatedNodenum = checkRepeatedNode(finalNodePath, totalNodeNum, repeatedNodeset);
        //判断是否有重复点,有的话增加需要进过的点,并将重复的点加入需经点的集合中
        if(repeatedNodenum)
        {
            requiredNodeNum += repeatedNodenum;   //增加需要经过的点数
            oldRequiredNodeset =  new int[requiredNodeNum];   //重新申请存储空间
            for(int i = 0; i < requiredNodeNum -1; i++)
            {
                oldRequiredNodeset[i]  =  	(i  < (requiredNodeNum - repeatedNodenum -1))  ?  newRequiredNodeset[i] : repeatedNodeset[i + repeatedNodenum + 1 -requiredNodeNum] ;
            }
            oldRequiredNodeset[requiredNodeNum -1]  = newRequiredNodeset[requiredNodeNum -repeatedNodenum -1];//起点为第一个,终点为最后一个
            newRequiredNodeset = oldRequiredNodeset;
        }


    }
    while(repeatedNodenum);
    //跳出循环,说明找到了解,可以输出最终的路径信息了
    int j = 0;
    //此处将TotalCost 重新置0,重新计算权值
   //
    TotalCost = 0;
//    if(TotalCost < INFINITY)
 //   {
    for(int i = 0 ; i < totalNodeNum -1; ++i)
    {
        int mincost = 100000000;  //
        EdgeNode *p = G.adjList[finalNodePath[i]].pFirstEdge;

        //先找出最小权值的边的cost
        while(p != NULL)
        {
            if(p -> adjvex == finalNodePath[i+1])
            {
                if(mincost > p ->cost)
                    mincost = p ->cost;
            }
            p = p -> next;    //没找到继续查找
        }

        p = G.adjList[finalNodePath[i]].pFirstEdge;
        while(p != NULL)
        {
            if(p -> adjvex == finalNodePath[i+1] && p->cost == mincost)
            {
                //record_result(p ->edgeID,totalNodeNum - 1);   //将结果写入最终的文件中
                FinalEdgeID[j].LinkID = p ->edgeID;
                FinalEdgeID[j].SourceID = finalNodePath[i];
                FinalEdgeID[j].DestinationID = p -> adjvex;
                FinalEdgeID[j].Cost = p -> cost;
                FinalEdgeID[j].demandID = 0;
               TotalCost +=( (p ->cost < INF)? p ->cost:p ->cost%INF);   //如果权值大于INF,其实际权值对INF取余
                j++;
                //       printf()
                break;                  //找到边的信息后跳出循环
            }
            else
                p = p -> next;    //没找到继续查找
        }
        if(i != j -1)
        {
            printf("ERROR:%d->%d edge does not exist!\n",finalNodePath[i],finalNodePath[i+1]);
            j++;
        }

    }

//   }
     if(j != totalNodeNum -1)    //如果j != Totalnodenum -1 说明路径中存在无效边,找出的路径不合法
     {printf("the path is invalid!");
       exit(0);
     }
    *EdgeNum = totalNodeNum -1;
//    printf("j = %d totalnodenum = %d\n",j,totalNodeNum);
//    printf("TotalEdges:%d\n",totalNodeNum-1);
//    printf("TotalCost:%d\n",TotalCost);
//   repeatedNodenum = checkRepeatedNode(finalNodePath, totalNodeNum, repeatedNodeset);
//   printf("repeatednodenum:%d\n",repeatedNodenum);
    //输出路径信息,以边表示

//     printf("pathID:");
//  for(int i = 0; i < totalNodeNum -1; i++ )
//    {
//        printf("%d:%d|",FinalEdgeID[i].LinkID,FinalEdgeID[i].Cost);
//    }
//    printf("\n");


////输出路径信息,以顶点表示
//    printf("nodeID:");
//    for(int i = 0; i < totalNodeNum ; i++ )
//        printf("%d->",finalNodePath[i]);
//    printf("\n");

//    clock_t end_time = clock();
//    double runtime = static_cast<double>(end_time - start_time)/CLOCKS_PER_SEC*1000;
//    printf("runtime is:%f ms\n",runtime);
    return TotalCost;        //返回路径权值


}

//检测Path1和path2是否有重边
static int CheckRepeatedEdges(EdgeLink *Path[2], int EdgeNum[2], EdgeLink *RepeatedEdges)
{
    int i =0;
    int j = 0;
    int k = 0;
//printEdgesInfo("path",Path[0],EdgeNum[0]);
    for(i = 0; i < EdgeNum[0]; i++)
        for(j = 0; j < EdgeNum[1]; j++)
        {
            if(Path[0][i].LinkID ==Path[1][j].LinkID)
            {
                RepeatedEdges[k].LinkID = Path[0][i].LinkID;
                RepeatedEdges[k].SourceID = Path[0][i].SourceID;
                RepeatedEdges[k].DestinationID = Path[0][i].DestinationID;
                RepeatedEdges[k].Cost = Path[0][i].Cost;
                RepeatedEdges[k].demandID = 0;
                //             printf("path[0][%d]==path[1][%d]:%d\n",i,j,Path[0][i].LinkID);
                k++;
                break;
            }
        }
    return k;
}

//该函数用于将冲突边的权值调整为INF =9999,确保计算路径时不再经过这些边,如
static int AdjustTopo(AdjListGraph G, EdgeLink *ConflictedlEdgeSet, int EdgesNum )
{
    EdgeNode *p = NULL;
    int count = 0;
    for(int i = 0; i < EdgesNum; i++)
    {
        p = G.adjList[ConflictedlEdgeSet[i].SourceID].pFirstEdge;
        while(p != NULL)
        {
            if(p->edgeID ==ConflictedlEdgeSet[i].LinkID)
            {
                //     p->cost = INF;
                p -> cost = ConflictedlEdgeSet[i].Cost + STEP;
                count++;
                break;
            }
            p = p ->next;
        }
    }
    return count;
}

//将改动的边重置到初始状态
static int ResetTopo(AdjListGraph G, EdgeLink *ConflictedlEdgeSet, int EdgesNum )
{
    EdgeNode *p = NULL;
    int count = 0;
    for(int i = 0; i < EdgesNum; i++)
    {
        p = G.adjList[ConflictedlEdgeSet[i].SourceID].pFirstEdge;
        while(p != NULL)
        {
            if(p->edgeID ==ConflictedlEdgeSet[i].LinkID)
            {
                p->cost = ConflictedlEdgeSet[i].Cost;
                count++;
                break;
            }
            p = p ->next;
        }
    }
    return count;
}

static int getmean(EdgeLink *edge,int edgeNum)
{
    int sum = 0;
    for(int i = 0; i < edgeNum; i++)
    {
        sum += edge[i].Cost;
    }
    return sum/edgeNum;
}


//打印存储的邻接链表信息
static void printGraph( AdjListGraph G)
{

    printf("the adjList of the Graph:\n");
    for(int i = 0; i <=G.VexNum; i++)
    {
        EdgeNode *pEdgeNode = G.adjList[i].pFirstEdge;
        while(pEdgeNode)
        {
            //     printf("index:%d  %d-->%d:%d ",pEdgeNode->edgeID,G.adjList[i].nodeID, pEdgeNode -> adjvex,pEdgeNode ->cost);
            printf("%d-->%d  ",G.adjList[i].nodeID, pEdgeNode -> adjvex);
            pEdgeNode  = pEdgeNode -> next;
        }
        printf("\n");

    }
}

//*打印必经节点信息*/
static void printDemandSet(int *RequiredNodeSet[2], int *RequiredNodeNum)
{
    for(int i = 0; i < 2 ; i ++)
    {
        printf("路径[%d]的必经点个数:%d \n",i+1,RequiredNodeNum[i]);
        for(int j = 0 ; j < RequiredNodeNum[i] ; j++)
            printf("%d  ",RequiredNodeSet[i][j]);
        printf("\n");
    }
}


//打印重复边信息
static void printEdgesInfo(char *str, EdgeLink *repeatededgeset, int edgenum)
{
    printf("%s\n",str);
    for(int i = 0; i < edgenum; i++)
    {
        printf("%d  ",repeatededgeset[i].LinkID);
    }
    printf("\n");
}

static int ChangeTopo(AdjListGraph G, EdgeLink *ConflictedlEdgeSet, int EdgesNum )
{
    EdgeNode *p = NULL;
    int count = 0;
    for(int i = 0; i < EdgesNum; i++)
    {
        p = G.adjList[ConflictedlEdgeSet[i].SourceID].pFirstEdge;
        while(p != NULL)
        {
            if(p->edgeID ==ConflictedlEdgeSet[i].LinkID)
            {
                p->cost = INF + ConflictedlEdgeSet[i].Cost;
                //        p -> cost = ConflictedlEdgeSet[i].Cost + STEP;
                count++;
                break;
            }
            p = p ->next;
        }
    }
    return count;
}

static void printPath(int pathId,EdgeLink *edgearray, int num)
{
    printf("Path[%d]:%d nodes\n",pathId,num);
    for(int i = 0; i < num; i++)
    {
        printf("%d|",edgearray[i].LinkID);
    }
    printf("\n");
}


static int CheckConflictedEdges(EdgeLink *edgeset1, int EdgeNum1,EdgeLink *edgeset2, int EdgeNum2, EdgeLink *RepeatedEdges)
{
    int i =0;
    int j = 0;
    int k = 0;
//printEdgesInfo("path",Path[0],EdgeNum[0]);
    for(i = 0; i < EdgeNum1; i++)
        for(j = 0; j < EdgeNum2; j++)
        {
            if(edgeset1[i].LinkID ==edgeset2[j].LinkID)
            {
                RepeatedEdges[k].LinkID = edgeset1[i].LinkID;
                RepeatedEdges[k].SourceID = edgeset1[i].SourceID;
                RepeatedEdges[k].DestinationID = edgeset1[i].DestinationID;
                RepeatedEdges[k].Cost = edgeset1[i].Cost;
                RepeatedEdges[k].demandID = 0;
                //             printf("path[0][%d]==path[1][%d]:%d\n",i,j,Path[0][i].LinkID);
                k++;
                break;
            }
        }
    return k;
}

