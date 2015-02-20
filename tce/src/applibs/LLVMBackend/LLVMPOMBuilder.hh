/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file LLVMPOMBuilder.hh
 *
 * Declaration of LLVMPOMBuilder class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2007-2010
 * @note reting: red
 */

#ifndef LLVM_POM_BUILDER_H
#define LLVM_POM_BUILDER_H

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

#include "LLVMTCEBuilder.hh"
#include "TCETargetMachine.hh"

#pragma clang diagnostic pop

namespace TTAMachine {
    class Machine;
}

namespace llvm {

    FunctionPass* createLLVMPOMBuilderPass(
        TCETargetMachine& tm, TTAMachine::Machine* mach);

    /**
     * Implements building TCE POM from RISC-style sequential input 
     * from LLVM codegen.
     */
    class LLVMPOMBuilder : public LLVMTCEBuilder {
    public:
        static char ID; // Pass identification, replacement for typeid

        LLVMPOMBuilder(char& ID);

        LLVMPOMBuilder(
            TCETargetMachine& tm,
            TTAMachine::Machine* mach);

        virtual ~LLVMPOMBuilder() {}

    protected:

        virtual unsigned spDRegNum() const { 
            return dynamic_cast<const TCETargetMachine&>(
                targetMachine()).spDRegNum(); 
        }

        // the return address register's llvm reg number
        virtual unsigned raPortDRegNum() const { 
            return dynamic_cast<const TCETargetMachine&>(
                targetMachine()).raPortDRegNum(); 
        }

        // the ADF register file name of the llvm reg number
        virtual TCEString registerFileName(unsigned llvmRegNum) const { 
            return dynamic_cast<const TCETargetMachine&>(
                targetMachine()).rfName(llvmRegNum); 
        }

        // the ADF register index of the llvm reg number
        virtual int registerIndex(unsigned llvmRegNum) const {
            return dynamic_cast<const TCETargetMachine&>(
                targetMachine()).registerIndex(llvmRegNum); 
        }

        // OSAL operation name from a LLVM MachineInstr
        virtual TCEString operationName(const MachineInstr& mi) const {
            return dynamic_cast<const TCETargetMachine&>(
                targetMachine()).operationName(mi.getDesc().getOpcode());
        }

        virtual TTAProgram::Terminal* createFUTerminal(
            const MachineOperand&) const {
            // no FU terminals in the RISC-style backend, always through GPRs
            return NULL; 
        }
    };
}
#endif
