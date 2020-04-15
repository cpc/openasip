/*
    Copyright (c) 2002-2015 Tampere University.

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

#ifdef LLVM_OLDER_THAN_6_0
#include "llvm/Target/TargetLowering.h"
#include "llvm/Target/TargetFrameLowering.h"
#else
#include "llvm/CodeGen/TargetLowering.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#endif
#include "llvm/Target/TargetMachine.h"
#ifdef LLVM_OLDER_THAN_3_9
#include "llvm/Target/TargetSelectionDAGInfo.h"
#else
#include "llvm/CodeGen/SelectionDAGTargetInfo.h"
#endif

#include "llvm/IR/PassManager.h"
//#include "TCESubtarget.hh"

#include "TCEStubTargetMachine.hh"
#include "TCETargetMachinePlugin.hh"
// tce_config.h defines these. this undef to avoid warning.
// TODO: how to do this in tce_config.h???
#ifdef LLVM_LIBDIR
#undef LLVM_LIBDIR
#endif
#include "tce_config.h"

#include "llvm/CodeGen/Passes.h"
#include "llvm/IR/DataLayout.h"

#ifndef LLVM_OLDER_THAN_3_9
#include "llvm/CodeGen/TargetPassConfig.h"
#endif

POP_COMPILER_DIAGS

namespace TTAMachine {
    class Machine;
}

class PluginTools;


// just to be able to manually register tce target if needed.
extern "C" void LLVMInitializeTCETarget();

namespace llvm {
    class TCEPassConfig : public TargetPassConfig {
    public:
	TCEPassConfig(
	    LLVMTargetMachine* tm, 
	    PassManagerBase& pm, 
	    TCETargetMachinePlugin* plugin) :
#ifdef LLVM_OLDER_THAN_5_0
	    TargetPassConfig(tm, pm),
#else
	    TargetPassConfig(*tm, pm),
#endif
plugin_(plugin) {
	    assert(plugin_ != NULL);
	}

	virtual bool addPreISel();
	virtual bool addInstSelector();

	virtual void addPreRegAlloc();
	virtual void addPreSched2();

	TCETargetMachinePlugin* plugin_;
    };

    class Module;

    /**
     * TCE Universal machine target description.
     */
    class TCETargetMachine : public TCEBaseTargetMachine {

    public:
#ifdef LLVM_OLDER_THAN_3_9
        TCETargetMachine(
            const Target &T, const Triple& TTriple,
            const std::string& CPU, const std::string &FS, 
            const TargetOptions &Options,
            Reloc::Model RM, CodeModel::Model CM, CodeGenOpt::Level OL);
#elif LLVM_OLDER_THAN_6_0
        TCETargetMachine(
            const Target &T, const Triple& TTriple,
            const std::string& CPU, const std::string &FS,
            const TargetOptions &Options,
            Optional<Reloc::Model> RM, CodeModel::Model CM,
            CodeGenOpt::Level OL);
#else
        TCETargetMachine(
            const Target &T, const Triple& TTriple,
            const std::string& CPU, const std::string &FS,
            const TargetOptions &Options,
            Optional<Reloc::Model> RM, Optional<CodeModel::Model> CM,
            CodeGenOpt::Level OL, bool isLittle);
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

        // This method is only in llvm < 3.7, but keep this here to
        // allow calling this ourselves.
        virtual const TCESubtarget* getSubtargetImpl() const {
            // compiler does not know it's derived without the plugin,
            // but this class cannow include the plugin. 
            return reinterpret_cast<const TCESubtarget*>(plugin_->getSubtarget());
        }

        virtual const TargetSubtargetInfo* getSubtargetImpl(const Function&) const {
            return plugin_->getSubtarget(); 
        }

        virtual const TargetInstrInfo* getInstrInfo() const {
            return plugin_->getInstrInfo();
        }

        virtual const TargetRegisterInfo* getRegisterInfo() const {
            return plugin_->getRegisterInfo();
        }

        virtual const DataLayout* getDataLayout() const {
            return plugin_->getDataLayout();
        }

        virtual const TargetFrameLowering* getFrameLowering() const {
            return plugin_->getFrameLowering();
        }
        virtual TargetLowering* getTargetLowering() const { 
            return plugin_->getTargetLowering();
        }

        virtual TargetPassConfig *createPassConfig(
            PassManagerBase &PM);

        std::string operationName(unsigned opc) const {
            return plugin_->operationName(opc);
        }

        bool hasOperation(TCEString operationName) const {
            return plugin_->hasOperation(operationName);
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

        bool has8bitLoads() const {
            return plugin_->has8bitLoads();
        }

        bool has16bitLoads() const {
            return plugin_->has16bitLoads();
        }

        const std::set<
            std::pair<unsigned, 
                      llvm::MVT::SimpleValueType> >* missingOperations();

        const std::set<
            std::pair<unsigned,
                      llvm::MVT::SimpleValueType> >* customLegalizedOperations();

    private:
        /* more or less llvm naming convention to make it easier to track llvm changes */
        
        TCETargetMachinePlugin* plugin_;
        PluginTools* pluginTool_;
        /// llvm::ISD opcode list of operations that have to be expanded.
        std::set<std::pair<unsigned, llvm::MVT::SimpleValueType> > missingOps_;
        std::set<std::pair<unsigned, llvm::MVT::SimpleValueType> > customLegalizedOps_;
    };
}

#endif
