/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file SimValue.icc
 *
 * Non-inline definitions of SimValue class.
 *
 * @author Pekka Jääskeläinen 2004 (pjaaskel-no.spam-cs.tut.fi)
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

