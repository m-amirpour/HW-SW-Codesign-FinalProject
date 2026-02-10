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
    std::uniform_real_distribution<double> noise_dist(-1.0, 1.0);

    std::cout << "Hardware accelerator ready." << std::endl;

    while (true) {
        UpdatePacket packet = in_port.read();

        for (int i = 0; i < POP_SIZE; ++i) {
            if (i == packet.best_idx) continue;

            double current[DIM];
            std::copy(&packet.current_pop[i * DIM], &packet.current_pop[i * DIM + DIM], current);

            double this_Q = packet.Q[i];

            double temp_pos[DIM];
            get_adjusted_position(current, packet.best_pos, this_Q, packet.b, packet.a, temp_pos, rng);

            // Add exploration noise
            for (int d = 0; d < DIM; ++d) {
                double noise_val = noise_dist(rng) * (packet.ub[d] - packet.lb[d]) * packet.exploration_rate;
                temp_pos[d] += noise_val;
                temp_pos[d] = std::max(packet.lb[d], std::min(packet.ub[d], temp_pos[d]));
            }

            // Write back
            std::copy(temp_pos, temp_pos + DIM, &packet.current_pop[i * DIM]);

            wait(10, SC_NS);  // Simulate per-penguin hardware latency
        }

        out_port.write(packet);
    }
}
