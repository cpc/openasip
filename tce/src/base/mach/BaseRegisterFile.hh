/**
 * @file BaseRegisterFile.hh
 *
 * Declaration of abstract BaseRegisterFile class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen@tut.fi)
 * @note reviewed 10 Jun 2004 by vpj, am, tr, ll
 * @note rating: red
 */

#ifndef TTA_BASE_REGISTER_FILE_HH
#define TTA_BASE_REGISTER_FILE_HH

#include <string>

#include "Unit.hh"
#include "RFPort.hh"

namespace TTAMachine {

/**
 * An abstract base class for register files as ImmediateUnit and
 * RegisterFile.
 */
class BaseRegisterFile : public Unit {
public:
    virtual ~BaseRegisterFile();

    virtual int numberOfRegisters() const;
    virtual int width() const;
    virtual int size() const;

    virtual void setNumberOfRegisters(int registers)
        throw (OutOfRange);
    virtual void setWidth(int width)
        throw (OutOfRange);

    virtual RFPort* port(const std::string& name) const
        throw (InstanceNotFound);
    virtual RFPort* port(int index) const
        throw (OutOfRange);

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// ObjectState name for BaseRegisterFile.
    static const std::string OSNAME_BASE_REGISTER_FILE;
    /// ObjectState attribute key for the number of registers.
    static const std::string OSKEY_SIZE;
    /// ObjectState attribute key for bit width of the registers.
    static const std::string OSKEY_WIDTH;

protected:
    BaseRegisterFile(const std::string& name, int size, int width)
        throw (OutOfRange, InvalidName);
    BaseRegisterFile(const ObjectState* state)
        throw (ObjectStateLoadingException);

private:
    void loadStateWithoutReferences(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// Number of registers in the register file.
    int size_;
    /// Bit width of the registers in the register file.
    int width_;
};
}

#include "BaseRegisterFile.icc"

#endif
