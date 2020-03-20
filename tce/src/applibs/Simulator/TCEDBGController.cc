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
 * @file TCEDBGController.cc
 *
 * Stand-in replacement for the FPGA controller in the HardwareDebugger branch on 
 * launchpad. Minimum implementation to get TCE to compile.
 *
 * @author Kalle Raiskila 2014
 */

#include "TCEDBGController.hh"
#include "SimValue.hh"

typedef MinimumAddressableUnit MAU;

TCEDBGController::TCEDBGController( 
    SimulatorFrontend& frontend,
    const TTAMachine::Machine& machine, 
    const TTAProgram::Program& program) : 
    RemoteController(frontend, machine, program) {
}

void
TCEDBGController::writeMem(
    Word /*address*/,
    MAU /*data*/,
    const AddressSpace& /*space*/) {
}

MAU
TCEDBGController::readMem(
    Word /*address*/,
    const AddressSpace& /*space*/) {
    return 0;
}

void
TCEDBGController::writeIMem(
    const char *,
    int /*size*/) {
}

void
TCEDBGController::step(double /*count = 1*/) {}

void
TCEDBGController::next(int /*count = 1*/) {}

void
TCEDBGController::run() {}

void
TCEDBGController::runUntil(UIntWord /*address*/) {}

void
TCEDBGController::reset() {
}
    
std::string
TCEDBGController::registerFileValue(
    const std::string& /*rfName*/, 
    int /*registerIndex = -1*/) {
    std::string rv="hello";
    return rv;
}
    
SimValue
TCEDBGController::immediateUnitRegisterValue(
    const std::string& /*iuName*/, int /*index = -1*/) {
    SimValue rv;
    return rv;
}
    
SimValue
TCEDBGController::FUPortValue(
    const std::string& /*fuName*/, 
    const std::string& /*portName*/) {
    SimValue rv;
    return rv;
}
    
InstructionAddress
TCEDBGController::programCounter() const {
    return 0;
}

/* vim: set ts=4 expandtab: */
