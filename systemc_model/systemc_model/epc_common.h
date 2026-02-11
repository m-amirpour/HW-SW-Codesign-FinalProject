#pragma once

#include <systemc.h>
#include <array>
#include <random>
#include <ostream>  // Add this

const int POP_SIZE = 60;
const int DIM = 10;
const int MAX_ITER = 300;
const double LB_VAL = -5.12;
const double UB_VAL = 5.12;
const double A_VAL = 1.0;
const double B_VAL = 0.3;

struct UpdatePacket {
    double best_pos[DIM];
    double current_pop[POP_SIZE * DIM];  // Flattened
    double Q[POP_SIZE];
    double exploration_rate;
    double lb[DIM];
    double ub[DIM];
    double a;
    double b;
    int best_idx;

    friend std::ostream& operator<<(std::ostream& os, const UpdatePacket& p) {
        os << "UpdatePacket { best_idx=" << p.best_idx
            << ", exploration_rate=" << p.exploration_rate << " }";
        return os;  // Minimal print — enough for SystemC debug
    }
};;
