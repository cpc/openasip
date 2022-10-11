/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file TTASimTandem.cc
 *
 * Implementation of a simulator that runs both compiled simulation and
 * the interpretive (debugging) simulation in parallel and compares the
 * execution to detect differences in the simulations, which indicate there's
 * a missimulation bug in one or both of the engines.
 *
 * @author Pekka Jääskeläinen 2009 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <iostream>
#include <algorithm>
#include <boost/format.hpp>
#include "Application.hh"
#include "Machine.hh"
#include "Program.hh"
#include "SimulatorFrontend.hh"
#include "RegisterFile.hh"
#include "SimValue.hh"
#include "POMDisassembler.hh"
#include "DisassemblyFUPort.hh"
#include "Instruction.hh"
#include "Procedure.hh"

/**
 * Simulates the given program+machine in "tandem" with the two
 * simulation engines.
 */
void 
tandemSimulate(
    TTAMachine::Machine& machine, 
    TTAProgram::Program& program) {

    // initialize both simulation engines
    SimulatorFrontend interp(SimulatorFrontend::SIM_NORMAL);
    interp.loadMachine(machine);
    interp.loadProgram(program);

    SimulatorFrontend compiled(SimulatorFrontend::SIM_COMPILED);

    // leave the compile simulation engine files at /tmp
    compiled.setCompiledSimulationLeaveDirty(true);

    compiled.loadMachine(machine);
    compiled.loadProgram(program);
    // reduce startup time by using dynamic compilation of the simulation
    // engine
    compiled.setStaticCompilation(false);


    assert(compiled.isSimulationInitialized());
    assert(interp.isSimulationInitialized());

    ClockCycleCount simulatedCycles = 0;
    while (!compiled.hasSimulationEnded()) {
        ClockCycleCount stepping;
        try {
            compiled.step(1);

            // the compiled simulator compiled BB at a time so we need to
            // ask it how many cycle to advance the cycle level interp. engine
            // to get to the same position in the simulation
            stepping = 
                compiled.cycleCount() - simulatedCycles;
        
            interp.step(stepping);
            simulatedCycles += stepping;
        } catch (const Exception& e) {
            std::cerr << "Simulation error: " << e.errorMessage() << std::endl;
            return;
            
        }

        if (!compiled.compareState(interp, &std::cerr))
            return;

        // print out the cycle count after simulating at least 1M cycles
        if (simulatedCycles / 1000000 > (simulatedCycles - stepping) / 1000000)
            std::cout
                << "simulated " << simulatedCycles << " cycles" << std::endl;
    }

    std::cout
        << "stop reasons:" << std::endl
        << "* compiled engine:" << std::endl;
    for (unsigned i = 0; i < compiled.stopReasonCount(); ++i) {
        std::cout << "** " << compiled.stopReason(i) << std::endl;
    }
    std::cout
        << "* interp. engine:" << std::endl;
    for (unsigned i = 0; i < interp.stopReasonCount(); ++i) {
        std::cout << "** " << interp.stopReason(i) << std::endl;
    }

}

int 
main(int argc, char* argv[]) {

    Application::initialize();    

    if (argc < 3) {
        std::cerr << "usage: ttasim-tandem machine.adf program.tpef";
            
    }

    try {
        TTAMachine::Machine* machine = 
            TTAMachine::Machine::loadFromADF(argv[1]);
        TTAProgram::Program* program =
            TTAProgram::Program::loadFromTPEF(argv[2], *machine);
        tandemSimulate(*machine, *program);

        delete machine; machine = NULL;
        delete program; program = NULL;
    } catch (const Exception& e) {
        std::cerr << e.errorMessage() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
