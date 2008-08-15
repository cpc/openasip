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
 * @file RFAccessTracker.hh
 *
 * Declaration of RFAccessTracker class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
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
