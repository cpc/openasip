/**
 * @file LongImmediateUnitState.hh
 *
 * Declaration of LongImmediateUnitState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_LONG_IMMEDIATE_UNIT_STATE_HH
#define TTA_LONG_IMMEDIATE_UNIT_STATE_HH

#include <vector>
#include <map>
#include <string>

#include "ClockedState.hh"
#include "Exception.hh"
#include "SimValue.hh"

class LongImmediateRegisterState;

//////////////////////////////////////////////////////////////////////////////
// LongImmediateUnitState
//////////////////////////////////////////////////////////////////////////////

/**
 * Class that represents the states of long immediate units.
 */
class LongImmediateUnitState : public ClockedState {
public:
    LongImmediateUnitState(
        int size, 
        int latency, 
        const std::string& name, 
        int width, 
        bool signExtend);
    virtual ~LongImmediateUnitState();

    virtual SimValue& registerValue(int index)
        throw (OutOfRange);
    virtual void setRegisterValue(int index, const SimValue& value)
        throw (OutOfRange);

    virtual LongImmediateRegisterState& immediateRegister(int i) 
        throw (OutOfRange);
    virtual int immediateRegisterCount() const;

    virtual void endClock();
    virtual void advanceClock();

private:
    /// Copying not allowed.
    LongImmediateUnitState(const LongImmediateUnitState&);
    /// Assignment not allowed.
    LongImmediateUnitState& operator=(const LongImmediateUnitState&);
    
    void clear();

    /**
     * Represents one value update request.
     */
    struct Item {
        /**
         * Constructor.
         */
        Item() : timer_(0), value_(32), index_(0) {}
      
        /// Timer of the item.
        int timer_;
        /// Value of the item.
        SimValue value_;
        /// Index of the item.
        int index_;
    };
    
    typedef std::vector<Item*> ItemQueue;
    typedef std::vector<LongImmediateRegisterState*> RegisterContainer;
    typedef std::vector<SimValue> ValueContainer;

    /// Latency of LongImmediateUnit.
    int latency_;
    /// Name of the unit.
    std::string name_;
    /// Queue of register value update requests.
    ItemQueue queue_;
    /// Contains all long immediate registers of the unit.
    RegisterContainer registers_;
    /// Contains all values of the registers.
    ValueContainer values_;
};

//////////////////////////////////////////////////////////////////////////////
// NullLongImmediateUnitState
//////////////////////////////////////////////////////////////////////////////

/**
 * Models non-existing LongImmediateUnitState.
 */
class NullLongImmediateUnitState : public LongImmediateUnitState {
public:
    static NullLongImmediateUnitState& instance();

    virtual ~NullLongImmediateUnitState();

    virtual SimValue& registerValue(int index)
        throw (OutOfRange);
    virtual void setRegisterValue(int index, SimValue& value)
        throw (OutOfRange);

    virtual LongImmediateRegisterState& immediateRegister(int i) 
	throw (OutOfRange);
    virtual int immediateRegisterCount() const;

    virtual void endClock();
    virtual void advanceClock();

private:
    NullLongImmediateUnitState();
    /// Copying not allowed.
    NullLongImmediateUnitState(const NullLongImmediateUnitState&);
    /// Assignment not allowed.
    NullLongImmediateUnitState& operator=(const NullLongImmediateUnitState&);

    /// Unique instance of NullLongImmediateUnitState.
    static NullLongImmediateUnitState* instance_;
};

#endif
