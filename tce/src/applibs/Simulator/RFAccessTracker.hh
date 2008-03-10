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
