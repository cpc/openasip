/**
 * @file TCETargetAsmInfo.cpp
 *
 * Implementation of TCETargetAsmInfo class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel@cs.tut.fi)
 */

#include "TCETargetAsmInfo.hh"

using namespace llvm;

/**
 * The Constructor.
 *
 * Sets some TCE-specific values in the base class attributes.
 */
TCETargetAsmInfo::TCETargetAsmInfo(const TCETargetMachine& tm) {
    Data16bitsDirective = "\t.word\t";
    Data64bitsDirective = 0;
    ZeroDirective = "\t.space\t";
    CommentString = " # ";

    ConstantPoolSection = "\t# -- Constant pool --\n\t.data\n";
    PrivateGlobalPrefix = "L";
    GlobalPrefix = "_";
    InlineAsmStart = "# INLINE ASM Start";
    InlineAsmEnd = "# INLINE ASM End";

    // TCEAsmPrinter doesn't print string initializations.
    AsciiDirective = NULL;
    AscizDirective = NULL;

    LCOMMDirective = "\t.comm\t";
}
