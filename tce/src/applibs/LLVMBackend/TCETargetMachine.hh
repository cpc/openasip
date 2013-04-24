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

#include <set>
#include "llvm/Target/TargetLowering.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetFrameLowering.h"
#include "llvm/PassManager.h"
#include "TCESubtarget.hh"
#include "TCETargetMachinePlugin.hh"
#include "TCETargetSelectionDAGInfo.hh"
// tce_config.h defines these. this undef to avoid warning.
// TODO: how to do this in tce_config.h???
#ifdef LLVM_LIBDIR
#undef LLVM_LIBDIR
#endif
#include "tce_config.h"

#include "llvm/CodeGen/Passes.h"

#ifdef LLVM_3_1

#include "llvm/Target/TargetData.h"

#elif defined(LLVM_3_2)

#include "llvm/DataLayout.h"
typedef llvm::DataLayout TargetData;

#else

#include "llvm/IR/DataLayout.h"
typedef llvm::DataLayout TargetData;

#endif


namespace TTAMachine {
    class Machine;
}

class PluginTools;

// just to be able to manually register tce target if needed.
extern "C" void LLVMInitializeTCETargetInfo();

namespace llvm {
#ifndef LLVM_3_0
    class TCEPassConfig : public TargetPassConfig {
    public:
	TCEPassConfig(
	    LLVMTargetMachine* tm, 
	    PassManagerBase& pm, 
	    TCETargetMachinePlugin* plugin) :
	    TargetPassConfig(tm, pm), plugin_(plugin) {
	    assert(plugin_ != NULL);
	}

	virtual bool addPreISel();
	virtual bool addPreRegAlloc();
	virtual bool addInstSelector();

	virtual bool addPreSched2();

	TCETargetMachinePlugin* plugin_;
    };
#endif

    class Module;

    /**
     * TCE Universal machine target description.
     */
    class TCETargetMachine : public LLVMTargetMachine {

    public:
#ifdef LLVM_3_0
        TCETargetMachine(
	    const Target &T, const std::string &TT,
	    const std::string& CPU, const std::string &FS,
	    Reloc::Model RM, CodeModel::Model CM);

#else
        TCETargetMachine(
	    const Target &T, const std::string &TT,
	    const std::string& CPU, const std::string &FS,
	    const TargetOptions &Options,
	    Reloc::Model RM, CodeModel::Model CM, CodeGenOpt::Level OL);
#endif
        virtual ~TCETargetMachine();

        virtual void setTargetMachinePlugin(TCETargetMachinePlugin& plugin);
        virtual TCETargetMachinePlugin& targetPlugin() const { return *plugin_; }

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

        virtual const TargetSubtargetInfo* getSubtargetImpl() const {
            return &Subtarget; }

        virtual const TargetInstrInfo* getInstrInfo() const {
            return plugin_->getInstrInfo();
        }

        virtual const TargetRegisterInfo* getRegisterInfo() const {
            return plugin_->getRegisterInfo();
        }

        virtual const TargetData* getTargetData() const {
            return &DL;
        }

#ifndef LLVM_3_1
        virtual const DataLayout* getDataLayout() const { 
            return &DL; 
        }
#endif

        virtual const TargetFrameLowering* getFrameLowering() const {
            return plugin_->getFrameLowering();
        }
        virtual TargetLowering* getTargetLowering() const { 
            return plugin_->getTargetLowering();
        }

        virtual const TCESelectionDAGInfo* getSelectionDAGInfo() const {
	    return &tsInfo;
	}

#ifdef LLVM_3_0
        virtual bool addPreISel(PassManagerBase& PM, 
                                CodeGenOpt::Level OptLevel);

        virtual bool addInstSelector(PassManagerBase& pm, 
                                     CodeGenOpt::Level OptLevel);

        // we do not want branch folder pass(we don not??)
        virtual bool getEnableTailMergeDefault() const;
#else

	virtual TargetPassConfig *createPassConfig(
	    PassManagerBase &PM);

#endif

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

        int getMinOpcode(llvm::SDNode* n) {
            return plugin_->getMinOpcode(n);
        }

        int getMinuOpcode(llvm::SDNode* n) {
            return plugin_->getMinuOpcode(n);
        }

        int getMaxOpcode(llvm::SDNode* n) {
            return plugin_->getMaxOpcode(n);
        }

        int getMaxuOpcode(llvm::SDNode* n) {
            return plugin_->getMaxuOpcode(n);
        }

        int maxVectorSize() const {
            return plugin_->maxVectorSize();
        }

        const std::set<
            std::pair<unsigned, 
                      llvm::MVT::SimpleValueType> >* missingOperations();

    private:
        /* more or less llvm naming convention to make it easier to track llvm changes */
        TCESubtarget        Subtarget;
        const TargetData    DL; // Calculates type size & alignment
        
        TCESelectionDAGInfo tsInfo;
        TCETargetMachinePlugin* plugin_;
        PluginTools* pluginTool_;
        /// llvm::ISD opcode list of operations that have to be expanded.
        std::set<std::pair<unsigned, llvm::MVT::SimpleValueType> > missingOps_;
    };
}

#endif
