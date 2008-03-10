/**
 * @file FixedRegisters.hh
 *
 * Declaration of FixedRegisters class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_FIXED_REGISTERS_HH
#define TTA_FIXED_REGISTERS_HH

#include <vector>

class SimValue;

/**
 * Contains all the registers that are assigned to special purpose.
 */
class FixedRegisters {
public:
    FixedRegisters();
    virtual ~FixedRegisters();
    
    SimValue& stackPointer() const;
    SimValue& integerArgumentRegister(int index) const;
    SimValue& floatArgumentRegister(int index) const;
    SimValue& integerReturnValue() const;
    SimValue& floatReturnValue() const;

    void setStackPointer(SimValue& value);
    void addIntegerArgumentRegister(SimValue& value);
    void addFloatArgumentRegister(SimValue& value);
    void setIntegerReturnValue(SimValue& value);
    void setFloatReturnValue(SimValue& value);

private:
    /// Copying not allowed.
    FixedRegisters(const FixedRegisters&);
    /// Assignment not allowed.
    FixedRegisters& operator=(const FixedRegisters);

    /// Stack pointer.
    SimValue* stackPointer_;
    /// Integer argument registers.
    std::vector<SimValue*> integerArgs_;
    /// Floating point argument registers.
    std::vector<SimValue*> floatArgs_;
    /// Integer return value.
    SimValue* intReturnValue_;
    /// Floating point return value.
    SimValue* floatReturnValue_;
};

#endif
