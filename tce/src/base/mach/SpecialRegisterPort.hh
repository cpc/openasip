/**
 * @file SpecialRegisterPort.hh
 *
 * Declaration of SpecialRegisterPort class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_SPECIAL_REGISTER_PORT_HH
#define TTA_SPECIAL_REGISTER_PORT_HH

#include "BaseFUPort.hh"

namespace TTAMachine {

class ControlUnit;

/**
 * SpecialRegisterPort class represent a port of a special register in
 * control unit.
 *
 * An example of such a port is the return address port.
 */
class SpecialRegisterPort : public BaseFUPort {
public:
    SpecialRegisterPort(
        const std::string& name,
        int width,
        ControlUnit& parent)
        throw (InvalidName, OutOfRange, ComponentAlreadyExists);
    SpecialRegisterPort(const ObjectState* state, Unit& parent)
        throw (ObjectStateLoadingException);
    virtual ~SpecialRegisterPort();

    virtual bool isTriggering() const;
    virtual bool isOpcodeSetting() const;

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// ObjectState name for special register port.
    static const std::string OSNAME_SPECIAL_REG_PORT;
};
}

#endif
