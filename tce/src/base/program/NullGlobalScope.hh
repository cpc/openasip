/**
 * @file NullGlobalScope.hh
 *
 * Declaration of NullGlobalScope class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_GLOBALSCOPE_HH
#define TTA_NULL_GLOBALSCOPE_HH

#include "GlobalScope.hh"

namespace TTAProgram {

/**
 * A singleton class that represents a null global scope.
 */
class NullGlobalScope : public GlobalScope {
public:
    virtual ~NullGlobalScope();
    static NullGlobalScope& instance();

protected:
    NullGlobalScope();

private:
    /// Copying not allowed.
    NullGlobalScope(const NullGlobalScope&);
    /// Assignment not allowed.
    NullGlobalScope& operator=(const NullGlobalScope&);

    /// Unique instance of NullGlobalScope.
    static NullGlobalScope instance_;
};

}

#endif
