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
 * @author Veli-Pekka J��skel�inen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @author Mikael Lepist� 2009 (mikael.lepisto-no.spam-tut.fi)
 */

#ifndef TCE_TARGET_MACHINE_H
#define TCE_TARGET_MACHINE_H

#include <set>

#include "tce_config.h"
#include "CompilerWarnings.hh"

IGNORE_COMPILER_WARNING("-Wunused-parameter")

#include "llvm/CodeGen/TargetLowering.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/CodeGen/SelectionDAGTargetInfo.h"

#include "llvm/IR/PassManager.h"
//#include "TCESubtarget.hh"

#include "TCEStubTargetMachine.hh"
#include "TCETargetMachinePlugin.hh"
// tce_config.h defines these. this undef to avoid warning.
// TODO: how to do this in tce_config.h???
#ifdef LLVM_LIBDIR
#undef LLVM_LIBDIR
#endif


#include "llvm/CodeGen/Passes.h"
#include "llvm/IR/DataLayout.h"

#include "llvm/CodeGen/TargetPassConfig.h"

POP_COMPILER_DIAGS

namespace TTAMachine {
    class Machine;
}

class PipelineableLoopFinder;

class PluginTools;


// just to be able to manually register tce target if needed.
extern "C" void LLVMInitializeTCETarget();

namespace llvm {

#if LLVM_MAJOR_VERSION >= 21
typedef llvm::CodeGenTargetMachineImpl LLVMTargetMachine;
#else
typedef llvm::CodeGenOpt::Level CodeGenOptLevel;
#endif

class TCEPassConfig : public TargetPassConfig {
public:
    TCEPassConfig(
        LLVMTargetMachine* tm, PassManagerBase& pm,
        TCETargetMachinePlugin* plugin)
        : TargetPassConfig(*tm, pm), plugin_(plugin) {
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
        TCETargetMachine(
            const Target& T, const Triple& TTriple,
            const llvm::StringRef& CPU, const llvm::StringRef& FS,
            const TargetOptions& Options, std::optional<Reloc::Model> RM,
            std::optional<CodeModel::Model> CM, CodeGenOptLevel OL,
            bool isLittle);

        virtual ~TCETargetMachine();

        virtual void setTargetMachinePlugin(
            TCETargetMachinePlugin& plugin, TTAMachine::Machine& target);
        virtual TCETargetMachinePlugin& targetPlugin() const { return *plugin_; }

        /**
         * These two parameter passing should be rethought... maybe TTAMach can be avoided
         * by changing pass to use plugin_ instead.
         */
        Module* emulationModule_;
        virtual void setEmulationModule(Module* mod) {
            emulationModule_ = mod;
        }

        virtual void setTTAMach(
            const TTAMachine::Machine* mach) override {
            TCEBaseTargetMachine::setTTAMach(mach);
            calculateSupportedImmediates();
        }

        virtual const TTAMachine::Machine& ttaMachine() const {
            return *ttaMach_;
        }


        // This method is only in llvm < 3.7, but keep this here to
        // allow calling this ourselves.
        virtual const TCESubtarget* getSubtargetImpl() const {
            // compiler does not know it's derived without the plugin,
            // but this class cannow include the plugin. 
            return reinterpret_cast<const TCESubtarget*>(plugin_->getSubtarget());
        }

        virtual const TargetSubtargetInfo* getSubtargetImpl(const Function&) const override {
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
            PassManagerBase &PM) override;

        TargetTransformInfo
        getTargetTransformInfo(const Function& F) const override {
            return plugin_->getTargetTransformInfo(F);
        }

        std::string operationName(unsigned opc) const {
            return plugin_->operationName(opc);
        }

        /**
         * Returns true if LLVM opcode if valid for stack variable accesses.
         *
         */
        bool validStackAccessOperation(const std::string& opName) const {
            return plugin_->validStackAccessOperation(opName);
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

        /**
         * Returns full name of the register.
         *
         */
        std::string registerName(unsigned dwarfRegNum) const {
            return rfName(dwarfRegNum) + "."
                + std::to_string(registerIndex(dwarfRegNum));
        }

        unsigned llvmRegisterId(const TCEString& ttaRegister) {
            return plugin_->llvmRegisterId(ttaRegister);
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

        int getAddOpcode(const llvm::EVT& vt) const {
            return plugin_->getAddOpcode(vt);
        }

        int getShlOpcode(const llvm::EVT& vt) const {
            return plugin_->getShlOpcode(vt);
        }

        int getIorOpcode(const llvm::EVT& vt) const {
            return plugin_->getIorOpcode(vt);
        }

        void setStackAlignment(unsigned align) {
            stackAlignment_ = align;
        }

        unsigned stackAlignment() const {
            if (stackAlignment_ == 0) {
                std::cerr << "ZERO STACK ALIGN\n";
                abort();
            }
            assert(stackAlignment_ > 0);
            return stackAlignment_;
        }

        bool has8bitLoads() const {
            return plugin_->has8bitLoads();
        }

        bool has16bitLoads() const {
            return plugin_->has16bitLoads();
        }

        int bitness() const {
            return plugin_->is64bit() ? 64 : 32;
        }

        const std::set<
            std::pair<unsigned, 
                      llvm::MVT::SimpleValueType> >* missingOperations();

        const std::set<
            std::pair<unsigned, 
                      llvm::MVT::SimpleValueType> >* promotedOperations();

        const std::set<
            std::pair<unsigned,
                      llvm::MVT::SimpleValueType> >* customLegalizedOperations();

        int64_t smallestImmValue() const {
            assert(ttaMach_ && "setTargetMachinePlugin() was not called");
            return smallestImm_;
        }
        uint64_t largestImmValue() const {
            assert(ttaMach_ && "setTargetMachinePlugin() was not called");
            return largestImm_;
        }
        bool canEncodeAsMOVI(const llvm::MVT& vt, int64_t val) const;
        bool canEncodeAsMOVF(const llvm::APFloat& fp) const;
        
        bool canMaterializeConstant(const ConstantInt& ci) const {
            return plugin_->canMaterializeConstant(ci);
        }

        int getLoadOpcode(int asid, int align, const llvm::EVT& vt) const;

    private:
        /* more or less llvm naming convention to make it easier to track llvm changes */
        
        TCETargetMachinePlugin* plugin_;
        PluginTools* pluginTool_;
        /// llvm::ISD opcode list of operations that have to be expanded.
        std::set<std::pair<unsigned, llvm::MVT::SimpleValueType> > missingOps_;
        std::set<std::pair<unsigned, llvm::MVT::SimpleValueType> > promotedOps_;
        std::set<std::pair<unsigned, llvm::MVT::SimpleValueType> > customLegalizedOps_;

        void calculateSupportedImmediates();

        int64_t smallestImm_ = std::numeric_limits<int64_t>::max();
        uint64_t largestImm_ = std::numeric_limits<int64_t>::min();
        int SupportedFPImmWidth_ = std::numeric_limits<int>::min();
        // The stack alignment. Note: this should be overridden per
        // generated Module if the Module requires a larger one.
        unsigned stackAlignment_;
    };
}

#endif
