/**
 * @file TCEPlugin.h
 *
 * TCE Target plugin declaration.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel@cs.tut.fi)
 */

#ifndef TCE_PLUGIN_H
#define TCE_PLUGIN_H

#include <iosfwd>

#include <llvm/Target/TargetInstrInfo.h>

/**
 * Instruction selector and code printer creator declarations.
 */
namespace llvm  {
    class FunctionPass;
    class TCETargetMachine;

    FunctionPass* createTCEISelDag(TCETargetMachine& tm);
    FunctionPass* createTCECodePrinterPass(
        std::ostream& os, TargetMachine& tm);
}

#include "TCEGenRegisterNames.inc"
#include "TCEGenInstrNames.inc"

#endif
