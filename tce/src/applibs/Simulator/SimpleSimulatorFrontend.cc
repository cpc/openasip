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
 * @file SimpleSimulatorFrontend.cc
 *
 * Implementation of SimpleSimulatorFrontend class.
 *
 * @author Pekka Jääskeläinen 2010 (pjaaskel-no.spam-cs.tut.fi)
 */
#include "SimpleSimulatorFrontend.hh"
#include "SimulatorFrontend.hh"
#include "MachineState.hh"
#include "DetailedOperationSimulator.hh"

SimpleSimulatorFrontend::SimpleSimulatorFrontend(
    TCEString machineFile, TCEString programFile) {
    simFront_ = new SimulatorFrontend();
    try {
        simFront_->loadMachine(machineFile);
        simFront_->loadProgram(programFile);
    } catch (Exception& e) {
        std::cerr 
            << e.errorMessage() + " in " +   
            e.fileName() + ":" +
            e.procedureName() + ":" 
            << e.lineNum() << std::endl;
        abort();
    }
}

SimpleSimulatorFrontend::~SimpleSimulatorFrontend() {
    delete simFront_;
}

void
SimpleSimulatorFrontend::step() {
    if (!simFront_->hasSimulationEnded())
        simFront_->step();
}

/**
 * Replaces all operation executors in the given FU with the
 * given one.
 */
void
SimpleSimulatorFrontend::setOperationSimulator(
    const TCEString& fuName, DetailedOperationSimulator& sim) {
    try {
        simFront_->machineState().fuState(fuName).
            setOperationSimulator(sim);
    } catch (Exception& e) {
        std::cerr 
            << e.errorMessage() + " in " +   
            e.fileName() + ":" +
            e.procedureName() + ":" 
            << e.lineNum() << std::endl;
        abort();
    }
}
