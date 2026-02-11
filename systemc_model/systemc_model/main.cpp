#include <systemc.h>
#include "epc_controller.h"
#include "epc_accelerator.h"

int sc_main(int argc, char* argv[]) {
    sc_fifo<UpdatePacket> to_accelerator(2);  // Small depth for pipelining demo
    sc_fifo<UpdatePacket> from_accelerator(2);

    EPC_Controller controller("Software_Controller");
    EPC_Accelerator accelerator("Hardware_Accelerator");

    controller.out_port(to_accelerator);
    controller.in_port(from_accelerator);
    accelerator.in_port(to_accelerator);
    accelerator.out_port(from_accelerator);

    std::cout << "Starting EPC Co-Design Simulation..." << std::endl;

    sc_start();  // Run until no more events

    std::cout << "Simulation finished at " << sc_time_stamp() << std::endl;

    return 0;
}
