#include <vector>
#include <future>
#include <thread>
#include <algorithm>
#include <chrono>

#pragma once

class SNP {
private:

    int width;
    int height;
    int x_ind[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };
    int y_ind[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };
    int chg_ptr[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int num_threads = 12;
    int elements = 8;
    int median_val[12];

    std::vector <std::vector <int>> med_list;
    std::vector <std::vector <int>> changes1;
    std::vector <std::vector <int>> changes2;
    std::vector <std::vector <int>> changes3;

    int median(std::vector<int> vec);
    void parallel_processing(int ref);
    void parallel_adjustments(int ref);
    std::future<void> spawn_calc(int ref);
    std::future<void> spawn_adj(int ref);

public:
    std::vector <std::vector <int>> image;

    void elapsed(std::chrono::high_resolution_clock::time_point t0, std::string process);
    void construct(int w, int h);
    void denoise();
};

