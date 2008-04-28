/**
 * @file MathTools.hh
 *
 * Mathematical tools.
 *
 * Declarations.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
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
