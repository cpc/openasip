/*
    Copyright (c) 2002-2009 Tampere University of Technology.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file TCEAsmPrinter.cpp
 *
 * TCEAsmPrinter implementatation.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel-no.spam-cs.tut.fi)
 */

#define DEBUG_TYPE "asm-printer"

#include <cctype>
#include <iostream>

#include <llvm/Constants.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Module.h>
#include <llvm/CodeGen/MachineFunctionPass.h>
#include <llvm/CodeGen/MachineConstantPool.h>
#include <llvm/CodeGen/MachineInstr.h>
#include <llvm/Target/TargetAsmInfo.h>
#include <llvm/Target/TargetData.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/Mangler.h>
#include <llvm/ADT/Statistic.h>
#include <llvm/ADT/StringExtras.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/MathExtras.h>
#include "llvm/Support/raw_ostream.h"

#include "TCEAsmPrinter.hh"
#include "TCEInstrInfo.hh"
#include "tce_config.h"


using namespace llvm;

/**
 * Constructor.
 */
TCEAsmPrinter::TCEAsmPrinter(
    llvm::raw_ostream& o, TargetMachine& tm, const TargetAsmInfo* t) :
    AsmPrinter(o, tm, t), asmInfo_(*t) {
}

/**
 * Destructor.
 */
TCEAsmPrinter::~TCEAsmPrinter() {
}


/**
 * Creates an instance of the asm printer.
 *
 * @param o Output stream where the assembly code is printed.
 * @param tm Target machine description.
FunctionPass*
llvm::createTCECodePrinterPass(std::ostream& o, TargetMachine& tm) {
    return new TCEAsmPrinter(o, tm, tm.getTargetAsmInfo());
}
 */


/**
 * Prints assembly code of a machine function.
 *
 * @param mf Machine function to print.
 */
bool
TCEAsmPrinter::runOnMachineFunction(MachineFunction& mf) {

    SetupMachineFunction(mf);
    EmitConstantPool(mf.getConstantPool());

    O << '\n' << '\n';
    CurrentFnName = Mang->getValueName(mf.getFunction());

    // label
    const Function* f = mf.getFunction();
    SwitchToTextSection(asmInfo_.SectionForGlobal(f)->getName().c_str(), f);
    EmitAlignment(1);
    O << ".globl\t" << CurrentFnName << ";\n";
    O << CurrentFnName << ":\n";

    // Basic block labels are numbered.
    static unsigned bbNumber = 0;

    NumberForBB_.clear();
    for (MachineFunction::const_iterator i = mf.begin();
         i != mf.end(); i++) {

        NumberForBB_[i->getBasicBlock()]  = bbNumber++;
    }

    // Print function code.
    for (MachineFunction::const_iterator i = mf.begin();
         i != mf.end(); i++) {

        if (i != mf.begin()) {
            printBasicBlockLabel(i, true);
            O << "\n";
        }

        for (MachineBasicBlock::const_iterator j = i->begin();
             j != i->end(); j++) {

            O << "\t";
            printInstruction(j);
        }
    }
    return false;
}


/**
 * Prints operand assembly code.
 *
 * @param mi Machine instruction containing the operand.
 * @param opNum Index of the operand in the instruction.
 */
void
TCEAsmPrinter::printOperand(const MachineInstr* mi, int opNum) {

    assert((unsigned)opNum <= mi->getNumOperands());

    const MachineOperand& mo = mi->getOperand(opNum);
    const TargetRegisterInfo& ri = *TM.getRegisterInfo();

    switch (mo.getType()) {
    case MachineOperand::MO_Register: {
        O << ri.get(mo.getReg()).Name;
        break;
    }
    case MachineOperand::MO_Immediate: {
        O << (int)mo.getImm();
        break;
    }

    case MachineOperand::MO_MachineBasicBlock:
        printBasicBlockLabel(mo.getMBB(), false, false);
        return;
    case MachineOperand::MO_GlobalAddress: {
        O << Mang->getValueName(mo.getGlobal());
        break;
    }
    case MachineOperand::MO_ExternalSymbol:
        O << mo.getSymbolName();
        break;
    case MachineOperand::MO_ConstantPoolIndex:
        O << TAI->getPrivateGlobalPrefix()
          << "CPI" << getFunctionNumber() << "_"
          << mo.getIndex();
        break;
    default:
        O << "<unknown operand type>";
        break;

    }
}

/**
 * Prints  a memory operand.
 *
 * @param mi Machine instruction containing the memory operand.
 * @param opNum Index of the operand.
 */
void
TCEAsmPrinter::printMemOperand(
    const MachineInstr* mi, int opNum, const char* modifier) {

    // TODO: modifier? Constant base + offset?
    printOperand(mi, opNum);
}

/**
 * Not implemented.
 */
void
TCEAsmPrinter::printCCOperand(const MachineInstr* /* mi */, int /* opNum */) {
    assert(false);
}

/**
 * Initializes the asm printer instance.
 */
bool
TCEAsmPrinter::doInitialization(Module& m) {
    AsmPrinter::doInitialization(m);
    Mang->markCharUnacceptable('.');
    return false;
}

/**
 * Finalizes asm printing.
 */
bool
TCEAsmPrinter::doFinalization(Module& m) {

    const TargetData* td = TM.getTargetData();
  

    // Print out module-level global variables here.
    for (Module::const_global_iterator i = m.global_begin();
         i != m.global_end(); i++) {

        if (i->hasInitializer()) {   // External global require no code
            // Check to see if this is a special global
            // used by LLVM, if so, emit it.
            if (EmitSpecialLLVMGlobal(i)) {
                continue;
            }

            O << "\n" << "\n";
            std::string name = Mang->getValueName(i);
            Constant* c = i->getInitializer();

            if (c->isNullValue() &&
                (i->hasLinkOnceLinkage() || i->hasInternalLinkage() ||
                 i->hasWeakLinkage())) {

                SwitchToDataSection(".data", i);
                O << "\t.comm ";
                O << name << ", " << td->getTypeSizeInBits(c->getType()) / 8;
                O << "\n";
            } else {
                switch (i->getLinkage()) {
                case GlobalValue::LinkOnceLinkage:
                case GlobalValue::WeakLinkage:
                    // TODO: Check these.
                case GlobalValue::AppendingLinkage:
                    // FIXME: appending linkage variables should go
                    // into a section of their name or something.
                    // For now, just emit them as external.
                case GlobalValue::ExternalLinkage:
                    // If external or appending, declare as a global symbol
                    O << "\t.globl " << name << "\n";
                    // FALL THROUGH
                case GlobalValue::InternalLinkage:
                    SwitchToDataSection(".data", i);
                    break;
                case GlobalValue::GhostLinkage:
                    cerr << "Should not have any unmaterialized functions!"
                         << "\n";
                    abort();
                case GlobalValue::DLLImportLinkage:
                    cerr << "DLLImport linkage is not supported." << "\n";
                    abort();
                case GlobalValue::DLLExportLinkage:
                    cerr << "DLLExport linkage is not supported." << "\n";
                    abort();
                default:
                    assert(0 && "Unknown linkage type!");
                }
                
                O << name << ":" << "\n";

                // We don't want byte arrays initialized by ascii-data.
                // Assembler has some problems interpreting the escaped
                // strings, for example 0x18 0x32 -> "\0302" is interpreted
                // as \302 -> 0xc2.
                const ConstantArray* cva = dyn_cast<ConstantArray>(c);
                if (cva != NULL && cva->isString()) {
                    // print string operand by operand
                    for (unsigned ci = 0, e = cva->getNumOperands();
                         ci != e; ++ci) {

                        EmitGlobalConstant(cva->getOperand(ci));
                    }
                } else {
                    // Not a string, emit as usual.
                    EmitGlobalConstant(c);
                }
            }
        }
    }

    AsmPrinter::doFinalization(m);
    return false;
}

/**
 * Prints assembly operand.
 *
 * TODO: comment parameters.
 */
bool
TCEAsmPrinter::PrintAsmOperand(
    const MachineInstr* mi, unsigned opNo,
    unsigned asmVariant, const char* extraCode){

    printOperand(mi, opNo);
    return false;
}

#include "TCEGenAsmWriter.inc"
