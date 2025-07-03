#ifndef RECORD_H
#define RECORD_H
#include "BasicLibraries.h"
#include <fstream>
#define COLS 4
using namespace std;

class Record {
    private:
        vector<string> arr1;
        vector<float> arr2;
        bool isString = false;
    public:
        // create a record with 4 columns are strings
        Record(const string &a, const string &b, const string &c, const string &d) : arr1(COLS), isString(true) {
            cout << "THIS IS STRING!" << endl;
            arr1[0] = a;
            arr1[1] = b;
            arr1[2] = c;
            arr1[3] = d;
        }

        // create a record with 4 columns are floats
        Record(const float &a, const float &b, const float &c, const float &d) : arr2(COLS) {
            arr2[0] = a;
            arr2[1] = b;
            arr2[2] = c;
            arr2[3] = d;
        }

        // print to a CSV type
        void printToCSV(ostream &os) {
            for (int i = 0; i < COLS; i++) {
                if (isString) {
                    os << arr1[i] << ",";
                } else {
                    os << arr2[i] << ",";
                }
            }
            os << "\n";
        }
};


#endif