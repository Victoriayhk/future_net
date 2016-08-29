#include "LKH.h"
#include "Heap.h"


static void CreateNodes(void);
static void Read_EDGE_WEIGHT_SECTION(void);

void ReadProblem()
{
    FreeStructures();
    FirstNode = 0;
    C = c = 0;
	/***************************** my code start**************************/
	Name = (char *) malloc(strlen("problem.atsp") + 1);
    Type = (char *) malloc(strlen("ATSP") + 1);
	ProblemType = ATSP;
	DimensionSaved = Dimension = Rp0->VVnum;

	Read_EDGE_WEIGHT_SECTION();
	WeightType = -1;
	Swaps = 0;

    /* Adjust parameters */
    MaxSwaps = Dimension;
    if (MaxCandidates > Dimension - 1)
        MaxCandidates = Dimension - 1;
	if (AscentCandidates > Dimension - 1)
		AscentCandidates = Dimension - 1;
	InitialPeriod = Dimension / 2;
	if (InitialPeriod < 100)
		InitialPeriod = 100;
	
	Excess = 1.0 / Dimension;
	MaxTrials = Dimension;
	MakeHeap(Dimension);
	
    C = C_FUNCTION ;
    D = D_FUNCTION;
    
	MoveFunction Movefun[5] = {Best2OptMove,Best2OptMove,Best3OptMove,Best4OptMove,Best5OptMove};
    BestMove = BestSubsequentMove = Movefun[MoveType-1];
}


static void CreateNodes()
{
    Node *Prev = 0, *N = 0;
    int i;

    Dimension *= 2;//atsp
    assert(NodeSet = (Node *) calloc(Dimension + 1, sizeof(Node)));
    for (i = 1; i <= Dimension; i++, Prev = N) 
	{
        N = &NodeSet[i];
        if (i == 1)
            FirstNode = N;
        else
            Link(Prev, N);
        N->Id = i;
    }
    Link(N, FirstNode);
}


static void Read_EDGE_WEIGHT_SECTION()
{
    Node *Ni, *Nj;
    int i, j, n, W;

    if (!FirstNode)
        CreateNodes();
	n = Dimension / 2;
	assert(CostMatrix = (int *) calloc((size_t) n * n, sizeof(int)));//sizeof(size_t)=8,分配n * n个int
	for (Ni = FirstNode; Ni->Id <= n; Ni = Ni->Suc)
		Ni->C = &CostMatrix[(size_t) (Ni->Id - 1) * n] - 1;
            
	n = Dimension / 2;
	for (i = 1; i <= n; i++)
	{
		Ni = &NodeSet[i];
		for (j = 1; j <= n; j++) 
		{
			W = Rp0->VVcost[i-1][j-1];/****************my data**********/
			Ni->C[j] = W;
			if (i != j && W > M)
				M = W;
		}
		Nj = &NodeSet[i + n];
		if (!Ni->FixedTo1)
			Ni->FixedTo1 = Nj;
		if (!Nj->FixedTo1)
			Nj->FixedTo1 = Ni;
	}
}

