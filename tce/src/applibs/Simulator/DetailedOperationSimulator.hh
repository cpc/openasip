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
 * @file DetailedOperationSimulator.hh
 *
 * Declaration of DetailedOperationSimulator class.
 *
 * @author Pekka J‰‰skel‰inen 2010 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DETAILED_OPERATION_SIMULATOR_HH
#define TTA_DETAILED_OPERATION_SIMULATOR_HH

class ExecutingOperation;

/**
 * Interface to allow modeling cycle-by-cycle operation simulation
 * behaviors.
 *
 * Used by MultiCycleOperationExecutor and the SystemC integration
 * layer (tce_systemc.hh). 
 *
 * @note each instance should be used to model the operation pipeline of 
 * a single FU only. Otherwise multiple cycle start notifications will be
 * recevied.
 */
class DetailedOperationSimulator {
public:

    virtual ~DetailedOperationSimulator() {}

   /**
    * Simulate a single stage in the operation's execution.
    *
    * This can be used in more detailed simulation models (SystemC at the moment)
    * to more accurately simulate each stage of operation's execution. 
    *
    * @param operation The operation being simulated.
    * @param cycle The stage/cycle to simulate, 0 being the trigger cycle.
    * @return Return true in case the simulation behavior was overridden by
    * the method implementation. In case false is returned, the default behavior
    * simulation is performed (by calling the OSAL TRIGGER at cycle 0 to produce
    * the results and just simulating the latency by making the result(s) 
    * visible at correct time).
    *
    */
    virtual bool simulateStage(ExecutingOperation& operation) = 0;

    /**
     * This is called once at the beginning of the TTA instruction cycle
     * before any simulateStage() calls for that cycle.
     *
     * Useful for initializing signals to their default values etc.
     */
    virtual void simulateCycleStart() {}
};
#endif
