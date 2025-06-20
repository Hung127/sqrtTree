#ifndef benchmark_h
#define benchmark_h

#include "BasicLibraries.h"
#include "SqrtTree.h"
#include "SegmentTree.h"
#include "FenwickTree.h"
#include <cstdlib>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <string>
#define ll long long

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

struct BenchmarkResult {
    string dataStructureName;
    ll buildTime; // Thoi gian build
    ll totalUpdateTime; // Tong thoi gian cap nhat (μs)
    ll totalQueryTime; // Tong thoi gian cua truy van (μs)
    int numUpdates; // So luong cap nhat
    int numQueries; // So luong truy van
    double avgUpdateTime; // Thoi gian truy cap trung binh (μs)
    double avgQueryTime; // Thoi gian truy van trung binh (μs)

    BenchmarkResult() : buildTime(0), totalUpdateTime(0), totalQueryTime(0),
        numUpdates(0), numQueries(0), avgUpdateTime(0), avgQueryTime(0) {
    }

    void calculateAverages() {
        avgUpdateTime = numUpdates > 0 ? (double)totalUpdateTime / numUpdates : 0;
        avgQueryTime = numQueries > 0 ? (double)totalQueryTime / numQueries : 0;
    }
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

// Ham benchmark cho SqrtTree
BenchmarkResult benchmarkSqrtTree(const string& filename) {
    BenchmarkResult result;
    result.dataStructureName = "SqrtTree";

    ifstream in(filename);
    if (!in.is_open()) {
        cerr << "Khong the mo file " << filename << endl;
        return result;
    }

    int n, q;
    in >> n >> q;

    vector<int> arr(n);
    for (int i = 0; i < n; i++) {
        in >> arr[i];
    }

    // Do thoi gian xay dung
    auto startBuild = high_resolution_clock::now();
    SqrtTree sqrtTree(arr);
    auto endBuild = high_resolution_clock::now();
    result.buildTime = duration_cast<microseconds>(endBuild - startBuild).count();

    // Xu ly cac truy van
    for (int i = 0; i < q; i++) {
        int type, x, y;
        in >> type >> x >> y;

        if (type == 1) { // Update
            auto startUpdate = high_resolution_clock::now();
            sqrtTree.update(x, y);
            auto endUpdate = high_resolution_clock::now();
            result.totalUpdateTime += duration_cast<microseconds>(endUpdate - startUpdate).count();
            result.numUpdates++;
        }
        else { // Query
            auto startQuery = high_resolution_clock::now();
            sqrtTree.query(x, y);
            auto endQuery = high_resolution_clock::now();
            result.totalQueryTime += duration_cast<microseconds>(endQuery - startQuery).count();
            result.numQueries++;
        }
    }

    in.close();
    result.calculateAverages();
    return result;
}

// Ham benchmark cho SegmentTree
BenchmarkResult benchmarkSegmentTree(const string& filename) {
    BenchmarkResult result;
    result.dataStructureName = "SegmentTree";

    ifstream in(filename);
    if (!in.is_open()) {
        cerr << "Khong the mo file " << filename << endl;
        return result;
    }

    int n, q;
    in >> n >> q;

    vector<int> arr(n);
    for (int i = 0; i < n; i++) {
        in >> arr[i];
    }

    // Do thoi gian xay dung
    auto startBuild = high_resolution_clock::now();
    SegmentTree segTree(arr);
    auto endBuild = high_resolution_clock::now();
    result.buildTime = duration_cast<microseconds>(endBuild - startBuild).count();

    // Xu ly cac truy van
    for (int i = 0; i < q; i++) {
        int type, x, y;
        in >> type >> x >> y;

        if (type == 1) { // Update
            auto startUpdate = high_resolution_clock::now();
            segTree.set(x, y);
            auto endUpdate = high_resolution_clock::now();
            result.totalUpdateTime += duration_cast<microseconds>(endUpdate - startUpdate).count();
            result.numUpdates++;
        }
        else { // Query
            auto startQuery = high_resolution_clock::now();
            segTree.query(x, y);
            auto endQuery = high_resolution_clock::now();
            result.totalQueryTime += duration_cast<microseconds>(endQuery - startQuery).count();
            result.numQueries++;
        }
    }

    in.close();
    result.calculateAverages();
    return result;
}

// Ham benchmark cho FenwickTree
BenchmarkResult benchmarkFenwickTree(const string& filename) {
    BenchmarkResult result;
    result.dataStructureName = "FenwickTree";

    ifstream in(filename);
    if (!in.is_open()) {
        cerr << "Khong the mo file " << filename << endl;
        return result;
    }

    int n, q;
    in >> n >> q;

    vector<int> arr(n);
    for (int i = 0; i < n; i++) {
        in >> arr[i];
    }

    // Do thoi gian xay dung
    auto startBuild = high_resolution_clock::now();
    FenwickTree fenwickTree(arr);
    auto endBuild = high_resolution_clock::now();
    result.buildTime = duration_cast<microseconds>(endBuild - startBuild).count();

    // Xu ly cac truy van
    for (int i = 0; i < q; i++) {
        int type, x, y;
        in >> type >> x >> y;

        if (type == 1) { // Update
            auto startUpdate = high_resolution_clock::now();
            fenwickTree.update(x, y);
            auto endUpdate = high_resolution_clock::now();
            result.totalUpdateTime += duration_cast<microseconds>(endUpdate - startUpdate).count();
            result.numUpdates++;
        }
        else { // Query
            auto startQuery = high_resolution_clock::now();
            fenwickTree.query(x, y);
            auto endQuery = high_resolution_clock::now();
            result.totalQueryTime += duration_cast<microseconds>(endQuery - startQuery).count();
            result.numQueries++;
        }
    }

    in.close();
    result.calculateAverages();
    return result;
}

// Ham chay tat ca cac benchmark va so sanh
vector<BenchmarkResult> runAllBenchmarks(const string& filename) {
    vector<BenchmarkResult> results;

    cout << "Dang chay benchmark cho file: " << filename << endl;
    cout << "=======================================" << endl;

    // Benchmark SqrtTree
    cout << "Benchmark SqrtTree..." << endl;
    results.push_back(benchmarkSqrtTree(filename));

    // Benchmark SegmentTree
    cout << "Benchmark SegmentTree..." << endl;
    results.push_back(benchmarkSegmentTree(filename));

    // Benchmark FenwickTree
    cout << "Benchmark FenwickTree..." << endl;
    results.push_back(benchmarkFenwickTree(filename));

    return results;
}

// Ham in ket qua benchmark
void printBenchmarkResults(const vector<BenchmarkResult>& results) {
    cout << "\n======= KET QUA BENCHMARK =======" << endl;
    cout << left << setw(15) << "Data Structure"
        << setw(12) << "Build(μs)"
        << setw(12) << "Updates"
        << setw(15) << "Avg Update(μs)"
        << setw(12) << "Queries"
        << setw(15) << "Avg Query(μs)"
        << setw(15) << "Total Update(μs)"
        << setw(15) << "Total Query(μs)" << endl;
    cout << string(110, '-') << endl;

    for (const auto& result : results) {
        cout << left << setw(15) << result.dataStructureName
            << setw(12) << result.buildTime
            << setw(12) << result.numUpdates
            << setw(15) << fixed << setprecision(2) << result.avgUpdateTime
            << setw(12) << result.numQueries
            << setw(15) << fixed << setprecision(2) << result.avgQueryTime
            << setw(15) << result.totalUpdateTime
            << setw(15) << result.totalQueryTime << endl;
    }
    cout << endl;
}

// Ham ghi ket qua ra file CSV
void saveBenchmarkToCSV(const vector<BenchmarkResult>& results, const string& csvFilename) {
    ofstream csvFile(csvFilename);
    if (!csvFile.is_open()) {
        cerr << "Khong the tao file CSV: " << csvFilename << endl;
        return;
    }

    // Header
    csvFile << "DataStructure,BuildTime(μs),NumUpdates,AvgUpdateTime(μs),NumQueries,AvgQueryTime(μs),TotalUpdateTime(μs),TotalQueryTime(μs)\n";

    // Data rows
    for (const auto& result : results) {
        csvFile << result.dataStructureName << ","
            << result.buildTime << ","
            << result.numUpdates << ","
            << result.avgUpdateTime << ","
            << result.numQueries << ","
            << result.avgQueryTime << ","
            << result.totalUpdateTime << ","
            << result.totalQueryTime << "\n";
    }

    csvFile.close();
    cout << "Da luu ket qua benchmark vao file: " << csvFilename << endl;
}

// Ham chay toan bo experiment
void runExperiment(const string& filename, const TestConfig& config) {
    // Tao test case
    generateTest(filename, config);

    // Chay benchmark
    vector<BenchmarkResult> results = runAllBenchmarks(filename);

    // In ket qua
    printBenchmarkResults(results);

    // Luu ket qua ra file CSV
    string csvFilename = filename.substr(0, filename.find_last_of('.')) + "_results.csv";
    saveBenchmarkToCSV(results, csvFilename);
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
