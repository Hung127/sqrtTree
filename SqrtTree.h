#ifndef SQRT_TREE
#define SQRT_TREE
#define SqrtTreeItem long long
#include "BasicLibraries.h"
using namespace std;

SqrtTreeItem op(const SqrtTreeItem &a, const SqrtTreeItem &b) {
    return (a + b);
}

int log2Up(int n) { // find the ceil(log2(n))
    int res = 0;
    while ((1 << res) < n) {
        res++;
    }
    return res;
}

// at a layer, we have the layers[i] is the blockSize on that layer
// we also have the log2(childBlockSize) which could be compute with the
// formula log2(chilBlockSize) = ceil(layers[i] / 2) on layer i
// a = (a + 1) >> 1 means that a = ceil(a / 2)
class SqrtTree {
    private:
        // ceilLog store the minimum k that 2^k >= n (n is input array size)
        // indexSize is number of blocks on the first layer
        int ceilLog, n, indexSize;
        // clz[i] is the highest (left most) bit 1 of number i
        // Ex: number 5 would be represented 0101, so the highest bit is 3 (counted from right to left) -> clz[5] = 3
        // layers: k = layers[i] is the log2(blockSize) on that layer
        // onLayer[i] is the block with size 2^i belongs to the layer onLayer[i], ex: onLayer[i] = 1 then the block size 2^2 is on layer 1
        vector<int> clz, layers, onLayer;
        vector<SqrtTreeItem> arr;
        // we can access a prefix of an element in a block by using prefix[layer][i].
        // [layer] would give us a hint about the size of a block on a layer, then we can compute the child block size of that layer
        // prefix[layer][i] is prefix of element i with the block size is the child block size on a layer
        // assume that we have an array with len = 5: {1, 2, 3, 4, 5} -> layers[0] = 3 (because 2^3 = 8 > 5)
        // so the block size is 2^3 = 8, child block size is 2^2 = 4
        // -> the prefix would be {1, 3, 6, 10,(new block) 5};
        // suffix is just like the prefix
        // between is a matrix that holds answers for the queries for sequences of whole child blocks on a layer
        // instead of accessing between using between[layer][i][j] -> 3d array, we convert [i][j] into [i * rowElements + j]
        // so we can access between using between[layer][i * rowElements + j]
        vector<vector<SqrtTreeItem>> prefix, suffix, between;

        // build prefix and suffix for a [l...r) block on a layer
        void buildBlock(int layer, int l, int r) {
            prefix[layer][l] = arr[l];
            for (int i = l + 1; i < r; i++) {
                prefix[layer][i] = op(prefix[layer][i - 1], arr[i]);
            }
            suffix[layer][r - 1] = arr[r - 1];
            for (int i = r - 2; i >= l; i--) {
                suffix[layer][i] = op(arr[i], suffix[layer][i + 1]);
            }
        }

        // build index for first layer
        void buildBetweenZero() {
            int childBlockSizeLog = (ceilLog + 1) >> 1;
            for (int i = 0; i < indexSize; i++) {
                // assign each child block answers
                arr[n + i] = suffix[0][i << childBlockSizeLog];
            }
            // build [n...n + indexSize - 1] as a separate SqrtTree with betweenOffset
            build(1, n, n + indexSize, (1 << ceilLog) - n);
        }

        void buildBetween(int layer, int lBound, int rBound, int betweenOffset) {
            int childBlockSizeLog = (layers[layer] + 1) >> 1;
            int childBlocksCountLog = layers[layer] >> 1;
            int childBlockSize = 1 << childBlockSizeLog;
            // childBlocksCount = ceil((rBound - lBound) / childBlockSize)
            int childBlocksCount = (rBound - lBound + childBlockSize - 1) >> childBlockSizeLog;
            for (int i = 0; i < childBlocksCount; i++) {
                // init empty answer
                SqrtTreeItem answer;
                for (int j = i; j < childBlocksCount; j++) {
                    SqrtTreeItem add = suffix[layer][lBound + (j << childBlockSizeLog)];
                    // accumulating answer
                    if (i == j) {
                        answer = add;    
                    } else {
                        answer = op(answer, add);
                    }
                    // access the right place in between to assign the answer
                    between[layer - 1][betweenOffset + lBound + (i << childBlocksCountLog) + j] = answer;
                }
            }
        }

        SqrtTreeItem query(int l, int r, int betweenOffset, int base) {
            // just one element
            if (l == r) {
                return arr[l];
            } else if (l + 1 == r) { // 2 elements
                return op(arr[l], arr[r]);
            }
            // find the layer that l and r are in the same block (not child block)
            int layer = onLayer[clz[(l - base) ^ (r - base)]];
            int childBlockSizeLog = (layers[layer] + 1) >> 1;
            int childBlockSize = 1 << childBlockSizeLog;
            int childBlocksCountLog = layers[layer] >> 1;
            // find the beginning child block that contains l
            // turn off all the unecessary bits to access the beginning of the block
            int lBound = (((l - base) >> layers[layer]) << layers[layer]) + base;
            
            // find between range (child block)
            int lBlock = ((l - lBound) >> childBlockSizeLog) + 1;
            int rBlock = ((r - lBound) >> childBlockSizeLog) - 1;
            SqrtTreeItem answer = suffix[layer][l];
            if (lBlock <= rBlock) {
                SqrtTreeItem add;
                // special case, we query using index array
                if (layer == 0) {
                    add = query(n + lBlock, n + rBlock, (1 << ceilLog) - n, n);
                } else {
                    // access the right answer in between array
                    add = between[layer - 1][betweenOffset + lBound + (lBound << childBlocksCountLog) + rBlock];
                }
                answer = op(answer, add);
            }
            answer = op(answer, prefix[layer][r]);
            return answer;
        }

        void updateBetweenZero(int blockIdx) {
            int childBlockSizeLog = (ceilLog + 1) >> 1;
            arr[n + blockIdx] = suffix[0][blockIdx << childBlockSizeLog];
            update(1, n, n + indexSize, (1 << ceilLog) - n, n + blockIdx);
        }

        // build prefix, suffix and between for current layer in [lBound...rBound]
        // we have a parameter called betweenOffset. This parameter is used for [n...n + idxSize - 1] in array
        // because the lBound for the first child block of the [n...n + idxSize - 1] would be n
        // we want to build the "between" FROM [1 << ceilLog], so we need to pass in a parameter called betweenOffset to make buildBetween function begin from [1 << ceilLog] between[layer] array.
        void build(int layer, int lBound, int rBound, int betweenOffset) {
            // base case: there is no more layer to build
            if (layer >= (int) layers.size()) {
                return;
            }
            int childBlockSizeLog = (layers[layer] + 1) >> 1;
            int childBlockSize = 1 << childBlockSizeLog;
            // build each child block iteratively
            for (int l = lBound; l < rBound; l += childBlockSize) {
                // we could have some un-full block (ex: the child block size was 3 but we only got 2 elements left)
                int r = min(l + childBlockSize, rBound);
                buildBlock(layer, l, r);
                // recursively build next layers for current child block
                build(layer + 1, l, r, betweenOffset);
            }
            // special case: first layer do not use a between
            if (layer == 0) {
                buildBetweenZero();
            } else {
                buildBetween(layer, lBound, rBound, betweenOffset);
            }
        }

        void update(int layer, int lBound, int rBound, int betweenOffset, int x) {
            if (layer >= (int)layers.size()) {
                return;
            }
            int bSzLog = (layers[layer] + 1) >> 1;
            int bSz = 1 << bSzLog;
            int blockIdx = (x - lBound) >> bSzLog;
            int l = lBound + (blockIdx << bSzLog);
            int r = min(l + bSz, rBound);
            buildBlock(layer, l, r);
            if (layer == 0) {
                updateBetweenZero(blockIdx);
            } else {
                buildBetween(layer, lBound, rBound, betweenOffset);
            }
            update(layer + 1, l, r, betweenOffset, x);
        }

    
    public:
        SqrtTreeItem query(int l, int r) {
            return query(l, r, 0, 0);
        }

        void update(int idx, const SqrtTreeItem &val) {
            arr[idx] = val;
            update(0, 0, n, 0, idx);
        }

        SqrtTree(const vector<SqrtTreeItem> &a)
        : n((int) arr.size()), ceilLog(log2Up(n)), onLayer(ceilLog + 1), arr(a), clz(1 << ceilLog) {
            // algorithm to compute clz
            clz[0] = 0; // base case
            for (int i = 1; i < clz.size(); i++) {
                clz[i] = clz[i >> 1] + 1;
            }
            int tempLog = ceilLog;
            while (tempLog > 1) {
                // layerSize is the right layer for onLayer[tempLog], base case: layerSize is 0 (empty)
                onLayer[tempLog] = (int) layers.size();
                layers.push_back(tempLog);
                // blockSize = 2^tempLog -> to compute the sqrt of blockSize, we make tempLog = ceil(tempLog / 2);
                tempLog = (tempLog + 1) >> 1;
                // but if we assigned tempLog to ceil(tempLog / 2), we would skip tempLog - 1 to ceil(tempLog / 2) + 1 inclusively
                // so we need to fill up skipped elements later
            }
            // fill
            for (int i = ceilLog - 1; i >= 0; i--) {
                onLayer[i] = max(onLayer[i], onLayer[i + 1]);
            }
            int childBlockSizeLog = (ceilLog + 1) >> 1;
            int childBlockSize = 1 >> childBlockSize;
            // we use layers.size() - 1 for between layers because the first layer (0) do not use a between, we use index
            int betweenLayers = max(0, (int) layers.size() - 1);
            // idx size is the number of child blocks on layer 0 (first layer)
            // idxSize = ceil(n / childBlockSize)
            int idxSize = (n + childBlockSize - 1) >> childBlockSizeLog;
            // add idxSize space to the array for child blocks
            // [n...n + idxSize - 1] is a subarray that each elements is the answer of a childBlock in the original array [0...n-1]
            arr.resize(n + idxSize);
            // each layer has a prefix and suffix, we treat first n elements as an array, next indexSize elements as an another distinct array array
            // we assign the default value for every element in prefix and suffix array
            prefix.assign(layers.size(), vector<SqrtTreeItem>(n + idxSize, 0));
            suffix.assign(layers.size(), vector<SqrtTreeItem>(n + idxSize, 0));
            // for every layer (except the first one), we create a array to holds the answers for all queries from a whole child block to another whole child blocks
            // this array has two part too, first (1 << ceilLog or 2^ceilLog) elements will holds the answers for childBlocks on layer - 1
            // the remaining (childBlockSize elements) is for all elements that we put at [n...n + idxSize-1] in the original array
            // we might not use all the space that we assigned (because 1 << ceilLog is >= n)
            between.assign(betweenLayers, vector<SqrtTreeItem>((1 << ceilLog) + childBlockSize, 0));
            // build the whole tree.
            build(0, 0, n, 0);
        }
};

#endif