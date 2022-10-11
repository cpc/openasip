#include <iostream>

#include "systemc.h"

#include "register.hh"
#include "lsu_model.hh"

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

    Register dataReg("data_reg");
    dataReg.input(dataRegDataIn);
    dataReg.output(dataRegDataOut);
    dataReg.updateValue(dataRegUpdate);
    dataReg.clock(clk.signal());

    Register busyReg("busy_reg");
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

    return EXIT_SUCCESS;
}
