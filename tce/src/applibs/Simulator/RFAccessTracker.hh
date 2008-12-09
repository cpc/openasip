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
 * @file RFAccessTracker.hh
 *
 * Declaration of RFAccessTracker class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_RF_ACCESS_TRACKER_HH
#define TTA_RF_ACCESS_TRACKER_HH

#include <string>
#include <map>

#include "boost/tuple/tuple.hpp"

#include "Listener.hh"
#include "Exception.hh"
#include "SimulatorConstants.hh"
#include "hash_map.hh"

class SimulatorFrontend;
class InstructionMemory;

/**
 * Tracks concurrent register file accesses.
 */
class RFAccessTracker : public Listener {
public:
    /// type to be used as a key for storing concurrent access info
    typedef boost::tuple<
        std::string, /* register file name */
        std::size_t, /* concurrent writes */
        std::size_t> /* concurrent reads */ ConcurrentRFAccess;
    /// concurrent accesses and their counts
    typedef std::map<ConcurrentRFAccess, ClockCycleCount> 
    ConcurrentRFAccessIndex;

    RFAccessTracker(
        SimulatorFrontend& frontend,
        const InstructionMemory& instructions);
    virtual ~RFAccessTracker();

    virtual void handleEvent(int event);
    
    ClockCycleCount concurrentRegisterFileAccessCount(
        const std::string& rfName,
        std::size_t concurrentWrites,
        std::size_t concurrentReads) const;

    const ConcurrentRFAccessIndex& accessDataBase() const;

private:
    /// Index for RF accesses in an instruction.
    typedef hash_map<
        const char*, /* funame */
        boost::tuple<std::size_t /* writes */, std::size_t /* reads */> >
    RFAccessIndex;
    /// the simulator frontend used to access simulation data
    SimulatorFrontend& frontend_;
    /// used to access instruction execution data
    const InstructionMemory& instructionExecutions_;
    /// conditional register file accesses are counted in this container
    ConcurrentRFAccessIndex conditionalAccesses_;  
    /// total (conditional + unconditional) register file accesses are counted
    /// in this container
    ConcurrentRFAccessIndex totalAccesses_;  
    /// container used in collecting register accesses in an instruction
    RFAccessIndex accessesInInstruction_;
};

#endif
