#ifndef EPC_ACCELERATOR_H
#define EPC_ACCELERATOR_H


#include <systemc.h>

SC_MODULE(EPC_Accelerator)
{
    sc_fifo_in<double> in_data;
    sc_fifo_out<double> out_data;

    void process();

    SC_CTOR(EPC_Accelerator)
    {
        SC_THREAD(process);
    }
};

#endif
