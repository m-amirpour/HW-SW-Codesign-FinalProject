#ifndef EPC_CONTROLLER_H
#define EPC_CONTROLLER_H

#include <systemc.h>

SC_MODULE(EPC_Controller)
{
    sc_fifo_out<double> out_data;
    sc_fifo_in<double> in_data;

    void control();

    SC_CTOR(EPC_Controller)
    {
        SC_THREAD(control);
    }
};

#endif
