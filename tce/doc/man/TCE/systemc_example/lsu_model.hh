#ifndef SC_LSU_MODEL_HH
#define SC_LSU_MODEL_HH

#include <systemc>
#include <tce_systemc.hh>
#include "mem_map.h"

TCE_SC_OPERATION_SIMULATOR(LSUModel) {
    /* The same LSU simulation model is used for the sender and
       the receiver TTAs. The former writes to the data reg and
       reads the busy reg, latter vice-versa. */
    sc_in<int> reg_value_in;
    sc_out<int> reg_value_out;
    sc_out<bool> reg_value_update;

    TCE_SC_OPERATION_SIMULATOR_CTOR(LSUModel) {}

    TCE_SC_SIMULATE_CYCLE_START {
        // initialize the update signal to 0 so we won't update any 
        // garbage to the register unless a write operation writes 
        // to it
        reg_value_update = 0;
    }

    TCE_SC_SIMULATE_STAGE {
        unsigned address = TCE_SC_UINT(1);
        // overwrite only the stage 0 simulation behavior of loads and 
        // stores to out of data memory addresses
        if (address <= LAST_DMEM_ADDR || TCE_SC_OPSTAGE > 0) {
            return false; 
        }
        // do not check for the address, assume all out of data memory
        // addresses update the shared register value
        if (TCE_SC_OPERATION.writesMemory()) {
            int value = TCE_SC_INT(2);
            reg_value_out.write(value);
            reg_value_update.write(1);
        } else { // a load, the operand 2 is the data output 
            int value = reg_value_in.read();
            TCE_SC_OUTPUT(2) = value;
        }
        return true;
    }
};

#endif
