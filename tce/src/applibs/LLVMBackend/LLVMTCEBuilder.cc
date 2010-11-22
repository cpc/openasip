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
 * @file LLVMTCEBuilder.cc
 *
 * Implementation of LLVMTCEBuilder class.
 *
 * @author Veli-Pekka Jääskeläinen 2007-2009 (vjaaskel-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2009 (mikael.lepisto-no.spam-tut.fi)
 * @author Esa Määttä 2009 (esa.maatta-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2007-2010
 * @note reting: red
 */

#include <list>

#include <boost/format.hpp>

#include "LLVMTCEBuilder.hh"
#include "Program.hh"
#include "Machine.hh"
#include "Procedure.hh"
#include "Instruction.hh"
#include "NullInstruction.hh"
#include "DataDefinition.hh"
#include "DataAddressDef.hh"
#include "DataInstructionAddressDef.hh"
#include "InstructionReference.hh"
#include "Move.hh"
#include "MoveGuard.hh"
#include "Guard.hh"
#include "Terminal.hh"
#include "TerminalInstructionAddress.hh"
#include "TerminalRegister.hh"
#include "TerminalImmediate.hh"
#include "TerminalFUPort.hh"
#include "ControlUnit.hh"
#include "SpecialRegisterPort.hh"
#include "UniversalMachine.hh"
#include "UniversalFunctionUnit.hh"
#include "ExecutionPipeline.hh"
#include "POMDisassembler.hh"
#include "DataMemory.hh"
#include "CodeLabel.hh"
#include "DataLabel.hh"
#include "BinaryStream.hh"
#include "LLVMTCEBuilder.hh"
#include "Operand.hh"
#include "CodeGenerator.hh"
#include "ProgramAnnotation.hh"
#include "TCEString.hh"

#include <llvm/Constants.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Module.h>
#include <llvm/CodeGen/MachineInstr.h>
#include <llvm/CodeGen/MachineMemOperand.h>
#include <llvm/CodeGen/MachineConstantPool.h>
#include <llvm/Target/TargetInstrInfo.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetData.h>
#include <llvm/Target/TargetLowering.h>
#include <llvm/Support/Debug.h>
#include <llvm/Target/TargetInstrDesc.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Analysis/DebugInfo.h>
#include <llvm/MC/MCContext.h>

#include "MapTools.hh"
#include "StringTools.hh"
#include "Operation.hh"
#include "OperationPool.hh"

#include "TCETargetMachine.hh"

#include "tce_config.h"

#define END_SYMBOL_NAME "_end"

using namespace TTAMachine;
using namespace llvm;
using TTAProgram::CodeGenerator;

unsigned LLVMTCEBuilder::MAU_BITS = 8;
unsigned LLVMTCEBuilder::POINTER_SIZE = 4; // Pointer size in maus.

char LLVMTCEBuilder::ID = 0;

// #define DEBUG_LLVMPOMBUILDER

LLVMTCEBuilder::LLVMTCEBuilder(
    TargetMachine& tm,
    TTAMachine::Machine* mach,
#ifdef LLVM_2_7
    char&) :
    MachineFunctionPass(this)
#else
    char& ID) :
    MachineFunctionPass(ID)
#endif
{
    initMembers();
    tm_ = &tm;
    mach_ = mach;
}

LLVMTCEBuilder::LLVMTCEBuilder(char& ID) : MachineFunctionPass(ID) {
    initMembers();
}

void
LLVMTCEBuilder::initMembers() {    
    mod_ = NULL; 
    tm_ = NULL; 
    prog_ = NULL;
    mach_ = NULL; 
    umach_ = NULL; 
    mang_ = NULL; 
    dmem_ = NULL; 
    end_ = 0;
    noAliasFound_ = false; 
    multiAddrSpacesFound_ = false;
    spillMoveCount_ = 0;
    dataInitialized_ = false;
}

/**
 * The Destructor.
 */
LLVMTCEBuilder::~LLVMTCEBuilder() {
    if (mang_ != NULL) {
        delete mang_;
        mang_ = NULL;
    }
}

/**
 * Initializes the data sections of the POM.
 *
 * Can be called multiple times. Doesn't do anything after the first
 * successful call.
 */
void
LLVMTCEBuilder::initDataSections() {

    if (dataInitialized_) 
        return;

    dataInitialized_ = true;    

    assert(mach_ != NULL);
    assert(tm_ != NULL);
    assert(mod_ != NULL);
    
    if (prog_ != NULL) {
        delete prog_;
        prog_ = NULL;
    }


    // List of supported operations.
    opset_.insert("jump");
    opset_.insert("call");
    const TTAMachine::Machine::FunctionUnitNavigator fuNav =
        mach_->functionUnitNavigator();

    for (int i = 0;i < fuNav.count(); i++) {
        const FunctionUnit& fu = *fuNav.item(i);
        for (int o = 0; o < fu.operationCount(); o++) {
            opset_.insert(StringTools::stringToLower(fu.operation(o)->name()));
        }
    }

    // Set GCU address space as the instruction address space.
    if (mach_->controlUnit() == NULL) {
        std::cerr << "ERROR: No control unit in the target machine!"
                  << std::endl;

        assert(false);
    }

    instrAddressSpace_ = mach_->controlUnit()->addressSpace();
    if (instrAddressSpace_ == NULL) {
        std::cerr << "ERROR: Address space set for the control unit in the "
                  << "target machine."
                  << std::endl;

        assert(false);
    }

    // FIXME: data address space
    const TTAMachine::Machine::AddressSpaceNavigator nav =
        mach_->addressSpaceNavigator();

    for (int i = 0; i < nav.count(); i++) {
        if (nav.item(i) != instrAddressSpace_) {
            dataAddressSpace_ = nav.item(i);
            break;
        }
    }

    if (dataAddressSpace_ == NULL) {
        std::cerr << "ERROR: Unable to determine data address space."
                  << std::endl;

        assert(false);
    }

    prog_ = new TTAProgram::Program(*instrAddressSpace_);
#ifdef LLVM_2_7
    mang_ = new Mangler(*tm_->getMCAsmInfo()); // Use prefix _ for all value names.
#else
    // this doesn't look right, creating a MCContext just to get the
    // mangler initialized... --Pekka
    MCContext* ctx = new MCContext(*tm_->getMCAsmInfo());
    mang_ = new Mangler(*ctx, *tm_->getTargetData()); 
#endif
    dmem_ = new TTAProgram::DataMemory(*dataAddressSpace_);
    end_ = dmem_->addressSpace().start();
    umach_ = &prog_->universalMachine();

    // Avoid placing data to address 0, it may break some null pointer
    // tests.

    // Waste a valuable word of memory 
    // (word to prevent writing bytes to 1,2,3 
    //  addresses and then reading word from 0)
    if (end_ == 0) {
        end_ += 4;
    }

    const TargetData* td = tm_->getTargetData();
    TTAProgram::GlobalScope& gscope = prog_->globalScope();

    // Global variables.
    for (Module::const_global_iterator i = mod_->global_begin();
         i != mod_->global_end(); i++) {

        TCEString name = mang_->getNameWithPrefix(i);
        
        const llvm::GlobalValue& gv = *i;

        if (gv.hasSection() && gv.getSection() == "llvm.metadata") {
            // do not write debug constants to the data section
            continue; 
        }

        if (name == END_SYMBOL_NAME) {
            // Skip original _end symbol.
            continue;
        }
                       
        if (!i->hasInitializer()) {
            std::cerr << "Initializer missing for: " << name << std::endl;
            assert(false && "No initializer. External linkage?");
        }

        const Constant* initializer = i->getInitializer();
        const Type* type = initializer->getType();

        DataDef def;
        def.name = name;
        def.address = 0;
        def.alignment = td->getPrefTypeAlignment(type);
        def.size = td->getTypeStoreSize(type);
        // memcpy seems to assume global values are aligned by 4
        if (def.size > def.alignment) {
            def.alignment = std::max(def.alignment,4u);
        }

        assert(def.alignment != 0);

        if (isInitialized(initializer)) {
            def.initialize = true;
            data_.push_back(def);
        } else {
            def.initialize = false;
            udata_.push_back(def);
        }
    }

    // Map initialized data to memory.
    for (unsigned i = 0; i < data_.size(); i++) {

        // padding
        unsigned pad = 0;
        while ((end_ + pad) % data_[i].alignment != 0) pad++;
        if (pad > 0) {
            std::vector<MinimumAddressableUnit> zeros(pad, 0);
            TTAProgram::Address address(end_, *dataAddressSpace_);
            dmem_->addDataDefinition(
                new TTAProgram::DataDefinition(address, zeros));

            end_ += pad;
        }

        dataLabels_[data_[i].name] = end_;
        data_[i].address = end_;

        // Add data label.
        TTAProgram::Address addr(end_, *dataAddressSpace_);
        TTAProgram::DataLabel* label =
            new TTAProgram::DataLabel(data_[i].name, addr, gscope);

        gscope.addDataLabel(label);


        end_ += data_[i].size;
    }

    // Map uninitialized data to memory.
    for (unsigned i = 0; i < udata_.size(); i++) {

        // padding
        unsigned pad = 0;
        while ((end_ + pad) % udata_[i].alignment != 0) pad++;
        if (pad > 0) {
            TTAProgram::Address address(end_, *dataAddressSpace_);
            dmem_->addDataDefinition(
                new TTAProgram::DataDefinition(address, pad));

            end_ += pad;
        }

        udata_[i].address = end_;
        dataLabels_[udata_[i].name] = end_;

        // Add data label.
        TTAProgram::Address addr(end_, *dataAddressSpace_);
        TTAProgram::DataLabel* label =
            new TTAProgram::DataLabel(udata_[i].name, addr, gscope);

        gscope.addDataLabel(label);
        end_ += udata_[i].size;
    }
}

/**
 * Initializer creates a new POM and adds all global data initializations.
 *
 * @param m Module to initialize the writer for.
 */
bool
LLVMTCEBuilder::doInitialization(Module& m) {
    mod_ = &m;
    dataInitialized_ = false;
    return false;
}

/**
 * Creates data definition from a Constant initializer and adds it to the
 * machine data memory.
 *
 * @param addr Address where the data is to be defined in the data memory.
 * @param cv Constant initializer for the data.
 */
void
LLVMTCEBuilder::emitDataDef(const DataDef& def) {

    if (!def.initialize) {

        if (def.address % def.alignment != 0) {
            std::cerr << def.name << " misaligned!" << std::endl;
            std::cerr << "    address: " << def.address
                      << "  alignment: " << def.alignment << std::endl;

            assert(false);
        }

        TTAProgram::Address addr(def.address, *dataAddressSpace_);
        dmem_->addDataDefinition(
            new TTAProgram::DataDefinition(addr, def.size));

        return;
    } else {

        const  GlobalVariable* var = NULL;
        for (Module::const_global_iterator i = mod_->global_begin();
             i != mod_->global_end(); i++) {
	    
            if (def.name == mang_->getNameWithPrefix(i)) {
                var = i;
                break;
            }
        }

        unsigned addr = def.address;

        assert(var != NULL && "Variable not found!");

#ifndef NDEBUG
        unsigned paddedAddr =
#endif
        createDataDefinition(addr, var->getInitializer());
        assert(paddedAddr == def.address);
    }
}

/**
 * Creates POM data definition from a llvm data initializer.
 *
 * @param addr Address for the POM data.
 * @param cv Initializer for the data in llvm.
 * @return POM data address after padding data to correct alignment.
 */
unsigned
LLVMTCEBuilder::createDataDefinition(
    unsigned& addr, const Constant* cv) {

    const TargetData* td = tm_->getTargetData();
    unsigned sz = td->getTypeStoreSize(cv->getType());
    unsigned align = td->getABITypeAlignment(cv->getType());

    unsigned pad = 0;
    while ((addr + pad) % align != 0) pad++;

    // Pad with zeros to correct alignment.
    if (pad > 0) {
        std::vector<MinimumAddressableUnit> zeros(pad, 0);
        TTAProgram::Address address(addr, *dataAddressSpace_);
        dmem_->addDataDefinition(
            new TTAProgram::DataDefinition(address, zeros));

        addr += pad;

    }

    // paddedAddr is the actual address data was put to
    // after alignment.
    unsigned paddedAddr = addr;

    // Initialize with zeros if this is an uninitialized part of a partially
    // initialized data structure.
    if (cv->isNullValue() || dyn_cast<UndefValue>(cv) != NULL) {

        std::vector<MinimumAddressableUnit> maus;
        for (unsigned i = 0; i < sz; i++) {
            maus.push_back(0);
        }

        TTAProgram::Address address(addr, *dataAddressSpace_);
        dmem_->addDataDefinition(
            new TTAProgram::DataDefinition(address, maus));

        addr += sz;
        return paddedAddr;
    }


    if ((dyn_cast<ConstantArray>(cv) != NULL) ||
        (dyn_cast<ConstantStruct>(cv) != NULL) ||
        (dyn_cast<ConstantVector>(cv) != NULL)) {

        for (unsigned i = 0, e = cv->getNumOperands(); i != e; ++i) {

#ifndef NDEBUG
            unsigned dataAddr = createDataDefinition(
		addr, cast<Constant>(cv->getOperand(i)));
            // First element of structured data should be in the paddedAddr.
            assert (i > 0 || dataAddr == paddedAddr);
#else
            createDataDefinition(addr, cast<Constant>(cv->getOperand(i)));
#endif
        }
    } else if (const ConstantInt* ci = dyn_cast<ConstantInt>(cv)) {
        createIntDataDefinition(addr, ci);
    } else if (const ConstantFP* cfp = dyn_cast<ConstantFP>(cv)) {
        createFPDataDefinition(addr, cfp);
    } else if (const GlobalValue* gv = dyn_cast<GlobalValue>(cv)) {
        createGlobalValueDataDefinition(addr, gv);
    } else if (const ConstantExpr* ce = dyn_cast<ConstantExpr>(cv)) {
        createExprDataDefinition(addr, ce);
    } else {
	cv->dump();
        abortWithError("Unknown cv type.");
    }

    return paddedAddr;
}


/**
 * Creates data definition of a constant integer value.
 *
 * @param addr Address where the integer is to be defined.
 * @param ci Constant initializer for the integer value.
 */
void
LLVMTCEBuilder::createIntDataDefinition(
    unsigned& addr, const ConstantInt* ci, bool isPointer) {

    assert(addr % (tm_->getTargetData()->getABITypeAlignment(ci->getType()))
           == 0 && "Invalid alignment for constant int!");

    std::vector<MinimumAddressableUnit> maus;

    unsigned sz = (ci->getBitWidth() / MAU_BITS);

    if (isPointer) {
        sz = POINTER_SIZE;
    }

    if (!(sz == 1 || sz == 2 || sz == 4 || sz == 8)) {

//        std::cerr << "## int with size " << sz << "!" << std::endl;
    }

    TTAProgram::Address start(addr, *dataAddressSpace_);

    // FIXME: Assuming 8bit MAU.
    union {
        int64_t d;
        char bytes[8];
    } u;

    u.d = ci->getZExtValue();

    for (unsigned i = 0; i < sz; i++) {
        maus.push_back(u.bytes[sz - i - 1]);
    }

    TTAProgram::DataDefinition* def =
        new TTAProgram::DataDefinition(start, maus);

    addr += def->size();
    dmem_->addDataDefinition(def);
}


/**
 * Creates data definition of a floating point constant.
 */
void
LLVMTCEBuilder::createFPDataDefinition(
    unsigned& addr, const ConstantFP* cfp) {

    assert(addr % (tm_->getTargetData()->getABITypeAlignment(cfp->getType()))
           == 0 && "Invalid alignment for constant fp!");

    TTAProgram::Address start(addr, *dataAddressSpace_);
    std::vector<MinimumAddressableUnit> maus;

    const Type* type = cfp->getType();
    unsigned sz = tm_->getTargetData()->getTypeStoreSize(type);
    TTAProgram::DataDefinition* def = NULL;

    if (type->getTypeID() == Type::DoubleTyID) {

        double val = cfp->getValueAPF().convertToDouble();
        assert(sz == 8);
        union {
            double d;
            char bytes[8];
        } u;

        u.d = val;
        for (unsigned i = 0; i < sz; i++) {
            maus.push_back(u.bytes[sz - i - 1]);
        }
        def = new TTAProgram::DataDefinition(start, maus);
    } else if (type->getTypeID() == Type::FloatTyID) {

        float val = cfp->getValueAPF().convertToFloat();
        assert(sz == 4);
        union {
            float f;
            char bytes[4];
        } u;

        u.f = val;
        for (unsigned i = 0; i < sz; i++) {
            maus.push_back(u.bytes[sz - i - 1]);
        }
        def = new TTAProgram::DataDefinition(start, maus);
    } else {
        assert(false && "Unknown floating point typeID!");
    }
    addr += def->size();
    dmem_->addDataDefinition(def);
}


/**
 * Creates data definition for a global value reference.
 *
 * @param addr Address where the reference is to be defined.
 * @param gv Global value reference.
 */
void
LLVMTCEBuilder::createGlobalValueDataDefinition(
    unsigned& addr, const GlobalValue* gv, int offset) {

    assert(addr % POINTER_SIZE == 0 &&
           "Invalid alignment for gv reference!");

    const Type* type = gv->getType();

    unsigned sz = tm_->getTargetData()->getTypeStoreSize(type);

    assert(sz == POINTER_SIZE && "Unexpected pointer size!");
    std::string label = mang_->getNameWithPrefix(gv);

    TTAProgram::Address start(addr, *dataAddressSpace_);

    TTAProgram::DataDefinition* def = NULL;
    if (codeLabels_.find(label) != codeLabels_.end()) {
        assert(
            offset == 0 &&
            "Instruction reference with an offset not supported yet.");

        TTAProgram::Instruction* instr = codeLabels_[label];
        TTAProgram::InstructionReference ref =
            prog_->instructionReferenceManager().createReference(*instr);

        def = new TTAProgram::DataInstructionAddressDef(start, sz, ref);
    } else if (dataLabels_.find(label) != dataLabels_.end()) {
        TTAProgram::Address ref(
            (dataLabels_[label] + offset), *dataAddressSpace_);

        def = new TTAProgram::DataAddressDef(start, sz, ref);
    } else {
        assert(false && "Global value label not found!");
    }
    addr += def->size();
    dmem_->addDataDefinition(def);
}

/**
 * Creates POM data definition from a llvm constant expression initializer.
 *
 * @param addr Address of the POM data definition.
 * @param ce Expression to create the data definition for.
 * @param offset Offset for an address defined by the expression.
 */
void
LLVMTCEBuilder::createExprDataDefinition(
    unsigned& addr, const ConstantExpr* ce, int offset) {

    assert(addr % (tm_->getTargetData()->getABITypeAlignment(ce->getType()))
           == 0 && "Invalid alignment for constant expr!");

    const TargetData* td = tm_->getTargetData();

    unsigned opcode = ce->getOpcode();
    if (opcode == Instruction::GetElementPtr) {
        const Constant* ptr = ce->getOperand(0);
        SmallVector<Value*, 8> idxVec(ce->op_begin() + 1, ce->op_end());

        int64_t ptrOffset = offset + td->getIndexedOffset(
            ptr->getType(), &idxVec[0], idxVec.size());

        if (const GlobalValue* gv = dyn_cast<GlobalValue>(ptr)) {
            createGlobalValueDataDefinition(addr, gv, ptrOffset);
        } else if (const ConstantExpr* ce =
                   dyn_cast<ConstantExpr>(ptr)) {
            createExprDataDefinition(addr, ce, ptrOffset);
        } else {
            assert(false && "Unsuported getElementPtr target!");
        }
    } else if (opcode == Instruction::BitCast) {
        const Constant* ptr = ce->getOperand(0);
        if (const ConstantExpr* ce  = dyn_cast<ConstantExpr>(ptr)) {
            createExprDataDefinition(addr, ce, offset);
        } else if (const GlobalValue* gv = dyn_cast<GlobalValue>(ptr)) {
            createGlobalValueDataDefinition(addr, gv, offset);
        } else {
            assert(offset == 0);
#ifndef NDEBUG
            unsigned dataAddr = createDataDefinition(addr, ptr);
            // Data should have been padded already:
            assert(dataAddr == addr);
#else
            createDataDefinition(addr, ptr);
#endif
        }
    } else if (opcode == Instruction::IntToPtr) {
        assert(offset == 0);
        const ConstantInt* ci = dyn_cast<ConstantInt>(ce->getOperand(0));
        assert(ci != NULL);
        createIntDataDefinition(addr, ci, true);
    } else if (opcode == Instruction::PtrToInt) {
        assert(offset == 0);
        // Data should have been padded already:
#ifndef NDEBUG
        unsigned dataAddr = 
#endif
        createDataDefinition(addr, ce->getOperand(0));
        assert(dataAddr == addr);
    } else if (opcode == Instruction::Add) {
        assert(false && "NOT IMPLEMENTED");
    } else if (opcode == Instruction::Sub) {
        assert(false && "NOT IMPLEMENTED");
    } else {
        assert(false && "NOT IMPLEMENTED");
    }
}


/**
 * Creates POM procedure of a MachineFunction object and adds it to the
 * current program.
 *
 * @param mf MachineFunction to process.
 * @return Always false.
 */
bool
LLVMTCEBuilder::runOnMachineFunction(MachineFunction& mf) {
    return writeMachineFunction(mf);
}

/**
 * Writes machine function to POM.
 *
 * Actually does things to MachineFunction which was supposed to be done
 * in runOnMachineFunction, but which cannot be done during that, because
 * MachineDCE is not ready yet at that time...
 */
bool
LLVMTCEBuilder::writeMachineFunction(MachineFunction& mf) {

    // the new TTA backend does not initialize TCETargetMachine
    // in construction (at least not yet)
    if (tm_ == NULL)
        tm_ = &mf.getTarget();
    // ensure data sections have been initialized
    initDataSections();

    // omit empty functions..
    if (mf.begin() == mf.end()) return true;

    // TODO: make list of mf's which for the pass will be ran afterwards..

    std::string fnName = mang_->getNameWithPrefix(mf.getFunction());

    emitConstantPool(*mf.getConstantPool());

    TTAProgram::Procedure* proc =
        new TTAProgram::Procedure(fnName, *instrAddressSpace_);

    prog_->addProcedure(proc);

    std::set<std::string> emptyMBBs;

    bool firstInsOfProc = true;
    spillMoveCount_ = 0;
    // iterate basic blocks from MachineFunction
    for (MachineFunction::const_iterator i = mf.begin();
         i != mf.end(); i++) {

        bool newMBB = true;

        // iterate MachineInstr from basic blocks
        for (MachineBasicBlock::const_iterator j = i->begin();
             j != i->end(); j++) {

            TTAProgram::Instruction* instr = NULL;
#ifdef DEBUG_LLVMPOMBUILDER
            std::cerr << "### converting: ";
            j->dump();
            std::cerr << std::endl;
#endif
            instr = emitInstruction(j, proc);

            // Pseudo instructions:
            if (instr == NULL) continue;

            // If there was any empty basic blocks before this instruction,
            // set the basic blocks to point the next available (this)
            // instruction.
            while (!emptyMBBs.empty()) {
                mbbs_[*emptyMBBs.begin()] = instr;
                emptyMBBs.erase(emptyMBBs.begin());
            }

            std::string mbb = mbbName(*i);

            // Keep book of first instructions in basic blocks.
            if (newMBB) {
                newMBB = false;
                assert(!MapTools::containsKey(mbbs_, mbb));
                mbbs_[mbb] = instr;
                bbIndex_[(*i).getBasicBlock()] = instr;
            }

            // Keep book of first instructions in functions.            
            if (firstInsOfProc) {
                TTAProgram::InstructionReference ref = prog_->
                    instructionReferenceManager().createReference(*instr);
                prog_->globalScope().addCodeLabel(
                    new TTAProgram::CodeLabel(ref, fnName));

                codeLabels_[fnName] = instr;
                firstInsOfProc = false;
            }
        }

        // If the basic block didn't hold any instructions
        // (i.e. it probably contained only pseudo instructions),
        // add it to the set of empty BBs that will be set to point
        // the next instruction in the program.
        if (newMBB) {
            emptyMBBs.insert(mbbName(*i));
        }
    }

    // if the procedure would otherwise be empty, add a dummy instruction there,
    // and make the procedure cdelabel to point it.
    if (firstInsOfProc) {
        TTAProgram::Instruction* dummyIns = new TTAProgram::Instruction;
        proc->add(dummyIns);

        TTAProgram::InstructionReference ref = prog_->
            instructionReferenceManager().createReference(*dummyIns);
        prog_->globalScope().addCodeLabel(
            new TTAProgram::CodeLabel(ref, fnName));
        
        codeLabels_[fnName] = dummyIns;
    }
#if 0
    if (Application::verboseLevel() > 0) {
        Application::logStream() 
            << "spill moves in " << mf.getFunction()->getNameStr() << ": " 
            << spillMoveCount_ << std::endl;
    }
#endif
    return false;
}

/**
 * Finalizes the POM building.
 *
 * Creates data initializers.
 * Fixes dummy code references to point the actual instructions.
 */
bool
LLVMTCEBuilder::doFinalization(Module& /* m */) {
    
    // errs() << "Finalize LLVMPOM builder\n";

    // get machine dce analysis
    MachineDCE& MDCE = getAnalysis<MachineDCE>();

    for (MachineDCE::UnusedFunctionsList::iterator i = 
             MDCE.removeableFunctions.begin();
         i != MDCE.removeableFunctions.end(); i++) {        
        std::string name = *i;
        // errs() << "Deleting unused function from pom: " 
        //        << name << "\n";
        TTAProgram::Procedure& notUsedProc = prog_->procedure(name);
        prog_->removeProcedure(notUsedProc);
    }
             
    // Create data initializers.
    for (unsigned i = 0; i < data_.size(); i++) {
        emitDataDef(data_[i]);
    }
    for (unsigned i = 0; i < udata_.size(); i++) {
        emitDataDef(udata_[i]);
    }

    // Create new _end symbol at the end of the data memory definitions.
    DataDef def;
    def.name = END_SYMBOL_NAME;
    def.address = end_;
    def.alignment = 1;
    def.size = 1;
    def.initialize = false;
    emitDataDef(def);

    TTAProgram::Address endAddr(end_, *dataAddressSpace_);
    TTAProgram::DataLabel* label = new TTAProgram::DataLabel(
        END_SYMBOL_NAME, endAddr, prog_->globalScope());

    prog_->globalScope().addDataLabel(label);

    // Fix references to _end symbol.
    unsigned i = 0;
    for (; i < endReferences_.size(); i++) {
        SimValue endLoc(32);
        endLoc = end_;
        TTAProgram::TerminalAddress* ea =
            new TTAProgram::TerminalAddress(endLoc, *dataAddressSpace_);

        endReferences_[i]->setSource(ea);
    }

    prog_->addDataMemory(dmem_);

    // Fix references to basic blocks.
    std::map<TTAProgram::TerminalInstructionAddress*,
        std::string>::iterator mbbRefIter =
        mbbReferences_.begin();

    for (; mbbRefIter != mbbReferences_.end(); mbbRefIter++) {
        TTAProgram::TerminalInstructionAddress* term = mbbRefIter->first;

        std::string mbb = mbbRefIter->second;
        if (mbbs_.find(mbb) == mbbs_.end()) {
            assert(false && "MBB not found from book keeping.");
        }
        TTAProgram::Instruction& instr = *mbbs_[mbb];
        TTAProgram::InstructionReference newRef =
            prog_->instructionReferenceManager().createReference(instr);
        term->setInstructionReference(newRef);
    }

    // Fix references to code labels.
    std::map<TTAProgram::TerminalInstructionAddress*,
        std::string>::iterator codeRefIter =
        codeLabelReferences_.begin();

    for (; codeRefIter != codeLabelReferences_.end(); codeRefIter++) {
        TTAProgram::TerminalInstructionAddress* term = codeRefIter->first;
        std::string label = codeRefIter->second;
        if (codeLabels_.find(label) == codeLabels_.end()) {
            std::cerr << (boost::format(
                              "Function '%s' not defined.\n") %
                          label).str();
            exit(EXIT_FAILURE);
        }

        TTAProgram::Instruction& instr = *codeLabels_[label];
        TTAProgram::InstructionReference newRef =
            prog_->instructionReferenceManager().createReference(instr);

        term->setInstructionReference(newRef);
    }

    // Add stackpointer initialization.
    emitSPInitialization();

    return false;
}

/**
 * Creates POM instructions for a llvm instruction.
 *
 * @param mi Machine instruction to emit to the POM.
 * @param proc POM procedure to append the instruction to.
 * @return First of the POM instructions emitted.
 */
TTAProgram::Instruction*
LLVMTCEBuilder::emitInstruction(
    const MachineInstr* mi, TTAProgram::Procedure* proc) {

    const llvm::TargetInstrDesc* opDesc = &mi->getDesc();

    if (opDesc->isReturn()) {
        return emitReturn(mi, proc);
    }

    unsigned opc = mi->getDesc().getOpcode();

    // when the -g option turn on, this will come up opc with this, therefore
    // add this to ignore however, it is uncertain whether the debug "-g" will
    // generate more opc, need to verify
    if (opc == TargetOpcode::DBG_VALUE) {
        return NULL;
    }	

    std::string opName = operationName(*mi);

    // Pseudo instructions don't require any actual instructions.
    if (opName == "PSEUDO") {
        return NULL;
    }

    if (opName == "MOVE") {
        return emitMove(mi, proc);
    }

    bool hasGuard = false;
    bool trueGuard = true;

    if (opName[0] == '?') {
        hasGuard = true;
        opName = opName.substr(1);
    }

    if (opName[0] == '!') {
        hasGuard = true;
        trueGuard = false;
        opName = opName.substr(1);
    }

    if (opName == "INLINEASM") {
        return emitInlineAsm(mi, proc);
    }

    if (opName == "SELECT") {
        return emitSelect(mi, proc);
    }

    Bus& bus = umach_->universalBus();
    const TTAMachine::FunctionUnit* fu = NULL;

    if (opDesc->isCall() || opDesc->isBranch()) {
        // Control flow operations.
        fu = umach_->controlUnit();
    } else {
        // Other operations.
        fu = &umach_->universalFunctionUnit();
    }

    if (!fu->hasOperation(opName)) {
        std::cerr << "ERROR: Operation '" << opName << "' not found!"
                  << std::endl;

        assert(false && "Operation not found.");
    }

    // Check that the target machine supports this instruction.
    if (opset_.find(StringTools::stringToLower(opName)) == opset_.end()) {
        std::cerr << "ERROR: Operation '" << opName
                  << "' is required by the program but not found "
                  << "in the machine." << std::endl;

        assert(false);
    }

    OperationPool pool;
    const Operation& operation = pool.operation(opName.c_str());
    HWOperation* op = fu->operation(opName);

    std::vector<TTAProgram::Instruction*> operandMoves;
    std::vector<TTAProgram::Instruction*> resultMoves;

    int inputOperand = 0;
    int outputOperand = operation.numberOfInputs();
    TTAProgram::MoveGuard* guard = NULL;
    for (unsigned o = 0; o < mi->getNumOperands(); o++) {

        const MachineOperand& mo = mi->getOperand(o);

        // Guarded operations have the guarded element as the first
        // operand.
        if (o == 0 && hasGuard) {
            // Create move from the condition operand register to bool register
            // which is used by the guard.
            TTAProgram::Terminal *t = createTerminal(mo);
            // inv guards not yet supported
            guard = createGuard(t, trueGuard);
            delete t;
            assert(guard != NULL);
            continue;
        }

        TTAProgram::Terminal* src = NULL;
        TTAProgram::Terminal* dst = NULL;
        if (!mo.isReg() || mo.isUse()) {
            ++inputOperand;
            if (inputOperand > operation.numberOfInputs())
                continue;

            assert(operation.operand(inputOperand).isInput() &&
                   "Operand mismatch.");

            // currently the input operand of the base+offset mem operations
            // are not marked as addresses as alias analysis does not work
            // in that case correctly, thus we have to treat those operations
            // as special cases for the time being
            if (operation.operand(inputOperand).isAddress() ||
                (isBaseOffsetMemOperation(operation) && inputOperand == 1)) {
                // MachineInstructions have two operands for each Operation
                // address operand: base and offset immediate, split it to
                // two in case of an add+ld/st.
                const MachineOperand& base = mo;
                src = createTerminal(base);
                dst = new TTAProgram::TerminalFUPort(*op, inputOperand);

                TTAProgram::Move* move = createMove(src, dst, bus, guard);
                TTAProgram::Instruction* instr = new TTAProgram::Instruction();
                instr->addMove(move);
            
                operandMoves.push_back(instr);

                debugDataToAnnotations(mi, move);
                addPointerAnnotations(mi, move);
                o += 1;                  
                const MachineOperand& offset = mi->getOperand(o);
                if (isBaseOffsetMemOperation(operation)) {
                    ++inputOperand;
                    // the offset is always the 2nd operand for the standard
                    // base+offset ops
                    assert(inputOperand == 2);

                    // create the offset operand move
                    src = createTerminal(offset);                
                    dst = new TTAProgram::TerminalFUPort(*op, inputOperand);
                    TTAProgram::MoveGuard* guardCopy = NULL;
                    if (guard != NULL)
                        guardCopy = guard->copy();
                    
                    move = createMove(src, dst, bus, guardCopy);
                    instr = new TTAProgram::Instruction();
                    instr->addMove(move);           
                    operandMoves.push_back(instr);
                    debugDataToAnnotations(mi, move);
                } else {
                    assert(offset.getImm() == 0);
                }
            } else {
                src = createTerminal(mo);
                dst = new TTAProgram::TerminalFUPort(*op, inputOperand);
                TTAProgram::Move* move = createMove(src, dst, bus, guard);
                TTAProgram::Instruction* instr = new TTAProgram::Instruction();
                instr->addMove(move);
                operandMoves.push_back(instr);
                debugDataToAnnotations(mi, move);
            }
        } else {
            ++outputOperand;
            if (operation.operand(outputOperand).isNull())
                continue;

            assert(operation.operand(outputOperand).isOutput() &&
                   !operation.operand(outputOperand).isAddress() &&
                   "Operand mismatch.");

            src = new TTAProgram::TerminalFUPort(*op, outputOperand);
            dst = createTerminal(mo);

            TTAProgram::Move* move = createMove(src, dst, bus, guard);
            TTAProgram::Instruction* instr = new TTAProgram::Instruction();
            instr->addMove(move);
            resultMoves.push_back(instr);
            debugDataToAnnotations(mi, move);
        }
    }

    // Return the first instruction of the whole operation.
    TTAProgram::Instruction* first = NULL;
    if (!operandMoves.empty()) {
        first = operandMoves[0];
    } else if (!resultMoves.empty()) {
        first = resultMoves[0];
    } else {
        assert(false && "No moves?");
    }

    for (unsigned i = 0; i < operandMoves.size(); i++) {
        proc->add(operandMoves[i]);
    }
    for (unsigned i = 0; i < resultMoves.size(); i++) {
        proc->add(resultMoves[i]);
    }
    return first;
}

/**
 * Returns true in case the operation is one of the standard base+offset
 * operations with operand 1 the base address and operand 2 the offset.
 */
bool
LLVMTCEBuilder::isBaseOffsetMemOperation(const Operation& operation) const {
    std::string name = operation.name().upper();

    return name == "ALDW" || name == "ALDHU" || 
        name == "ALDH" || name == "ALDQU" ||
        name == "ALDQ" || name == "ASTW" ||
        name == "ASTH" || name == "ASTQ";
}

/**
 * Adds annotations to a pointer register Move that assist the
 * TCE-side alias analysis.
 */
void
LLVMTCEBuilder::addPointerAnnotations(
    const llvm::MachineInstr* mi, TTAProgram::Move* move) {
     
    // copy some pointer data to Move annotations

    for (MachineInstr::mmo_iterator i = mi->memoperands_begin();
         i != mi->memoperands_end(); i++) {
        
        // annotate only RF -> {ld,st}.1 moves
        // @todo make this more foolproof
        if (!(move->source().isGPR() && 
              move->destination().operationIndex() == 1))
            continue;
            
        const llvm::Value* memOpValue = (*i)->getValue();
        if (memOpValue != NULL) {
            std::string pointerName = "";
            // can we get the name right away or have to through
            // GetElemntPtrInst
                
            if (memOpValue->hasName()) {
                pointerName = memOpValue->getNameStr();
            } else if (isa<GetElementPtrInst>(memOpValue)) {
                memOpValue =
                    cast<GetElementPtrInst>(
                        memOpValue)->getPointerOperand();
                if (memOpValue->hasName()) {
                    pointerName = memOpValue->getNameStr();
                }
            } 

            /// TODO: this is not very optimal, it gets the offset
            /// info only for the memory accesses to function argument
            /// pointers?
            if (pointerName.length() > 0 && 
                isa<Argument>(memOpValue)) {
                unsigned offset;
                offset = (*i)->getOffset();
                TTAProgram::ProgramAnnotation progAnnotation(
                    TTAProgram::ProgramAnnotation::ANN_POINTER_OFFSET, 
                    offset);
                move->addAnnotation(progAnnotation); 
            }

            // try to find the origin for the pointer which can be
            // a function argument with 'noalias' attribute set
            const llvm::Value* originMemOpValue = memOpValue;
            while (originMemOpValue != NULL) {
                TCEString currentPointerName = originMemOpValue->getNameStr();

                // the OpenCL work item pointer variables start with __wi_,
                // annotate the move with the work item offset so we can
                // use it in AA
                if (currentPointerName.startsWith("__wi_") &&
                    currentPointerName.size() > 16) {
                    int x, y, z;
                    sscanf(
                        currentPointerName.c_str(), 
                        "__wi_%03d_%03d_%03d", &x, &y, &z);

                    int id = (z & 0x0FF) | ((y & 0x0FF) << 8) | ((x & 0x0FF) << 16);
                    TTAProgram::ProgramAnnotation progAnnotation(
                        TTAProgram::ProgramAnnotation::
                        ANN_OPENCL_WORK_ITEM_ID, id);
                    move->addAnnotation(progAnnotation);                     
                }
                    
                if (isa<Argument>(originMemOpValue) && 
                    cast<Argument>(originMemOpValue)->hasNoAliasAttr()) {
                    TTAProgram::ProgramAnnotation progAnnotation(
                        TTAProgram::ProgramAnnotation::
                        ANN_POINTER_NOALIAS, 1);
                    move->addAnnotation(progAnnotation); 
                    noAliasFound_ = true;


                    /// TODO: this is not correct, especially
                    /// when the above offset info is set!

                    /*
                      As the restrict keyword is assigned only to the pointer
                      we found, we now pretend we are accessing through
                      that pointer even though we might not be as the
                      new pointer might be created through pointer 
                      arithmetic. In case
                      we are not, the offset in the pointer arithmetic
                      should be associated with the real pointer, not
                      the origin pointer with the restrict keyword!

                      Correct way is to add another annotation from
                      which restrict pointer the current pointer is
                      derived from and separate annotation for the 
                      real pointer name + offset.
                    */

                    pointerName = originMemOpValue->getNameStr();
                    break;
                } else if (isa<GetElementPtrInst>(originMemOpValue)) {
                    originMemOpValue =
                        cast<GetElementPtrInst>(originMemOpValue)->
                        getPointerOperand();
                } else if (isa<BitCastInst>(originMemOpValue)) {
                    originMemOpValue =                            
                        cast<BitCastInst>(originMemOpValue)->
                        getOperand(0);
                } else {
                    break;
                }
            }

            if (pointerName != "") {
                TTAProgram::ProgramAnnotation pointerAnn(
                    TTAProgram::ProgramAnnotation::ANN_POINTER_NAME, 
                    pointerName);
                move->addAnnotation(pointerAnn); 
            }

            int addrSpaceId =
                cast<PointerType>(memOpValue->getType())->
                getAddressSpace();
            if (addrSpaceId != 0) {
                std::string addressSpace =
                    (boost::format("%d") % addrSpaceId).str();
                TTAProgram::ProgramAnnotation progAnnotation(
                    TTAProgram::ProgramAnnotation::ANN_POINTER_ADDR_SPACE, 
                    addressSpace);
                move->addAnnotation(progAnnotation); 
                multiAddrSpacesFound_ = true;
            }                
        }
    }
}

/**
 * Helper method that converts LLVM debug data markers to Move annotations.
 */
void
LLVMTCEBuilder::debugDataToAnnotations(
    const llvm::MachineInstr* mi, TTAProgram::Move* move) {

    DebugLoc dl = mi->getDebugLoc();

    // annotate the moves generated from known spill instructions
#ifdef LLVM_2_7
    if (dl.getIndex() == 0xFFFFFFF0) {
#else
    if (dl.getLine() == 0xFFFFFFF0) {
#endif
        TTAProgram::ProgramAnnotation progAnnotation(
            TTAProgram::ProgramAnnotation::ANN_STACKUSE_SPILL);
        move->setAnnotation(progAnnotation); 
        ++spillMoveCount_;
    } else {
        // annotate the moves generated from known ra save.
#ifdef LLVM_2_7
        if (dl.getIndex() == 0xFFFFFFF1) {
#else
        if (dl.getLine() == 0xFFFFFFF1) {
#endif
            TTAProgram::ProgramAnnotation progAnnotation(
                TTAProgram::ProgramAnnotation::ANN_STACKUSE_RA_SAVE);
            move->setAnnotation(progAnnotation); 
        } else {

// these interfaces are different in llvm 2.7.
// not going to backport this to llvm 2.7 so disabled with that
#ifndef LLVM_2_7
            // handle file+line number debug info
            if (!dl.isUnknown()) {
		
                int sourceLineNumber = -1;
                TCEString sourceFileName = "";
                
                // inspired from lib/codegen/MachineInstr.cpp
                const LLVMContext &Ctx = 
	           mi->getParent()->getParent()->getFunction()->getContext();
                DIScope scope(dl.getScope(Ctx));
                sourceLineNumber = dl.getLine();
                sourceFileName = static_cast<TCEString>(scope.getFilename());

                TTAProgram::ProgramAnnotation progAnnotation(
                    TTAProgram::ProgramAnnotation::ANN_DEBUG_SOURCE_CODE_LINE, 
                    sourceLineNumber);
                move->addAnnotation(progAnnotation); 
                       
                if (sourceFileName != "") {
                    TTAProgram::ProgramAnnotation progAnnotation(
                    TTAProgram::ProgramAnnotation::ANN_DEBUG_SOURCE_CODE_PATH, 
                        sourceFileName);
	            move->addAnnotation(progAnnotation); 
                }
            }
#endif
        }
    }
}

/**
 * Creates a POM source terminal from an LLVM machine operand.
 *
 * @param mo LLVM machine operand.
 * @return POM terminal.
 */
TTAProgram::Terminal*
LLVMTCEBuilder::createTerminal(const MachineOperand& mo) {

    if (mo.isReg()) {
        unsigned dRegNum = mo.getReg();

        // is it the RA register?
        if (dRegNum == raPortDRegNum()) {
            return new TTAProgram::TerminalFUPort(
                *umach_->controlUnit()->returnAddressPort());
        }

        // an FU port register?
        TTAProgram::Terminal* term = createFUTerminal(mo);
        if (term != NULL)
            return term;

        // a general purpose register?
        std::string rfName = registerFileName(dRegNum);
        int idx = registerIndex(dRegNum);
        if (!mach_->registerFileNavigator().hasItem(rfName)) {
            std::cerr << "regfile: " << rfName << " not found!" << "\n";
        }

        assert(mach_->registerFileNavigator().hasItem(rfName));
        const RegisterFile* rf = 
            mach_->registerFileNavigator().item(rfName);
        assert(idx >= 0 && idx < rf->size());
        const RFPort* port = NULL;
        for (int i = 0; i < rf->portCount(); i++) {
            if (rf->port(i)->isOutput()) {
                port = rf->port(i);
                break;
            }
        }
        assert(port != NULL);
        return new TTAProgram::TerminalRegister(*port, idx);

    } else if (mo.isFPImm()) {
        assert(false && "FP immediates not implemented.");
    } else if (mo.isImm()) {
        int width = 32; // FIXME
        SimValue val(mo.getImm(), width);
        return new TTAProgram::TerminalImmediate(val);
    } else if (mo.isMBB()) {

        TTAProgram::InstructionReference dummy(NULL);

        TTAProgram::TerminalInstructionAddress* ref =
            new TTAProgram::TerminalInstructionAddress(dummy);


        mbbReferences_[ref] = mbbName(*mo.getMBB());
        return ref;
    } else if (mo.isFI()) {
        std::cerr << " Frame index source operand NOT IMPLEMENTED!"
                  << std::endl;
        assert(false);
    } else if (mo.isCPI()) {
        int width = 32; // FIXME
        unsigned idx = mo.getIndex();
        assert(currentFnCP_.find(idx) != currentFnCP_.end() &&
               "CPE not found!");

        unsigned addr = currentFnCP_[idx];
        SimValue cpeAddr(addr, width);

        return new TTAProgram::TerminalImmediate(cpeAddr);
    } else if (mo.isGlobal()) {
        std::string name = mang_->getNameWithPrefix(mo.getGlobal());
        if (name == "_end") {
            return NULL;
        } else if (dataLabels_.find(name) != dataLabels_.end()) {
            SimValue address(dataLabels_[name] + mo.getOffset(), 32);
            return new TTAProgram::TerminalAddress(
                address, *dataAddressSpace_);

        } else {
            TTAProgram::InstructionReference dummy(NULL);

            TTAProgram::TerminalInstructionAddress* ref =
                new TTAProgram::TerminalInstructionAddress(dummy);

            codeLabelReferences_[ref] = name;
            return ref;
        }
    } else if (mo.isJTI()) {
        std::cerr << " Jump table index operand NOT IMPLEMENTED!\n";
        assert(false);
    } else if (mo.isSymbol()) {
        //} else if (mo.isExternalSymbol()) {

        /**
         * NOTE: Hack to get code compiling even if llvm falsely makes libcalls to
         *       external functions even if they are found from currently lowered program.
         *
         *       http://llvm.org/bugs/show_bug.cgi?id=2673
         *
         *       Should be removed after fix is applied to llvm.. (maybe never...)
         */

        std::string name = mo.getSymbolName();

        TTAProgram::InstructionReference* dummy =
            new TTAProgram::InstructionReference(NULL);


        TTAProgram::TerminalInstructionAddress* ref =
            new TTAProgram::TerminalInstructionAddress(
                *dummy);

        codeLabelReferences_[ref] = name;
        return ref;
        /**
         * END OF HACK
         */
    } else {
        std::cerr << "Unknown src operand type!" << std::endl;
        assert(false);
    }
    abortWithError("Should not get here!");
    return NULL;
}


/**
 * Emits data definitions for a function constant pool.
 *
 * The constant pool is appended to the end of the data memory.
 *
 * @param mcp Constant pool to emit.
 */
void
LLVMTCEBuilder::emitConstantPool(const MachineConstantPool& mcp) {

    currentFnCP_.clear();

    const std::vector<MachineConstantPoolEntry>& cp = mcp.getConstants();

    for (unsigned i = 0, e = cp.size(); i != e; ++i) {
        MachineConstantPoolEntry cpe = cp[i];

        assert(!(cp[i].isMachineConstantPoolEntry()) && "NOT SUPPORTED");
        currentFnCP_[i] = createDataDefinition(end_, cp[i].Val.ConstVal);
    }
}


/**
 * Creates POM instruction for a move.
 *
 * @param src The source operand.
 * @param dst The dst operand.
 * @return POM Move.
 */
TTAProgram::Move*
LLVMTCEBuilder::createMove(
    const MachineOperand& src, const MachineOperand& dst) {
    assert(!src.isReg() || src.isUse());
    assert(dst.isDef());
    
    // eliminate register-to-itself moves
    if (dst.isReg() && src.isReg() && dst.getReg() == src.getReg()) {
        return NULL;
    }

    Bus& bus = umach_->universalBus();
    TTAProgram::Move* move = createMove(
        createTerminal(src), createTerminal(dst), bus);

    return move;
}

/**
 * Creates POM instruction for a move.
 *
 * @param mi Move machine instruction.
 * @param proc POM procedure to add the move to.
 * @return Emitted POM instruction.
 */
TTAProgram::Instruction*
LLVMTCEBuilder::emitMove(
    const MachineInstr* mi, TTAProgram::Procedure* proc) {

    assert(mi->getNumOperands() == 2); // src, dst
    const MachineOperand& dst = mi->getOperand(0);
    const MachineOperand& src = mi->getOperand(1);
    TTAProgram::Move* move = createMove(src, dst);

    TTAProgram::Instruction* instr = new TTAProgram::Instruction();

    instr->addMove(move);
    proc->add(instr);
    return instr;
}


/**
 * Creates POM instructions for a return.
 *
 * @param mi Return machine instruction.
 * @param proc POM procedure to add the return to.
 * @return First of the emitted POM instructions.
 */
TTAProgram::Instruction*
LLVMTCEBuilder::emitReturn(
    const MachineInstr* /* mi */, TTAProgram::Procedure* proc) {

    Bus& bus = umach_->universalBus();
    TTAProgram::TerminalFUPort* src = new TTAProgram::TerminalFUPort(
        *umach_->controlUnit()->returnAddressPort());

    TTAMachine::HWOperation& jump =
        *umach_->controlUnit()->operation("jump");

    TTAProgram::TerminalFUPort* dst = new TTAProgram::TerminalFUPort(jump, 1);
    TTAProgram::Move* move = createMove(src, dst, bus);
    TTAProgram::Instruction* instr = new TTAProgram::Instruction();
    instr->addMove(move);
    proc->add(instr);
    return instr;
}

/**
 * Creates POM instructions for a select.
 *
 * @param mi select machine instruction.
 * @param proc POM procedure to add the select to.
 * @return First of the emitted POM instructions.
 */
TTAProgram::Instruction*
LLVMTCEBuilder::emitSelect(
    const MachineInstr* mi, TTAProgram::Procedure* proc) {

// 0 = dest?

    const MachineOperand& guardMo = mi->getOperand(1);

    // Create move from the condition operand register to bool register
    // which is used by the guard.
    TTAProgram::Terminal *guardTerminal = createTerminal(guardMo);

    TTAProgram::Terminal* dstT = createTerminal(mi->getOperand(0));
    TTAProgram::Terminal* dstF = createTerminal(mi->getOperand(0));
    TTAProgram::Terminal* srcT = createTerminal(mi->getOperand(2));
    TTAProgram::Terminal* srcF = createTerminal(mi->getOperand(3));

    Bus& bus = umach_->universalBus();
    TTAProgram::Instruction *firstIns = NULL;

    // do no create X -> X moves.
    if (dstT->equals(*srcT)) {
        delete srcT;
        delete dstT;
    } else {
        TTAProgram::MoveGuard* trueGuard = createGuard(guardTerminal, true);
        assert(trueGuard != NULL);
        TTAProgram::Move* trueMove = createMove(srcT, dstT, bus, trueGuard);
        TTAProgram::Instruction *trueIns = new TTAProgram::Instruction;
        trueIns->addMove(trueMove);
        proc->add(trueIns);
        firstIns = trueIns;
    }

    // do no create X -> X moves.
    if (dstF->equals(*srcF)) {
        delete srcF;
        delete dstF;
    } else {
        TTAProgram::MoveGuard* falseGuard = createGuard(guardTerminal, false);
        assert(falseGuard != NULL);
        TTAProgram::Move* falseMove = createMove(srcF, dstF, bus, falseGuard);
        TTAProgram::Instruction *falseIns = new TTAProgram::Instruction;
        falseIns->addMove(falseMove);
        proc->add(falseIns);
        if (firstIns == NULL) {
            firstIns = falseIns;
        }
    }

    // guardTerminal was just temporary used as helper when creating guards.
    delete guardTerminal;

    assert(firstIns != NULL);
    return firstIns;
}

/**
 * Returns string identifier for a basic block.
 *
 * These identifiers are used for the BB -> POM instruction book keeping.
 *
 * @param mbb Basic block object.
 * @return String identifier for the basic block.
 */
std::string
LLVMTCEBuilder::mbbName(const MachineBasicBlock& mbb) {
    std::string name = mang_->getNameWithPrefix(mbb.getParent()->getFunction());
    name += " ";
    name += Conversion::toString(mbb.getNumber());
    return name;
}

/**
 * Returns true if the Constant value structure has initialized data.
 *
 * @param cv Initializer to check.
 * @return True, if the initializer has any non-Null data.
 */
bool
LLVMTCEBuilder::isInitialized(const Constant* cv) {

    if ((dyn_cast<ConstantArray>(cv) != NULL) ||
        (dyn_cast<ConstantStruct>(cv) != NULL) ||
        (dyn_cast<ConstantVector>(cv) != NULL)) {

        for (unsigned i = 0, e = cv->getNumOperands(); i != e; ++i) {
            if (isInitialized(cast<Constant>(cv->getOperand(i)))) {
                return true;
            }
        }
        return false;
    }

    if (cv->isNullValue()) {
        return false;
    }

    return true;
}

/**
 * Emit stack pointer initialization move to the begining of the program.
 */
void
LLVMTCEBuilder::emitSPInitialization() {

    unsigned spDRN = spDRegNum();
    std::string rfName = registerFileName(spDRN);
    int idx = registerIndex(spDRN);
    assert(mach_->registerFileNavigator().hasItem(rfName));
    const RegisterFile* rf = mach_->registerFileNavigator().item(rfName);
    assert(idx >= 0 && idx < rf->size());
    const RFPort* port = NULL;
    for (int i = 0; i < rf->portCount(); i++) {
        if (rf->port(i)->isOutput()) {
            port = rf->port(i);
            break;
        }
    }
    assert(port != NULL);
    TTAProgram::TerminalRegister* dst = new
        TTAProgram::TerminalRegister(*port, idx);

    unsigned ival = (dataAddressSpace_->end() & 0xfffffff8);
    SimValue val(ival, 32);
    TTAProgram::TerminalImmediate* src =
        new TTAProgram::TerminalImmediate(val);

    Bus& bus = umach_->universalBus();
    TTAProgram::Move* move = createMove(src, dst, bus);
    TTAProgram::Instruction* spInit = new TTAProgram::Instruction();
    spInit->addMove(move);
    TTAProgram::Instruction& first = prog_->firstInstruction();
    TTAProgram::Procedure& proc =
        dynamic_cast<TTAProgram::Procedure&>(first.parent());

    proc.insertBefore(first, spInit);
    prog_->instructionReferenceManager().replace(first, *spInit);
}


/**
 * Handles INLINEASM nodes.
 *
 * Currently the inline assembly string is expected to be just a name
 * of a (custom) operation. Operation operands are expected to be listed
 * as the inline assembly use and def registers. Architecture specific
 * pseudo assembly constructs are also supported (they start with dot) and
 * are delegated to emitSpecialInlineAsm().
 */
TTAProgram::Instruction*
LLVMTCEBuilder::emitInlineAsm(
    const MachineInstr* mi, TTAProgram::Procedure* proc) {

#ifndef NDEBUG
    unsigned numOperands = 
#endif
    mi->getNumOperands();
    // Count the number of register definitions.
    unsigned numDefs = 0;
    for (; mi->getOperand(numDefs).isReg() &&
             mi->getOperand(numDefs).isDef();
         ++numDefs) {

    }
    std::string opName =  mi->getOperand(numDefs).getSymbolName();
    
    // ignore the dummy placeholder asm string
    if (opName == "") {
        return NULL;
    }

    if (opName[0] == '.') {
        // Special handling for dotted architecture-dependant asm contructs.
        return emitSpecialInlineAsm(opName, mi, proc);
    }

    assert(numDefs != numOperands-1 && "No asm string?");
    assert(mi->getOperand(numDefs).isSymbol() && "No asm string?");


    if (StringTools::containsChar(opName, ' ') ||
        StringTools::containsChar(opName, ';') ||
        StringTools::containsChar(opName, '>') ||
        StringTools::containsChar(opName, '<')) {

        std::cerr << "ERROR: Inline assembly not supported!" << std::endl;
        assert(false);
    }

    const UniversalFunctionUnit& fu = umach_->universalFunctionUnit();
    if (!fu.hasOperation(opName)) {
        std::cerr << "ERROR: Custom operation '" << opName << "' not found."
                  << std::endl;        
        assert(false);
    }

    HWOperation* op = fu.operation(opName);

    Bus& bus = umach_->universalBus();
    std::vector<TTAProgram::Instruction*> operandMoves;
    std::vector<TTAProgram::Instruction*> resultMoves;
    ExecutionPipeline::OperandSet useOps = op->pipeline()->readOperands();
    ExecutionPipeline::OperandSet defOps = op->pipeline()->writtenOperands();

    // go through the operands (0 is the chain, 1 is the asm string)
#ifdef LLVM_2_7
    for (unsigned o = 2; o < mi->getNumOperands(); o++) {
#else
    for (unsigned o = 3; o < mi->getNumOperands(); o++) {
#endif
        const MachineOperand& mo = mi->getOperand(o);
        if (!(mo.isReg() || mo.isImm() || mo.isGlobal()))   {
            // All operands should be in registers. Everything else is ignored.
            std::cerr << "Ignoring an operand of " << opName << std::endl;
            continue;
        }

        TTAProgram::Terminal* src = NULL;
        TTAProgram::Terminal* dst = NULL;
        if (mo.isImm() || mo.isGlobal() || mo.isUse()) {
            if (useOps.empty()) {
                std::cerr << std::endl;
                std::cerr <<"ERROR: Too many input operands for custom "
                          << "operation '" << opName << "'." << std::endl;

                assert(false);
            }
            src = createTerminal(mo);
            dst = new TTAProgram::TerminalFUPort(*op, (*useOps.begin()));
            useOps.erase(useOps.begin());
        } else {
            if (defOps.empty()) {
                std::cerr << std::endl;
                std::cerr << "ERROR: Too many output operands for custom "
                          << "operation '" << opName << "'." << std::endl;

                assert(false);
            }
            src = new TTAProgram::TerminalFUPort(*op, (*defOps.begin()));
            dst = createTerminal(mo);
            defOps.erase(defOps.begin());

        }

        TTAProgram::Move* move = createMove(src, dst, bus);
        TTAProgram::Instruction* instr = new TTAProgram::Instruction();
        instr->addMove(move);

        if (mo.isImm() || mo.isGlobal() || mo.isUse()) {
            operandMoves.push_back(instr);
        } else {
            resultMoves.push_back(instr);
        }

        // this is Operand #2n+2, let's skip the #2n+3 because it's
        // the TargetConstant indicating if the reg is a use/def
        ++o;
    }

    if (!defOps.empty() || !useOps.empty()) {
        std::cerr << "ERROR: All operands not defined for custom operation '"
                  << opName << "'." << std::endl;

        std::cerr << "Undefined: " << defOps.size() << " output operands, "
                  << useOps.size() << " input operands." << std::endl;

        mi->dump();
        abortWithError("Cannot continue");
    }

    // Return the first instruction of the whole operation.
    TTAProgram::Instruction* first = NULL;
    if (!operandMoves.empty()) {
        first = operandMoves[0];
    } else if (!resultMoves.empty()) {
        first = resultMoves[0];
    } else {
        assert(false && "No moves?");
    }

    for (unsigned i = 0; i < operandMoves.size(); i++) {
        proc->add(operandMoves[i]);
    }
    for (unsigned i = 0; i < resultMoves.size(); i++) {
        proc->add(resultMoves[i]);
    }
    return first;
}

/**
 * Constructs moves for architecture-dependant special asm.
 *
 * @param op Assembly instruction string.
 * @param mi Machine instruction including the inline asm.
 * @param proc TTA procedure to emit moves into.
 *
 * @return First instruction in emitted block.
 */
TTAProgram::Instruction*
LLVMTCEBuilder::emitSpecialInlineAsm(
    const std::string op, const MachineInstr* mi, TTAProgram::Procedure* proc) {

    assert(op[0] == '.');

    TCEString subOp(std::string(op, 1, op.length() - 1));

    if (subOp == "setjmp")
        return emitSetjmp(mi, proc);

    if (subOp == "longjmp")
        return emitLongjmp(mi, proc);

    if (subOp == "call_global_ctors")
        return emitGlobalXXtructorCalls(mi, proc, true);

    if (subOp == "call_global_dtors")
        return emitGlobalXXtructorCalls(mi, proc, false);

    if (subOp == "read_sp") 
        return emitReadSP(mi, proc);

    // memory_category pseudo asms can be used to define
    // categories for different pointers. They mark those
    // pointers to alias only with others pointers in the
    // same category.
    if (subOp.startsWith("pointer_category"))
        return handleMemoryCategoryInfo(mi, proc);

    debugLog(subOp);
    abortWithError("Undetected special inline asm.");

    return NULL;
}

/**
 * Emits moves to read the stack pointer value.
 */
TTAProgram::Instruction*
LLVMTCEBuilder::emitReadSP(
    const MachineInstr* mi, TTAProgram::Procedure* proc) {

    if (mi->getNumOperands() != 3) {
        abortWithError(
            "ERROR: wrong number of operands in \".read_sp\"");
    }

    // Get the stack pointer. It will be used as index into
    // the buffer.
    unsigned spDRN = spDRegNum();
    TCEString sp = (boost::format("%s.%d") %
                    registerFileName(spDRN) %
                    registerIndex(spDRN)).str();

    // We need to know where current procedure ends to
    // be able to return first generated instruction.
    TTAProgram::Instruction& lastInstruction =
        proc->lastInstruction();

    CodeGenerator codeGenerator(*mach_, *umach_);

    TTAProgram::Terminal* srcTerminal =
        codeGenerator.createTerminalRegister(sp, false);

    const MachineOperand& dest = mi->getOperand(2);
    // Save SP at the first position in the buffer.
    TTAProgram::Terminal* destTerminal = createTerminal(dest);

    codeGenerator.addMoveToProcedure(*proc, srcTerminal, destTerminal);
    return &(proc->nextInstruction(lastInstruction));
}

/** 
 * Handles the .pointer_category pseudo assembler instruction.
 * 
 * First argument is a string defining the category, second refers to
 * the pointer.
 */
TTAProgram::Instruction*
LLVMTCEBuilder::handleMemoryCategoryInfo(
    const MachineInstr* mi, TTAProgram::Procedure* proc) {

    if (mi->getNumOperands() != 5) {
        Application::logStream() 
            << "got " << mi->getNumOperands() << " operands" << std::endl;
        mi->dump();
        abortWithError(
            "ERROR: wrong number of operands in \".pointer_category\"");
    }

    TTAProgram::Instruction& lastInstruction =
        proc->lastInstruction();

    CodeGenerator codeGenerator(*mach_, *umach_);

    TTAProgram::Terminal* srcTerminal = 
        createTerminal(mi->getOperand(4));

    TTAProgram::Terminal* dstTerminal = 
        createTerminal(mi->getOperand(2));

    codeGenerator.addMoveToProcedure(*proc, srcTerminal, dstTerminal);
    return &(proc->nextInstruction(lastInstruction));
}


/*
 * setjmp/longjmp buffer structure description:
 *
 * buffer -> |-----------------|
 *           |       SP        |
 *           |-----------------|
 *           |       RA        |
 *           |-----------------|
 *           | Return value    |
 *           |-----------------|
 *           |       ...       |
 *           | All RF regs     |
 *           | except SP       |
 *           |       ...       |
 *           |-----------------|
 *           | Return address  |
 *           | (to setjmp tail |
 *           | code)           |
 *           |-----------------| <- buffer +
 *                                  <number regs in all RFs> + 3
 */

/**
 * Constructs moves for ".setjmp"
 *
 * @param mi Machine instruction including the inline asm.
 * @param proc TTA procedure to emit moves into.
 *
 * @return First instruction in emmited block.
 */

TTAProgram::Instruction*
LLVMTCEBuilder::emitSetjmp(
    const MachineInstr* mi, TTAProgram::Procedure* proc) {

    if (mi->getNumOperands() != 5) {
        std::cerr << "ERROR: wrong number of operands in "".setjmp"""
            << std::endl;
        assert(false);
    }

    const MachineOperand& val = mi->getOperand(2);
    const MachineOperand& env = mi->getOperand(4);

    // Get the stack pointer. It will be used as index into
    // the buffer.
    unsigned spDRN = spDRegNum();
    TCEString sp = (boost::format("%s.%d") %
                    registerFileName(spDRN) %
                    registerIndex(spDRN)).str();

    // We need to know where current procedure ends to
    // be able to return first generated instruction.
    TTAProgram::Instruction& last_instruction =
        proc->lastInstruction();

    CodeGenerator codeGenerator(*mach_, *umach_);

    // Save SP at the first position in the buffer.
    TTAProgram::Terminal* buffer = createTerminal(env);
    codeGenerator.storeToAddress(*proc, buffer, sp);

    // Now we can scratch the stack pointer.

    // First thing we need is to store buffer address in SP.
    buffer = createTerminal(env);
    TTAProgram::Terminal* spTerminal =
        codeGenerator.createTerminalRegister(sp, false);

    codeGenerator.addMoveToProcedure(*proc, buffer, spTerminal);

    // Increment index to jump over the place where SP was
    // stored.
    codeGenerator.incrementRegisterAddress(*proc, sp);

    // Save RA first (special register).
    codeGenerator.pushRegisterToBuffer(*proc, sp, "RA");

    // Now save the desired return value.
    SimValue immVal(32);
    immVal = 0;
    TTAProgram::TerminalImmediate *immTerminal =
        new TTAProgram::TerminalImmediate(immVal);
    codeGenerator.pushToBuffer(*proc, sp, immTerminal);

    // Now we can save every register there.
    const TTAMachine::Machine::RegisterFileNavigator nav =
        mach_->registerFileNavigator();

    int buffer_words = 0;

    for (int i = 0; i < nav.count(); i++) {
        const TTAMachine::RegisterFile& rf = *nav.item(i);
        for (int j = 0; j < rf.numberOfRegisters(); j++) {
            TCEString reg =
                (boost::format("%s.%d") % rf.name() % j).str();
            if (reg != sp) { // sp already saved, ignore it.
                codeGenerator.pushRegisterToBuffer(*proc, sp, reg);
                buffer_words++;
            }
        }
    }

    // Save the setjmp return point.
    TTAProgram::Instruction* returnInstruction =
        new TTAProgram::Instruction(
            TTAMachine::NullInstructionTemplate::instance());

    TTAProgram::InstructionReference returnReference =
        prog_->instructionReferenceManager().createReference(
            *returnInstruction);

    codeGenerator.pushInstructionReferenceToBuffer(
        *proc, sp, returnReference);

    codeGenerator.decrementRegisterAddress(*proc, sp);

    proc->add(returnInstruction);

    // Move back the stored registers.
    for (int i = 0; i < buffer_words; i++)
        codeGenerator.decrementRegisterAddress(*proc, sp);

    // Get return value.
    TTAProgram::Terminal* rv = createTerminal(val);
    codeGenerator.popFromBuffer(*proc, sp, rv);

    // Restore original RA.
    codeGenerator.popRegisterFromBuffer(*proc, sp, "RA");

    // Restore SP from first position in the buffer.
    codeGenerator.popRegisterFromBuffer(*proc, sp, sp);

    return &(proc->nextInstruction(last_instruction));
}

/**
 * Constructs moves for calling all global constructors or
 * destructors, if any.
 *
 * @param mi Machine instruction including the inline asm.
 * @param proc TTA procedure to append moves into.
 * @param constructors True, if emitting constructors, otherwise 
 *                     destructors.
 */
TTAProgram::Instruction*
LLVMTCEBuilder::emitGlobalXXtructorCalls(
    const MachineInstr* /*mi*/, TTAProgram::Procedure* proc,
    bool constructors) {

    std::string globalName = 
        constructors ? 
        ("llvm.global_ctors") : ("llvm.global_dtors");
    
    TTAProgram::Instruction* firstInstruction = NULL;

    // find the _llvm.global_Xtors global with the
    // function pointers and priorities

    for (Module::const_global_iterator i = mod_->global_begin();
         i != mod_->global_end(); i++) {

        const GlobalVariable* gv = i;

        if (gv->getName() == globalName && gv->use_empty()) {
            // The initializer should be an array of '{ int, void ()* }' structs.  
            // The first value is the init priority, which we ignore.
            ConstantArray* initList = cast<ConstantArray>(gv->getInitializer());
            for (unsigned i = 0, e = initList->getNumOperands(); i != e; ++i) {
                if (ConstantStruct* cs = 
                    dyn_cast<ConstantStruct>(initList->getOperand(i))) {
                    // Not array of 2-element structs.
                    if (cs->getNumOperands() != 2) 
                        return firstInstruction;  

                     // Found a null terminator, exit printing.
                    if (cs->getOperand(1)->isNullValue())
                        return firstInstruction;  
                    // Emit the call.

                    TTAProgram::InstructionReference* dummy =
                        new TTAProgram::InstructionReference(NULL);

                    TTAProgram::TerminalInstructionAddress* xtorRef =
                        new TTAProgram::TerminalInstructionAddress(
                            *dummy);

		    GlobalValue* gv =  dynamic_cast<GlobalValue*>(
			cs->getOperand(1));
		    assert(gv != NULL&&"global constructor name not constv");
		    std::string name = mang_->getNameWithPrefix(gv);
                    // who deletes xtorRef?
                    codeLabelReferences_[xtorRef] = name; 

                    CodeGenerator codeGenerator(*mach_, *umach_); 
                    codeGenerator.addMoveToProcedure(
                        *proc, xtorRef, 
                        codeGenerator.createTerminalFUPort("call", 1));

                    if (firstInstruction == NULL)
                        firstInstruction = &proc->lastInstruction();
                }
            }
            return firstInstruction;
        }
    }
    return NULL;
}

/**
 * Constructs moves for ".longjmp"
 *
 * @param mi Machine instruction including the inline asm.
 * @param proc TTA procedure to emit moves into.
 *
 * @return First instruction in emitted block.
 */
TTAProgram::Instruction*
LLVMTCEBuilder::emitLongjmp(
    const MachineInstr* mi, TTAProgram::Procedure* proc) {

    if (mi->getNumOperands() != 5) {
        std::cerr << "ERROR: wrong number of operands in "".longjmp"""
            << std::endl;
        assert(false);
    }

    const MachineOperand& env = mi->getOperand(2);
    const MachineOperand& val = mi->getOperand(4);

    // Get the stack pointer. It will be used as index into
    // the buffer.
    unsigned spDRN = spDRegNum();
    TCEString sp = (boost::format("%s.%d") %
                    registerFileName(spDRN) %
                    registerIndex(spDRN)).str();

    // We need to know where current procedure ends to
    // be able to return first generated instruction.
    TTAProgram::Instruction& last_instruction =
        proc->lastInstruction();

    CodeGenerator codeGenerator(*mach_, *umach_);

    // First thing we need is to load buffer address in SP.
    TTAProgram::Terminal* buffer = createTerminal(env);
    TTAProgram::Terminal* spTerminal =
        codeGenerator.createTerminalRegister(sp, false);

    codeGenerator.addMoveToProcedure(*proc, buffer, spTerminal);

    // Increment index to jump over the place where SP was
    // stored.
    codeGenerator.incrementRegisterAddress(*proc, sp);

    // Jump over RA (will be restored in setjmp tail).
    codeGenerator.incrementRegisterAddress(*proc, sp);

    // Now save the desired return value.
    TTAProgram::Terminal* rv = createTerminal(val);
    codeGenerator.pushToBuffer(*proc, sp, rv);

    // Reload all registers but SP.
    const TTAMachine::Machine::RegisterFileNavigator nav =
        mach_->registerFileNavigator();

    for (int i = 0; i < nav.count(); i++) {
        const TTAMachine::RegisterFile& rf = *nav.item(i);
        for (int j = 0; j < rf.numberOfRegisters(); j++) {
            TCEString reg =
                (boost::format("%s.%d") % rf.name() % j).str();
            if (reg != sp) {
                // Using fromStack as I am restoring towards
                // higher memory addresses.
                codeGenerator.popRegisterFromStack(*proc, sp, reg);
            }
        }
    }

    // Done, jump to setjmp ending code.
    codeGenerator.loadFromRegisterAddress(*proc, sp, "RA");
    codeGenerator.registerJump(*proc, "RA");

    return &(proc->nextInstruction(last_instruction));
}

/**
 * Creates program object model move.
 *
 * @param src Source terminal of the move.
 * @param dst Destination terminal of the move.
 * @param bus Bus utilized to do the move.
 * @param guard Guard object for the move or NULL if the move is not
 *              guarded.
 * @return Created move.
 */
TTAProgram::Move*
LLVMTCEBuilder::createMove(
    TTAProgram::Terminal* src,
    TTAProgram::Terminal* dst,
    TTAMachine::Bus& bus,
    TTAProgram::MoveGuard* guard) {

    TTAProgram::Move* move = NULL;

    bool endRef = false;

    if (src == NULL) {
        // Create a dummy source Terminal so the move can be added to an
        // instruction.
        SimValue val(0, 32);
        src = new TTAProgram::TerminalImmediate(val);
        endRef = true;
    }

    if (guard == NULL) {
        move = new TTAProgram::Move(src, dst, bus);
    } else {
        move = new TTAProgram::Move(src, dst, bus, guard);
    }

    if (endRef) {
        endReferences_.push_back(move);
    }

    return move;
}


/**
 * Returns the program built during the pass.
 *
 * @return Result program object built.
 * @throw NotAvailable If program is not ready.
 */
TTAProgram::Program*
LLVMTCEBuilder::result() throw (NotAvailable) {

    return prog_;
}

/**
 * Creates a registergaurd to given guard register.
 */
TTAProgram::MoveGuard* LLVMTCEBuilder::createGuard(
    const TTAProgram::Terminal* terminal, bool trueOrFalse) {
    const TTAProgram::TerminalRegister* guardReg =
        dynamic_cast<const TTAProgram::TerminalRegister*>(terminal);
    if (guardReg == NULL) {
        return NULL;
    }

    Machine::BusNavigator busNav = mach_->busNavigator();
    for (int i = 0; i < busNav.count(); i++) {
        Bus* bus = busNav.item(i);
        for (int i = 0; i < bus->guardCount(); i++) {
            RegisterGuard* regGuard = dynamic_cast<RegisterGuard*>(
                bus->guard(i));
            if (regGuard != NULL &&
                regGuard->registerFile() == &guardReg->registerFile() &&
                regGuard->registerIndex() == (int)guardReg->index() &&
                regGuard->isInverted() != trueOrFalse) {
                return new TTAProgram::MoveGuard(*regGuard);
            }
        }
    }
    return NULL;
}
