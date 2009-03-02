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
 * @file LLVMPOMBuilder.cpp
 *
 * Implementation of LLVMPOMBuilder class.
 *
 * @author Veli-Pekka J��skel�inen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @note reting: red
 */

#include "LLVMPOMBuilder.hh"
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
#include "LLVMPOMBuilder.hh"
#include "NullTerminal.hh"
#include "Operand.hh"

#include "llvm/Constants.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Module.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/Target/TargetInstrInfo.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetLowering.h"
#include "llvm/Support/Debug.h"
#include "llvm/Target/TargetInstrDesc.h"

// this class was renamed in LLVM 2.2
typedef llvm::TargetInstrDesc TargetInstrDescriptor;

#include "MapTools.hh"
#include "StringTools.hh"
#include "Operation.hh"
#include "OperationPool.hh"

#include "tce_config.h"

#define END_SYMBOL_NAME "_end"

using namespace TTAMachine;
using namespace llvm;

unsigned LLVMPOMBuilder::MAU_BITS = 8;
unsigned LLVMPOMBuilder::POINTER_SIZE = 4; // Pointer size in maus.

char LLVMPOMBuilder::ID = 0;

/**
 * The Constructor.
 *
 * @param tm Target machine description.
 */
LLVMPOMBuilder::LLVMPOMBuilder(
    TCETargetMachine& tm, TTAMachine::Machine* mach):
    MachineFunctionPass(this),
    mod_(NULL), tm_(tm), mach_(mach), umach_(NULL), prog_(NULL),
    mang_(NULL), dmem_(NULL), end_(0), programReady_(false) {
}

/**
 * The Destructor.
 */
LLVMPOMBuilder::~LLVMPOMBuilder() {
    if (prog_ != NULL) {
        delete prog_;
        prog_ = NULL;
    }
    if (mang_ != NULL) {
        delete mang_;
        mang_ = NULL;
    }
}


/**
 * Initializer creates a new POM and adds all global data initializations.
 *
 * @param m Module to initialize the writer for.
 */
bool
LLVMPOMBuilder::doInitialization(Module& m) {

    assert(mach_ != NULL);

    if (prog_ != NULL) {
        delete prog_;
        prog_ = NULL;
    }

    mod_ = &m;

    // FIXME: Program owns umach?
    umach_ = new UniversalMachine();

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

    for (int i = 0; i <nav.count(); i++) {
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
    mang_ = new Mangler(m, "_"); // Use prefix _ for all value names.
    dmem_ = new TTAProgram::DataMemory(*dataAddressSpace_);
    end_ = dmem_->addressSpace().start();

    const TargetData* td = tm_.getTargetData();
    TTAProgram::GlobalScope& gscope = prog_->globalScope();

    // Global variables.
    for (Module::const_global_iterator i = m.global_begin();
         i != m.global_end(); i++) {

        std::string name = mang_->getValueName(i);

        if (name == END_SYMBOL_NAME) {
            // Skip original _end symbol.
            continue;
        }

        if (!i->hasInitializer()) {
	    assert(false && "No initializer. External linkage?");
        }

        const Constant* initializer = i->getInitializer();
        const Type* type = initializer->getType();

        DataDef def;
        def.name = name;
        def.address = 0;
        def.alignment = td->getPrefTypeAlignment(type);
        def.size = td->getTypeStoreSize(type);
        
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
LLVMPOMBuilder::emitDataDef(const DataDef& def) {

    if (!def.initialize) {

        if(def.address % def.alignment != 0) {
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

            if (def.name == mang_->getValueName(i)) {
                var = i;
                break;
            }
        }

        assert(var != NULL && "Variable not found!");

        unsigned addr = def.address;
        unsigned paddedAddr =
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
LLVMPOMBuilder::createDataDefinition(
    unsigned& addr, const Constant* cv) {

    const TargetData* td = tm_.getTargetData();
    unsigned sz = td->getTypeStoreSize(cv->getType());
    unsigned align = td->getPrefTypeAlignment(cv->getType());

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
    if (cv->isNullValue()) {

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
            unsigned dataAddr = createDataDefinition(addr, cv->getOperand(i));
            // First element of structured data should be in the paddedAddr.
            assert (i > 0 || dataAddr == paddedAddr);
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
        assert(false && "Unknown cv type.");
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
LLVMPOMBuilder::createIntDataDefinition(
    unsigned& addr, const ConstantInt* ci, bool isPointer) {

    assert(addr % (tm_.getTargetData()->getABITypeAlignment(ci->getType()))
           == 0 && "Invalid alignment for constant int!");

    std::vector<MinimumAddressableUnit> maus;
    
    unsigned sz = (ci->getBitWidth() / MAU_BITS);

    if (isPointer) {
        sz = POINTER_SIZE;
    }

    if (!(sz == 1 || sz == 2 || sz == 4 || sz == 8)) {
        std::cerr << "## int with size " << sz << "!" << std::endl;
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
LLVMPOMBuilder::createFPDataDefinition(
    unsigned& addr, const ConstantFP* cfp) {

    assert(addr % (tm_.getTargetData()->getPrefTypeAlignment(cfp->getType()))
           == 0 && "Invalid alignment for constant fp!");

    TTAProgram::Address start(addr, *dataAddressSpace_);
    std::vector<MinimumAddressableUnit> maus;

    const Type* type = cfp->getType();
    unsigned sz = tm_.getTargetData()->getTypeStoreSize(type);
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
LLVMPOMBuilder::createGlobalValueDataDefinition(
    unsigned& addr, const GlobalValue* gv, int offset) {

    assert(addr % POINTER_SIZE == 0 &&
           "Invalid alignment for gv reference!");

    const Type* type = gv->getType();

    unsigned sz = tm_.getTargetData()->getTypeStoreSize(type);
    
    assert(sz == POINTER_SIZE && "Unexpected pointer size!");
    std::string label = mang_->getValueName(gv);

    TTAProgram::Address start(addr, *dataAddressSpace_);

    TTAProgram::DataDefinition* def = NULL;
    if (codeLabels_.find(label) != codeLabels_.end()) {
        assert(
            offset == 0 &&
            "Instruction reference with an offset not supported yet.");

        TTAProgram::Instruction* instr = codeLabels_[label];
        TTAProgram::InstructionReference& ref =
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
LLVMPOMBuilder::createExprDataDefinition(
    unsigned& addr, const ConstantExpr* ce, int offset) {

    assert(addr % (tm_.getTargetData()->getPrefTypeAlignment(ce->getType()))
           == 0 && "Invalid alignment for constant expr!");

    const TargetData* td = tm_.getTargetData();

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
            unsigned dataAddr = createDataDefinition(addr, ptr);
            // Data should have been padded already:
            assert(dataAddr == addr);
        }
    } else if (opcode == Instruction::IntToPtr) {
        assert(offset == 0);
        const ConstantInt* ci = dyn_cast<ConstantInt>(ce->getOperand(0));
        assert(ci != NULL);
        createIntDataDefinition(addr, ci, true);
    } else if (opcode == Instruction::PtrToInt) {
        assert(offset == 0);
        // Data should have been padded already:
        unsigned dataAddr = createDataDefinition(addr, ce->getOperand(0));
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
LLVMPOMBuilder::runOnMachineFunction(MachineFunction& mf) {

    // omit empty functions..
    if (mf.begin() == mf.end()) return true;


    std::string fnName = mang_->getValueName(mf.getFunction());

    emitConstantPool(*mf.getConstantPool());

    TTAProgram::Procedure* proc =
        new TTAProgram::Procedure(fnName, *instrAddressSpace_);

    prog_->addProcedure(proc);

    std::set<std::string> emptyMBBs;

    for (MachineFunction::const_iterator i = mf.begin();
         i != mf.end(); i++) {

        bool newMBB = true;

        for (MachineBasicBlock::const_iterator j = i->begin();
             j != i->end(); j++) {

            TTAProgram::Instruction* instr = NULL;
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
            }

            // Keep book of first instructions in functions.
            if (j == i->begin() && i == mf.begin()) {

                TTAProgram::InstructionReference& ref = prog_->
                    instructionReferenceManager().createReference(*instr);
                prog_->globalScope().addCodeLabel(
                    new TTAProgram::CodeLabel(ref, fnName));

                codeLabels_[fnName] = instr;
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

    return false;
}

/**
 * Finalizes the POM building.
 *
 * Creates data initializers.
 * Fixes dummy code references to point the actual instructions. 
 */
bool
LLVMPOMBuilder::doFinalization(Module& /* m */) {

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
        TTAProgram::InstructionReference& newRef =
            prog_->instructionReferenceManager().createReference(instr);

        TTAProgram::InstructionReference* dummyRef =
            &term->instructionReference();

        term->setInstructionReference(newRef);

        // Delete old dummy reference as it's not owned by any manager.
        delete dummyRef;
    }

    // Fix references to code labels.
    std::map<TTAProgram::TerminalInstructionAddress*,
        std::string>::iterator codeRefIter =
        codeLabelReferences_.begin();

    for (; codeRefIter != codeLabelReferences_.end(); codeRefIter++) {
        TTAProgram::TerminalInstructionAddress* term = codeRefIter->first;
        std::string label = codeRefIter->second;
        if (codeLabels_.find(label) == codeLabels_.end()) {
            std::cerr << "Error: code label '" << label << "' not defined."
                      << std::endl;

            
            assert(false);
        }

        TTAProgram::Instruction& instr = *codeLabels_[label];
        TTAProgram::InstructionReference& newRef =
            prog_->instructionReferenceManager().createReference(instr);

        TTAProgram::InstructionReference* dummyRef =
            &term->instructionReference();

        term->setInstructionReference(newRef);
        // Delete old dummy reference as it's not owned by any manager.
        delete dummyRef;
    }

    // Add stackpointer initialization.
    emitSPInitialization();

    programReady_ = true;

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
LLVMPOMBuilder::emitInstruction(
    const MachineInstr* mi, TTAProgram::Procedure* proc) {

    const TargetInstrDescriptor* opDesc = &mi->getDesc();

    if (opDesc->isReturn()) {
        return emitReturn(mi, proc);
    }

    unsigned opc = mi->getDesc().getOpcode();
    std::string opName = tm_.operationName(opc);

    // Pseudo instructions don't require any actual instructions.
    if (opName == "PSEUDO") {
        return NULL;
    }

    if (opName == "MOVE") {
        return emitMove(mi, proc);
    }

    bool hasGuard = false;
    if (opName[0] == '?') {
        hasGuard = true;
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

    if ((opDesc->isCall() || opDesc->isBranch())) {
        // Control flow operations.
        fu = umach_->controlUnit();
    } else {
        // Other operations.
        fu = &umach_->universalFunctionUnit();
    }

    if(!fu->hasOperation(opName)) {
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

    ExecutionPipeline::OperandSet useOps = op->pipeline()->readOperands();
    ExecutionPipeline::OperandSet defOps = op->pipeline()->writtenOperands();

    std::vector<TTAProgram::Instruction*> operandMoves;
    std::vector<TTAProgram::Instruction*> resultMoves;

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
            guard = createGuard(t,true);
            delete t;
            assert(guard != NULL);
            continue;
        }

        TTAProgram::Terminal* src = NULL;
        TTAProgram::Terminal* dst = NULL;
        if (!mo.isReg() || mo.isUse()) {
            if (useOps.empty()) {
                DOUT << " WARNING: Skipping input operand "
                     << o << " for " << opName << std::endl;
                continue;
            }
            int opNum = *useOps.begin();
            assert(operation.operand(opNum).isInput() &&
                                "Operand mismatch.");

            dst = new TTAProgram::TerminalFUPort(*op, (*useOps.begin()));
            useOps.erase(useOps.begin());

            if (operation.operand(opNum).isAddress()) {
                // MachineInstructions have two operands for each Operation
                // address operand: base and offset immediate.
                o += 1;
                const MachineOperand& offset = mi->getOperand(o);
                src = createAddrTerminal(mo, offset);
            } else {
                src = createTerminal(mo);
            }

        } else {
            if (defOps.empty()) {
                DOUT << " WARNING: Skipping output operand "
                     << o << " for " << opName << std::endl;
                continue;
            }
            int opNum = *defOps.begin();
            assert(operation.operand(opNum).isOutput() &&
                   !operation.operand(opNum).isAddress() &&
                "Operand mismatch.");

            src = new TTAProgram::TerminalFUPort(*op, (*defOps.begin()));
            dst = createTerminal(mo);
            defOps.erase(defOps.begin());

        }

        TTAProgram::Move* move = createMove(src, dst, bus, guard);
/*
        if (hasGuard) {
            TTAMachine::Guard* nonInvGuard = NULL;
            for (int g = 0; g < bus.guardCount(); g++) {
                if (!bus.guard(g)->isInverted()) {
                    nonInvGuard = bus.guard(g);
                }
            }
            assert(nonInvGuard != NULL);
            TTAProgram::MoveGuard* guard =
                new TTAProgram::MoveGuard(*nonInvGuard);

        if (guard != NULL) {
            move->setGuard(guard);        
        }
*/
        TTAProgram::Instruction* instr = new TTAProgram::Instruction();
        instr->addMove(move);

        if (!mo.isReg() || mo.isUse()) {
            operandMoves.push_back(instr);
        } else {
            resultMoves.push_back(instr);
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
 * Creates a POM source terminal from a llvm machine operand.
 *
 * @param mo LLVM machine operand.
 * @return POM terminal.
 */
TTAProgram::Terminal*
LLVMPOMBuilder::createTerminal(const MachineOperand& mo) {

    if (mo.isReg()) {
        unsigned dRegNum = mo.getReg();

        if (dRegNum == tm_.raPortDRegNum()) {
            return new TTAProgram::TerminalFUPort(
                *umach_->controlUnit()->returnAddressPort());            
        }

        std::string rfName = tm_.rfName(dRegNum);
        int idx = tm_.registerIndex(dRegNum);
        if (!mach_->registerFileNavigator().hasItem(rfName)) {
            std::cerr << "regfile: " << rfName << " not found!" << std::endl;
        }

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
        return new TTAProgram::TerminalRegister(*port, idx);

    } else if (mo.isFPImm()) {
        assert(false && "FP immediates not implemented.");
    } else if (mo.isImm()) {
        int width = 32; // FIXME
        SimValue val(mo.getImm(), width);
        return new TTAProgram::TerminalImmediate(val);
    } else if (mo.isMBB()) {

        TTAProgram::InstructionReference* dummy =
            new TTAProgram::InstructionReference(
                TTAProgram::NullInstruction::instance());

        TTAProgram::TerminalInstructionAddress* ref =
            new TTAProgram::TerminalInstructionAddress(
                *dummy);


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
        std::string name = mang_->getValueName(mo.getGlobal());
        if (name == "_end") {
            return &TTAProgram::NullTerminal::instance();
        } else if (dataLabels_.find(name) != dataLabels_.end()) {
            SimValue address(dataLabels_[name], 32);
            return new TTAProgram::TerminalAddress(
                address, *dataAddressSpace_);

        } else {
            TTAProgram::InstructionReference* dummy =
                new TTAProgram::InstructionReference(
                    TTAProgram::NullInstruction::instance());

            TTAProgram::TerminalInstructionAddress* ref =
                new TTAProgram::TerminalInstructionAddress(
                    *dummy);

            codeLabelReferences_[ref] = name;
            return ref;
        }
    } else if (mo.isJTI()) {
        std::cerr << " Jump table index operand NOT IMPLEMENTED!"
                  << std::endl;
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

        std::string name = mang_->makeNameProper(mo.getSymbolName(), "_");

        std::cerr << "TCE proper name: " << name << std::endl;

        TTAProgram::InstructionReference* dummy =
            new TTAProgram::InstructionReference(
                TTAProgram::NullInstruction::instance());
        
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
    assert(false);
}


/**
 * Emits data definitions for a function constant pool.
 *
 * The constant pool is appended to the end of the data memory.
 *
 * @param mcp Constant pool to emit.
 */
void
LLVMPOMBuilder::emitConstantPool(const MachineConstantPool& mcp) {

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
 * @param mi Move machine instruction.
 * @param proc POM procedure to add the move to.
 * @return Emitted POM instruction.
 */
TTAProgram::Instruction*
LLVMPOMBuilder::emitMove(
    const MachineInstr* mi, TTAProgram::Procedure* proc) {

    assert(mi->getNumOperands() == 2); // src, dst
    const MachineOperand& dst = mi->getOperand(0);
    const MachineOperand& src = mi->getOperand(1);
    assert(!src.isReg() || src.isUse());
    assert(dst.isDef());

    Bus& bus = umach_->universalBus();
    TTAProgram::Move* move = createMove(
        createTerminal(src), createTerminal(dst), bus);

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
LLVMPOMBuilder::emitReturn(
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
LLVMPOMBuilder::emitSelect(
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
    TTAProgram::Instruction *lastIns = NULL;

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
        lastIns = trueIns;
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
        lastIns = falseIns;
    }

    // guardTerminal was just temporary used as helper when creating guards.
    delete guardTerminal;

    assert(lastIns != NULL);
    return lastIns;
}





/**
 * Creates an address terminal.
 *
 * @param base Base address operand.
 * @param offset Offset operand for the base address.
 * @return Address terminal for POM.
 */
TTAProgram::Terminal*
LLVMPOMBuilder::createAddrTerminal(
    const MachineOperand& base, const MachineOperand& offset) {

    assert(offset.isImm());
    assert(offset.getImm() == 0);
    return createTerminal(base);
}


/**
 * Creates an instance of the POMBuilder
 *
 * @param o Output stream where the assembly code is printed.
 * @param tm Target machine description.
 */
FunctionPass*
llvm::createLLVMPOMBuilderPass(
    TCETargetMachine& tm, TTAMachine::Machine* mach) {

    return new LLVMPOMBuilder(tm, mach);
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
LLVMPOMBuilder::mbbName(const MachineBasicBlock& mbb) {
    std::string name = mang_->getValueName(mbb.getParent()->getFunction());
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
LLVMPOMBuilder::isInitialized(const Constant* cv) {

    if ((dyn_cast<ConstantArray>(cv) != NULL) ||
        (dyn_cast<ConstantStruct>(cv) != NULL) ||
        (dyn_cast<ConstantVector>(cv) != NULL)) {

        for (unsigned i = 0, e = cv->getNumOperands(); i != e; ++i) {
            if (isInitialized(cv->getOperand(i))) {
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
LLVMPOMBuilder::emitSPInitialization() {
    
    unsigned spDRN = tm_.spDRegNum();
    std::string rfName = tm_.rfName(spDRN);
    int idx = tm_.registerIndex(spDRN);
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
 * as the inline assembly use and def registers.
 */
TTAProgram::Instruction*
LLVMPOMBuilder::emitInlineAsm(
    const MachineInstr* mi, TTAProgram::Procedure* proc) {

    unsigned numOperands = mi->getNumOperands();
    // Count the number of register definitions.
    unsigned numDefs = 0;
    for (; mi->getOperand(numDefs).isReg() &&
             mi->getOperand(numDefs).isDef();
         ++numDefs) {

    }
    assert(numDefs != numOperands-1 && "No asm string?");
    assert(mi->getOperand(numDefs).isSymbol() && "No asm string?");
    std::string opName =  mi->getOperand(numDefs).getSymbolName();

    if (StringTools::containsChar(opName, ' ') ||
        StringTools::containsChar(opName, ';') ||
        StringTools::containsChar(opName, '>') ||
        StringTools::containsChar(opName, '<')) {

        std::cerr << "ERROR: Inline assembly not supported!" << std::endl;
        assert(false);
    }

    const UniversalFunctionUnit& fu = umach_->universalFunctionUnit();
    if(!fu.hasOperation(opName)) {
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
    for (unsigned o = 2; o < mi->getNumOperands(); o++) {

        const MachineOperand& mo = mi->getOperand(o);
        if (!(mo.isReg() || mo.isImm() || mo.isGlobal()))   {
            // All operands should be in registers. Everything else is ignored.
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
        assert(false);
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
LLVMPOMBuilder::createMove(
    TTAProgram::Terminal* src,
    TTAProgram::Terminal* dst,
    TTAMachine::Bus& bus,
    TTAProgram::MoveGuard* guard) {

    TTAProgram::Move* move = NULL;

    bool endRef = false;

    if (src == &TTAProgram::NullTerminal::instance()) {
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
 * This function must be called only once after the pass is ran.
 * Caller gets the ownership of the Program object returned.
 *
 * @return Result program object built.
 * @throw NotAvailable If program is not ready.
 */
TTAProgram::Program*
LLVMPOMBuilder::result() throw (NotAvailable) {

    if (!programReady_) {
        std::string msg = "No result program ready.";
        throw (NotAvailable(__FILE__, __LINE__, __func__, msg));
    }

    TTAProgram::Program* result = prog_;
    prog_ = NULL;
    programReady_ = false;

    return result;
}



/**
 * Creates a registergaurd to given guard register.
 */
TTAProgram::MoveGuard* LLVMPOMBuilder::createGuard(
    const TTAProgram::Terminal* terminal, bool trueOrFalse) {
    const TTAProgram::TerminalRegister* guardReg = 
        dynamic_cast<const TTAProgram::TerminalRegister*>(terminal);
    if ( guardReg == NULL) {
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
