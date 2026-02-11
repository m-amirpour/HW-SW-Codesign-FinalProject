#pragma once

#include <systemc.h>
#include "epc_common.h"
#include <random>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>

SC_MODULE(EPC_Accelerator) {
    sc_fifo_in<UpdatePacket> in_port;
    sc_fifo_out<UpdatePacket> out_port;

    static const int BATCH_SIZE = 5;

    // Shared state for workers
    UpdatePacket pkt;
    sc_signal<bool> start_batch;
    sc_signal<bool> worker_done[BATCH_SIZE];
    int batch_toggle;  // Toggles each batch to create a new edge

    int batch_indices[BATCH_SIZE];

    void accelerate_process();
    void compute_one_penguin_position(int worker_id);

    void worker_0() { compute_one_penguin_position(0); }
    void worker_1() { compute_one_penguin_position(1); }
    void worker_2() { compute_one_penguin_position(2); }
    void worker_3() { compute_one_penguin_position(3); }
    void worker_4() { compute_one_penguin_position(4); }

    void random_dim_pairs(int indices[DIM], std::mt19937 & rng, std::vector<std::pair<int, int>>&pairs);
    void get_new_position(const double current_pair[2], const double best_pair[2], double Q, double b, double a, double new_pair[2]);
    void get_adjusted_position(const double current[DIM], const double best[DIM], double Q, double b, double a, double new_pos[DIM], std::mt19937 & rng);

    std::mt19937 rng[BATCH_SIZE];

    SC_CTOR(EPC_Accelerator) {
        SC_THREAD(accelerate_process);

        SC_THREAD(worker_0);
        SC_THREAD(worker_1);
        SC_THREAD(worker_2);
        SC_THREAD(worker_3);
        SC_THREAD(worker_4);

        for (int i = 0; i < BATCH_SIZE; ++i) {
            rng[i].seed(2 + i);
        }
        batch_toggle = 0;
    }
};