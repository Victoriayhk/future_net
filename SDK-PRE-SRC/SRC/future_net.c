/***
文件描述
future_net.c,future_net.h		:main()主函数,自己的宏定义和结构体变量
readinfo.c,readinfo.h			:读入数据
writeresult.c,writeresult.h		:输出结果
route.c,route.h					:寻找路径的主要流程
ShortestPathAlgorithm.c,route.h	:多源最短路算法，包括记录路径
RouteAlgorithm.c,route.h		:解决Atsp问题的算法
run.sh							:shell运行脚本，更改casename可以测试不同的用例，打包等功能
runout.txt						:是每次运行时候程序stdout（即printf）的输出结果（执行future_net时做了输出重定向）

更新日志
2016.3.16  	  星期三：直接把重复的点加入到must-node里
2016.3.17 V1.0星期四：修改代码美工
2016.3.18 V1.1星期五：解决易惠康的极端测试用例、atsp文件指定问题！！！(对ReadProblem.c的225、240、402、410、867行进行修改)
2016.3.21 V1.2星期六：修改topo读入，FLOYD改成SPFA、DIS
2016.3.22 V1.3星期二：加入暴力BFS搜索，49.94分，解决第四个问题
2016.3.26 V1.4星期六：修复LKH每次运行都要初始化Gain32.c变量的问题(Gain23.c的42行增加LKH_useCn变量)
2016.3.27 V1.5星期日：增加判断result.csv是否合理的函数exam_result(read_info.c),把LKH.h中EXAM_RESULT解注释可使用该功能
2016.3.27 V1.5.1星期日：对于demand文件的读入做了优化，尤其是只有s、t的例子
2016.3.27 V1.5.2星期一：1、更新record_result()函数中log数据输出格式(增加权重)
					  2、exam_repeated_node()中更新isVV时候bug修复（原来是<V.num，(⊙﹏⊙)b）
					  3、找到第一个useCn>2的非必须点就加入到必须点中（增加break语句）
					  4、有待验证正着找还是反着找，还是分支进行择优录取
2016.4.6  V1.6.1星期三：1、V_init中读入方式从fscanf改成getc，据说会快(然而交了并没有什么卵用，不过本地貌似快了。。。)
					  2、删除好多LKH代码
2016.4.14 V2.0.0星期四：复赛代码改各种变量、读入输出，除了exam_future已经实现初赛所有功能
2016.4.15 V2.0.1星期五：继续修改变量和函数名，基本完成易慧康的算法思路
2016.4.15 V2.0.2星期六：初赛和复赛均可用，并且复赛的大用例都能跑，只是加入重复点的时候要考虑
2016.4.19 V2.1.0星期二：删除好多LKH无用变量和文件
2016.4.22 V2.1.1星期五：时间计时函数改成clock()、解决了id互换的一个小bug

每次提交前注意事项：
0、简单的几个用例的时间和cost是否对
1、write_result.c中#define OUT_DEBUG_INFO是否注释
2、main里面的printf是否注释
4、LKH.h的MyDebug和EXAM_RESULT是否注释
***/


#include "LKH.h"

int main(int argc, char *argv[])
{
	//while(clock()<0);
	V_init(argv[1]);
	Road_init(argv[2]);
	
	#ifdef EXAM_RESULT
		void exam_result(const char * const topofile,const char * const resultfile,int mode);
		Rp0=&Rd[0];	Rp1=&Rd[1];
		exam_result(argv[1],argv[3],(argc==5));
		return 0;
	#endif
	
	
	#ifndef PRELIMINARY
		search_double_route();
	#else
		search_single_route(Rp0=&Rd[0],Rp1=&Rd[1]);	
	#endif
	record_result(V.RCostSum < MaxECost);
	write_result2file(argv[3]);

		
	#ifndef PRELIMINARY
		#if 1
			Rp0=&Rd[0],Rp1=&Rd[1];
			printf("MaxE,SubMaxE,MaxCost = %5lld,%5lld,%5lld\n",MaxECost,SubMaxECost,MaxReachECost);
			printf("LKH use count: %d\n",V.LKHuseCnSum);
			printf("node num  = %5d,%5d,%5d\n",V.num,Rp0->VVnumInit,Rp1->VVnumInit);
			printf("repeat edge num =%5d\n", V.reIdCn);
			//if(V.RCostSum >= MaxECost)	printf("Road cost	=    NA\n");
			//else	printf("Road cost 		=%5lld,%5lld,%5lld\n",V.RCostSum,Rp0->cost,Rp1->cost);
			printf("end time		=%5ldms\n", clock()/1000);
		#else
			//if(V.RCostSum >= MaxECost)	printf("Road cost =    NA\n");
			//else	printf("Road cost = %5lld,%5lld,%5lld\n",V.RCostSum,Rp0->cost,Rp1->cost);
			//printf("end time  = %4ld\n", clock()/1000);
		#endif
	#else
		#if 1
			printf("node num  = %5d,%5d,%5d\n",V.num,Rp0->VVnumInit,Rp1->VVnumInit);
			if(V.RCostSum >= MaxECost)	printf("999999	%4ld\n",clock()/1000);
			else printf("cost=%6lld	%4ldms\n", (V.RCostSum<MaxECost)?V.RCostSum:MaxECost,clock()/1000);
		#endif
	#endif

	return 0;
}
