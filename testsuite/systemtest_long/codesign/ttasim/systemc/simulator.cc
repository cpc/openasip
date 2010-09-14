#include <iostream>

#include "systemc.h"
#include "tce_systemc.hh"

#include "mem_map.h"

//#define DEBUG

SC_MODULE(SharedReg) {
    sc_in<int> input;
    sc_out<int> output;
    sc_in<bool> updateValue;
    sc_in<bool> clock;

    int value;

    void run() {
        if (updateValue) {
            value = input;
        }
        output = value;
    }

    SC_CTOR(SharedReg) {
        SC_METHOD(run);
        sensitive << clock.pos();
        sensitive << input;
        sensitive << updateValue;
        value = 0;
    }
};

TCE_SC_OPERATION_SIMULATOR(LSUModel) {
    /*
      The same LSU model is shared between the sender and
      the receiver TTAs. The former writes to the data reg and
      reads the busy reg, latter vice-versa. */
    sc_in<int> reg_value_in;
    sc_out<int> reg_value_out;
    sc_out<bool> reg_value_update;

    TCE_SC_OPERATION_SIMULATOR_CTOR(LSUModel) {}

    TCE_SC_SIMULATE_CYCLE_START {
        // update signal to 0 so we don't update any garbage
        // to the register unless a write operation writes 
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

int sc_main(int argc, char* argv[]) {

    // 100MHz clock frequency (1 us clock period)
    sc_clock clk("clock", 1, SC_US);
    
    sc_signal<bool> glock;
    sc_signal<int> busyRegDataIn;
    sc_signal<int> dataRegDataIn;
    sc_signal<int> busyRegDataOut;
    sc_signal<int> dataRegDataOut;
    sc_signal<bool> busyRegUpdate;
    sc_signal<bool> dataRegUpdate;

    SharedReg dataReg("data_reg");
    dataReg.input(dataRegDataIn);
    dataReg.output(dataRegDataOut);
    dataReg.updateValue(dataRegUpdate);
    dataReg.clock(clk.signal());

    SharedReg busyReg("busy_reg");
    busyReg.input(busyRegDataIn);
    busyReg.output(busyRegDataOut);
    busyReg.updateValue(busyRegUpdate);
    busyReg.clock(clk.signal());

    // the sender TTA:

    TTACore sender_tta("sender_tta", "mmio.adf", "mmio_send.tpef");
    sender_tta.clock(clk.signal());
    sender_tta.global_lock(glock);

    // the LSU writes to the data register and reads from the 
    // busy reg to synchronize

    LSUModel lsu1("LSU1");
    sender_tta.setOperationSimulator("LSU", lsu1);
    lsu1.reg_value_in(busyRegDataOut);
    lsu1.reg_value_out(dataRegDataIn);
    lsu1.reg_value_update(dataRegUpdate);

    // the receiver TTA:

    TTACore recv_tta("recv_tta", "mmio.adf", "mmio_recv.tpef");
    recv_tta.clock(clk.signal());
    recv_tta.global_lock(glock);

    // the LSU writes to the busy reg to synchronize the execution
    // and reads from the data reg

    LSUModel lsu2("LSU2");
    recv_tta.setOperationSimulator("LSU", lsu2);
    lsu2.reg_value_in(dataRegDataOut);
    lsu2.reg_value_out(busyRegDataIn);
    lsu2.reg_value_update(busyRegUpdate);

    // simulate for 0.2 sec = 200K cycles
    sc_time runtime(0.2, SC_SEC);
    sc_start(runtime);

#ifdef DEBUG
    std::cout << std::endl;
    std::cout 
        << "sender_tta: instruction count = " 
        << sender_tta.instructionCycles()
        << " lock cycles = " 
        << sender_tta.lockCycles() << std::endl;

    std::cout 
        << "recv_tta: instruction count = " 
        << recv_tta.instructionCycles()
        << " lock cycles = " 
        << recv_tta.lockCycles() << std::endl;

    std::cout << std::endl;
    std::cerr << std::endl;
#endif
    return EXIT_SUCCESS;
}
