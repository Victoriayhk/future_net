#include "future_net.h"


/**********************************************************************
堆栈

本堆栈的写法为了效率, 用起来和priority_queue还是有很多差别的, 使用请咨询本
菜鸟, 或者查看本源代码, 不难.

作者: 易惠康
日期: 2016-05-04

***********************************************************************/

Heap::Heap() {
	memset(pos, 0, sizeof(pos));
}

bool Heap::empty() {
	return sum == 0;
}

int Heap::size() {
	return sum;
}

bool Heap::has(int x) {
	return pos[x] != 0;
}

void Heap::init_with_key(int keyArr[]) {
	sum = 0;
	key = keyArr;
}

void Heap::push(int id) {
	lazy_insert(id);
	shift_up(id);
}

void Heap::change(int id) {
	shift_up(id);
	//shift_down(id);
}

int Heap::pop() {
	int x = h[1];		// 取出堆顶元素
	pos[x] = 0;

	h[1] = h[sum--];
	pos[h[1]] = 1;
	shift_down(h[1]);	// 调整剩下的元素

	return x;
}

inline void Heap::lazy_insert(int id) {	// 只插入元素, 不调堆
	pos[id] = ++sum;
	h[sum] = id;
}

void Heap::shift_up(int id) {		// 向上调
	int loc = pos[id], parent = loc / 2;

	while (parent && key[id] < key[h[parent]]) {
		h[loc] = h[parent];
		pos[h[loc]] = loc;
		loc = parent;
		parent /= 2;
	}
	h[loc] = id;
	pos[id] = loc;
}

void Heap::shift_down(int id) {	// 向下调
	int loc = pos[id], child;

	while (loc <= sum / 2) {
		child = 2 * loc;
		if (child < sum && key[h[child + 1]] < key[h[child]])
			child++;
		if (key[id] <= key[h[child]])
			break;
		h[loc] = h[child];
		pos[h[loc]] = loc;
		loc = child;
	}
	h[loc] = id;
	pos[id] = loc;
}
