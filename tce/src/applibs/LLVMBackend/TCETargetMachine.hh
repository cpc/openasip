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
 * @file TCETargetMachine.h
 *
 * Declaration of TCETargetMachine class.
 *
 * @author Veli-Pekka Jääskeläinen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2009 (mikael.lepisto-no.spam-tut.fi)
 */

#ifndef TCE_TARGET_MACHINE_H
#define TCE_TARGET_MACHINE_H

#include "tce_config.h"

#include <set>
#include "llvm/Target/TargetLowering.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetFrameLowering.h"
#include "llvm/PassManager.h"
#include "TCESubtarget.hh"
#include "TCETargetMachinePlugin.hh"
#include "TCETargetSelectionDAGInfo.hh"

namespace TTAMachine {
    class Machine;
}

class PluginTools;

// just to be able to manually register tce target if needed.
extern "C" void LLVMInitializeTCETargetInfo();

namespace llvm {

    class Module;

    /**
     * TCE Universal machine target description.
     */
    class TCETargetMachine : public LLVMTargetMachine {

    public:
#ifndef LLVM_2_9
        TCETargetMachine(
	    const Target &T, const std::string &TT,
	    const std::string& CPU, const std::string &FS, 
	    Reloc::Model RM, CodeModel::Model CM);
#else
        TCETargetMachine(
	    const Target &T, const std::string &TT, const std::string &FS);
#endif


        virtual ~TCETargetMachine();

        virtual void setTargetMachinePlugin(TCETargetMachinePlugin& plugin);

        /**
         * These two parameter passing should be rethought... maybe TTAMach can be avoided
         * by changing pass to use plugin_ instead.
         */
        Module* emulationModule_;
        virtual void setEmulationModule(Module* mod) {
            emulationModule_ = mod;
        }

        TTAMachine::Machine* ttaMach_;
        virtual void setTTAMach(TTAMachine::Machine* mach) {
            ttaMach_ = mach;
        }

#ifdef LLVM_2_9
        virtual const TargetSubtarget* getSubtargetImpl() const {
            return &Subtarget; }
#else
        virtual const TargetSubtargetInfo* getSubtargetImpl() const {
            return &Subtarget; }
#endif
        virtual const TargetInstrInfo* getInstrInfo() const {
            return plugin_->getInstrInfo();
        }

        virtual const TargetRegisterInfo* getRegisterInfo() const {
            return plugin_->getRegisterInfo();
        }

        virtual const TargetData* getTargetData() const {
            return &DataLayout;
        }

        virtual const TargetFrameLowering* getFrameLowering() const {
            return plugin_->getFrameLowering();
        }
        virtual TargetLowering* getTargetLowering() const { 
            return plugin_->getTargetLowering();
        }

        virtual const TCESelectionDAGInfo* getSelectionDAGInfo() const {                                                                                                                                                                     return &tsInfo;                                                                                                                                                                                                            }                                                                                                                                                                                                                            
        virtual bool addPreISel(PassManagerBase& PM, 
                                CodeGenOpt::Level OptLevel);

        virtual bool addInstSelector(PassManagerBase& pm, 
                                     CodeGenOpt::Level OptLevel);
        
        // we do not want branch folder pass
        virtual bool getEnableTailMergeDefault() const;

        std::string operationName(unsigned opc) const {
            return plugin_->operationName(opc);
        }

        std::string rfName(unsigned dwarfRegNum) const {
            return plugin_->rfName(dwarfRegNum);
        }
        unsigned registerIndex(unsigned dwarfRegNum) const {
            return plugin_->registerIndex(dwarfRegNum);
        }

        TTAMachine::Machine* createMachine();

        std::string dataASName() {
            return plugin_->dataASName();
        }

        unsigned raPortDRegNum() const {
            return plugin_->raPortDRegNum();
        }

        void loadPlugin();

        unsigned spDRegNum() const {
            return plugin_->spDRegNum();
        }

        const std::set<
            std::pair<unsigned, 
                      llvm::MVT::SimpleValueType> >* missingOperations();

    private:
        /* more or less llvm naming convention to make it easier to track llvm changes */
        TCESubtarget        Subtarget;
        const TargetData    DataLayout; // Calculates type size & alignment
        
        TCESelectionDAGInfo tsInfo;
        TCETargetMachinePlugin* plugin_;
        PluginTools* pluginTool_;
        /// llvm::ISD opcode list of operations that have to be expanded.
        std::set<std::pair<unsigned, llvm::MVT::SimpleValueType> > missingOps_;
    };
}

#endif
