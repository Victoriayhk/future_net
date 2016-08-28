#include "LKH.h"

/*
 * After the candidate set has been created the FindTour function is called 
 * a predetermined number of times (Runs). 
 *
 * FindTour performs a number of trials, where in each trial it attempts 
 * to improve a chosen initial tour using the modified Lin-Kernighan edge 
 * exchange heuristics. 
 *
 * Each time a better tour is found, the tour is recorded, and the candidates 
 * are reorderded by the AdjustCandidateSet function. Precedence is given to 
 * edges that are common to two currently best tours. The candidate set is 
 * extended with those tour edges that are not present in the current set. 
 * The original candidate set is re-established at exit from FindTour.  
 */

static void SwapCandidateSets();
static GainType OrdinalTourCost;

GainType FindTour()
{
    GainType Cost;
    Node *t;
	int i;
	
	//初始化OldPred、OldSuc、NextBestSuc、BestSuc 均为0
    t = FirstNode;
    do
        t->OldPred = t->OldSuc = t->NextBestSuc = t->BestSuc = 0;
    while ((t = t->Suc) != FirstNode);
	
    BetterCost = PLUS_INFINITY;
    HashInitialize(HTable);//HTable.Count=0

	//使用MaxTrials次ChooseInitialTour()和LinKernighan()找解
    for (Trial = 1; Trial <= MaxTrials; Trial++) //MaxTrials=Dimension
	{
        /* Choose FirstNode at random.对时间有影响???*/
        //for (i = Random() % Dimension; i > 0; i--)
		//	FirstNode = FirstNode->Suc;
		FirstNode = &NodeSet[Random()%Dimension + 1];
        
        int a=clock();
        ChooseInitialTour();
        Cost = LinKernighan();
        
         // printf("Cost = %d \t",Cost);
         // int b= clock();
         // printf_1var(b-a);
		
		//???
  //       if (FirstNode->BestSuc)
		// {
  //           /* Merge tour with current best tour */
  //           t = FirstNode;
  //           while ((t = t->Next = t->BestSuc) != FirstNode);
  //           Cost = MergeWithTour();
  //       }
		//每次找到更好的路径
        if (Cost < BetterCost) 
		{
            BetterCost = Cost;
            RecordBetterTour();
            //AdjustCandidateSet();//没有多大卵用
            HashInitialize(HTable);
            HashInsert(HTable, Hash, Cost);
        }
    }
    
	//记录最好的路径
	t = FirstNode;
	do
        (t->Suc = t->BestSuc)->Pred = t;
    while ((t = t->BestSuc) != FirstNode);

    return BetterCost;
}

/*
 * The SwapCandidateSets function swaps the normal and backbone candidate sets.
 */

static void SwapCandidateSets()
{
    Node *t = FirstNode;
    do 
	{
        Candidate *Temp = t->CandidateSet;
        t->CandidateSet = t->BackboneCandidateSet;
        t->BackboneCandidateSet = Temp;
    } while ((t = t->Suc) != FirstNode);
}
