/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
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
      
    virtual void loadMachine(const std::string& fileName)
	throw (FileNotFound, IOException, SimulationStillRunning,
               IllegalMachine);

    virtual void loadProgram(const std::string& fileName)
	throw (FileNotFound, IOException, SimulationStillRunning,
	       IllegalProgram, IllegalMachine);

    virtual void run()
	throw (SimulationExecutionError);

    virtual void runUntil(UIntWord address)
	throw (SimulationExecutionError);

    virtual void step(double count = 1)
	throw (SimulationExecutionError);

    virtual void next(int count = 1)
	throw (SimulationExecutionError);

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
