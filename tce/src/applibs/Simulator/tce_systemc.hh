/*
    Copyright (c) 2002-2010 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file tce_systemc.hh
 *
 * Wrappers and utilities for connecting TTA core simulation models to
 * SystemC simulations.
 *
 * @author Pekka Jääskeläinen 2010 (pjaaskel-no.spam-cs.tut.fi)
 */

#ifndef TCE_SYSTEMC_HH
#define TCE_SYSTEMC_HH

#include <stdint.h>
#include <systemc.h>
#include <Operation.hh>
#include <SimpleSimulatorFrontend.hh>
#include <DetailedOperationSimulator.hh>
#include <ExecutingOperation.hh>
#include <SimValue.hh>

/**
 * A sc_module wrapper for the whole TTA core simulation model.
 *
 * This model is sentive to clock signal. Propagates the clock signal
 * internally to other clocked datapath models (mainly FU models).
 */
class TTACore : public sc_module, public SimpleSimulatorFrontend {
public:
    sc_in<bool> clock;
    sc_in<bool> global_lock;

    SC_HAS_PROCESS(TTACore);

    TTACore(
        sc_module_name name, std::string machineFile, 
        std::string programFile) :
        sc_module(name), 
        SimpleSimulatorFrontend(machineFile, programFile, false, true) {
        SC_METHOD(step);
        sensitive << clock.pos();
        lockCycles_ = 0;
        instructionCycles_ = 0;
    }

    // number of cycles the tta has been locked during the simulation
    uint64_t lockCycles() const { return lockCycles_; }
    // number of instructions executed
    uint64_t instructionCycles() const { return instructionCycles_; }
private:
    void step() { 
        if (!global_lock) {
            SimpleSimulatorFrontend::step();
            ++instructionCycles_;
        } else {
            ++lockCycles_;
        }
    }
    uint64_t instructionCycles_;
    uint64_t lockCycles_;
};

/**
 * Macros used to override FU operation simulation behavior.
 */

#define TCE_SC_OPERATION_SIMULATOR(__CLASS__) \
    struct __CLASS__ : public DetailedOperationSimulator, public sc_module  

#define TCE_SC_OPERATION_SIMULATOR_CTOR(__CLASS__) \
    SC_HAS_PROCESS(__CLASS__); \
    __CLASS__(sc_module_name name) : sc_module(name)

#define TCE_SC_SIMULATE_STAGE \
    bool simulateStage(ExecutingOperation& __eop)

#define TCE_SC_SIMULATE_CYCLE_START \
    void simulateCycleStart()


#define TCE_SC_OPNAME \
    (__eop.operation().name())

#define TCE_SC_OPSTAGE \
    (__eop.stage())

#define TCE_SC_OPERATION \
    __eop.operation()

#define TCE_SC_INT(OPERAND) (__eop.io(OPERAND).intValue())
#define TCE_SC_UINT(OPERAND)(__eop.io(OPERAND).unsignedValue())
#define TCE_SC_FLT(OPERAND) (__eop.io(OPERAND).floatWordValue())
#define TCE_SC_DBL(OPERAND) (__eop.io(OPERAND).doubleWordValue())
#define TCE_SC_OUTPUT(OPERAND) (__eop.io(OPERAND))
#define TCE_SC_FUPORT_BWIDTH(OPERAND) (__eop.io(OPERAND).width())

#endif
