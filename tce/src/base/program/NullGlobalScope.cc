/**
 * @file NullGlobalScope.cc
 *
 * Implementation of NullGlobalScope class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#include "NullGlobalScope.hh"

using namespace TTAMachine;

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// NullGlobalScope
/////////////////////////////////////////////////////////////////////////////

NullGlobalScope NullGlobalScope::instance_;

/**
 * The constructor.
 */
NullGlobalScope::NullGlobalScope() :
    GlobalScope() {
}

/**
 * The destructor.
 */
NullGlobalScope::~NullGlobalScope() {
}

/**
 * Returns an instance of NullGlobalScope class (singleton).
 *
 * @return Singleton instance of NullGlobalScope class.
 */
NullGlobalScope&
NullGlobalScope::instance() {
    return instance_;
}

}
