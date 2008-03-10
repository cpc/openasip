/**
 * @file BusState.hh
 *
 * Declaration of BusState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_BUS_STATE_HH
#define TTA_BUS_STATE_HH

#include <string>

#include "RegisterState.hh"

//////////////////////////////////////////////////////////////////////////////
// BusState
//////////////////////////////////////////////////////////////////////////////

/**
 * Models the state of the bus.
 */
class BusState : public RegisterState {
public:
    BusState(int width);
    virtual ~BusState();

    virtual void setValue(const SimValue& value);
    void setValueInlined(const SimValue& value);

    void clear();

    void setSquashed(bool isSquashed);
    bool isSquashed() const;    

private:
    /// Copying not allowed.
    BusState(const BusState&);
    /// Assignment not allowed.
    BusState& operator=(const BusState&);

    /// Name of the bus.
    std::string name_;
    /// Width of the bus.
    int width_;
    /// True in case this bus was squashed the last time a move was executed
    /// in this bus.
    bool squashed_;
};

//////////////////////////////////////////////////////////////////////////////
// NullBusState
//////////////////////////////////////////////////////////////////////////////

/**
 * Models null BusState object.
 */
class NullBusState : public BusState {
public:
    static NullBusState& instance();
    
    virtual ~NullBusState();

    virtual void setValue(const SimValue& value);
    virtual const SimValue& value() const;

private:
    NullBusState();
    /// Copying not allowed.
    NullBusState(const NullBusState&);
    /// Assignment not allowed.
    NullBusState& operator=(const NullBusState&);

    /// Unique instance of NullBusState.
    static NullBusState* instance_;
};

#include "BusState.icc"

#endif
