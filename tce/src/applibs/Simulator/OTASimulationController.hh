/*
    Copyright (c) 2002-2016 Tampere University.

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
 * @file OTASimulationController.hh
 *
 * OTASimulationController simulates an "operation triggered architecture"
 * using the TTA simulation model internally and treating some of the
 * instructions as implicit data transports.
 *
 * @author Pekka Jääskeläinen 2016
 * @note rating: red
 */

#ifndef OTA_SIMULATION_CONTROLLER_HH
#define OTA_SIMULATION_CONTROLLER_HH

#include "SimulationController.hh"

class OTASimulationController : public SimulationController {
public:

    OTASimulationController(
        SimulatorFrontend& frontend,
        const TTAMachine::Machine& machine,
        const TTAProgram::Program& program);

    virtual ~OTASimulationController();

protected:
    void advanceMachineCycle(unsigned pcAdd);
    virtual bool simulateCycle();
};

#endif
