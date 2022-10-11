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
 * @file SimulationEventHandler.hh
 *
 * Declaration of SimulationEventHandler class.
 *
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
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
