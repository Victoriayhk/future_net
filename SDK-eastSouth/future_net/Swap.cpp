#include "Structure.h"
#include "Algorthm.h"


static void Flip(Node * N1, Node * N2, Node * N3);
static void SplitSegment(Node * N1, Node * N2);

#define SPLIT_CUTOFF 0.75

void FLIP(Node * N1, Node * N2, Node * N3)
{
    Node *N4, *Na, *Nb, *Nc, *Nd;
    Segment *P1, *P2, *P3, *P4, *Q1, *Q2;
    Node *s1, *s2;
    int i, Temp;

    assert(N1->Pred == N2 || N1->Suc == N2);
    if (N3 == N2->Pred || N3 == N2->Suc)
        return;
    if (SegmentNums == 1) {
        Flip(N1, N2, N3);
        return;
    }
    N4 = N2 == SUC(N1) ? PRED(N3) : SUC(N3);
    P1 = N1->Parent;
    P2 = N2->Parent;
    P3 = N3->Parent;
    P4 = N4->Parent;

    if (P1 != P3 && P2 != P4) {
        if (P1 == P2) {
            SplitSegment(N1, N2);
            P1 = N1->Parent;
            P2 = N2->Parent;
        }
        if (P3 == P4 && P1 != P3 && P2 != P4) {
            SplitSegment(N3, N4);
            P3 = N3->Parent;
            P4 = N4->Parent;
        }
    } else
        if ((P1 == P3
             && abs(N3->Priority - N1->Priority) > SPLIT_CUTOFF * SegSize)
            || (P2 == P4
                && abs(N4->Priority - N2->Priority) > SPLIT_CUTOFF * SegSize)) {
        if (P1 == P2) {
            SplitSegment(N1, N2);
            P1 = N1->Parent;
            P2 = N2->Parent;
            P3 = N3->Parent;
            P4 = N4->Parent;
        }
        if (P3 == P4) {
            SplitSegment(N3, N4);
            P1 = N1->Parent;
            P2 = N2->Parent;
            P3 = N3->Parent;
            P4 = N4->Parent;
        }
    }

    Nb = 0;
    if (P1 == P3) {

        if (N1->Priority < N3->Priority) {
            if (P1 == P2 && P1 == P4 && N2->Priority > N1->Priority) {
                Na = N1;
                Nb = N2;
                Nc = N3;
                Nd = N4;
            } else {
                Na = N2;
                Nb = N1;
                Nc = N4;
                Nd = N3;
            }
        } else {
            if (P1 == P2 && P1 == P4 && N2->Priority < N1->Priority) {
                Na = N3;
                Nb = N4;
                Nc = N1;
                Nd = N2;
            } else {
                Na = N4;
                Nb = N3;
                Nc = N2;
                Nd = N1;
            }
        }
    } else if (P2 == P4) {

        if (N4->Priority < N2->Priority) {
            Na = N3;
            Nb = N4;
            Nc = N1;
            Nd = N2;
        } else {
            Na = N1;
            Nb = N2;
            Nc = N3;
            Nd = N4;
        }
    }
    if (Nb) {
        int Cbc = COST(Nb, Nc), Cda = COST(Nd, Na);

        i = Nd->Priority;
        Nd->Suc = 0;
        s2 = Nb;
        while ((s1 = s2)) {
            s2 = s1->Suc;
            s1->Suc = s1->Pred;
            s1->Pred = s2;
            s1->Priority = i--;
            Temp = s1->SucCost;
            s1->SucCost = s1->PredCost;
            s1->PredCost = Temp;
        }
        Nd->Pred = Na;
        Nb->Suc = Nc;
        Nd->PredCost = Cda;
        Nb->SucCost = Cbc;
        if (Na->Suc == Nb) {
            Na->Suc = Nd;
            Na->SucCost = Nd->PredCost;
        } else {
            Na->Pred = Nd;
            Na->PredCost = Nd->PredCost;
        }
        if (Nc->Pred == Nd) {
            Nc->Pred = Nb;
            Nc->PredCost = Nb->SucCost;
        } else {
            Nc->Suc = Nb;
            Nc->SucCost = Nb->SucCost;
        }
        if (Nb->Parent->First == Nb)
            Nb->Parent->First = Nd;
        else if (Nd->Parent->First == Nd)
            Nd->Parent->First = Nb;
        if (Nb->Parent->Last == Nb)
            Nb->Parent->Last = Nd;
        else if (Nd->Parent->Last == Nd)
            Nd->Parent->Last = Nb;
    } else {
        int Ct2t3, Ct4t1;

        if (P1->Suc != P2) {
            Na = N1;
            N1 = N2;
            N2 = Na;
            Na = N3;
            N3 = N4;
            N4 = Na;
            Q1 = P1;
            P1 = P2;
            P2 = Q1;
            Q1 = P3;
            P3 = P4;
            P4 = Q1;
        }


        if ((i = P2->Priority - P3->Priority) < 0)
            i += SegmentNums;
        if (2 * i > SegmentNums) {
            Na = N3;
            N3 = N2;
            N2 = Na;
            Na = N1;
            N1 = N4;
            N4 = Na;
            Q1 = P3;
            P3 = P2;
            P2 = Q1;
            Q1 = P1;
            P1 = P4;
            P4 = Q1;
        }
        Ct2t3 = COST(N2, N3);
        Ct4t1 = COST(N4, N1);

        i = P1->Priority;
        P1->Suc = 0;
        Q2 = P3;
        while ((Q1 = Q2)) {
            Q2 = Q1->Suc;
            Q1->Suc = Q1->Pred;
            Q1->Pred = Q2;
            Q1->Priority = i--;
            Q1->IsPathReversed ^= 1;
        }
        P3->Suc = P2;
        P2->Pred = P3;
        P1->Pred = P4;
        P4->Suc = P1;
        if (N3->Suc == N4) {
            N3->Suc = N2;
            N3->SucCost = Ct2t3;
        } else {
            N3->Pred = N2;
            N3->PredCost = Ct2t3;
        }
        if (N2->Suc == N1) {
            N2->Suc = N3;
            N2->SucCost = Ct2t3;
        } else {
            N2->Pred = N3;
            N2->PredCost = Ct2t3;
        }
        if (N1->Pred == N2) {
            N1->Pred = N4;
            N1->PredCost = Ct4t1;
        } else {
            N1->Suc = N4;
            N1->SucCost = Ct4t1;
        }
        if (N4->Pred == N3) {
            N4->Pred = N1;
            N4->PredCost = Ct4t1;
        } else {
            N4->Suc = N1;
            N4->SucCost = Ct4t1;
        }
    }
    SwapStack[ExchangeTimes].t1 = N1;
    SwapStack[ExchangeTimes].t2 = N2;
    SwapStack[ExchangeTimes].t3 = N3;
    SwapStack[ExchangeTimes].t4 = N4;
    ExchangeTimes++;
    KeyValue ^= (RandomNumSet[N1->Id] * RandomNumSet[N2->Id]) ^
        (RandomNumSet[N3->Id] * RandomNumSet[N4->Id]) ^
        (RandomNumSet[N2->Id] * RandomNumSet[N3->Id]) ^ (RandomNumSet[N4->Id] * RandomNumSet[N1->Id]);
}


void SplitSegment(Node * N1, Node * N2)
{
    Segment *P = N1->Parent, *Q;
    Node *N, *u;
    int i, Temp, Count;

    if (N2->Priority < N1->Priority) {
        N = N1;
        N1 = N2;
        N2 = N;
    }
    Count = N1->Priority - P->First->Priority + 1;
    if (2 * Count < P->Size) {

        Q = P->IsPathReversed ? P->Suc : P->Pred;
        N = P->First->Pred;
        i = N->Priority;
        if (N == Q->Last) {
            if (N == Q->First && N->Suc != P->First) {
                u = N->Suc;
                N->Suc = N->Pred;
                N->Pred = u;
                Q->IsPathReversed ^= 1;
                Temp = N->SucCost;
                N->SucCost = N->PredCost;
                N->PredCost = Temp;
            }
            for (N = P->First; N != N2; N = N->Suc) {
                N->Parent = Q;
                N->Priority = ++i;
            }
            Q->Last = N1;
        } else {
            for (N = P->First; N != N2; N = u) {
                N->Parent = Q;
                N->Priority = --i;
                u = N->Suc;
                N->Suc = N->Pred;
                N->Pred = u;
                Temp = N->SucCost;
                N->SucCost = N->PredCost;
                N->PredCost = Temp;
            }
            Q->First = N1;
        }
        P->First = N2;
    } else {

        Q = P->IsPathReversed ? P->Pred : P->Suc;
        N = P->Last->Suc;
        i = N->Priority;
        if (N == Q->First) {
            if (N == Q->Last && N->Pred != P->Last) {
                u = N->Suc;
                N->Suc = N->Pred;
                N->Pred = u;
                Q->IsPathReversed ^= 1;
                Temp = N->SucCost;
                N->SucCost = N->PredCost;
                N->PredCost = Temp;
            }
            for (N = P->Last; N != N1; N = N->Pred) {
                N->Parent = Q;
                N->Priority = --i;
            }
            Q->First = N2;
        } else {
            for (N = P->Last; N != N1; N = u) {
                N->Parent = Q;
                N->Priority = ++i;
                u = N->Pred;
                N->Pred = N->Suc;
                N->Suc = u;
                Temp = N->SucCost;

                N->SucCost = N->PredCost;
                N->PredCost = Temp;
            }
            Q->Last = N2;
        }
        Count = P->Size - Count;
        P->Last = N1;
    }
    P->Size -= Count;
    Q->Size += Count;
}

static void Flip(Node * N1, Node * N2, Node * N3)
{
    Node *s1, *s2, *N4;
    int R, Temp, Ct2t3, Ct4t1;

    assert(N1->Pred == N2 || N1->Suc == N2);
    if (N3 == N2->Pred || N3 == N2->Suc)
        return;
    N4 = N1->Suc == N2 ? N3->Pred : N3->Suc;
    if (N1->Suc != N2) {
        s1 = N1;
        N1 = N2;
        N2 = s1;
        s1 = N3;
        N3 = N4;
        N4 = s1;
    }

    if ((R = N2->Priority - N3->Priority) < 0)
        R += Dim;
    if (2 * R > Dim) {
        s1 = N3;
        N3 = N2;
        N2 = s1;
        s1 = N4;
        N4 = N1;
        N1 = s1;
    }
    Ct2t3 = COST(N2, N3);
    Ct4t1 = COST(N4, N1);

    R = N1->Priority;
    N1->Suc = 0;
    s2 = N3;
    while ((s1 = s2)) {
        s2 = s1->Suc;
        s1->Suc = s1->Pred;
        s1->Pred = s2;
        s1->Priority = R--;
        Temp = s1->SucCost;
        s1->SucCost = s1->PredCost;
        s1->PredCost = Temp;
    }
    (N3->Suc = N2)->Pred = N3;
    (N4->Suc = N1)->Pred = N4;
    N3->SucCost = N2->PredCost = Ct2t3;
    N1->PredCost = N4->SucCost = Ct4t1;
    SwapStack[ExchangeTimes].t1 = N1;
    SwapStack[ExchangeTimes].t2 = N2;
    SwapStack[ExchangeTimes].t3 = N3;
    SwapStack[ExchangeTimes].t4 = N4;
    ExchangeTimes++;
    KeyValue ^= (RandomNumSet[N1->Id] * RandomNumSet[N2->Id]) ^
        (RandomNumSet[N3->Id] * RandomNumSet[N4->Id]) ^
        (RandomNumSet[N2->Id] * RandomNumSet[N3->Id]) ^ (RandomNumSet[N4->Id] * RandomNumSet[N1->Id]);
}
