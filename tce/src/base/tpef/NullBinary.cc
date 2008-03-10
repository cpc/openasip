/**
 * @file NullBinary.cc
 *
 * Implementation of NullBinary class.
 *
 * @author Jari Mäntyneva 2006 (jari.mantyneva@tut.fi)
 * @note rating: yellow
 */

#include "NullBinary.hh"

namespace TPEF {

/////////////////////////////////////////////////////////////////////////////
// NullBinary
/////////////////////////////////////////////////////////////////////////////

NullBinary NullBinary::instance_;

/**
 * The constructor.
 */
NullBinary::NullBinary() : Binary() {
}

/**
 * The destructor.
 */
NullBinary::~NullBinary() {
}

/**
 * Returns an instance of NullBinary class (singleton).
 *
 * @return Singleton instance of NullBinary class.
 */
NullBinary&
NullBinary::instance() {
    return instance_;
}

}
