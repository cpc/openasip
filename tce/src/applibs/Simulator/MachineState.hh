/**
 * @file MachineState.hh
 *
 * Declaration of MachineState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_MACHINE_STATE_HH
#define TTA_MACHINE_STATE_HH

#include <string>
#include <map>
#include <vector>

#include "PortState.hh"
#include "Guard.hh"

class GCUState;
class BusState;
class FUState;
class LongImmediateUnitState;
class RegisterFileState;
class OperationExecutor;
class GuardState;

/**
 * Root class of machine state model.
 *
 * Owns all state classes.
 */
class MachineState {
public:
    MachineState();
    virtual ~MachineState();

    void clear();

    GCUState& gcuState();
    BusState& busState(const std::string& name);
    FUState& fuState(const std::string& name);
    
    int FUStateCount() const;
    FUState& fuState(int index)
        throw (OutOfRange);
    void advanceClockOfAllFUStates();
    void endClockOfAllFUStates();
    void advanceClockOfAllGuardStates();
    void advanceClockOfAllLongImmediateUnitStates();
    void clearBuses();
    
    PortState& portState(
        const std::string& portName, 
        const std::string& fuName);
    LongImmediateUnitState& longImmediateUnitState(const std::string& name);
    RegisterFileState& registerFileState(const std::string& name);
    GuardState& guardState(const TTAMachine::Guard& guard);

    void addGCUState(GCUState* state);
    void addBusState(BusState* state, const std::string& name);
    void addFUState(FUState* state, const std::string& name);
    void addPortState(
        PortState* state, 
        const std::string& name, 
        const std::string& fuName);
    void addLongImmediateUnitState(
        LongImmediateUnitState* state,
        const std::string& name);
    void addRegisterFileState(
        RegisterFileState* state, 
        const std::string& name);
    void addGuardState(GuardState* state, const TTAMachine::Guard& guard);
        
    void addOperationExecutor(OperationExecutor* executor);

private:
    /// Copying not allowed.
    MachineState(const MachineState&);
    /// Assignment not allowed.
    MachineState& operator=(const MachineState&);

    /// Contains bus states indexed by names.
    typedef std::map<std::string, BusState*> BusContainer;
    /// Contains function unit states indexed by names.
    typedef std::map<std::string, FUState*> FUContainer;
    /// Contains port states indexed by names.
    typedef std::map<std::string, PortState*> PortContainer;
    /// Contains long immediate unit states indexed by names.
    typedef std::map<std::string, LongImmediateUnitState*> 
    LongImmediateContainer;
    /// Contains register file states indexed by names.
    typedef std::map<std::string, RegisterFileState*> RegisterFileContainer;
    /// Contains guard states indexed by their MOM object.
    typedef std::map<const TTAMachine::Guard*, GuardState*> GuardContainer;
    /// Contains operation executors.
    typedef std::vector<OperationExecutor*> ExecutorContainer;

    // Contains all states in vectors that are faster to iterate than maps.
    typedef std::vector<BusState*> BusCache;
    typedef std::vector<FUState*> FUCache;
    typedef std::vector<PortState*> PortCache;
    typedef std::vector<LongImmediateUnitState*> LongImmediateUnitCache;
    typedef std::vector<RegisterFileState*> RegisterFileCache;
    typedef std::vector<GuardState*> GuardCache;

    /// GCU state.
    GCUState* GCUState_;
    /// Contains all bus states.
    BusContainer busses_;
    /// Container of function unit states for fast traversal.
    FUContainer FUStates_;
    /// Contains all port states.
    PortContainer ports_;
    /// Contains all long immediate unit states.
    LongImmediateContainer longImmediates_;
    /// Contains all register file states.
    RegisterFileContainer registers_;
    /// Contains all operation executors.
    ExecutorContainer executors_;
    /// Contains all guard states.
    GuardContainer guards_;
    /// Count of FUStates added in MachineState (optimization).
    std::size_t fuStateCount_;

    // Contains all states in vectors that are faster to iterate than maps.
    BusCache busCache_;
    FUCache fuCache_;
    PortCache portCache_;
    LongImmediateUnitCache longImmediateCache_;
    RegisterFileCache rfCache_;
    GuardCache guardCache_;
};

#include "MachineState.icc"

#endif
