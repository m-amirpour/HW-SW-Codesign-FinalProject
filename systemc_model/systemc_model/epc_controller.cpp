#include "epc_controller.h"
#include <iostream>

void EPC_Controller::control()
{
    std::cout << "Simulation started - processing 10 penguin updates..." << std::endl;

    double fake_best = 0.0;  // Simulate a fixed "best" position for demo

    for (int i = 1; i <= 10; ++i)
    {
        double current_pos = static_cast<double>(i);  // Fake current position

        // Send pair: current + best
        out_data.write(current_pos);
        out_data.write(fake_best);

        // Receive updated position
        double new_pos = in_data.read();

        std::cout << "Penguin update " << i
            << ": current=" << current_pos
            << ", best=" << fake_best
            << " → new_pos=" << new_pos << std::endl;
    }

    std::cout << "All updates complete." << std::endl;
}