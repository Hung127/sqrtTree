#ifndef SEGMENT_TREE
#define SEGMENT_TREE
#define SegmentTreeItem int

#include "BasicLibraries.h"
using namespace std;


class SegmentTree {
private:
	vector<int> tree;
	vector<int> lazy;
	vector<int> arr;

	int n; //Size of origin array
	int size; //Size of segment tree;

    //Create Segment tree from origin array
    void buildTree(int node, int start, int end) {
        if (start == end) {
            // Leaf node
            tree[node] = arr[start];
            return;
        }

        int mid = start + (end - start) / 2;
        int leftChild = 2 * node + 1;
        int rightChild = 2 * node + 2;

        // Build leftChild and rightChild
        buildTree(leftChild, start, mid);
        buildTree(rightChild, mid + 1, end);

        // Fusion leftChild and rightChild
        tree[node] = tree[leftChild] + tree[rightChild];
    }

    //Lazy propagation
    void propagate(int node, int start, int end) {
        if (lazy[node] != 0) {
            //Update current node
            tree[node] += (end - start + 1) * lazy[node];

            if (start != end) {
                //If not leaf node, push lazy to child node
                lazy[2 * node + 1] += lazy[node];
                lazy[2 * node + 2] += lazy[node];
            }

            // Update value of lazy of current node
            lazy[node] = 0;
        }
    }

    //Query sumRange
    int queryRange(int node, int start, int end, int left, int right) {
        //Lazy value passing before processing
        propagate(node, start, end);

        //Non-intersecting case
        if (start > right || end < left) {
            return 0;
        }

        //Entirely in query range
        if (start >= left && end <= right) {
            return tree[node];
        }

        //Part of the query
        int mid = start + (end - start) / 2;
        int leftSum = queryRange(2 * node + 1, start, mid, left, right);
        int rightSum = queryRange(2 * node + 2, mid + 1, end, left, right);

        return leftSum + rightSum;
    }

    //Update value at point
    void updatePoint(int node, int start, int end, int idx, int val) {
        //Lazy value passing before processing
        propagate(node, start, end);

        //Out of range
        if (idx < start || idx > end) {
            return;
        }

        //Found updated node
        if (start == end) {
            tree[node] = val;
            arr[idx] = val;
            return;
        }

        int mid = start + (end - start) / 2;
        int leftChild = 2 * node + 1;
        int rightChild = 2 * node + 2;

        //Update in suitable tree
        if (idx <= mid) {
            updatePoint(leftChild, start, mid, idx, val);
        }
        else {
            updatePoint(rightChild, mid + 1, end, idx, val);
        }

        //Update current node
        tree[node] = tree[leftChild] + tree[rightChild];
    }

    //Update range
    void updateRange(int node, int start, int end, int left, int right, int val) {
        //Lazy value passing before processing
        propagate(node, start, end);

        //Out of range
        if (start > right || end < left) {
            return;
        }

        //Entirely in query range
        if (start >= left && end <= right) {
            //Update current node
            tree[node] += (end - start + 1) * val;

            //If not leaf node, push to lazy
            if (start != end) {
                lazy[2 * node + 1] += val;
                lazy[2 * node + 2] += val;
            }

            return;
        }

        //Part of query range
        int mid = start + (end - start) / 2;
        updateRange(2 * node + 1, start, mid, left, right, val);
        updateRange(2 * node + 2, mid + 1, end, left, right, val);

        //Update current node
        tree[node] = tree[2 * node + 1] + tree[2 * node + 2];
    }
public:
    SegmentTree(const vector<int>& input) {
        arr = input;
        n = input.size();

        //Calculate size of segment tree
        int height = (int)(ceil(log2(n)));
        size = 2 * (int)pow(2, height) - 1;

        tree.resize(size, 0);
        lazy.resize(size, 0);

        buildTree(0, 0, n - 1);
    }

    //Change value
    void update(int idx, int val) {
        if (idx < 0 || idx >= n) {
            cout << "Invalid index!" << std::endl;
            return;
        }
        updatePoint(0, 0, n - 1, idx, val);
    }

    //Update by adding value
    void updateRange(int left, int right, int val) {
        if (left < 0 || right >= n || left > right) {
            cout << "Invalid query range!" << std::endl;
            return;
        }
        updateRange(0, 0, n - 1, left, right, val);
    }

    //Sum
    int query(int left, int right) {
        if (left < 0 || right >= n || left > right) {
            cout << "Invalid query range!" << std::endl;
            return INT_MIN;
        }
        return queryRange(0, 0, n - 1, left, right);
    }
};