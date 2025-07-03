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
#include <ctime>
#include <random>

#define ll long long

using namespace std;
using namespace chrono;

class Timer {
public:
    Timer() {
        m_StartTimepoint = ::high_resolution_clock::now(); // get current time
    }

    ~Timer() {
        Stop();
    }

    auto Stop() {
        auto endTimePoint = chrono::high_resolution_clock::now();

        auto start = chrono::time_point_cast<chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
        auto end = chrono::time_point_cast<chrono::microseconds>(endTimePoint).time_since_epoch().count();

        return end - start;
    }

private:
    chrono::time_point<chrono::high_resolution_clock> m_StartTimepoint;
};

enum ArrayPattern {
    RANDOM = 0,
    ASC = 1,
    DESC = 2,
    CONSTANT = 3
};

enum RangePattern {
    RANDOM_RANGE = 0, // Random query ranges
    SMALL_RANGES = 1, // Short query ranges
    LARGE_RANGES = 2, // Long query ranges
    FIXED_LENGTH = 3 // Query ranges of fixed length
};

struct TestConfig {
    int n; // Array size
    int q; // Number of queries
    double ratio; // Update query ratio
    int	minVal; // Minimum value in the array
    int maxVal; // Maximum value in the array
    ArrayPattern arrPat; // Array generation pattern
    RangePattern rangePat; // Query range pattern
    int fixLength; // Fixed length for query range (if using FIXED_LENGTH)
};

struct BenchmarkResult {
    string dataStructureName;
    ll buildTime; // Build time
    ll totalUpdateTime; // Total update time (μs)
    ll totalQueryTime; // Total query time (μs)
    int numUpdates; // Number of updates
    int numQueries; // Number of queries
    double avgUpdateTime; // Average update time (μs)
    double avgQueryTime; // Average query time (μs)

    BenchmarkResult() : buildTime(0), totalUpdateTime(0), totalQueryTime(0),
        numUpdates(0), numQueries(0), avgUpdateTime(0), avgQueryTime(0) {
    }

    void calculateAverages() {
        avgUpdateTime = numUpdates > 0 ? (double)totalUpdateTime / numUpdates : 0;
        avgQueryTime = numQueries > 0 ? (double)totalQueryTime / numQueries : 0;
    }
};

// General partition function with comparator
template<typename Compare>
int partition(vector<int>& arr, int low, int high, Compare comp) {
    int pivot = arr[high]; // Choose the last element as pivot
    int i = low - 1; // Index of smaller element

    for (int j = low; j < high; j++) {
        // If current element satisfies comparator with pivot
        if (comp(arr[j], pivot)) {
            i++;
            swap(arr[i], arr[j]);
        }
    }

    swap(arr[i + 1], arr[high]);
    return i + 1;
}

// General quickSort function with comparator
template<typename Compare>
void quickSort(vector<int>& arr, int low, int high, Compare comp) {
    if (low < high) {
        int pi = partition(arr, low, high, comp);

        quickSort(arr, low, pi - 1, comp);
        quickSort(arr, pi + 1, high, comp);
    }
}

// Wrapper function for ascending sort
void quickSortASC(vector<int>& arr, int left, int right) {
    quickSort(arr, left, right, [](int a, int b) { return a <= b; });
}

// Wrapper function for descending sort
void quickSortDESC(vector<int>& arr, int left, int right) {
    quickSort(arr, left, right, [](int a, int b) { return a >= b; });
}

// Generate random integer in range [minVal, maxVal]
int randomInt(int minVal, int maxVal) {
    std::random_device rd; // Initialize seed for std::mt19937
    std::mt19937 gen(rd()); // Random number generator
    std::uniform_int_distribution<> dis(minVal, maxVal); // Random distribution

    return dis(gen);
}

// Generate random double in range [0.0, 1.0]
double randomDouble() {
    return (double)rand() / RAND_MAX;
}

// Generate array based on config
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

// Generate a pair of indices (l, r) according to the specified pattern
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
        int length = config.fixLength; // Use fixed length (if specified)
        l = randomInt(0, config.n - length); // Ensure there is enough space for a segment
        r = l + length - 1;
        // If r exceeds array size
        if (r >= config.n) {
            r = config.n - 1; // Return the last valid index
        }
        break;
    }
    default:
        break;
    }

    return { l, r };
}

// Generate test case according to config
void generateTest(const string& filename, const TestConfig& config) {
    ofstream out(filename);
    if (!out.is_open()) {
        std::cerr << "Cannot open file " << filename << " for writing" << endl;
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
    cout << "Created test case \"" << filename << "\" with:" << endl;
    cout << "- Array size: " << config.n << endl;
    cout << "- Number of queries: " << config.q << endl;
    cout << "- Update ratio: " << config.ratio * 100 << "%" << endl;
}

// Benchmark for SqrtTree
BenchmarkResult benchmarkSqrtTree(const string& filename) {
    BenchmarkResult result;
    result.dataStructureName = "SqrtTree";

    ifstream in(filename);
    if (!in.is_open()) {
        cerr << "Cannot open file " << filename << endl;
        return result;
    }

    int n, q;
    in >> n >> q;

    vector<int> arr(n);
    for (int i = 0; i < n; i++) {
        in >> arr[i];
    }

    // Measure build time
    {
        Timer timer;
        SqrtTree sqrtTree(arr);
        result.buildTime = timer.Stop;
    }

    // Handle queries
    for (int i = 0; i < q; i++) {
        int type, x, y;
        in >> type >> x >> y;

        if (type == 1) { // Update
            Timer timer;
            sqrtTree.update(x, y);
            result.totalUpdateTime += timer.Stop;
            result.numUpdates++;
        }
        else { // Query
            Timer timer;
            sqrtTree.query(x, y);
            result.totalQueryTime += timer.Stop;
            result.numQueries++;
        }
    }

    in.close();
    result.calculateAverages();
    return result;
}

// Benchmark for SegmentTree
BenchmarkResult benchmarkSegmentTree(const string& filename) {
    BenchmarkResult result;
    result.dataStructureName = "SegmentTree";

    ifstream in(filename);
    if (!in.is_open()) {
        cerr << "Cannot open file " << filename << endl;
        return result;
    }

    int n, q;
    in >> n >> q;

    vector<int> arr(n);
    for (int i = 0; i < n; i++) {
        in >> arr[i];
    }

    // Measure build time
    Timer timer
    SegmentTree segTree(arr);
    result.buildTime = timer.Stop;

    // Handle queries
    for (int i = 0; i < q; i++) {
        int type, x, y;
        in >> type >> x >> y;

        if (type == 1) { // Update
            Timer timer;
            segTree.set(x, y);
            result.totalUpdateTime += timer.Stop;
            result.numUpdates++;
        }
        else { // Query
            Timer timer;
            segTree.query(x, y);
            result.totalQueryTime += timer.Stop;
            result.numQueries++;
        }
    }

    in.close();
    result.calculateAverages();
    return result;
}

// Benchmark for FenwickTree
BenchmarkResult benchmarkFenwickTree(const string& filename) {
    BenchmarkResult result;
    result.dataStructureName = "FenwickTree";

    ifstream in(filename);
    if (!in.is_open()) {
        cerr << "Cannot open file " << filename << endl;
        return result;
    }

    int n, q;
    in >> n >> q;

    vector<int> arr(n);
    for (int i = 0; i < n; i++) {
        in >> arr[i];
    }

    // Measure build time
    Timer timer;
    FenwickTree fenwickTree(arr);
    result.buildTime = timer.Stop;

    // Handle queries
    for (int i = 0; i < q; i++) {
        int type, x, y;
        in >> type >> x >> y;

        if (type == 1) { // Update
            Timer timer;
            fenwickTree.update(x, y);
            result.totalUpdateTime += timer.Stop;
            result.numUpdates++;
        }
        else { // Query
            Timer timer;
            fenwickTree.query(x, y);
            result.totalQueryTime += timer.Stop;
            result.numQueries++;
        }
    }

    in.close();
    result.calculateAverages();
    return result;
}

// Run all benchmarks and compare
vector<BenchmarkResult> runAllBenchmarks(const string& filename) {
    vector<BenchmarkResult> results;

    cout << "Running benchmark for file: " << filename << endl;
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

// Print benchmark results
void printBenchmarkResults(const vector<BenchmarkResult>& results) {
    cout << "\n======= BENCHMARK RESULTS =======" << endl;
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

// Save benchmark results to a CSV file
void saveBenchmarkToCSV(const vector<BenchmarkResult>& results, const string& csvFilename) {
    ofstream csvFile(csvFilename);
    if (!csvFile.is_open()) {
        cerr << "Cannot create CSV file: " << csvFilename << endl;
        return;
    }

    // Header
    csvFile << "DataStructure,BuildTime(μs),NumUpdates,AvgUpdateTime(μs),NumQueries,AvgQueryTime(μs),TotalUpdateTime(μs),TotalQueryTime(μs)\n";

    // Data rows
    for (const auto& result : results) {
        csvFile << result.dataStructureName << ";"
            << result.buildTime << ";"
            << result.numUpdates << ";"
            << result.avgUpdateTime << ";"
            << result.numQueries << ";"
            << result.avgQueryTime << ";"
            << result.totalUpdateTime << ";"
            << result.totalQueryTime << "\n";
    }

    csvFile.close();
    cout << "Saved benchmark results to file: " << csvFilename << endl;
}

// Run the entire experiment
void runExperiment(const string& filename, const TestConfig& config) {
    // Generate test case
    generateTest(filename, config);

    // Run benchmarks
    vector<BenchmarkResult> results = runAllBenchmarks(filename);

    // Print results
    printBenchmarkResults(results);

    // Save results to CSV file
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
