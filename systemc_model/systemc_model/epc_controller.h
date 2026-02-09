#pragma once

#include <systemc.h>
#include "epc_common.h"

SC_MODULE(EPC_Controller) {
    sc_fifo_out<UpdatePacket> out_port;
    sc_fifo_in<UpdatePacket> in_port;

    void control_process();

    // Simple Sphere fitness (matches Python benchmarks)
    double compute_fitness(const double pos[DIM]) {
        double sum = 0.0;
        for (int d = 0; d < DIM; ++d) {
            sum += pos[d] * pos[d];
        }
        return sum;
    }

    double compute_distance(const double x1[DIM], const double x2[DIM]) {
        double sum = 0.0;
        for (int d = 0; d < DIM; ++d) {
            double dx = x1[d] - x2[d];
            sum += dx * dx;
        }
        return std::sqrt(sum);
    }

    SC_CTOR(EPC_Controller) {
        SC_THREAD(control_process);
    }
};
