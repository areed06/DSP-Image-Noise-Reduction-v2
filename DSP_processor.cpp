// DSP_processor.cpp : Executes noise reduction processing on specified image.

#include <iostream>
#include <iomanip>
#include <cmath>
#include <chrono>
#include <vector>
#include <fstream>
#include <algorithm>
#include <future>
#include <stdio.h>
#include <thread>
#include <string>

// headers for the different denoise types
#include "SnP.h"

using namespace std;
using namespace chrono;

void elapsed(std::chrono::high_resolution_clock::time_point t0, std::string process) {

    // outputs total processing time
    high_resolution_clock::time_point t2 = high_resolution_clock::now(); // end timer
    duration<double> time_span = duration_cast<duration<double>>((t2 - t0));
    cout << endl << " -> " << process << " complete in " << time_span.count() << " seconds";
}

int main(int argc, char* argv[]) {

    cout << "Executable running from location:: " << argv[0] << endl;

    // variable declarations
    string i_file_path = argv[1];
    string o_file_path = argv[2];
    ofstream output;
    ifstream input;

    int width = atoi(argv[3]), height = atoi(argv[4]);
    SNP snp;

    // timing Code
    high_resolution_clock::time_point t1 = high_resolution_clock::now(); // start time

    snp.construct(width, height); // configures denoise class based on dimensions of image

    // reads input image from txt file
    errno_t returnValue;
    FILE* file_ptr; // pointer to file
    returnValue = fopen_s(&file_ptr, i_file_path.c_str(), "r"); // stdio.h file object

    // validates proper file open
    if (!file_ptr) {
        cout << "Error loading file..." << endl << "Process exited with code 0";
        exit(0);
    }
    else {

        // loads file data into 'snp.image' vector
        for (int k = 0; k < height; k++) {

            for (int m = 0; m < width; m++) {
                fscanf_s(file_ptr, "%i", &snp.image[k][m]);
            }
        }

        cout << "File successfully loaded.";
    }
    fclose(file_ptr);

    elapsed(t1, "Image intake");
    high_resolution_clock::time_point t2 = high_resolution_clock::now(); // image process time start

    snp.denoise(); // processes image to remove salt and pepper noise

    elapsed(t2, "Image processing");
    high_resolution_clock::time_point t3 = high_resolution_clock::now(); // file write time start

    // writes resulting image to txt file
    errno_t returnVal;
    FILE* out_ptr;
    returnVal = fopen_s(&out_ptr, o_file_path.c_str(), "w");

    if (!out_ptr) {
        cout << endl << "File creation failed..." << endl << "Ending with code 0";
        exit(0);
    }
    else {
        cout << endl << "Output file created...";
    }

    for (int c = 0; c < height; c++) {

        for (int d = 0; d < width; d++) {
            fprintf(out_ptr, "%i", snp.image[c][d]);

            if (d != width - 1) {
                fprintf(out_ptr, ",");
            }
            else {
                fprintf(out_ptr, "\n");
            }
        }
    }
    fclose(out_ptr);

    elapsed(t3, "Output write");
    elapsed(t1, "All processes");
    cout << endl;

    return 0;
}

// ---------------NOTES------------------
// ** To get array from function, return ARRAY but function datatype is POINTER
//      ** functions CAN return vectors tho
// ** Create classes in a way such that you can't cause errors by running one member function before another
//      ** i.e make member functions run all necessary functions internally rather than needing to be called in main loop
// ** Duplicate values handled by ignoring them in the partitioning
// ** Speed up run time by only declaring variables once then modifying values later (potentially multiple times)
// ** fstream is much slower than stdio.h
// ** How to resolve C6387 issue?

// ---------THINGS-TO-RESEARCH-----------
// ** stdio.h functions
// ** how to use 'functional' library to pass functions to other functions

// ---------------IDEAS------------------
// ** Is it better to use a 1D vector or 2D array to store all the image values?
// ** Need to introduce flexibility in the dimensions of the image