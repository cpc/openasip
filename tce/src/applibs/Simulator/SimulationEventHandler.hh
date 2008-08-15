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
 * @file SimulationEventHandler.hh
 *
 * Declaration of SimulationEventHandler class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIMULATION_EVENT_HANDLER_HH
#define TTA_SIMULATION_EVENT_HANDLER_HH

#include "Informer.hh"

/**
 * The informer of the simulation specific events.
 */
class SimulationEventHandler : public Informer {
public:
    SimulationEventHandler();
    virtual ~SimulationEventHandler();

    /// simulation specific event codes
    enum {
        SE_NEW_INSTRUCTION = 0,
        ///< Generated before executing a new instructon.
        SE_CYCLE_END,       ///< Generated before advancing the simulator 
                            ///< clock at the end of a simulation cycle.
        SE_RUNTIME_ERROR,   ///< Sent when a runtime error is detected in 
                            ///< the simulated program.
        SE_SIMULATION_STOPPED, ///< Generated after simulation has stopped,
                               ///< temporarily or permantently, and control
                               ///< is being returned to user interface.
        SE_MEMORY_ACCESS    ///< Genereated when memory read or write is
                            ///< initiated.
        
    } SimulationEvent;
private:
};

#endif
