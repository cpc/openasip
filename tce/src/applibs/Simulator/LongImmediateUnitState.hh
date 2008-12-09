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
 * @file LongImmediateUnitState.hh
 *
 * Declaration of LongImmediateUnitState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
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
