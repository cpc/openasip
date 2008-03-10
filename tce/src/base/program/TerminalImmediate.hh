/**
 * @file TerminalImmediate.hh
 *
 * Declaration of TerminalImmediate class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_TERMINAL_IMMEDIATE_HH
#define TTA_TERMINAL_IMMEDIATE_HH

#include "Terminal.hh"
#include "AnnotatedInstructionElement.hh"

namespace TTAProgram {

/**
 * Represents an inline immediate.
 */
class TerminalImmediate : public Terminal, public AnnotatedInstructionElement {
public:
    TerminalImmediate(SimValue value);
    virtual ~TerminalImmediate();

    virtual bool isImmediate() const;
    virtual SimValue value() const throw (WrongSubclass);

    virtual Terminal* copy() const;
    virtual bool equals(const Terminal& other) const;

private:
    /// Assignment not allowed.
    TerminalImmediate& operator=(const TerminalImmediate&);
protected:
    /// Value of the inline immediate.
    SimValue value_;
};

}

#endif
