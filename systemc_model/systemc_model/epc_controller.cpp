#include "epc_controller.h"
#include <iostream>
#include <random>
#include <cmath>
#include <string>
#include <functional>
#include <vector>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846  // Define for MSVC
#endif

void EPC_Controller::control_process() {
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    auto sphere = [](const double pos[]) -> double {
        double sum = 0.0;
        for (int d = 0; d < DIM; ++d) sum += pos[d] * pos[d];
        return sum;
        };

    auto rosenbrock = [](const double pos[]) -> double {
        double sum = 0.0;
        for (int d = 0; d < DIM - 1; ++d) {
            sum += 100.0 * (pos[d + 1] - pos[d] * pos[d]) * (pos[d + 1] - pos[d] * pos[d])
                + (pos[d] - 1.0) * (pos[d] - 1.0);
        }
        return sum;
        };


    // Array of benchmarks
    struct Benchmark {
        std::string name;
        std::function<double(const double[])> func;
    };

    std::vector<Benchmark> benchmarks = {
        {"Sphere",     sphere},
        {"Rosenbrock", rosenbrock},
    };

    double population[POP_SIZE * DIM];
    double fitness_vals[POP_SIZE];

    // Run EACH benchmark separately
    for (const auto& bench : benchmarks) {
        std::cout << "\n=== Running Benchmark: " << bench.name << " ===\n";

        // Re-initialize population
        for (int i = 0; i < POP_SIZE; ++i) {
            for (int d = 0; d < DIM; ++d) {
                population[i * DIM + d] = LB_VAL + dist(rng) * (UB_VAL - LB_VAL);
            }
            fitness_vals[i] = bench.func(&population[i * DIM]);
        }

        std::cout << "Initial best fitness: " << *std::min_element(fitness_vals, fitness_vals + POP_SIZE) << std::endl;

        for (int iter = 0; iter < MAX_ITER; ++iter) {
            // Find best
            int best_idx = 0;
            double best_fit = fitness_vals[0];
            for (int i = 1; i < POP_SIZE; ++i) {
                if (fitness_vals[i] < best_fit) {
                    best_fit = fitness_vals[i];
                    best_idx = i;
                }
            }

            double best_pos[DIM];
            std::copy(&population[best_idx * DIM], &population[best_idx * DIM + DIM], best_pos);

            // Prepare packet
            UpdatePacket packet;
            std::copy(best_pos, best_pos + DIM, packet.best_pos);
            std::copy(population, population + POP_SIZE * DIM, packet.current_pop);
            packet.best_idx = best_idx;
            packet.a = A_VAL;
            packet.b = B_VAL;
            packet.exploration_rate = 0.5 * (1.0 - iter / (double)MAX_ITER);

            for (int d = 0; d < DIM; ++d) {
                packet.lb[d] = LB_VAL;
                packet.ub[d] = UB_VAL;
            }

            double u = 0.05 * std::pow(1.0 - iter / (double)MAX_ITER, 2);
            for (int i = 0; i < POP_SIZE; ++i) {
                double D = compute_distance(&population[i * DIM], best_pos);
                packet.Q[i] = std::exp(-u * D);
            }

            out_port.write(packet);
            packet = in_port.read();

            std::copy(packet.current_pop, packet.current_pop + POP_SIZE * DIM, population);
            for (int i = 0; i < POP_SIZE; ++i) {
                fitness_vals[i] = bench.func(&population[i * DIM]);
            }

            std::cout << "Iteration " << iter << " | Best fitness: " << *std::min_element(fitness_vals, fitness_vals + POP_SIZE) << std::endl;
        }

        std::cout << "=== " << bench.name << " completed. Final best fitness: "
            << *std::min_element(fitness_vals, fitness_vals + POP_SIZE) << " ===\n\n";
    }

    std::cout << "All three benchmarks completed." << std::endl;
}
