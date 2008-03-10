/**
 * @file RegisterState.hh
 *
 * Declaration of RegisterState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_REGISTER_STATE_HH
#define TTA_REGISTER_STATE_HH

#include <string>

#include "StateData.hh"
#include "SimValue.hh"


//////////////////////////////////////////////////////////////////////////////
// RegisterState
//////////////////////////////////////////////////////////////////////////////

/**
 * Class that represents the contents of a register.
 */
class RegisterState : public StateData {
public:
    RegisterState(int width);
    RegisterState(SimValue& sharedRegister);
    virtual ~RegisterState();
    
    virtual void setValue(const SimValue& value);
    virtual const SimValue& value() const;
    
protected:
    /// Value of the RegisterState. @todo Fix this mutable mess.
    /// It's needed because OutputPortState needs to mask the value_ in
    /// its value() implementation.
    mutable SimValue& value_;

private:
    /// Copying not allowed.
    RegisterState(const RegisterState&);
    /// Assignment not allowed.
    RegisterState& operator=(const RegisterState&);    
    /// Is the storage of this RegisterState shared with someone else?
    bool shared_;
};

//////////////////////////////////////////////////////////////////////////////
// NullRegisterState
//////////////////////////////////////////////////////////////////////////////

/**
 * Models non-existing RegisterState.
 */
class NullRegisterState : public RegisterState {
public:
    static NullRegisterState& instance();

    virtual ~NullRegisterState();
    
    virtual void setValue(const SimValue& value);
    virtual const SimValue& value() const;

private:
    NullRegisterState();
    /// Copying not allowed.
    NullRegisterState(const NullRegisterState&);
    /// Assignment not allowed.
    NullRegisterState& operator=(const NullRegisterState&);

    /// Unique instance of NullRegisterState.
    static NullRegisterState* instance_;
};

#endif
