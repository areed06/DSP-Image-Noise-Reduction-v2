#include "SnP.h"
#include <iostream>
#include <algorithm>
#include <future>
#include <thread>
#include <vector>
#include <chrono>
#include <cmath>

int SNP::median(std::vector<int> vec) {

        int med;
        std::sort(vec.begin(), vec.end());
        med = int(round((vec[3] + vec[4]) / 2));

        return med;
}

void SNP::parallel_processing(int ref) {
    // code to be executed on each individual thread

    for (int y = ref + 1; y < height - 1; y += num_threads) {

        for (int x = 1; x < width - 1; x++) {

            for (int e = 0; e < elements; e++) {
                med_list[ref][e] = image[y + y_ind[e]][x + x_ind[e]];
            }

            median_val[ref] = SNP::median(med_list[ref]);

            if ((image[y][x] - median_val[ref]) > 0 || (image[y][x] - median_val[ref]) < 0) {

                changes1[ref][chg_ptr[ref]] = y;
                changes2[ref][chg_ptr[ref]] = x;
                changes3[ref][chg_ptr[ref]] = median_val[ref];

                chg_ptr[ref]++;
            }

        }
    }

    return;
}

void SNP::parallel_adjustments(int ref) {
    // performs the adjustments present in the changes 2D vector

    for (int p = 0; p < changes1[ref].size(); p++) {
        image[changes1[ref][p]][changes2[ref][p]] = changes3[ref][p];
    }
}

std::future<void> SNP::spawn_calc(int ref) {
    return std::async([=]{SNP::parallel_processing(ref);});
}

std::future<void> SNP::spawn_adj(int ref) {
    return std::async([=]{SNP::parallel_adjustments(ref);});
}

// -------- PUBLIC MEMBERS ---------
std::vector <std::vector <int>> image;

void SNP::elapsed(std::chrono::high_resolution_clock::time_point t0, std::string process) {

    // outputs total processing time
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now(); // end timer
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>((t2 - t0));
    std::cout << std::endl << " -> " << process << " complete in " << time_span.count() << " seconds";
}

void SNP::construct(int w, int h) {

    // width and height in pixels
    width = w;
    height = h;

    int chg_len = ceil(w * h / num_threads);
    std::vector <int> v(3);
    std::vector <int> inner(w);
    inner.resize(w);
    v.resize(3);

    // allocates proper dimensions to the 2D vector
    std::vector <std::vector <int>> outer(height, inner);
    outer.resize(height);

    std::vector <int> chgs(chg_len);
    chgs.resize(chg_len);

    std::vector <std::vector <int>> chg_list(num_threads, chgs);
    chg_list.resize(num_threads);

    changes1 = chg_list;
    changes2 = chg_list;
    changes3 = chg_list;

    image = outer;

    std::vector <int> med(8);
    med.resize(8);

    std::vector <std::vector<int>> meds(num_threads, med);
    meds.resize(num_threads);

    med_list = meds;
}

void SNP::denoise() {

    std::vector <std::future<void>> process_threads(num_threads);
    std::vector <std::future<void>> adjust_threads(num_threads);

    std::chrono::high_resolution_clock::time_point t_a = std::chrono::high_resolution_clock::now(); // start time

    for (int f = 0; f < num_threads; f++) {

        //parallel_processing(f);
        process_threads[f] = spawn_calc(f);
    }

    for (int l = 0; l < num_threads; l++) {
        process_threads[l].wait();
    }

    elapsed(t_a, "Calculations");

    std::chrono::high_resolution_clock::time_point t_b = std::chrono::high_resolution_clock::now(); // start time

    int new_size = 0;
    for (int v = 0; v < num_threads; v++) {
        new_size += chg_ptr[v];
    }

    std::cout << std::endl << new_size << "/" << width * height << " (" << 100 * new_size / (width * height) << "%) of pixel values modified\n";

    for (int t = 0; t < num_threads; t++) {
        changes1[t].resize(chg_ptr[t]);
        changes1[t].shrink_to_fit();
        changes2[t].resize(chg_ptr[t]);
        changes2[t].shrink_to_fit();
        changes3[t].resize(chg_ptr[t]);
        changes3[t].shrink_to_fit();
    }

    for (int g = 0; g < num_threads; g++) {

        //parallel_adjustments(g);
        std::cout << "Thread " << g+1 << " spawned\n";
        adjust_threads[g] = spawn_adj(g);
    }

    for (int b = 0; b < num_threads; b++) {
        adjust_threads[b].wait();
        std::cout << "Thread " << b + 1 << " completed\n";
    }

    std::cout << "  ";
    elapsed(t_b, "Adjustments");
}