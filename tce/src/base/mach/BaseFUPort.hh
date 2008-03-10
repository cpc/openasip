/**
 * @file BaseFUPort.hh
 *
 * Declaration of BaseFUPort class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_BASE_FU_PORT_HH
#define TTA_BASE_FU_PORT_HH

#include "Port.hh"

namespace TTAMachine {

/**
 * BaseFUPort is an abstract base class for FUPort and SpecialRegisterPort
 * classes.
 */
class BaseFUPort : public Port {
public:
    virtual ~BaseFUPort();

    FunctionUnit* parentUnit() const;

    virtual int width() const;
    void setWidth(int width)
        throw (OutOfRange);

    virtual bool isTriggering() const = 0;
    virtual bool isOpcodeSetting() const = 0;

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// ObjectState attribute key for bit width of the port.
    static const std::string OSKEY_WIDTH;

protected:
    BaseFUPort(
        const std::string& name,
        int width,
        FunctionUnit& parent)
        throw (InvalidName, ComponentAlreadyExists, OutOfRange);
    BaseFUPort(const ObjectState* state, Unit& parent)
        throw (ObjectStateLoadingException);

private:
    void loadStateWithoutReferences(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// Bit width of the port.
    int width_;
};
}

#endif
