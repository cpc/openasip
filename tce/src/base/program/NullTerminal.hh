/**
 * @file NullTerminal.hh
 *
 * Declaration of NullTerminal class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_TERMINAL_HH
#define TTA_NULL_TERMINAL_HH

#include "Terminal.hh"

namespace TTAProgram {

/**
 * A singleton class that represents a null terminal.
 *
 * Calling any method causes the program to abort.
 */
class NullTerminal : public Terminal {
public:
    virtual ~NullTerminal();
    static NullTerminal& instance();

    virtual bool isImmediate() const;
    virtual bool isImmediateRegister() const;
    virtual bool isGPR() const;
    virtual bool isFUPort() const;
    virtual bool isOpcodeSetting() const throw (WrongSubclass);

    virtual SimValue value() const throw (WrongSubclass);
    virtual const TTAMachine::RegisterFile& registerFile() const
        throw (WrongSubclass);
    virtual const TTAMachine::ImmediateUnit& immediateUnit() const
        throw (WrongSubclass);
    virtual const TTAMachine::FunctionUnit& functionUnit() const
        throw (WrongSubclass);
    virtual int index() const throw (WrongSubclass);
    virtual Operation& operation() const throw (InvalidData);
    virtual const TTAMachine::Port& port() const throw (WrongSubclass);
    virtual void setIndex(int index) throw (OutOfRange);
    virtual void setOperation(Operation& operation) throw (WrongSubclass);
    virtual void setInstructionReference(InstructionReference& ref)
        throw(WrongSubclass);

    virtual Terminal* copy() const;
    virtual bool equals(const Terminal& other) const;
protected:
    NullTerminal();

private:
    /// Copying not allowed.
    NullTerminal(const NullTerminal&);
    /// Assignment not allowed.
    NullTerminal& operator=(const NullTerminal&);

    /// Unique instance of NullTerminal.
    static NullTerminal instance_;
};

}

#endif
