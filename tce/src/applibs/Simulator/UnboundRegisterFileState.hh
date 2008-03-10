/**
 * @file UnboundRegisterFileState.hh
 *
 * Declaration of UnboundRegisterFileState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_UNBOUND_REGISTER_FILE_STATE_HH
#define TTA_UNBOUND_REGISTER_FILE_STATE_HH

#include <vector>

#include "RegisterFileState.hh"

class RegisterState;

/**
 * Holds unlimited number of RegisterState objects.
 */
class UnboundRegisterFileState : public RegisterFileState {
public:
    UnboundRegisterFileState();
    virtual ~UnboundRegisterFileState();

    virtual RegisterState& registerState(int index)
        throw (OutOfRange);

    virtual std::size_t registerCount() const;

    bool isRegisterInUse(std::size_t index) const;

private:
    /// Copying not allowed.
    UnboundRegisterFileState(const UnboundRegisterFileState&);
    /// Assignment not allowed.
    UnboundRegisterFileState& operator=(const UnboundRegisterFileState&);

    /// Contains all the registers of the register file.
    std::vector<RegisterState*> registerStates_;
};

#endif
