/**
 * @file Immediate.hh
 *
 * Declaration of Immediate class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_IMMEDIATE_HH
#define TTA_IMMEDIATE_HH

#include "SimValue.hh"
#include "Exception.hh"

namespace TTAMachine {
    class InstructionTemplate;
}

namespace TTAProgram {

class Terminal;
class TerminalImmediate;

/**
 * Represents the long immediate value and its transport into a
 * dedicated register
 */
class Immediate {
public:
    Immediate(
        TerminalImmediate* value, Terminal* dst)
        throw (IllegalParameters);

    virtual ~Immediate();

    const Terminal& destination() const;

    TerminalImmediate& value() const;
    void setValue(TerminalImmediate* value);

    Immediate* copy() const;

private:
    /// Copying not allowed.
    Immediate(const Immediate&);
    /// Assignment not allowed.
    Immediate& operator=(const Immediate&);

    /// Value of the immediate.
    // SimValue value_;
    TerminalImmediate* value_;

    /// The destination register.
    Terminal* dst_;
};

}

#endif
