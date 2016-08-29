#include "future_net.h"


//5-OPT中的8种情况
enum case5OPT{CaseNon,Case123456,Case78,Case12,Case345,
			  Case1,Case2,Case3,Case4,Case5,Case6,Case7,Case8,Case_3OPT};
enum case5OPT Case,bestCase,CaseT5,CaseT7;

//5-sequential-OPT
unsigned char Road::_5_OPT(long long &bestCost)
{
	int i,j,k,m;
	unsigned char isImproved = False;
	long long bestGain,Gain1,Gain2,Gain3,Gain5;
	VVNode *t1=NULL,*t2=NULL,*t3=NULL,*t4=NULL,*t5=NULL,*t6=NULL,*t7=NULL,*t8=NULL,*t9=NULL,*t10=NULL;
	VVNode *T1=NULL,*T2=NULL,*T3=NULL,*T4=NULL,*T5=NULL,*T6=NULL,*T7=NULL,*T8=NULL,*T9=NULL,*T10=NULL;
	
	#if PRINT_TIME_INFO == 1
		long long timeStart = clock();
	#endif
	#if PRINT_OPT_INFO == 1
		Print("start run %s() *********************************************\n",__FUNCTION__);
	#endif
	
	resetActive();
	while(NULL != (t1=RemoveFirstActive()))
	{
		t2 = t1->succ;
		bestGain = 0;	bestCase = CaseNon;
		for(i = 0;i < t1->candCn;i++)
		{
			AssertContinue((t3=t1->cand[i]) != t2);
			Gain1 = COST(t1,t2) - COST(t1,t3);
			AssertContinue(Gain1 > 0);
			t4 = t3->pred;
			for(j = 0;j < t4->candCn;j++)
			{
				t5 = t4->cand[j];
				if(isInSegment(t5,t3,t1))		CaseT5 = Case123456;
				else if(isInSegment(t5,t2,t4))	CaseT5 = Case78;
				else continue;
				Gain2 = Gain1 + COST(t4,t3) - COST(t4,t5);
				t6 = t5->pred;
		
				//special case 3-opt start(能3opt就不5opt)**************************************
				if(CaseT5 == Case123456)
				{
					Gain3 = Gain2 + COST(t6,t5) - COST(t6,t2);
					if(bestGain < Gain3)
					{
						bestGain = Gain3;	bestCase = Case_3OPT;
						RecordT(1);RecordT(2);RecordT(3);RecordT(4);RecordT(5);RecordT(6);
						goto _5_OPT_Update;//用不用时间差不多
					}
				}
				//special case 3-opt end*****************************************************
				
				for(k = 0;k < t6->candCn;k++)
				{
					t7 = t6->cand[k];
					if(CaseT5 == Case123456)
					{
						if(isInSegment(t7,t5,t1))		CaseT7 = Case12;
						else if(isInSegment(t7,t2,t4))	CaseT7 = Case345;
						else if(isInSegment(t7,t3,t6))	CaseT7 = Case6;
						else continue;
					}
					else//Case == Case78
					{
						if(isInSegment(t7,t3,t1))		CaseT7 = Case7;
						else if(isInSegment(t7,t5,t4))	CaseT7 = Case8;
						else continue;
					}
					Gain3 = Gain2 + COST(t6,t5) - COST(t6,t7);
					t8 = t7->pred;
					for(m = 0;m < t8->candCn;m++)
					{
						t9 = t8->cand[m];
						Case = CaseNon;
						if(CaseT7 == Case12)
						{
							if(isInSegment(t9,t3,t6))		{Case = Case1;}
							else if(isInSegment(t9,t7,t1))	{Case = Case2;}
						}
						else if(CaseT7 == Case345)
						{
							if(isInSegment(t9,t5,t1))		{Case = Case3;}
							else if(isInSegment(t9,t3,t6))	{Case = Case4;}
							else if(isInSegment(t9,t7,t4))	{Case = Case5;}
						}
						else if(CaseT7 == Case6)
						{	if(isInSegment(t9,t7,t6))		{Case = Case6;}}
						else if(CaseT7 == Case7)
						{	if(isInSegment(t9,t5,t4))		{Case = Case7;}}
						else//CaseT7 == Case8
						{	if(isInSegment(t9,t3,t1))		{Case = Case8;}}
						if(Case != CaseNon)
						{
							t10 = t9->pred;//t1-t10 create a new loop
							Gain5 = Gain3 + COST(t8,t7) - COST(t8,t9)
										  + COST(t10,t9) - COST(t10,t2);
							if(bestGain < Gain5)
							{
								bestGain = Gain5;	bestCase = Case;
								RecordT(1);RecordT(2);RecordT(3);RecordT(4);RecordT(5);RecordT(6);RecordT(7);RecordT(8);RecordT(9);RecordT(10);
								goto _5_OPT_Update;//用了貌似快点
							}
						}//switch end
					}//t9 end
				}//t7 end
			}//t5 end
		}//t3 end
_5_OPT_Update:
		if(bestGain > 0)
		{
			#if PRINT_OPT_INFO == 1
				if(bestCase == Case_3OPT)	{Print("%s 3-OPT Gain: ",__FUNCTION__);}
				else						{Print("%s 5-OPT Gain: ",__FUNCTION__);}
				printf_3var(bestCost>>7u,bestGain>>7u,(bestCost-bestGain)>>7u,%lld);
			#endif
			bestCost -= bestGain;
			isImproved = True;
			//printf_1var(bestCase,%d);
			if(bestCase == Case_3OPT)//creat a shorter tour,store it and reset Order
			{
				Active(T1);	Active(T2);	Active(T3);	Active(T4);	Active(T5);	Active(T6);
				LinkA2B(T1,T3);	LinkA2B(T4,T5);	LinkA2B(T6,T2);
			}
			else
			{
				Active(T1);	Active(T2);	Active(T3);	Active(T4);	Active(T5);	Active(T6);	Active(T7);	Active(T8);	Active(T9);	Active(T10);
				LinkA2B(T1,T3);	LinkA2B(T4,T5);	LinkA2B(T6,T7);	LinkA2B(T8,T9);	LinkA2B(T10,T2);
			}
			resetOrder();
			//if(UsedTourTable && ((bestCost-sumPi)>>7u) < UnReachCost)
			//	tt.insert(this,(bestCost-sumPi)>>7u);
		}
	}
	#if PRINT_TIME_INFO == 1
		Print("Time : %s() used time %lld\n\n",__FUNCTION__,clock() - timeStart);
	#endif
	
	return isImproved;
}
