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

    void accelerate_process();

    // Translated Python functions
    void random_dim_pairs(int indices[DIM], std::mt19937 & rng, std::vector<std::pair<int, int>>&pairs, int& leftover);
    void get_new_position(const double current_pair[2], const double best_pair[2], double Q, double b, double a, double new_pair[2]);
    void get_adjusted_position(const double current[DIM], const double best[DIM], double Q, double b, double a, double new_pos[DIM], std::mt19937 & rng);

    std::mt19937 rng;

    SC_CTOR(EPC_Accelerator) {
        SC_THREAD(accelerate_process);
        rng.seed(123);  // Different seed from controller
    }
};
