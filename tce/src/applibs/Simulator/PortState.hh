/**
 * @file PortState.hh
 *
 * Declaration of PortState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PORT_STATE_HH
#define TTA_PORT_STATE_HH

#include <string>

#include "RegisterState.hh"
#include "Application.hh"

class FUState;

//////////////////////////////////////////////////////////////////////////////
// PortState
//////////////////////////////////////////////////////////////////////////////

/**
 * Models port of function unit.
 */
class PortState : public RegisterState {
public:
    PortState(FUState& parent, int width);
    PortState(FUState& parent, SimValue& sharedRegister);
    virtual ~PortState();

    virtual FUState& parent() const;

protected:
    /// Parent of the port.
    FUState* parent_;
private:
    /// Copying not allowed.
    PortState(const PortState&);
    /// Assignment not allowed.
    PortState& operator=(const PortState&);    
};

//////////////////////////////////////////////////////////////////////////////
// NullPortState
//////////////////////////////////////////////////////////////////////////////

/**
 * Models non-existing PortState.
 */
class NullPortState : public PortState {
public:
    static NullPortState& instance();

    virtual ~NullPortState();
    
    virtual void setValue(const SimValue& value);
    virtual const SimValue& value() const;
    virtual FUState& parent() const;

private:
    NullPortState();
    /// Copying not allowed.
    NullPortState(const NullPortState&);
    /// Assignment not allowed.
    NullPortState& operator=(const NullPortState&);

    /// Unique instance of NullPortState class.
    static NullPortState* instance_;
};

#endif
