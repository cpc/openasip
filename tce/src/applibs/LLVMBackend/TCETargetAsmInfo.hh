/**
 * @file TCETargetAsmInfo.h
 *
 * Declaration of TCETargetAsmInfo class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel@cs.tut.fi)
 */

#ifndef TCE_TARGET_ASM_INFO_H
#define TCE_TARGET_ASM_INFO_H

#include "llvm/Target/TargetAsmInfo.h"

namespace llvm {
    class TCETargetMachine;

    /**
     * TCE assembly information.
     *
     * Initializes some directice strings etc.
     */
    struct TCETargetAsmInfo : public TargetAsmInfo {
        TCETargetAsmInfo(const TCETargetMachine& tm);
    };
}

#endif
