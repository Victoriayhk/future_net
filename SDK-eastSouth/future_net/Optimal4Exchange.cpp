#include "Algorthm.h"
#include "Structure.h"
int IsBetween(const Node * Na, const Node * Nb, const Node * Nc);
static int Eliminable(Node * Na, Node * Nb);
static void Eliminate(Node * Na, Node * Nb);
/*
 * 实现一次边交换操作
*/

Node *Optimal4Exchange(Node * N1, Node * N2, CostType * Gain0, CostType * Gain)
{
    Node *N3, *N4, *N5, *N6 = 0, *N7, *N8 = 0,
        *n3 = 0, *n4 = 0, *n5 = 0, *n6 = 0, *n7 = 0, *n8 = 0;
    PromisedEdges *Nt2, *Nt4, *Nt6;
    CostType Gain1, Gain2, Gain3, Gain4, Gain5, Gain6, BestG6 = INT_MAX;
    int Case6 = 0, Case8 = 0, BestCase8 = 0, X4, X6, X8;
    int Width2 = 0, Width4, Width6;

    *Gain = 0;
    if (SUC(N1) != N2)
        IsPathReversed ^= 1;
    for (Nt2 = N2->PromisedEdgesSet; (N3 = Nt2->To); Nt2++) {
        if (N3 == N2->Pred || N3 == N2->Suc ||
            ((Gain1 = *Gain0 - Nt2->Cost) <= 0 ))
            continue;
        if (++Width2 > MAXWidth)
            break;

        for (X4 = 1; X4 <= 2; X4++) {
            N4 = X4 == 1 ? PRED(N3) : SUC(N3);
            if (FixedOrCommon(N3, N4))
                continue;
            Gain2 = Gain1 + COST(N3, N4);
            if (X4 == 1 &&
                !Forbidden(N4, N1) &&
                (!c || Gain2 - c(N4, N1) > 0) && (*Gain = Gain2 - COST(N4, N1)) > 0)
            {
                SWAP1(N1, N2, N3);
                return 0;
            }

            Width4 = 0;

            for (Nt4 = N4->PromisedEdgesSet; (N5 = Nt4->To); Nt4++) {
                if (N5 == N4->Pred || N5 == N4->Suc ||
                    ((Gain3 = Gain2 - Nt4->Cost) <= 0))
                    continue;
                if (++Width4 > MAXWidth)
                    break;

                for (X6 = 1; X6 <= 2; X6++) {
                    if (X4 == 1) {
                        if (X6 == 1) {
                            Case6 = 1 + !IsBetween(N2, N5, N4);
                            N6 = Case6 == 1 ? SUC(N5) : PRED(N5);
                        } else {
                            N6 = N6 == N5->Pred ? N5->Suc : N5->Pred;
                            if ((N5 == N1 && N6 == N2) ||
                                (N5 == N2 && N6 == N1))
                                continue;
                            Case6 += 2;
                        }
                    } else if (IsBetween(N2, N5, N3)) {
                        Case6 = 4 + X6;
                        N6 = X6 == 1 ? SUC(N5) : PRED(N5);
                        if (N6 == N1)
                            continue;
                    } else {
                        if (X6 == 2)
                            break;
                        Case6 = 7;
                        N6 = PRED(N5);
                        if (N6 == N2)
                            continue;
                    }
                    if (FixedOrCommon(N5, N6))
                        continue;
                    Gain4 = Gain3 + COST(N5, N6);
                    if ((Case6 <= 2 || Case6 == 5 || Case6 == 6) &&
                        !Forbidden(N6, N1) &&
                        (!c || Gain4 - c(N6, N1) > 0) &&
                        (*Gain = Gain4 - COST(N6, N1)) > 0) {
                        Make3OptMove(N1, N2, N3, N4, N5, N6, Case6);
                        return 0;
                    }

                    Width6 = 0;

                    for (Nt6 = N6->PromisedEdgesSet; (N7 = Nt6->To); Nt6++) {
                        if (N7 == N6->Pred || N7 == N6->Suc ||
                            (N6 == N2 && N7 == N3) ||
                            (N6 == N3 && N7 == N2) ||
                            ((Gain5 = Gain4 - Nt6->Cost) <= 0 ))
                            continue;
                        if (++Width6 > MAXWidth)
                            break;
            for (X8 = 1; X8 <= 2; X8++) {
                            if (X8 == 1) {
                                Case8 = Case6;
                                N8 = 0;
                                switch (Case6) {
                                case 1:
                                    N8 = IsBetween(N2, N7,
                                                 N5) ? SUC(N7) : PRED(N7);
                                    break;
                                case 2:
                                    N8 = IsBetween(N3, N7,
                                                 N6) ? SUC(N7) : PRED(N7);
                                    break;
                                case 3:
                                    if (IsBetween(N5, N7, N4))
                                        N8 = SUC(N7);
                                    break;
                                case 4:
                                    if (IsBetween(N2, N7, N5))
                                        N8 = IsBetween(N2, N7,
                                                     N4) ? SUC(N7) :
                                            PRED(N7);
                                    break;
                                case 5:
                                    N8 = PRED(N7);
                                    break;
                                case 6:
                                    N8 = IsBetween(N2, N7,
                                                 N3) ? SUC(N7) : PRED(N7);
                                    break;
                                case 7:
                                    if (IsBetween(N2, N7, N3))
                                        N8 = SUC(N7);
                                    break;
                                }
                                if (N8 == 0)
                                    break;
                            } else {
                                if (Case6 != 3 && Case6 != 4 && Case6 != 7)
                                    break;
                                N8 = N8 == N7->Pred ? N7->Suc : N7->Pred;
                                Case8 += 8;
                            }
                            if (N8 == N1 ||
                                (N7 == N3 && N8 == N4) ||
                                (N7 == N4 && N8 == N3))
                                continue;
                            if (FixedOrCommon(N7, N8))
                                continue;
                            Gain6 = Gain5 + COST(N7, N8);
                            if (!Forbidden(N8, N1) &&
                                (!c || Gain6 - c(N8, N1) > 0) &&
                                (*Gain = Gain6 - COST(N8, N1)) > 0) {
                                Make4OptMove(N1, N2, N3, N4, N5, N6, N7,
                                             N8, Case8);
                                return 0;
                            }
                            if ( Gain6 - Accuracy < N8->Cost)

                                continue;
                            if (ExchangeTimes > 0) {
                                if ((Gain6 > BestG6 ||
                                     (Gain6 == BestG6 && !Near(N7, N8) &&
                                      Near(n7, n8))) &&
                                    ExchangeTimes < MAXExchanges &&
                                    Eliminable(N7, N8) ) {

                                    if (
                                        Gain2 - N4->Pi == Gain4 - N6->Pi &&
                                        Gain4 - N6->Pi == Gain6 - N8->Pi &&
                                        Gain3 + N5->Pi == Gain1 + N3->Pi &&
                                        Gain5 + N7->Pi == Gain3 + N5->Pi)
                                        continue;
                                    n3 = N3;
                                    n4 = N4;
                                    n5 = N5;
                                    n6 = N6;
                                    n7 = N7;
                                    n8 = N8;
                                    BestCase8 = Case8;
                                    BestG6 = Gain6;
                                }
                            } else if (MAXExchanges > 0) {


                                Make4OptMove(N1, N2, N3, N4, N5, N6, N7,
                                             N8, Case8);
                                Eliminate(N1, N2);
                                Eliminate(N3, N4);
                                Eliminate(N5, N6);
                                Eliminate(N7, N8);

                                if (*Gain > 0)
                                    return 0;
                                RestoreTour();
                                if (N2 != SUC(N1))
                                    IsPathReversed ^= 1;
                            }
                        }
                    }
                }
            }
        }
}
    *Gain = 0;
    if (n8) {

        Make4OptMove(N1, N2, n3, n4, n5, n6, n7, n8, BestCase8);
        Eliminate(N1, N2), Eliminate(n3, n4);
        Eliminate(n5, n6);
        Eliminate(n7, n8);
        *Gain0 = BestG6;
    }
    return n8;
}




/*
 * 判断一条边是否可以排除在最优路径之外
 */

static int Eliminable(Node * Na, Node * Nb)
{
    if (Na == Nb->OldPred)
        return !Nb->OldPredExcluded;
    if (Na == Nb->OldSuc)
        return !Nb->OldSucExcluded;
    return 0;
}


/*
 * 设置记录某条边是否可排除的状态
 */

static void Eliminate(Node * Na, Node * Nb)
{
    if (Na == Nb->Pred || Na == Nb->Suc)
        return;
    if (Na == Nb->OldPred)
        Nb->OldPredExcluded = 1;
    else if (Na == Nb->OldSuc)
        Nb->OldSucExcluded = 1;
    if (Nb == Na->OldPred)
        Na->OldPredExcluded = 1;
    else if (Nb == Na->OldSuc)
        Na->OldSucExcluded = 1;
}

/*
 *判断某条边是否是不可访问
 */

int Forbidden(const Node * Na, const Node * Nb)
{
    return   (Na->Id <= SavedDim) == (Nb->Id <= SavedDim);
}

/*
 * 判断一个节点是否在其它两个节点之间
 */

int IsBetween(const Node * Na, const Node * Nb, const Node * Nc)
{
    const Segment *Parenta, *Parentb, *Parentc;

    if (Nb == Na || Nb == Nc)
        return 1;
    if (Na == Nc)
        return 0;
    Parenta = Na->Parent;
    Parentb = Nb->Parent;
    Parentc = Nc->Parent;
    if (Parenta == Parentc) {
        if (Parentb == Parenta)
            return (IsPathReversed == Parenta->IsPathReversed) ==
                (Na->Priority < Nc->Priority ?
                 Nb->Priority > Na->Priority && Nb->Priority < Nc->Priority :
                 Nb->Priority > Na->Priority || Nb->Priority < Nc->Priority);
        return (IsPathReversed == Parenta->IsPathReversed) == (Na->Priority > Nc->Priority);
    }
    if (Parentb == Parentc)
        return (IsPathReversed == Parentb->IsPathReversed) == (Nb->Priority < Nc->Priority);
    if (Parenta == Parentb)
        return (IsPathReversed == Parenta->IsPathReversed) == (Na->Priority < Nb->Priority);
    return IsPathReversed !=
        (Parenta->Priority < Parentc->Priority ?
         Parentb->Priority > Parenta->Priority && Parentb->Priority < Parentc->Priority :
         Parentb->Priority > Parenta->Priority || Parentb->Priority < Parentc->Priority);
}


/*
 *进行一次3交换
 */

void Make3OptMove(Node * N1, Node * N2, Node * N3, Node * N4,
                  Node * N5, Node * N6, int Case)
{
    switch (Case)
    {
    case 1:
    case 2:
        SWAP2(N1, N2, N3, N6, N5, N4);
        return;
    case 5:
        SWAP3(N1, N2, N4, N6, N5, N4, N6, N2, N3);
        return;
    case 6:
        SWAP2(N3, N4, N5, N1, N2, N3);
        return;
    default:
        printf("Make3OptMove failed!");
    }
}


/*
 * 进行一次4交换
 */

void Make4OptMove(Node * N1, Node * N2, Node * N3, Node * N4,
                  Node * N5, Node * N6, Node * N7, Node * N8, int Case)
{
    if (SUC(N1) != N2)
        IsPathReversed ^= 1;
    switch (Case)
    {
    case 1:
    case 2:
        SWAP3(N1, N2, N3, N6, N5, N4, N7, N8, N1);
        return;
    case 3:
    case 4:
        SWAP3(N1, N2, N3, N8, N7, N6, N5, N8, N1);
        return;
    case 5:
        if (IsBetween(N2, N7, N6))
            SWAP3(N5, N6, N7, N5, N8, N3, N3, N8, N1);
        else  if (!IsBetween(N2, N7, N3))
            SWAP3(N5, N6, N7, N2, N1, N4, N1, N4, N5);
        else
            SWAP3(N1, N2, N7, N7, N2, N3, N4, N7, N6);
        return;
    case 6:
        SWAP3(N3, N4, N5, N6, N3, N2, N1, N6, N7);
        return;
    case 7:
        SWAP3(N6, N5, N8, N2, N1, N4, N8, N5, N4);
        return;
    case 11:
        SWAP3(N1, N2, N7, N3, N4, N5, N3, N6, N7);
        return;
    case 12:
        SWAP3(N3, N4, N5, N7, N8, N1, N3, N6, N7);
        return;
    case 15:
        SWAP3(N3, N4, N5, N3, N6, N7, N8, N3, N2);
        return;
    default:
        printf("Make4OptMove failed!");
    }
}


