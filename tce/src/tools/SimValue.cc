/**
 * @file SimValue.icc
 *
 * Non-inline definitions of SimValue class.
 *
 * @author Pekka Jääskeläinen 2004 (pjaaskel@cs.tut.fi)
 * @note This file is used in compiled simulation. Keep dependencies *clean*
 * @note rating: red
 */

#include "SimValue.hh"
#include "MathTools.hh"

//////////////////////////////////////////////////////////////////////////////
// NullSimValue
//////////////////////////////////////////////////////////////////////////////

SimValue NullSimValue::instance_(0);

/**
 * Returns an instance of NullSimValue class (singleton).
 *
 * @return Singleton instance of NullSimValue class.
 */
SimValue&
NullSimValue::instance() {
    return instance_;
}

/**
 * Returns the value as a signed integer.
 *
 * @return Signed integer value of the SimValue.
 */
int
SimValue::intValue() const {
    int bitWidth = (bitWidth_ > 32) ? 32 : bitWidth_;
    return MathTools::fastSignExtendTo(value_.sIntWord, bitWidth);
}

/**
 * Returns the value as a signed integer.
 *
 * @return Unsigned integer value of the SimValue.
 */
unsigned int
SimValue::unsignedValue() const {
    int bitWidth = (bitWidth_ > 32) ? 32 : bitWidth_;
    return MathTools::fastZeroExtendTo(value_.uIntWord, bitWidth);
}

