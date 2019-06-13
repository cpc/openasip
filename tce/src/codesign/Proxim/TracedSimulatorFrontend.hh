/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file TracedSimulatorFrontend.hh
 *
 * Declaration of TracedSimulatorFrontend class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_TRACED_SIMULATOR_FRONTEND_HH
#define TTA_TRACED_SIMULATOR_FRONTEND_HH

#include "SimulatorFrontend.hh"
#include "Informer.hh"
#include "BaseType.hh"

/**
 * TracedSimulatorFrontend is a class which adds event passing to the
 * SimulatorFrontend baseclass.
 *
 * TracedSimulatorFrontend implements the Informer interface and passes
 * events whenever the simulator state changes. This allows a client to
 * track the simulator state by registering itself as a listener of the
 * simulator events.
 */
class TracedSimulatorFrontend : public SimulatorFrontend, public Informer {
public:
    TracedSimulatorFrontend();
    virtual ~TracedSimulatorFrontend();

    virtual void loadMachine(const std::string& fileName);

    virtual void loadProgram(const std::string& fileName);

    virtual void run();

    virtual void runUntil(UIntWord address);

    virtual void step(double count = 1);

    virtual void next(int count = 1);

    virtual void killSimulation();

    /// IDs for the simulator events.
    enum event {
	SIMULATOR_MACHINE_LOADED = 0,
	SIMULATOR_PROGRAM_LOADED,
	SIMULATOR_START,
	SIMULATOR_STOP,
        SIMULATOR_RUN,
        SIMULATOR_LOADING_PROGRAM,
        SIMULATOR_LOADING_MACHINE,
        SIMULATOR_RESET
    };    
};
#endif
