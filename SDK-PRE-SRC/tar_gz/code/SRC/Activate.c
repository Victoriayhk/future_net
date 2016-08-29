#include "LKH.h"

/*
 * If a node is made "active", attempts are made to find an improving 
 * move with the node as anchor(锚) node, t1.
 *
 * The Active function makes a node active by inserting it into a 
 * queue of active nodes. FirstActive denotes the front node of 
 * the queue. LastActive denotes the rear. 
 *
 * The queue is implemented as a circular list in which the Next field 
 * of each Node references the successor node. 
 *
 * A node is member of the queue iff its Next != 0. The function has no 
 * effect if the node is already in the queue. 
 *
 * The function is called from the StoreTour function.  
*/

void Activate(Node * N)
{
    if (N->Next != 0)
        return;
    if (FirstActive == 0)//队列为空
        FirstActive = LastActive = N;
    else//队列不为空
        LastActive = LastActive->Next = N;
	//队尾指向队首、循环队列
    LastActive->Next = FirstActive;
}



/* 
 * The RemoveFirstActive function removes the first node in the list 
 * of "active" nodes (i.e., nodes to be tried as an anchor node, t1,
 * by the LinKernighan algorithm).
 *
 * The function returns a pointer to the removed node. 
 *
 * The list must not be empty before the call. 
 */

Node *RemoveFirstActive()
{
    Node *N = FirstActive;
    if (FirstActive == LastActive)//队列只有一个元素
        FirstActive = LastActive = 0;
    else
        LastActive->Next = FirstActive = FirstActive->Next;
    if (N)
        N->Next = 0;//表示不再队列里面
    return N;
}
