
# Square Root Tree Project

## Introduction

The Square Root Tree is a data structure designed to optimize certain types of queries on a tree, such as finding the lowest common ancestor or computing distances between nodes. It leverages the square root decomposition technique to achieve efficient query performance by dividing the tree into blocks of size approximately equal to the square root of the number of nodes. This approach balances preprocessing time and query time, making it suitable for static tree structures. The `SqrtTree.h` file contains the implementation of this data structure.

## Installation

1.  Ensure a C++ compiler is installed (e.g., g++).
2.  Clone or download the repository containing the files: `.gitignore`, `SqrtTree.h`, `BenchmarkAndAnalyst.h`, `Record.h`, `main.cpp`, and this `README.md`.
3.  Navigate to the project directory in your terminal.
4.  Compile the project using the following command:
    
    ```bash
    g++ -o sqrt_tree main.cpp
    
    ```
    or
    ```bash
    make main
    ```
    
5.  The executable `sqrt_tree` will be generated (or `main` if you use `make main`). From now, when you see ./sqrt_tree, you can use ./main if you used `make main`.

## Usage

1.  **Compile the Code**:  
    Run the compilation command as shown above to generate an executable named `sqrt_tree`.
    
2.  **Run with Parameters**:  
    The program accepts the following command-line options:
    
    -   `-h`: Show the help message.
    -   `-i <file>`: Specify the input test file (if not provided, a test file will be generated).
    -   `-n <num>`: Number of array elements (must be positive).
    -   `-q <num>`: Number of queries (must be positive).
    -   `-u <ratio>`: Update ratio between 0.0 and 1.0.
    -   `-t <type>`: Data pattern type (options: `Random`, `Ascending`, `Descending`, `Constant`).
    -   `-r <type>`: Range pattern type (options: `Random_Range`, `Small_Ranges`, `Large_Ranges`, `Fixed_Length`).
    -   `--min <val>`: Minimum value in the array.
    -   `--max <val>`: Maximum value in the array (must be greater than `--min`).
    -   `--fixed-len <len>`: Fixed length for ranges (must be positive and not greater than array size).
    
    **Examples**:
    
    -   To see the help message:
        
        ```bash
        ./sqrt_tree -h
        
        ```
        
    -   To run with specific options (e.g., 100,000 elements, 100,000 queries, 50% update ratio, random data, large ranges):
        
        ```bash
        ./sqrt_tree -n 100000 -q 100000 -u 0.5 -t Random -r Large_Ranges --min 0 --max 100000
        
        ```
        
    -   To process data from an input file:
        
        ```bash
        ./sqrt_tree -i test.txt
        
        ```
        
3.  **Output**:
    
    -   By default, results are displayed in the console.
    -   If an input file is provided (using `-i`), results are saved to a `.csv` file with the same name as the input file but with a `_results.csv` suffix.
4.  **Customize**:
    
    -   Modify `main.cpp` to adjust the logic or integrate with `SqrtTree.h` for Square Root Tree functionality.
    - Ensure input files (e.g., test.txt) exist and are in the correct format as expected by the program.


