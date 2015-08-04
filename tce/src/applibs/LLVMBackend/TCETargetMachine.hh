/*
    Copyright (c) 2002-2015 Tampere University of Technology.

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

#include "CompilerWarnings.hh"

IGNORE_COMPILER_WARNING("-Wunused-parameter")

#include "llvm/Target/TargetLowering.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetFrameLowering.h"
#include "llvm/Target/TargetSelectionDAGInfo.h"
#ifdef LLVM_OLDER_THAN_3_7
#include "llvm/PassManager.h"
#else
#include "llvm/IR/PassManager.h"
#endif
//#include "TCESubtarget.hh"

#include "TCETargetMachinePlugin.hh"
// tce_config.h defines these. this undef to avoid warning.
// TODO: how to do this in tce_config.h???
#ifdef LLVM_LIBDIR
#undef LLVM_LIBDIR
#endif
#include "tce_config.h"

#include "llvm/CodeGen/Passes.h"
#include "llvm/IR/DataLayout.h"

POP_COMPILER_DIAGS

namespace TTAMachine {
    class Machine;
}

class PluginTools;


// just to be able to manually register tce target if needed.
extern "C" void LLVMInitializeTCETargetInfo();

namespace llvm {
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
	virtual bool addInstSelector();

#ifdef LLVM_OLDER_THAN_3_6
	virtual bool addPreRegAlloc();
	virtual bool addPreSched2();
#else
	virtual void addPreRegAlloc();
	virtual void addPreSched2();
#endif

	TCETargetMachinePlugin* plugin_;
    };

    class Module;

    /**
     * TCE Universal machine target description.
     */
    class TCETargetMachine : public LLVMTargetMachine {

    public:
#ifdef LLVM_OLDER_THAN_3_7
        TCETargetMachine(
            const Target &T, const std::string &TT,
            const std::string& CPU, const std::string &FS,
            const TargetOptions &Options,
            Reloc::Model RM, CodeModel::Model CM, CodeGenOpt::Level OL);
#else
        TCETargetMachine(
            const Target &T, const Triple& TTriple,
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

#ifdef LLVM_OLDER_THAN_3_7
        virtual const TargetSubtargetInfo* getSubtargetImpl() const {
            return plugin_->getSubtarget(); 
        }
#else
        virtual const TargetSubtargetInfo* getSubtargetImpl(const Function&) const {
            return plugin_->getSubtarget(); 
        }
#endif

        virtual const TargetInstrInfo* getInstrInfo() const {
            return plugin_->getInstrInfo();
        }

        virtual const TargetRegisterInfo* getRegisterInfo() const {
            return plugin_->getRegisterInfo();
        }

#if (defined(LLVM_3_2) || defined(LLVM_3_3) || defined(LLVM_3_4))
        virtual const DataLayout* getDataLayout() const { 
            return &dl_; 
        }
#else
        virtual const DataLayout* getDataLayout() const {
            return plugin_->getDataLayout();
        }
#endif
        virtual const TargetFrameLowering* getFrameLowering() const {
            return plugin_->getFrameLowering();
        }
        virtual TargetLowering* getTargetLowering() const { 
            return plugin_->getTargetLowering();
        }

#if (defined(LLVM_3_2) || defined(LLVM_3_3) || defined(LLVM_3_4))
        virtual const TargetSelectionDAGInfo* getSelectionDAGInfo() const {
            return &tsInfo_;
        }
#else
#ifdef LLVM_3_5
        virtual const TargetSelectionDAGInfo* getSelectionDAGInfo() const override {
            return plugin_->getSelectionDAGInfo();
        }
#endif
#endif

        virtual TargetPassConfig *createPassConfig(
            PassManagerBase &PM);

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

        unsigned opcode(TCEString operationName) const {
            return plugin_->opcode(operationName);
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

        unsigned getMaxMemoryAlignment() const {
            return plugin_->getMaxMemoryAlignment();
        }

        const std::set<
            std::pair<unsigned, 
                      llvm::MVT::SimpleValueType> >* missingOperations();

    private:
        /* more or less llvm naming convention to make it easier to track llvm changes */
#if (defined(LLVM_3_2) || defined(LLVM_3_3) || defined(LLVM_3_4))
        DataLayout dl_; // Calculates type size & alignment
        TargetSelectionDAGInfo tsInfo_;
#endif
        
        TCETargetMachinePlugin* plugin_;
        PluginTools* pluginTool_;
        /// llvm::ISD opcode list of operations that have to be expanded.
        std::set<std::pair<unsigned, llvm::MVT::SimpleValueType> > missingOps_;
    };
}

#endif
