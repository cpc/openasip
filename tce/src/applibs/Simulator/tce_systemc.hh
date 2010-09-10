/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @author Pekka J‰‰skel‰inen 2010 (pjaaskel-no.spam-cs.tut.fi)
 */

#ifndef TCE_SYSTEMC_HH
#define TCE_SYSTEMC_HH

#include "Operation.hh"
#include "OperationExecutor.hh"
#include "SimpleSimulatorFrontend.hh"
#include "systemc.h"

/**
 * A sc_module wrapper for the whole TTA core simulation model.
 *
 * This model is sentive to clock signal. Propagates the clock signal
 * internally to other clocked datapath models (mainly FU models).
 */
class TTACore : public sc_module, SimpleSimulatorFrontend {
public:
    sc_in<bool> clock;

    void step() { SimpleSimulatorFrontend::step(); }

    SC_HAS_PROCESS(TTACore);

    TTACore(
        sc_module_name name, 
        std::string machineFile, std::string programFile) :
        sc_module(name), 
        SimpleSimulatorFrontend(machineFile, programFile) {
        SC_METHOD(step);
        sensitive << clock;
    }
};


/**
 * An sc_module wrapper for a single FU simulation model.
 *
 * Can be used to replace the default FU simulation models with
 * SystemC-defined ones in order to model more detailed communication
 * behavior with other modules. Note: this module is not sensitive to SystemC
 * clock signal because the TTACore simulation model is responsible for
 * notifying it about clock events.
 */
class TTAFUState : public sc_module, FUState {
public:
    TTAFUState() : triggered_(false), idle_(false) {};
    virtual ~TTAFUState() {};

    /* More readable method names to override. Note: the results should
       be made visible at the end of cycle so the moves in the same
       instruction reading the FU read the old result. */
    virtual void cycleStart() {}
    virtual void cycleEnd() {}


    virtual void advanceClock() { cycleStart(); }
    virtual void endClock() { cycleEnd(); triggered_ = false; }
    void setTriggered() { triggered_ = true; idle_ = false; }
    void setOperation(Operation& op) { operation = &op; }

    SimValue& port(Operation& op, int operand) { 
        return *(bindings_[&op][operand - 1]);
    }

protected:
    bool triggered;
    Operation* operation;

    /* The operand to FU port bindings need to be initialized to point to
       the original port models. */
    typedef std::map<Operation*, std::vector<PortState*> > BindingIndex;
    BindingIndex bindings_;
};

#endif
