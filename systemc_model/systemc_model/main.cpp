#include <systemc.h>
#include "epc_controller.h"
#include "epc_accelerator.h"

int sc_main(int argc, char* argv[])
{
    sc_fifo<double> fifo1(10);
    sc_fifo<double> fifo2(10);

    EPC_Controller controller("controller");
    EPC_Accelerator accelerator("accelerator");

    controller.out_data(fifo1);
    controller.in_data(fifo2);

    accelerator.in_data(fifo1);
    accelerator.out_data(fifo2);

    //sc_start();

    sc_start(1000, SC_NS);  // Run long enough
    std::cout << "Simulation finished at " << sc_time_stamp() << std::endl;
    return 0;
}
