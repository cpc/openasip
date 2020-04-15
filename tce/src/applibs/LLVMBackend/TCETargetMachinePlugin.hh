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
 * @file TCETargetMachinePlugin.hh
 *
 * Declaration of TCETargetMachinePlugin class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TCE_TARGET_MACHINE_PLUGIN_HH
#define TCE_TARGET_MACHINE_PLUGIN_HH

#include <iostream>

#include "tce_config.h"

#define TCEBEDLString \
    "E-p:32:32:32"    \
    "-a0:0:32"        \
    "-i1:8:8"         \
    "-i8:8:32"        \
    "-i16:16:32"      \
    "-i32:32:32"      \
    "-i64:32:32"      \
    "-f16:16:16"      \
    "-f32:32:32"      \
    "-f64:32:64"      \
    "-v64:32:64"      \
    "-v128:32:128"    \
    "-v512:32:512"    \
    "-v1024:32:1024"

#include <llvm/IR/DataLayout.h>
#ifdef LLVM_OLDER_THAN_3_9
#include <llvm/Target/TargetSelectionDAGInfo.h>
#else
#include <llvm/CodeGen/SelectionDAGTargetInfo.h>
#endif

typedef llvm::DataLayout TargetData;

#include "TCEString.hh"

namespace TTAMachine {
    class Machine;
}

/**
 * TCE target machine plugin interface.
 */
namespace llvm { 
   class TargetInstrInfo;
   class TargetLowering;
   class TargetRegisterInfo;
   class TargetFrameLowering;
   class TargetSubtargetInfo;
   class TCESubtarget;
   class FunctionPass;
   class TCETargetMachine;
   class TargetRegisterClass;
   class MachineInstr;
   class SDNode;

   class TCETargetMachinePlugin {
    public:
       TCETargetMachinePlugin() : lowering_(NULL), tm_(NULL),
                                  dl_(TCEBEDLString) {};
       virtual ~TCETargetMachinePlugin() {};

       virtual const TargetInstrInfo* getInstrInfo() const = 0;
       virtual const TargetRegisterInfo* getRegisterInfo() const = 0;
       virtual const TargetFrameLowering* getFrameLowering() const = 0;
       virtual TargetLowering* getTargetLowering() const = 0;
       virtual const TargetSubtargetInfo* getSubtarget() const = 0;

       virtual FunctionPass* createISelPass(TCETargetMachine* tm) = 0;

       /// Returns name of the physical register file corresponding
       /// to a generated register ID.
       virtual std::string rfName(unsigned dwarfRegNum) = 0;

       /// Returns name of the physical register index corresponding
       /// to a generated register ID.
       virtual unsigned registerIndex(unsigned dwarfRegNum) = 0;

       /// Returns operation name corresponding to llvm target opcode.
       virtual std::string operationName(unsigned opc) const = 0;
       /// Returns true in case the target supports the given osal operation

       virtual bool hasOperation(TCEString operationName) const = 0;
       /// Returns the opcode for the given osal operation, undefined if not found.
       virtual unsigned opcode(TCEString operationName) const = 0;
       virtual unsigned int extractElementLane(const MachineInstr& mi) const = 0;

       virtual int getTruePredicateOpcode(unsigned opc) const = 0;
       virtual int getFalsePredicateOpcode(unsigned opc) const = 0;

       /// Returns pointer to xml string of the target machine .adf
       virtual const std::string* adfXML() = 0;
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

       virtual unsigned getMaxMemoryAlignment() const = 0;

       /// Clustered-TTA-subtemplate related methods.
       virtual const llvm::TargetRegisterClass* extrasRegClass(
           const llvm::TargetRegisterClass* current) const = 0;
       virtual const llvm::TargetRegisterClass* nodeRegClass(
           unsigned nodeId, const llvm::TargetRegisterClass* current) const = 0;

       virtual bool isLittleEndian() const = 0;

       virtual const DataLayout* getDataLayout() const {
           return &dl_;
       }

       virtual DataLayout* getDataLayout() {
           return &dl_;
       }

       virtual TCETargetMachine *getCurrentTargetMachine() {
           return tm_;
       }
#ifdef LLVM_OLDER_THAN_3_9
       virtual const TargetSelectionDAGInfo* getSelectionDAGInfo() const {
#else
       virtual const SelectionDAGTargetInfo* getSelectionDAGInfo() const {
#endif
           return &tsInfo_;
       }


   protected:
       /// Target machine instruction info for the llvm framework. 
       TargetInstrInfo* instrInfo_;
       TargetLowering* lowering_;
       TargetFrameLowering* frameInfo_;
       TCETargetMachine* tm_;
       TCESubtarget* subTarget_;
       DataLayout dl_; // Calculates type size & alignment
#ifdef LLVM_OLDER_THAN_3_9
       TargetSelectionDAGInfo tsInfo_;
#else
       SelectionDAGTargetInfo tsInfo_;
#endif
   };

}
#endif
