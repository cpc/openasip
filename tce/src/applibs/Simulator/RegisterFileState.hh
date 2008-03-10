/**
 * @file RegisterFileState.hh
 *
 * Declaration of RegisterFileState class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_REGISTER_FILE_STATE_HH
#define TTA_REGISTER_FILE_STATE_HH

#include <vector>

#include "Exception.hh"

class RegisterState;

//////////////////////////////////////////////////////////////////////////////
// RegisterFileState
//////////////////////////////////////////////////////////////////////////////

/**
 * Class that manages RegisterState objects.
 */ 
class RegisterFileState {
public:
    explicit RegisterFileState(int size, int width);
    virtual ~RegisterFileState();

    virtual RegisterState& registerState(int index)
        throw (OutOfRange);

    virtual std::size_t registerCount() const;

private:
    /// Copying not allowed.
    RegisterFileState(const RegisterFileState&);
    /// Assignment not allowed.
    RegisterFileState& operator=(const RegisterFileState);

    /// Contains all the registers of the state.
    std::vector<RegisterState*> registerStates_;
};

//////////////////////////////////////////////////////////////////////////////
// NullRegisterFileState
//////////////////////////////////////////////////////////////////////////////

/**
 * Models non-existing RegisterFileState.
 */
class NullRegisterFileState : public RegisterFileState {
public:
    static NullRegisterFileState& instance();

    virtual ~NullRegisterFileState();

    virtual RegisterState& registerState(int index)
        throw (OutOfRange);

private:
    NullRegisterFileState();
    /// Copying not allowed.
    NullRegisterFileState(const NullRegisterFileState&);
    /// Assignment not allowed.
    NullRegisterFileState& operator=(const NullRegisterFileState&);

    /// Unique instance of NullRegisterFileState.
    static NullRegisterFileState* instance_;
};

#endif
