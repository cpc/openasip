/*
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
 * @file CustomDBGController.cc
 * @author Kalle Raiskila 2014
 *
 * Stub file for user's custom debugger control
 */

#include "CustomDBGController.hh"
#include "SimValue.hh"
#include "SimulatorFrontend.hh"
#include <cstdio>
    
#define DEBUG_REMOTE_DEBUGGER_CONTROLLER
#ifdef DEBUG_REMOTE_DEBUGGER_CONTROLLER
#define DPRINT(...) printf(__VA_ARGS__)
#define _WHERE() printf("%s:%d - %s\n", __FILE__, __LINE__, __func__)
#else
#define _WHERE()
#define DPRINT()
#endif

typedef MinimumAddressableUnit MAU;

CustomDBGController::CustomDBGController( 
    SimulatorFrontend& frontend,
    const TTAMachine::Machine& machine, 
    const TTAProgram::Program& program) : 
    RemoteController(frontend, machine, program) {
    _WHERE();
    // Write instructions to TTA memory. This is not done by TCE in the 
    // simulator modes. At some later point in time TCE fills the global 
    // data memory.
}

void 
CustomDBGController::writeMem(
    Word /*address*/,
    MAU /*data*/,
    const AddressSpace& /*space*/) {
    _WHERE();
}

MAU 
CustomDBGController::readMem(
    Word /*address*/,
    const AddressSpace& /*space*/) {
    return 0;
}

void
CustomDBGController::writeIMem(
    const char* /*buff*/,
    int /*size*/) {
    _WHERE();
}


void
CustomDBGController::step(double /*count = 1*/)
   throw (SimulationExecutionError) {
    _WHERE();
}

void
CustomDBGController::next(int /*count = 1*/)
    throw (SimulationExecutionError) {
    _WHERE();
}

void
CustomDBGController::run()
    throw (SimulationExecutionError) {
    _WHERE();
}

void
CustomDBGController::runUntil(UIntWord /*address*/)
    throw (SimulationExecutionError) {
    _WHERE();
}

void
CustomDBGController::reset() {
    _WHERE();
}
    
std::string
CustomDBGController::registerFileValue(
    const std::string& /*rfName*/, 
    int /*registerIndex*/ ) {
    std::string rv="hello";
    _WHERE();
    return rv;
}
    
SimValue
CustomDBGController::immediateUnitRegisterValue(
    const std::string& /*iuName*/,
    int /*index*/) {
    SimValue rv;
    return rv;
}
    
SimValue
CustomDBGController::FUPortValue(
    const std::string& /*fuName*/, 
    const std::string& /*portName*/) {
    SimValue rv;
    return rv;
}
    
InstructionAddress
CustomDBGController::programCounter() const {
    _WHERE();
    return 0;
}

/* vim: set ts=4 expandtab: */
