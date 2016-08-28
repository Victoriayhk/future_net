/***
文件描述
future_net.h					:自己所有的头文件、宏定义、类声明、变量声明、类定义
future_net.cpp					:有main()主函数、结果检查函数、自己的结果写入result.csv函数
route.cpp,route.h				:寻找路径的几个主要函数,全局变量定义,Road类静态变量初始化
Graph.cpp						:Graph类几个基础函数
Road.cpp						:Road类几个基础函数
LKH.cpp							:LKH算法,函数均为Road的成员函数
candidate.cpp					:使用次梯度优化求候选集
Minimun1Tree.cpp				:求最小1-tree
MinimunSpanningTree.cpp			:求最小生成树
initial.cpp						:使用不同规则初始化路,函数均为Road的成员函数
_3_OPT.cpp						:有向边交换操作(3opt只有一种,且是sequential的)
_4_OPT.cpp						:有向边交换操作(4opt只有一种,且是non-sequential的)
_5_OPT.cpp						:有向边交换操作(5opt只有八种,且是sequential的)(已经把3opt包含,所以不需要再使用3opt)
exam_repeat.cpp					:检查重复点,选择加入的规则,函数均为Road的成员函数

io.cpp,lib/...					:SDK自带的读入输出和头文件
run1.sh							:shell运行脚本,更改casename可以测试不同的用例,打包等功能
run2.sh							:测试大量用例(使用ls命令)
runout.txt						:是每次运行时候程序stdout(即printf)的输出结果(执行future_net时做了输出重定向)

更新日志
2016.3.16  	  星期三:直接把重复的点加入到must-node里
2016.3.17 V1.0星期四:修改代码美工
2016.3.18 V1.1星期五:解决易惠康的极端测试用例、atsp文件指定问题！！！(对ReadProblem.c的225、240、402、410、867行进行修改)
2016.3.21 V1.2星期六:修改topo读入,FLOYD改成SPFA、DIS
2016.3.22 V1.3星期二:加入暴力BFS搜索,49.94分,解决第四个问题
2016.3.26 V1.4星期六:修复LKH每次运行都要初始化Gain32.c变量的问题(Gain23.c的42行增加LKH_useCn变量)
2016.3.27 V1.5星期日:增加判断result.csv是否合理的函数exam_result(read_info.c),把LKH.h中EXAM_RESULT解注释可使用该功能
2016.3.27 V1.5.1星期日:对于demand文件的读入做了优化,尤其是只有s、t的例子
2016.3.27 V1.5.2星期一:1、更新record_result()函数中log数据输出格式(增加权重)
					  2、exam_repeated_node()中更新isVV时候bug修复(原来是<V.num,(⊙﹏⊙)b)
					  3、找到第一个useCn>2的非必须点就加入到必须点中(增加break语句)
					  4、有待验证正着找还是反着找,还是分支进行择优录取
2016.4.6  V1.6.1星期三:1、V_init中读入方式从fscanf改成getc,据说会快(然而交了并没有什么卵用,不过本地貌似快了。。。)
					  2、删除好多LKH代码
2016.4.14 V2.0.0星期四:复赛代码改各种变量、读入输出,除了exam_future已经实现初赛所有功能
2016.4.15 V2.0.1星期五:继续修改变量和函数名,基本完成易慧康的算法思路
2016.4.15 V2.0.2星期六:初赛和复赛均可用,并且复赛的大用例都能跑,只是加入重复点的时候要考虑
2016.4.19 V2.1.0星期二:删除好多LKH无用变量和文件
2016.4.22 V2.1.1星期五:时间计时函数改成clock()、解决了id互换的一个小bug
2016.4.29 V3.0.0星期五:3-opt成型,可以计算简单问题
2016.5.1  V3.1.0星期日:程序基本大的框架不用改了，记录最优解，准备次梯度优化和4-opt
2016.5.1  V3.1.1星期日:3、4、5-opt完成，2000的一个终极例子2375(2370)
2016.5.3  V3.1.2星期二:修复一个swapCost的bug(SubUnReachCost应该加上原始cost值，但是之前没加)
2016.5.3  V3.2.0星期二:修复一个initialTour()中i、j使用的低级bug,加入ascent效果很好
2016.5.5  V3.2.1星期四:1、initialTour改好了
					  2、storeBestTour放在FindTour最后
					  3、减少时间:加入pi的使用,5opt使用Gain1Positive,4-opt时保证Gain2Positive整个环找
2016.5.8  V3.2.2星期日:加入NORM==0判断,ascent求出解直接输出,不需要opt
2016.5.8  V3.3.0星期日:加入超哥的算法:每个重复点搞一个复制点,设计t到所有复制点,再到s的新tsp
					  0、修复bug:Road::initial()中Node[i].id未完全赋值
					  1、examRepeatNode()、setCost()、setTour()进行了修改
2016.5.8  V3.3.1星期一:SPFA加入随机...还没完全加好
2016.5.9  V3.3.2星期二:动态使用加不加复制点的规则
2016.5.9  V3.3.3星期二:SPFA随机设置确定了:spfa队列里面相邻两个点使用rand随机
2016.5.9  V3.3.4星期二:1、两条路迭代使用了路径哈希值判断,有几个减少了时间
					  2、search_double_route():order为外层循环,决定先从Road0或先从Road1都找
					  3、iteration是内层循环,如果找的第一轮找就无重复边数则跳出
					  4、准备进一步减少时间...如果上一次没有重复,而且第二大轮第一条路得值和上一大轮最好值一样
2016.5.11 V3.3.5星期三:加了四行代码官方例子好了很多...临时版本(有&&的bug)
2016.5.13 V3.3.6星期三:加了routeCn<4和iteration<8的变量,前两轮不使用启发式规则(路A抢回B经过的A的必须点)
					  改善:1、时间有待改善	2、初始化需要路加算法。。。
2016.5.14 V3.3.7星期六:比赛版本,没有改啥。详见future_netV3.3.7复赛决赛5.13-23.53(代码和例子和脚本).zip
2016.5.18 V3.3.8星期三:1、search_double_route迭代跳出条件变成roadCost是否相同,而不是roadHash
					  2、加上road计算结果的存储,减少重算
2016.5.19 V3.3.9星期四:1、if(Rpo == this)的判断放到了swapGraphCost里面
					  2、initialTour()中如果最后一次还没求出来可行路就使用KM算法初始化路
					  3、Graph::setCost()中,如果id和subId没有边的位置初始化为-1(因为swapGraphCost()用到了判断)
					  4、Road的成员函数分散了好几个文件
					  5、次梯度优化优化了(减少最大的那个数组)
2016.5.21 V3.4.0星期六:1、spfa中的queue换成类,减少内存数组
					  2、增加setVcost_reduced_SPFA(),暂时没啥用,以后再调
2016.5.22 V3.4.1星期日:1、省略intUnReachCost、新增Active.cpp调整LKH相关的结构(主要是函数接口)
					  2、改变(bestCost < bestCostMin)的更新方式,三个变好
					  3、增加CandidatesNum变量
					  4、增加TourTable(在lock()中真实存储时候要判断有无重复点)(暂时没用)
					  5、加上易惠康的惩罚方式(暂时没用)
					  6、增加iter迭代上限,singleTime时间(未用)
2016.5.23 V3.5.0星期一:预览版,忘了改啥了
2016.5.25 V3.6.0星期三:改了V3.5.1的swapGraphCostBack()中最开始一个判断未加的bug
2016.5.25 V3.6.1星期三:1、增加setPunishMethod判断例子的难度,决定用什么惩罚方式
					  2、增加setVtourId(),执行顺序为setVtourId();swapGraphCostBack(Rpo);setVtourCost();
					  3、移除PenaltyMechanism相关代码


每次提交前注意事项:
0、简单的几个用例的时间和cost是否对
1、printf是否注释
3、node[x].outPi和inPi是不是0
***/

#include "route.h"
#include "lib_io.h"
#include "lib_time.h"
#include "stdio.h"
#include "time.h"

int main(int argc, char *argv[])
{	
	//SDK读入=90ms
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
	printf("program end time =%5ldms\n\n", clock()/1000);
	return 0;
}
