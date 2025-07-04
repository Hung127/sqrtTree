#include "SqrtTree.h"
#include "SegmentTree.h"
#include "FenwickTree.h"
#include "BasicLibraries.h"
#include "benchmark.h"

const std::vector<std::string> dataTypes = { "Random", "Ascending", "Descending", "Constant" };
const std::vector<std::string> rangeTypes = { "Random_Range", "Small_Ranges", "Large_Ranges", "Fixed_Length" };

bool isNumber(const std::string& s) {
    if (s.empty()) return false;
    for (int i = 0; i < s.length(); i++) {
        if (!isdigit(s[i])) {
            return false;
        }
    }
    return true;
}

bool isDouble(const std::string& str) {
    if (str.empty()) return false;

    char* end;
    std::strtod(str.c_str(), &end);
    return *end == '\0';
}

ArrayPattern stringToArrayPattern(const std::string& str) {
    if (str == "Random") return RANDOM;
    if (str == "Ascending") return ASC;
    if (str == "Descending") return DESC;
    if (str == "Constant") return CONSTANT;
    return RANDOM; // Gia tri mac dinh
}

RangePattern stringToRangePattern(const std::string& str) {
    if (str == "Random_Range") return RANDOM_RANGE;
    if (str == "Small_Ranges") return SMALL_RANGES;
    if (str == "Large_Ranges") return LARGE_RANGES;
    if (str == "Fixed_Length") return FIXED_LENGTH;
    return RANDOM_RANGE; // Gia tri mac dinh
}

void showHelp() {
    std::cout << "Data Structure Benchmark Tool\n"
        << "=============================\n\n"
        << "Usage: ./benchmark [OPTIONS]\n\n"
        << "Options:\n"
        << "  -h                      Show this help message\n"
        << "  -i <file>               Input test file (if not provided, will generate test)\n"
        << "  -n <num>                Number of array elements\n"
        << "  -q <num>                Number of queries\n"
        << "  -u <ratio>              Update ratio 0.0-1.0\n"
        << "  -t <type>               Data pattern type\n"
        << "  -r <type>               Range pattern type\n"
        << "  --min <val>             Minimum value in array\n"
        << "  --max <val>             Maximum value in array\n"
        << "  --fixed-len <len>       Fixed length for ranges\n"
        << "Data Pattern Types:\n"
        << "  Random                  Random values\n"
        << "  Ascending               Ascending sorted values\n"
        << "  Descending              Descending sorted values\n"
        << "  Constant                All elements same value\n\n"
        << "Range Pattern Types:\n"
        << "  Random_Range            Random query ranges\n"
        << "  Small_Ranges            Small query ranges (≤ n/20)\n"
        << "  Large_Ranges            Large query ranges (≥ n/2)\n"
        << "  Fixed_Length            Fixed-length query ranges\n\n"
        << "Examples:\n"
        << "  ./benchmark -n 100000 -q 100000 -u 0.5 -t Random -r Large_Ranges --min 0 --max 100000\n";
}

void parseArgs(int argc, char* argv[], std::string& inputFile, int& n, int& numQueries,
    double& updateRatio, std::string& dataType,
    std::string& rangeType, int& minVal, int& maxVal, int& fixedLength) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        // Kiem tra tham so help
        if (arg == "-h") {
            showHelp();
            return;
        }
        // Tham so file dau vao
        else if (arg == "-i" && i + 1 < argc) {
            inputFile = argv[++i];
        }
        // Tham so so luong phan tu mang
        else if (arg == "-n" && i + 1 < argc) {
            if (isNumber(argv[i + 1])) {
                n = std::stoi(argv[++i]);
                if (n <= 0) {
                    std::cerr << "Error: Number of keys must be positive\n";
                    exit(1);
                }
            }
            else {
                std::cerr << "Error: Invalid number for -n option\n";
                exit(1);
            }
        }
        // Tham so so luong query
        else if (arg == "-q" && i + 1 < argc) {
            if (isNumber(argv[i + 1])) {
                numQueries = std::stoi(argv[++i]);
                if (numQueries <= 0) {
                    std::cerr << "Error: Number of queries must be positive\n";
                    exit(1);
                }
            }
            else {
                std::cerr << "Error: Invalid number for -q option\n";
                exit(1);
            }
        }
        // Tham so ti le cap nhat
        else if (arg == "-u" && i + 1 < argc) {
            if (isDouble(argv[i + 1])) {
                updateRatio = std::stod(argv[++i]);
                if (updateRatio < 0.0 || updateRatio > 1.0) {
                    std::cerr << "Error: Update ratio must be between 0.0 and 1.0\n";
                    exit(1);
                }
            }
            else {
                std::cerr << "Error: Invalid number for -u option\n";
                exit(1);
            }
        }
        // Tham so loai du lieu
        else if (arg == "-t" && i + 1 < argc) {
            std::string type = argv[++i];
            if (std::find(dataTypes.begin(), dataTypes.end(), type) != dataTypes.end()) {
                dataType = type;
            }
            else {
                std::cerr << "Error: Invalid data type. Use -h for available types\n";
                exit(1);
            }
        }
        // Tham so loai range
        else if (arg == "-r" && i + 1 < argc) {
            std::string type = argv[++i];
            if (std::find(rangeTypes.begin(), rangeTypes.end(), type) != rangeTypes.end()) {
                rangeType = type;
            }
            else {
                std::cerr << "Error: Invalid range type. Use -h for available types\n";
                exit(1);
            }
        }
        // Gia tri toi thieu
        else if (arg == "--min" && i + 1 < argc) {
            if (isNumber(argv[i + 1])) {
                minVal = std::stoi(argv[++i]);
            }
            else {
                std::cerr << "Error: Invalid number for --min option\n";
                exit(1);
            }
        }
        // Gia tri toi da
        else if (arg == "--max" && i + 1 < argc) {
            if (isNumber(argv[i + 1])) {
                maxVal = std::stoi(argv[++i]);
            }
            else {
                std::cerr << "Error: Invalid number for --max option\n";
                exit(1);
            }
        }
        // Do dai co dinh cho range
        else if (arg == "--fixed-len" && i + 1 < argc) {
            if (isNumber(argv[i + 1])) {
                fixedLength = std::stoi(argv[++i]);
                if (fixedLength <= 0) {
                    std::cerr << "Error: Fixed length must be positive\n";
                    exit(1);
                }
            }
            else {
                std::cerr << "Error: Invalid number for --fixed-len option\n";
                exit(1);
            }
        }
        else {
            // Thu phan tich nhu tham so vi tri (tuong thich nguoc)
            if (isNumber(arg)) {
                n = std::stoi(arg);
            }
            else if (std::find(dataTypes.begin(), dataTypes.end(), arg) != dataTypes.end()) {
                dataType = arg;
            }
            else {
                std::cerr << "Error: Unknown argument: " << arg << "\n";
                std::cerr << "Use -h for usage information\n";
                exit(1);
            }
        }
    }

    // Kiem tra moi quan he min/max
    if (minVal >= maxVal) {
        std::cerr << "Error: Minimum value must be less than maximum value\n";
        exit(1);
    }

    // Kiem tra do dai co dinh
    if (fixedLength > n) {
        std::cerr << "Error: Fixed length cannot be greater than array size\n";
        exit(1);
    }
}


void processArgs(int argc, char* argv[]) {
    std::string inputFile, dataType, rangeType;
    int n = 0, minVal, maxVal, fixedLength, numQueries;
    double updateRatio;

    if (argc <= 1) {
        showHelp();
        return;
    }

    parseArgs(argc, argv, inputFile, n, numQueries, updateRatio, dataType, rangeType, minVal, maxVal, fixedLength);

    // Neu co file input, chi chay benchmark khong tao file moi/
    if (!inputFile.empty()) {
        cout << "Sử dụng file input có sẵn: " << inputFile << endl;
        vector<BenchmarkResult> results = runAllBenchmarks(inputFile);
        printBenchmarkResults(results);

        // Luu ket qua ra file CSV
        string csvFilename = inputFile.substr(0, inputFile.find_last_of('.')) + "_results.csv";
        saveBenchmarkToCSV(results, csvFilename);
    }
    else {
        // Tao file moi voi config
        ArrayPattern arrPat = stringToArrayPattern(dataType);
        RangePattern rangePat = stringToRangePattern(rangeType);
        TestConfig config = create_custom_config(n, numQueries, updateRatio, minVal, maxVal, arrPat, rangePat, fixedLength);

        // Tao ten file mac đinh
        string defaultFilename = "test_" + to_string(n) + "_" + to_string(numQueries) + ".txt";
        runExperiment(defaultFilename, config);
    }
}

int main(int argc, char* argv[]) {
    processArgs(argc, argv);
    return 0;
}