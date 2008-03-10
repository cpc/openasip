/**
 * @file NullImmediate.hh
 *
 * Declaration of NullImmediate class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_IMMEDIATE_HH
#define TTA_NULL_IMMEDIATE_HH

#include "Immediate.hh"

namespace TTAProgram {

/**
 * A singleton class that represents a null immediate.
 *
 * Calling any method causes the program to abort.
 */
class NullImmediate : public Immediate {
public:
    virtual ~NullImmediate();
    static NullImmediate& instance();

    Terminal& destination();
    TerminalImmediate& value();

protected:
    NullImmediate();

private:
    /// Copying not allowed.
    NullImmediate(const NullImmediate&);
    /// Assignment not allowed.
    NullImmediate& operator=(const NullImmediate&);

    static NullImmediate instance_;
};

}

#endif
