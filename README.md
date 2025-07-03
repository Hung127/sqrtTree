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
    
5.  The executable `sqrt_tree` will be generated.
    

## Usage

1.  **Compile the Code**:  
    Run the compilation command as shown above to generate an executable named `sqrt_tree`.
    
2.  **Run with Parameters**:
    
    -   To see the help message:
        
        ```bash
        ./sqrt_tree -h
        
        ```
        
    -   To process data with specific options (e.g., reading from `input.txt`, writing to `output.csv`, using 1000 nodes of `Balanced` type):
        
        ```bash
        ./sqrt_tree -i input.txt -o output.csv 1000 Balanced
        
        ```
        
    -   The program accepts the following command-line options:
        
        -   `-h`: Show this help message.
            
        -   `-i <file>`: Store data from the input file.
            
        -   `-o <file>`: Output results to the specified file.
            
        -   `<num>`: Input number of nodes (integer).
            
        -   `<type>`: Input type of tree data (options: `Random`, `Sequential`, `Clustered`, `Balanced`, `Skewed`).
            
        -   **Example**:
            
            ```
            ./sqrt_tree -i test.txt -o test.csv 1000 Balanced
            
            ```
            
            -   Reads input from `test.txt`.
            -   Writes output to `test.csv`.
            -   Processes 1000 nodes.
            -   Uses the `Balanced` tree type.
3.  **Output**:
    
    -   By default, output is displayed in the console.
    -   If `-o <file>` is specified, results are saved to the designated `.csv` file instead.
4.  **Customize**:
    
    -   Modify `main.cpp` to adjust the logic or integrate with `SqrtTree.h` for Square Root Tree functionality.
    -   Ensure all input files (e.g., `test.txt`) exist and contain the expected data format.

## Notes

-   Ensure all header files (`SqrtTree.h`, `BenchmarkAndAnalyst.h`, `Record.h`) are in the same directory as `main.cpp`.
-   Refer to the code comments in `SqrtTree.h` for detailed function explanations.
-   The program expects the input file (if `-i` is used) to be in a format it can parse.
-   The output `.csv` file will contain the processed results, likely related to query performance or tree structure details.
