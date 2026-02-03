#include "epc_accelerator.h"
#include <cmath>
#include <cstdlib>   // For rand()
#include <iostream>  // Optional: for debug prints

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void EPC_Accelerator::process()
{
    // Optional: seed random (use time or fixed for repeatable results)
    // srand(42);  // Uncomment for fixed seed

    std::cout << "Accelerator ready." << std::endl;  // Debug print

    while (true)
    {
        double current_pos = in_data.read();
        double best_pos = in_data.read();

        double D = std::abs(best_pos - current_pos);
        double l = (double)rand() / RAND_MAX * 2.0 - 1.0;  // Random l in [-1,1]
        double b = 1.0;
        double spiral = std::exp(b * l) * std::cos(2 * M_PI * l);
        double new_pos = best_pos + D * spiral;

        wait(10, SC_NS);  // Hardware delay

        out_data.write(new_pos);
    }
}
