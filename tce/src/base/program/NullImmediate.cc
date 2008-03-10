/**
 * @file NullImmediate.cc
 *
 * Implementation of NullImmediate class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#include "NullImmediate.hh"
#include "NullTerminal.hh"
#include "NullInstructionTemplate.hh"

using namespace TTAMachine;

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// NullImmediate
/////////////////////////////////////////////////////////////////////////////

NullImmediate NullImmediate::instance_;

/**
 * The constructor.
 */
NullImmediate::NullImmediate() :
    Immediate(
        NULL, &NullTerminal::instance()) {
}

/**
 * The destructor.
 */
NullImmediate::~NullImmediate() {
}

/**
 * Returns an instance of NullImmediate class (singleton).
 *
 * @return Singleton instance of NullImmediate class.
 */
NullImmediate&
NullImmediate::instance() {
    return instance_;
}

/**
 * Aborts program with error log message.
 *
 * @return A null terminal.
 */
Terminal&
NullImmediate::destination() {
    abortWithError("destination()");
    return NullTerminal::instance();
}

/**
 * Aborts program with error log message.
 *
 * @return Null since there is no NullTerminalImmediate.
 */
TerminalImmediate& NullImmediate::value() {
    abortWithError("value()");
    return *static_cast<TerminalImmediate*>(NULL);
}

}
