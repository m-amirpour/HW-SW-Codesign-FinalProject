#include "epc_accelerator.h"
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void EPC_Accelerator::random_dim_pairs(int indices[DIM], std::mt19937& rng, std::vector<std::pair<int, int>>& pairs) {
    std::iota(indices, indices + DIM, 0);
    std::shuffle(indices, indices + DIM, rng);
    pairs.clear();
    for (int i = 0; i < DIM - 1; i += 2) {
        pairs.emplace_back(indices[i], indices[i + 1]);
    }
}

void EPC_Accelerator::get_new_position(const double current_pair[2], const double best_pair[2], double Q, double b, double a, double new_pair[2]) {
    double x_i = current_pair[0], y_i = current_pair[1];
    double x_j = best_pair[0], y_j = best_pair[1];

    double theta_i = std::atan2(y_i, x_i);
    double theta_j = std::atan2(y_j, x_j);

    double theta_k = (1.0 / b) * std::log((1 - Q) * std::exp(b * theta_i) + Q * std::exp(b * theta_j));
    double r_k = a * std::exp(b * theta_k);

    new_pair[0] = r_k * std::cos(theta_k);
    new_pair[1] = r_k * std::sin(theta_k);
}

void EPC_Accelerator::get_adjusted_position(const double current[DIM], const double best[DIM], double Q, double b, double a, double new_pos[DIM], std::mt19937& rng) {
    std::copy(current, current + DIM, new_pos);

    int indices[DIM];
    std::vector<std::pair<int, int>> pairs;
    random_dim_pairs(indices, rng, pairs);

    for (const auto& p : pairs) {
        int i = p.first, j = p.second;
        double current_pair[2] = { current[i], current[j] };
        double best_pair[2] = { best[i], best[j] };
        double new_pair[2];
        get_new_position(current_pair, best_pair, Q, b, a, new_pair);
        new_pos[i] = new_pair[0];
        new_pos[j] = new_pair[1];
    }
}

void EPC_Accelerator::accelerate_process() {
    std::cout << "Hardware accelerator ready." << std::endl;

    while (true) {
        pkt = in_port.read();

        for (int i = 0; i < POP_SIZE; i += BATCH_SIZE) {
            for (int w = 0; w < BATCH_SIZE; ++w) {
                batch_indices[w] = i + w;
            }

            // Toggle start signal to create a value-change event
            batch_toggle = !batch_toggle;
            start_batch.write(batch_toggle);
            
            // Wait 1ns for workers to see the signal change
            wait(1, SC_NS);

            // Wait for all workers to finish
            for (int w = 0; w < BATCH_SIZE; ++w) {
                while (worker_done[w].read() != batch_toggle) {
                    wait(1, SC_NS);
                }
            }
        }

        out_port.write(pkt);
    }
}

void EPC_Accelerator::compute_one_penguin_position(int worker_id) {
    std::uniform_real_distribution<double> noise_dist(-1.0, 1.0);
    bool last_seen = start_batch.read();

    while (true) {
        // Wait for start_batch to change value
        while (start_batch.read() == last_seen) {
            wait(1, SC_NS);
        }
        last_seen = start_batch.read();

        int i = batch_indices[worker_id];

        if (i >= POP_SIZE || i == pkt.best_idx) {
            worker_done[worker_id].write(last_seen);
            continue;
        }

        double current[DIM];
        std::copy(&pkt.current_pop[i * DIM], &pkt.current_pop[i * DIM + DIM], current);

        double this_Q = pkt.Q[i];

        double temp_pos[DIM];
        get_adjusted_position(current, pkt.best_pos, this_Q, pkt.b, pkt.a, temp_pos, rng[worker_id]);

        for (int d = 0; d < DIM; ++d) {
            double noise_val = noise_dist(rng[worker_id]) * (pkt.ub[d] - pkt.lb[d]) * pkt.exploration_rate;
            temp_pos[d] += noise_val;
            temp_pos[d] = std::max(pkt.lb[d], std::min(pkt.ub[d], temp_pos[d]));
        }

        std::copy(temp_pos, temp_pos + DIM, &pkt.current_pop[i * DIM]);

        wait(10, SC_NS);  // Simulate HW latency

        worker_done[worker_id].write(last_seen);
    }
}