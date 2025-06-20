#ifndef benchmark_h
#define benchmark_h

#include "BasicLibraries.h"
#include "SqrtTree.h"
#include "SegmentTree.h"
#include "FenwickTree.h"
#include <cstdlib>
#include <chrono>
#include <fstream>
#include <string>

using namespace std;

enum ArrayPattern {
    RANDOM = 0,
    ASC = 1,
    DESC = 2,
    CONSTANT = 3
};

enum RangePattern {
    RANDOM_RANGE = 0, // Doan truy van ngau nhien
    SMALL_RANGES = 1, // Cac doan truy van ngan 
    LARGE_RANGES = 2, // Cac doan truy van dai
    FIXED_LENGTH = 3 // Cac doan truy van co do dai co dinh
};

struct TestConfig {
    int n; // Kich thuoc mang
    int q; // So luong truy van
    double ratio; // Ty le truy van cap nhat
    int	minVal; // Gia tri nho nhat trong mang
    int maxVal; // Gia tri lon nhat trong mang
    ArrayPattern arrPat; // Mau sinh mang
    RangePattern rangePat; // Mau sinh doan truy van
    int fixLength; // Do dai co dinh cho doan truy van (neu dung FIXED_LENGTH)
};

// Ham partition tong quat voi comparator
template<typename Compare>
int partition(vector<int>& arr, int low, int high, Compare comp) {
    int pivot = arr[high]; // Chon phan tu cuoi lam pivot
    int i = low - 1; // Chi so cua phan tu nho hon

    for (int j = low; j < high; j++) {
        // Neu phan tu hien tai thoa man dieu kien so sanh voi pivot
        if (comp(arr[j], pivot)) {
            i++;
            swap(arr[i], arr[j]);
        }
    }

    swap(arr[i + 1], arr[high]);
    return i + 1;
}

// Ham quickSort tong quat voi comparator
template<typename Compare>
void quickSort(vector<int>& arr, int low, int high, Compare comp) {
    if (low < high) {
        int pi = partition(arr, low, high, comp);

        quickSort(arr, low, pi - 1, comp);
        quickSort(arr, pi + 1, high, comp);
    }
}

// Ham wrapper de sap xep tang dan
void quickSortASC(vector<int>& arr, int left, int right) {
    quickSort(arr, left, right, [](int a, int b) { return a <= b; });
}

// Ham wrapper de sap xep giam dan
void quickSortDESC(vector<int>& arr, int left, int right) {
    quickSort(arr, left, right, [](int a, int b) { return a >= b; });
}

// Ham sinh so ngau nhien trong khoang [minVal, maxVal]
int randomInt(int minVal, int maxVal) {
    return minVal + rand() % (maxVal - minVal + 1);
}

// Ham sinh so ngau nhien trong khoang [0.0, 1.0]
double randomDouble() {
    return (double)rand() / RAND_MAX;
}

// Ham sinh mang theo config
vector<int> generateArray(const TestConfig& config) {
    vector<int> arr(config.n);

    switch (config.arrPat) {
    case RANDOM: {
        for (int i = 0; i < config.n; i++) {
            arr[i] = randomInt(config.minVal, config.maxVal);
        }
        break;
    }
    case ASC: {
        for (int i = 0; i < config.n; i++) {
            arr[i] = randomInt(config.minVal, config.maxVal);
        }
        quickSortASC(arr, 0, config.n - 1);
        break;
    }
    case DESC: {
        for (int i = 0; i < config.n; i++) {
            arr[i] = randomInt(config.minVal, config.maxVal);
        }
        quickSortDESC(arr, 0, config.n - 1);
        break;
    }
    case CONSTANT: {
        int val = randomInt(config.minVal, config.maxVal);
        for (int i = 0; i < config.n; i++) {
            arr[i] = val;
        }
        break;
    }
    default:
        break;
    }

    return arr;
}

// Ham sinh cap chi so (l, r) theo mau chi dinh
pair<int, int> generateRange(const TestConfig& config, int queryIdx = -1) {
    int l, r;
    switch (config.rangePat)
    {
    case RANDOM_RANGE: {
        l = randomInt(0, config.n - 1);
        r = randomInt(0, config.n - 1);
        if (l > r) {
            swap(l, r);
        }
        break;
    }
    case SMALL_RANGES: {
        l = randomInt(0, config.n - 1);
        int maxRange = max(1, config.n / 20);
        r = min(config.n - 1, l + randomInt(0, maxRange));
        break;
    }
    case LARGE_RANGES: {
        l = randomInt(0, config.n - 1);
        int minRange = config.n / 2;
        r = min(config.n - 1, l + randomInt(minRange, config.n - 1));
        if (r >= config.n) {
            r = config.n - 1;
        }
        break;
    }
    case FIXED_LENGTH: {
        int length = config.fixLength; // Lay do dai co dinh (Neu co)
        l = randomInt(0, config.n - length); // Dam bao co du choox cho 1 doan
        r = l + length - 1;
        // Neu r vuot qua kich thuoc mang
        if (r >= config.n) {
            r = config.n - 1; // Tra ve chi so hop le cuoi cung
        }
        break;
    }
    default:
        break;
    }

    return { l, r };
}

// Ham sinh test theo config
void generateTest(const string& filename, const TestConfig& config) {
    ofstream out(filename);
    if (!out.is_open()) {
        std::cerr << "Khong the mo duoc file" << filename << "de ghi" << endl;
        return;
    }

    srand(time(0));
    out << config.n << " " << config.q << "\n";

    vector<int> arr = generateArray(config);
    for (int i = 0; i < config.n; i++) {
        out << arr[i] << (i < config.n - 1 ? " " : "\n");
    }

    int updateQueries = 0;
    for (int i = 0; i < config.q; i++) {
        bool isUpdate = (randomDouble() < config.ratio);

        if (isUpdate && updateQueries < config.q) {
            int idx = randomInt(0, config.n - 1);
            int val = randomInt(config.minVal, config.maxVal);

            out << "1. " << idx << " " << val << "\n";
            updateQueries++;
        }
        else {
            auto [l, r] = generateRange(config, i);
            out << "0. " << l << " " << r << "\n";
        }
    }

    out.close();
    cout << "Da tao test case \"" << filename << "\" voi:" << endl;
    cout << "- Kich thuoc mang: " << config.n << endl;
    cout << "- So truy van: " << config.q << endl;
    cout << "- Ty le cap nhat: " << config.ratio * 100 << "%" << endl;
}

TestConfig create_custom_config(
    int n, int q, double updateRatio,
    int minVal, int maxVal,
    ArrayPattern arrPat,
    RangePattern rangePat,
    int fixLength = 0
) {
    TestConfig config;
    config.n = n;
    config.q = q;
    config.ratio = updateRatio;
    config.minVal = minVal;
    config.maxVal = maxVal;
    config.arrPat = arrPat;
    config.rangePat = rangePat;
    config.fixLength = fixLength;
    return config;
}
#endif // !benchmark_h
