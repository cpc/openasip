/**
 * @file Terminal.hh
 *
 * Declaration of Terminal class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_TERMINAL_HH
#define TTA_TERMINAL_HH

#include "BaseType.hh"
#include "SimValue.hh"
#include "Address.hh"

class Operation;

namespace TTAMachine {
    class RegisterFile;
    class ImmediateUnit;
    class FunctionUnit;
    class Port;
}

namespace TTAProgram {

class InstructionReference;

/**
 * Models any possible source or destination of a move.
 */
class Terminal {
public:
    Terminal();
    virtual ~Terminal();

    virtual bool isImmediate() const;
    virtual bool isAddress() const;
    virtual bool isInstructionAddress() const;
    virtual bool isImmediateRegister() const;
    virtual bool isGPR() const;
    virtual bool isFUPort() const;
    virtual bool isRA() const;
    virtual SimValue value() const throw (WrongSubclass);
    virtual Address address() const throw (WrongSubclass);
    virtual InstructionReference& instructionReference() const
        throw (WrongSubclass);
    virtual const TTAMachine::RegisterFile& registerFile() const
        throw (WrongSubclass);
    virtual const TTAMachine::ImmediateUnit& immediateUnit() const
        throw (WrongSubclass);
    virtual const TTAMachine::FunctionUnit& functionUnit() const
        throw (WrongSubclass);
    virtual int index() const 
        throw (WrongSubclass);

    virtual bool isOpcodeSetting() const 
        throw (WrongSubclass);

    virtual bool isTriggering() const 
        throw (WrongSubclass);

    virtual Operation& operation() const 
        throw (WrongSubclass, InvalidData);

    virtual Operation& hintOperation() const 
        throw (WrongSubclass, InvalidData);

    virtual int operationIndex() const 
        throw (WrongSubclass, InvalidData);

    virtual const TTAMachine::Port& port() const throw (WrongSubclass);
    virtual void setIndex(int index) throw (WrongSubclass, OutOfRange);
    virtual void setInstructionReference(InstructionReference& ref)
        throw(WrongSubclass);

    virtual Terminal* copy() const = 0;
    
    bool operator==(const Terminal& other) const;

    virtual bool equals(const Terminal& other) const = 0;
};

}

#endif
