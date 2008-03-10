/**
 * @file NullRegisterFile.hh
 *
 * Declaration of NullRegisterFile class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: yellow
 */

#ifndef TTA_NULL_REGISTER_FILE_HH
#define TTA_NULL_REGISTER_FILE_HH

#include "RegisterFile.hh"

namespace TTAMachine {

/**
 * Null version of RegisterFile class. This is a singleton class.
 */
class NullRegisterFile : public RegisterFile {
public:
    static NullRegisterFile& instance();

    virtual Machine* machine() const;
    virtual void ensureRegistration(const Component& component) const
        throw (IllegalRegistration);
    virtual bool isRegistered() const;

    virtual bool hasPort(const std::string& name) const;
    virtual int portCount() const;

    virtual void setMachine(Machine& mach)
        throw (ComponentAlreadyExists);
    virtual void unsetMachine();

    virtual int numberOfRegisters() const;
    virtual int width() const;

    virtual void setNumberOfRegisters(int registers)
        throw (OutOfRange);
    virtual void setWidth(int width)
        throw (OutOfRange);

    virtual int maxReads() const;
    virtual int maxWrites() const;
    virtual RegisterFile::Type type() const;

    virtual bool isNormal() const;
    virtual bool isVolatile() const;
    virtual bool isReserved() const;

    virtual std::string name() const;
    virtual void setName(const std::string& name)
        throw (ComponentAlreadyExists, InvalidName);
    virtual void setMaxReads(int reads)
        throw (OutOfRange);
    virtual void setMaxWrites(int maxWrites)
        throw (OutOfRange);
    virtual void setType(RegisterFile::Type type);

    virtual int guardLatency() const;
    virtual void setGuardLatency(int latency)
        throw (OutOfRange);

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

private:
    NullRegisterFile();
    virtual ~NullRegisterFile();

    /// The only instance.
    static NullRegisterFile instance_;
};
}

#endif
