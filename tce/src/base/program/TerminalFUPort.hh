/**
 * @file TerminalFUPort.hh
 *
 * Declaration of TerminalFUPort class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_TERMINAL_FU_PORT_HH
#define TTA_TERMINAL_FU_PORT_HH

#include "Exception.hh"
#include "Terminal.hh"

namespace TTAMachine {
    class HWOperation;
    class BaseFUPort;
}

namespace TTAProgram {

/**
 * Represents an input or output port of a function unit and (when
 * applicable) the operation code written into it.
 *
 * Notice that, in principle, operation codes can be written into FU output 
 * ports.
 */
class TerminalFUPort : public Terminal {
public:
    TerminalFUPort(const TTAMachine::BaseFUPort& port)
        throw (IllegalParameters);
    TerminalFUPort(TTAMachine::HWOperation& operation, int opIndex)
        throw (IllegalParameters);
    virtual ~TerminalFUPort();

    virtual bool isOpcodeSetting() const 
        throw (WrongSubclass);

    virtual bool isTriggering() const 
        throw (WrongSubclass);

    virtual bool isFUPort() const;
    virtual bool isRA() const;

    virtual const TTAMachine::FunctionUnit& functionUnit() const
        throw (WrongSubclass);
    virtual std::string HWOperationName() const;
    virtual Operation& operation() const 
        throw (WrongSubclass, InvalidData);
    virtual Operation& hintOperation() const
        throw (WrongSubclass, InvalidData);
    virtual int operationIndex() const 
        throw (WrongSubclass, InvalidData);
    virtual const TTAMachine::Port& port() const throw (WrongSubclass);
    virtual Terminal* copy() const;
    virtual bool equals(const Terminal& other) const;
    
    virtual TTAMachine::HWOperation* hwOperation() const;

private:
    // copy constructor used internally by copy();
    TerminalFUPort(const TerminalFUPort& tfup);

    /// Assignment not allowed.
    TerminalFUPort& operator=(const TerminalFUPort&);
    int findNewOperationIndex() const;

    /// Port of the unit.
    const TTAMachine::BaseFUPort& port_;
    /// Operation code transported to the port.
    TTAMachine::HWOperation* operation_;
    /// The OSAL operation.
    Operation* opcode_;
    /// Operation index.
    int opIndex_;
};

}

#endif
