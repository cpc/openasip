/**
 * @file TerminalRegister.hh
 *
 * Declaration of TerminalRegister class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_TERMINAL_REGISTER_HH
#define TTA_TERMINAL_REGISTER_HH

#include "Terminal.hh"

namespace TTAMachine {
    class Unit;
    class Port;
}

namespace TTAProgram {

/**
 * Represents a reference to registers from general-purpose register
 * files and immediate units.
 *
 * These types of terminals are characterised by the fact that
 * multiple, functionally identical registers are accessed through the
 * same port. Registers are identified by an index.
 */
class TerminalRegister : public Terminal{
public:
    TerminalRegister(
        const TTAMachine::Port& port,
        int index)
        throw (InvalidData);

    /// Copying is allowed.
    //  TerminalRegister(const TerminalRegister&);

    virtual ~TerminalRegister();

    virtual bool isImmediateRegister() const;
    virtual bool isGPR() const;

    virtual const TTAMachine::RegisterFile& registerFile() const
        throw (WrongSubclass);
    virtual const TTAMachine::ImmediateUnit& immediateUnit() const
        throw (WrongSubclass);
    virtual int index() const throw (WrongSubclass);
    virtual const TTAMachine::Port& port() const throw (WrongSubclass);
    virtual void setIndex(int index) throw (OutOfRange);

    virtual Terminal* copy() const;
    virtual bool equals(const Terminal& other) const;

private:
    /// Assignment not allowed.
    TerminalRegister& operator=(const TerminalRegister&);

    /// Unit of the terminal.
    const TTAMachine::Unit& unit_;
    /// Port of the unit.
    const TTAMachine::Port& port_;
    /// Index of the register of the register file or immediate unit.
    int index_;
    /// Unit type flag: true if immediate unit, false if register file.
    bool isImmUnit_;
};

#include "TerminalRegister.icc"

}

#endif
