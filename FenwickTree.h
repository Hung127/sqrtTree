#ifndef SQRT_TREE
#define SQRT_TREE
#define SqrtTreeItem int
#include "BasicLibraries.h"
using namespace std;

class FenwickTree {
private:
	vector<int> bit;
	int size;
public:
	//Create tree
	FenwickTree(const vector<int>& arr) {
		size = arr.size();
		bit.resize(size + 1, 0); //BIT is indexed start at 1

		for (int i = 0; i < size; i++) {
			update(i, arr[i]);
		}
	}

	//Update node: arr[idx] = arr[idx] + val
	void update(int idx, int val) {
		idx++; //Trans idx start 0 to idx start at 1

		//Update nodes are affected
		while (idx <= size) {
			bit[idx] += val;
			idx += idx & -idx; //LSB
		}
	}

	//Update node: arr[idx] = val
	void set(int idx, int val) {
		int current = getSum(idx) - getSum(idx - 1);
		update(idx, val - current);
	}

	//Sum from arr[0] to arr[idx]
	int getSum(int idx) {
		if (idx < 0) return 0;

		idx++;
		int sum = 0;

		while (idx > 0) {
			sum += bit[idx];
			idx -= idx & -idx;
		}

		return sum;
	}

	//Sum range from left to right
	int sumRange(int left, int right) {
		return getSum(right) - getSum(left - 1);
	}

	//Get value
	int get(int idx) {
		return sumRange(idx, idx);
	}
};

#endif