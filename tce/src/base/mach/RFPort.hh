/**
 * @file RFPort.hh
 *
 * Declaration of RFPort class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_RF_PORT_HH
#define TTA_RF_PORT_HH

#include "Port.hh"

namespace TTAMachine {

class BaseRegisterFile;

/**
 * RFPort instances are used as ports of register files.
 */
class RFPort : public Port {
public:
    RFPort(const std::string& name, BaseRegisterFile& parent)
        throw (InvalidName, ComponentAlreadyExists);
    RFPort(const ObjectState* state, Unit& parent)
        throw (ObjectStateLoadingException);
    virtual ~RFPort();

    virtual int width() const;
    BaseRegisterFile* parentUnit() const;

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// ObjectState name for register file port.
    static const std::string OSNAME_RFPORT;
};
}

#endif
