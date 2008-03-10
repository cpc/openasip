/**
 * @file UnboundedRegisterFile.hh
 *
 * Declaration of UnboundedRegisterFile class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen@tut.fi)
 * @note rating: yellow
 */

#ifndef TTA_UNIVERSAL_REGISTER_FILE_HH
#define TTA_UNIVERSAL_REGISTER_FILE_HH

#include "RegisterFile.hh"

/**
 * UnboundedRegisterFile class represents a register file which has
 * unlimited amount of registers. This kind of register files are used in
 * UniversalMachine.
 */
class UnboundedRegisterFile : public TTAMachine::RegisterFile {
public:
    UnboundedRegisterFile(
        const std::string& name,
        int width,
        RegisterFile::Type type)
        throw (InvalidName, OutOfRange);
    virtual ~UnboundedRegisterFile();

    virtual int numberOfRegisters() const;

    virtual void setMaxReads(int reads)
        throw (OutOfRange);
    virtual void setMaxWrites(int maxWrites)
        throw (OutOfRange);
    virtual void setNumberOfRegisters(int registers)
        throw (OutOfRange);
    virtual void setWidth(int width)
        throw (OutOfRange);
    virtual void setName(const std::string& name)
        throw (ComponentAlreadyExists, InvalidName);
    virtual void setType(RegisterFile::Type type);

    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);
};

#endif
