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
 * @file MathTools.hh
 *
 * Mathematical tools.
 *
 * Declarations.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 *
 */

#ifndef TTA_MATH_TOOLS_HH
#define TTA_MATH_TOOLS_HH

#include "BaseType.hh"

class MathTools {
public:
    static int requiredBits(long unsigned int number);
    static int requiredBitsSigned(long int number);
    static int requiredBitsSigned(int number);
    static int requiredBitsSigned(UInt32 number);
    static int requiredBitsSigned(unsigned long number);
    static int signExtendTo(int value, int width);
    static int zeroExtendTo(int value, int width);
    
    static int fastSignExtendTo(int value, int width);
    static int fastZeroExtendTo(int value, int width);

    static bool bit(unsigned int integer, unsigned int index);
    
    static int random(int, int);
};

#include "MathTools.icc"

#endif
