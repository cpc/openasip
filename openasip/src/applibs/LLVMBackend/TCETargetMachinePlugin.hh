/*
    Copyright (c) 2002-2025 Tampere University.

    This file is part of OpenASIP.

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
 * @file TCETargetMachinePlugin.hh
 *
 * Declaration of TCETargetMachinePlugin class.
 *
 * @author Veli-Pekka Jääskeläinen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TCE_TARGET_MACHINE_PLUGIN_HH
#define TCE_TARGET_MACHINE_PLUGIN_HH

#include <iostream>

#include "tce_config.h"

#include <llvm/IR/DataLayout.h>
#include <llvm/CodeGen/SelectionDAGTargetInfo.h>

typedef llvm::DataLayout TargetData;

#include "TCEString.hh"

namespace llvm {
    class MachineOperand;
}

namespace TTAMachine {
    class Machine;
}

/**
 * TCE target machine plugin interface.
 */
namespace llvm {
   class TargetInstrInfo;
   class TCEInstrInfo;
   class TargetLowering;
   class TargetRegisterInfo;
   class TargetFrameLowering;
   class TargetSubtargetInfo;
   class TargetTransformInfo;
   class TCESubtarget;
   class FunctionPass;
   class TCETargetMachine;
   class TargetRegisterClass;
   class MachineInstr;
   class SDNode;
   struct EVT;

   class TCETargetMachinePlugin {
    public:
       TCETargetMachinePlugin() : lowering_(NULL), tm_(NULL),
                                  dl_("") {};

       virtual ~TCETargetMachinePlugin() {};

       virtual const TargetInstrInfo* getInstrInfo() const = 0;
       virtual const TargetRegisterInfo* getRegisterInfo() const = 0;
       virtual const TargetFrameLowering* getFrameLowering() const = 0;
       virtual TargetLowering* getTargetLowering() const = 0;
       virtual const TargetSubtargetInfo* getSubtarget() const = 0;
       virtual TargetTransformInfo getTargetTransformInfo(
           const Function& F) const = 0;

       virtual FunctionPass* createISelPass(TCETargetMachine* tm) = 0;

       /// Returns name of the physical register file corresponding
       /// to a generated register ID.
       virtual std::string rfName(unsigned dwarfRegNum) = 0;

       /// Returns name of the physical register index corresponding
       /// to a generated register ID.
       virtual unsigned registerIndex(unsigned dwarfRegNum) = 0;

       /** Returns LLVM register ID corresponding to TTA register name.
        *
        * @param ttaRegister The name of the TTA register. For example "RF.5".
        * @returns The register number if it exists in the target.
        *          Otherwise, returns TCE::NoRegister.
        */
       virtual unsigned llvmRegisterId(const TCEString& ttaRegister) = 0;

       /// Returns operation name corresponding to llvm target opcode.
       virtual std::string operationName(unsigned opc) const = 0;
       /// Returns true if OSAL operation is valid for stack accesses.
       virtual bool validStackAccessOperation(
           const std::string& opName) const = 0;
       /// Returns true in case the target supports the given osal operation
       virtual bool hasOperation(TCEString operationName) const = 0;
       /// Returns the opcode for the given osal operation, undefined if not found.
       virtual unsigned opcode(TCEString operationName) const = 0;
       virtual unsigned int extractElementLane(const MachineInstr& mi) const = 0;

       virtual int getTruePredicateOpcode(unsigned opc) const = 0;
       virtual int getFalsePredicateOpcode(unsigned opc) const = 0;

       /// Returns name of the data address space.
       virtual std::string dataASName() = 0;
       /// Returns ID number of the return address register.
       virtual unsigned raPortDRegNum() = 0;
       /// Returns ID number of the stack pointer register.
       virtual unsigned spDRegNum() = 0;
       /// Returns ID number of the frame pointer register.
       virtual unsigned fpDRegNum() = 0;

       virtual unsigned rvDRegNum() = 0;

       virtual unsigned rvHighDRegNum() = 0;

       virtual std::vector<unsigned> getParamDRegNums() const = 0;

       virtual std::vector<unsigned> getVectorRVDRegNums() const = 0;

       virtual bool hasUDIV() const = 0;
       virtual bool hasSDIV() const = 0;
       virtual bool hasUREM() const = 0;
       virtual bool hasSREM() const = 0;
       virtual bool hasMUL() const = 0;
       virtual bool hasROTL() const = 0;
       virtual bool hasROTR() const = 0;
       virtual bool hasSXHW() const = 0;
       virtual bool hasSXQW() const = 0;
       virtual bool hasSQRTF() const = 0;
       virtual bool hasSHR() const = 0;
       virtual bool hasSHL() const = 0;
       virtual bool hasSHRU() const = 0;

       virtual bool has8bitLoads() const = 0;
       virtual bool has16bitLoads() const = 0;

       virtual int maxVectorSize() const = 0;
       /// Plugin needs target machine for TragetLowering generation
       virtual void registerTargetMachine(TCETargetMachine &tm) = 0;
       // if not found, return -1;
       virtual int getMinOpcode(llvm::SDNode* n) const = 0;
       virtual int getMaxOpcode(llvm::SDNode* n) const = 0;
       virtual int getMinuOpcode(llvm::SDNode* n) const = 0;
       virtual int getMaxuOpcode(llvm::SDNode* n) const = 0;

       virtual int getLoad(const TargetRegisterClass *rc) const = 0;
       virtual int getStore(const TargetRegisterClass *rc) const = 0;

       virtual int getLoadOpcode(const llvm::EVT& vt) const = 0;
       // -1 or add opcode. Implementation generated to Backend.inc
       virtual int getAddOpcode(const llvm::EVT& vt) const = 0;
       // -1 or shl opcode. Implementation generated to Backend.inc
       virtual int getShlOpcode(const llvm::EVT& vt) const = 0;
       // -1 or ior opcode. Implementation generated to Backend.inc
       virtual int getIorOpcode(const llvm::EVT& vt) const = 0;

       /// Clustered-TTA-subtemplate related methods.
       virtual const llvm::TargetRegisterClass* extrasRegClass(
           const llvm::TargetRegisterClass* current) const = 0;
       virtual const llvm::TargetRegisterClass* nodeRegClass(
           unsigned nodeId, const llvm::TargetRegisterClass* current) const = 0;

       virtual bool isLittleEndian() const = 0;
       virtual bool is64bit() const = 0;

       virtual const DataLayout* getDataLayout() const {
           assert(!dl_.isDefault());
           return &dl_;
       }

       virtual DataLayout* getDataLayout() {
           assert(!dl_.isDefault());
           return &dl_;
       }

       virtual void
       setDataLayout(const std::string& DLString) {
#if LLVM_MAJOR_VERSION < 21
           dl_->reset(dataLayoutStr.c_str());
#else
           auto DL = DataLayout::parse(DLString);
           if (!DL) {
             assert (false && "Couldn't generate datalayout.");
           }
           dl_ = DL.get();
#endif
       }

       virtual TCETargetMachine*
       getCurrentTargetMachine() {
           return tm_;
       }
       virtual const TCETargetMachine*
       getCurrentTargetMachine() const {
           return tm_;
       }

       virtual bool analyzeCCBranch(
           llvm::MachineInstr& i,
           llvm::SmallVectorImpl<llvm::MachineOperand>& cond) const {
           return true;
       }

       virtual const SelectionDAGTargetInfo* getSelectionDAGInfo() const {
           return &tsInfo_;
       }

        // Implementation generated to Backend.inc
        virtual bool canMaterializeConstant(const ConstantInt& ci) const = 0;

        virtual std::tuple<int, int> getPointerAdjustment(
            int offset) const = 0;

       virtual MVT::SimpleValueType getDefaultType() const = 0;
   protected:
       /// Target machine instruction info for the llvm framework. 
       TCEInstrInfo* instrInfo_;
       TargetLowering* lowering_;
       TargetFrameLowering* frameInfo_;
       TCETargetMachine* tm_;
       TCESubtarget* subTarget_;
       DataLayout dl_; // Calculates type size & alignment
       SelectionDAGTargetInfo tsInfo_;
   };

}
#endif
