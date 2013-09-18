/*
    Copyright (c) 2002-2012 Tampere University of Technology.

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
 * @file SimpleSimulatorFrontend.hh
 *
 * Declaration of SimpleSimulatorFrontend class.
 *
 * @author Pekka J‰‰skel‰inen 2010,2012 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIMPLE_SIMULATOR_FRONTEND
#define TTA_SIMPLE_SIMULATOR_FRONTEND

#include <stdint.h>

#include "TCEString.hh"

class SimulatorFrontend;
class OperationExecutor;
class DetailedOperationSimulator;
class MemorySystem;
class Listener;
namespace TTAMachine {
    class Machine;
}

namespace TTAProgram {
    class Program;
}
/**
 * Simplified simulator frontend e.g. for calling from system level simulators
 * without needing to include the huge SimulatorFrontend.hh with lots of
 * include deps.
 */
class SimpleSimulatorFrontend {
public:
    SimpleSimulatorFrontend(TCEString machineFile);
    SimpleSimulatorFrontend(TCEString machineFile, TCEString programFile);
    virtual ~SimpleSimulatorFrontend();

    void step();
    void run();
    void stop();

    uint64_t cycleCount();

    bool isRunning() const;
    bool isFinished() const;
    bool hadRuntimeError() const;

    virtual void setOperationSimulator(
        const TCEString& fuName, DetailedOperationSimulator& sim);

    MemorySystem& memorySystem();

    void loadProgram(const std::string& fileName);
    const TTAMachine::Machine& machine() const;
    const TTAProgram::Program& program() const;

    SimulatorFrontend& frontend() { return *simFront_; }

    virtual bool registerEventListener(int event, Listener* listener);
    virtual bool unregisterEventListener(int event, Listener* listener);

private:
    SimulatorFrontend* simFront_;
};

#endif
