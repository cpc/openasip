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
 * @file LLVMTCEBuilder.cc
 *
 * Implementation of LLVMTCEBuilder class.
 *
 * @author Veli-Pekka Jääskeläinen 2007-2009 (vjaaskel-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2009 (mikael.lepisto-no.spam-tut.fi)
 * @author Esa Määttä 2009 (esa.maatta-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2007-2011
 * @note reting: red
 */
#ifdef NDEBUG
#undef NDEBUG
#endif

#include <list>

#include <boost/format.hpp>

#include "CompilerWarnings.hh"

IGNORE_COMPILER_WARNING("-Wunused-parameter")

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
#include "TerminalInstructionReference.hh"
#include "TerminalSymbolReference.hh"
#include "TerminalAddress.hh"
#include "TerminalRegister.hh"
#include "TerminalImmediate.hh"
#include "TerminalFUPort.hh"
#include "TerminalProgramOperation.hh"
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
#include "ProgramOperation.hh"
#include "LLVMTCECmdLineOptions.hh"
#include "GlobalScope.hh"
#include "InstructionReferenceManager.hh"
#include "HWOperation.hh"
#include "AssocTools.hh"
#include "Conversion.hh"
#include "InstructionElement.hh"

#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Module.h>
#include <llvm/CodeGen/MachineInstr.h>
#include <llvm/CodeGen/MachineMemOperand.h>
#include <llvm/CodeGen/MachineConstantPool.h>
#ifdef LLVM_OLDER_THAN_6_0
#include <llvm/Target/TargetInstrInfo.h>
#include <llvm/Target/TargetLowering.h>
#else
#include <llvm/CodeGen/TargetInstrInfo.h>
#include <llvm/CodeGen/TargetLowering.h>
#endif
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/Debug.h>
#include <llvm/Support/raw_ostream.h>

#include <llvm/IR/DebugInfo.h>

#include <llvm/MC/MCContext.h>
#include <llvm/MC/MCSymbol.h>

#include "MapTools.hh"
#include "StringTools.hh"
#include "Operation.hh"
#include "OperationPool.hh"
#include "MoveNode.hh"

#include "TCETargetMachine.hh"

#include "tce_config.h"

#include "llvm/IR/DataLayout.h"

#define TYPE_CONST
#include <llvm/MC/MCInstrDesc.h>

#include <llvm/ADT/SmallString.h>

#define END_SYMBOL_NAME "_end"

POP_COMPILER_DIAGS

using namespace TTAMachine;
using namespace llvm;
using TTAProgram::CodeGenerator;

unsigned LLVMTCEBuilder::MAU_BITS = 8;
unsigned LLVMTCEBuilder::POINTER_SIZE = 4; // Pointer size in maus.

char LLVMTCEBuilder::ID = 0;

//#define DEBUG_LLVMTCEBUILDER

LLVMTCEBuilder::LLVMTCEBuilder(
    const TargetMachine& tm,
    TTAMachine::Machine* mach,
    char& ID,
    bool functionAtATime) :
    MachineFunctionPass(ID) {
    initMembers();
    tm_ = &tm;
    mach_ = mach;
    functionAtATime_ = functionAtATime;
#if (defined(LLVM_OLDER_THAN_3_8))
    // The required type size info is the same for both BE and LE targets,
    // no need to ask for the sub target as it requires llvm::Function in
    // LLVM 3.7 for some reason. If the endianness is needed, ask for it
    // separately.
    dl_ = tm_->getDataLayout();
#else
    dl_ = new DataLayout(tm_->createDataLayout());
#endif
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
    mang_ = NULL; 
    noAliasFound_ = false; 
    multiAddrSpacesFound_ = false;
    multiDataMemMachine_ = false;
    spillMoveCount_ = 0;
    dataInitialized_ = false;
    initialStackPointerValue_ = 0;
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

    multiDataMemMachine_ = nav.count() > 2;
    defaultDataAddressSpace_ = NULL;

    for (int i = 0; i < nav.count(); i++) {
        if (nav.item(i) != instrAddressSpace_) {
            if (!multiDataMemMachine_ || nav.item(i)->hasNumericalId(0)) {
                defaultDataAddressSpace_ = nav.item(i);
                break;
            }
        }
    }

    if (defaultDataAddressSpace_ == NULL) {
        std::cerr << "ERROR: Unable to determine the default data address space."
                  << std::endl;
        abort();
    } else {
        if (Application::verboseLevel() > 0 && multiDataMemMachine_) {
            Application::logStream()
                << "using '" << defaultDataAddressSpace_->name() 
                << "' as the default data address space" 
                << std::endl;
        }
    }

    prog_ = new TTAProgram::Program(*instrAddressSpace_);
    mang_ = new Mangler();

    const TCETargetMachine* tm = dynamic_cast<const TCETargetMachine*>(tm_);
    assert(tm != NULL);
    const unsigned stackAlignment = tm->getMaxMemoryAlignment();
        
#if 0
    dmem_ = new TTAProgram::DataMemory(*dataAddressSpace_);
    end_ = dmem_->addressSpace().start();

    // Avoid placing data to address 0, it may break some null pointer
    // tests.

    // Waste a valuable word of memory 
    // (word to prevent writing bytes to 1,2,3 
    //  addresses and then reading word from 0)
    if (end_ == 0) {
        end_ += stackAlignment;
    }
#endif

    TTAProgram::GlobalScope& gscope = prog_->globalScope();

    // Global variables.
    for (Module::const_global_iterator i = mod_->global_begin();
         i != mod_->global_end(); i++) {

        SmallString<256> Buffer;
#ifdef LLVM_OLDER_THAN_3_8
        mang_->getNameWithPrefix(Buffer, i, false);
#else
	mang_->getNameWithPrefix(Buffer, &(*i), false);
#endif
        TCEString name(Buffer.c_str());
        
        const llvm::GlobalValue& gv = *i;
        unsigned int gvAlign = gv.getAlignment();

        if (gv.hasSection() && 
            gv.getSection() == std::string("llvm.metadata")) {
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
        TYPE_CONST Type* type = initializer->getType();

        DataDef def;
        def.name = name;
        def.address = 0;
        def.addressSpaceId = 
            cast<PointerType>(gv.getType())->getAddressSpace();
        def.alignment = std::max(gvAlign, dl_->getPrefTypeAlignment(type));
        def.size = dl_->getTypeStoreSize(type);
        // memcpy seems to assume global values are aligned by 4
        if (def.size > def.alignment) {
            def.alignment = std::max(def.alignment, stackAlignment);
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

        TTAMachine::AddressSpace& aSpace = 
            addressSpaceById(data_[i].addressSpaceId);

        unsigned& dataEndPos = dataEnd(aSpace);
        TTAProgram::DataMemory& dmem = dataMemoryForAddressSpace(aSpace);

        // padding
        unsigned pad = 0;
        while ((dataEndPos + pad) % data_[i].alignment != 0) pad++;
        if (pad > 0) {
            TTAProgram::Address address(dataEndPos, aSpace);
            dmem.addDataDefinition(
                new TTAProgram::DataDefinition(
                    address, pad, mach_->isLittleEndian(), NULL, true));
            dataEndPos += pad;
        }

        dataLabels_[data_[i].name] = dataEndPos;
        data_[i].address = dataEndPos;

        // Add data label.
        TTAProgram::Address addr(dataEndPos, aSpace);
        TTAProgram::DataLabel* label =
            new TTAProgram::DataLabel(data_[i].name, addr, gscope);

        gscope.addDataLabel(label);

        dataEndPos += data_[i].size;
    }

    // Map uninitialized data to memory.
    for (unsigned i = 0; i < udata_.size(); i++) {

        TTAMachine::AddressSpace& aSpace = 
            addressSpaceById(udata_[i].addressSpaceId);

        unsigned& dataEndPos = dataEnd(aSpace);
        TTAProgram::DataMemory& dmem = dataMemoryForAddressSpace(aSpace);

        // padding
        unsigned pad = 0;
        while ((dataEndPos + pad) % udata_[i].alignment != 0) pad++;
        if (pad > 0) {
            TTAProgram::Address address(dataEndPos, aSpace);
            dmem.addDataDefinition(
                new TTAProgram::DataDefinition(
                    address, pad, mach_->isLittleEndian()));

            dataEndPos += pad;
        }

        udata_[i].address = dataEndPos;
        dataLabels_[udata_[i].name] = dataEndPos;

        // Add data label.
        TTAProgram::Address addr(dataEndPos, aSpace);
        TTAProgram::DataLabel* label =
            new TTAProgram::DataLabel(udata_[i].name, addr, gscope);

        gscope.addDataLabel(label);
        dataEndPos += udata_[i].size;
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

    TTAMachine::AddressSpace& aSpace = 
        addressSpaceById(def.addressSpaceId);
    TTAProgram::DataMemory& dmem = dataMemoryForAddressSpace(aSpace);

    if (!def.initialize) {

        if (def.address % def.alignment != 0) {
            std::cerr << def.name << " misaligned!" << std::endl;
            std::cerr << "    address: " << def.address
                      << "  alignment: " << def.alignment << std::endl;

            assert(false);
        }

        TTAProgram::Address addr(def.address, aSpace);
        dmem.addDataDefinition(
            new TTAProgram::DataDefinition(
                addr, def.size, mach_->isLittleEndian()));

        return;
    } else {

        const  GlobalVariable* var = NULL;
        for (Module::const_global_iterator i = mod_->global_begin();
             i != mod_->global_end(); i++) {

            SmallString<256> Buffer;
#ifdef LLVM_OLDER_THAN_3_8
            mang_->getNameWithPrefix(Buffer, i, false);
            if (def.name == Buffer.c_str()) {
                var = i;
                break;
            }
#else
            mang_->getNameWithPrefix(Buffer, &(*i), false);
            if (def.name == Buffer.c_str()) {
                var = &(*i);
                break;
            }
#endif
        }

        unsigned addr = def.address;

        assert(var != NULL && "Variable not found!");

#ifndef NDEBUG
        unsigned paddedAddr =
#endif
        createDataDefinition(def.addressSpaceId, addr, var->getInitializer());
        assert(paddedAddr == def.address);
    }
}

/**
 * Creates POM data definition from a llvm data initializer.
 *
 * @param addr Address for the POM data.
 * @param cv Initializer for the data in llvm.
 * @param forceInitialize In case wanting to use initialization data even 
 *                        for null values.
 * @return POM data address after padding data to correct alignment.
 */
unsigned
LLVMTCEBuilder::createDataDefinition(
    int addressSpaceId, unsigned& addr, const Constant* cv, 
    bool forceInitialize) {

    unsigned sz = dl_->getTypeStoreSize(cv->getType());
    unsigned align = dl_->getABITypeAlignment(cv->getType());

    unsigned pad = 0;
    while ((addr + pad) % align != 0) pad++;

    TTAMachine::AddressSpace& aSpace = 
        addressSpaceById(addressSpaceId);
    TTAProgram::DataMemory& dmem = dataMemoryForAddressSpace(aSpace);

    // Pad with zeros to correct alignment.
    if (pad > 0) {
        std::vector<MinimumAddressableUnit> zeros(pad, 0);
        TTAProgram::Address address(addr, aSpace);
        dmem.addDataDefinition(
            new TTAProgram::DataDefinition(
                address, zeros, mach_->isLittleEndian()));
        addr += pad;
    }

    // paddedAddr is the actual address data was put to
    // after alignment.
    unsigned paddedAddr = addr;

    // Initialize with zeros if this is an uninitialized part of a partially
    // initialized data structure.
    if (!forceInitialize && 
        (cv->isNullValue() || dyn_cast<UndefValue>(cv) != NULL)) {
        TTAProgram::Address address(addr, aSpace);
        dmem.addDataDefinition(
            new TTAProgram::DataDefinition(
                address, sz, mach_->isLittleEndian(), NULL, true));
        addr += sz;
        return paddedAddr;
    }

    if (isa<ConstantArray>(cv) ||
        isa<ConstantStruct>(cv) ||
        isa<ConstantVector>(cv)) {

        for (unsigned i = 0, e = cv->getNumOperands(); i != e; ++i) {
            createDataDefinition(
                addressSpaceId, addr, cast<Constant>(cv->getOperand(i)));
        }
    } else if (const ConstantInt* ci = dyn_cast<ConstantInt>(cv)) {
        createIntDataDefinition(addressSpaceId, addr, ci);
    } else if (const ConstantFP* cfp = dyn_cast<ConstantFP>(cv)) {
        createFPDataDefinition(addressSpaceId, addr, cfp);
    } else if (const GlobalValue* gv = dyn_cast<GlobalValue>(cv)) {
        createGlobalValueDataDefinition(addressSpaceId, addr, gv);
    } else if (const ConstantExpr* ce = dyn_cast<ConstantExpr>(cv)) {
        createExprDataDefinition(addressSpaceId, addr, ce);
    } else if (const ConstantDataArray* cda = dyn_cast<ConstantDataArray>(cv)){
        if (cda->isNullValue()) {
            TTAProgram::Address address(addr, aSpace);
            dmem.addDataDefinition( 
                new TTAProgram::DataDefinition(
                    address, sz, mach_->isLittleEndian(), NULL, false) );
        } else {
            /* If the array has non-zero values, do not split the
               definitions to initialized and uninitialized data sections, but
               initialize them all. Otherwise we might end up having zillions
               of UData and Data sections after each other in the TPEF for the
               initialization data, because the sections have only one start 
               address. It soon exceeds the maximum number of
               sections in case of large initialized arrays with some of the
               values being zeros. */
            bool allZeros = true;
            for (unsigned i = 0, e = cda->getNumElements(); i != e; ++i) {
                llvm::Constant *ace = 
                    cast<Constant>(cda->getElementAsConstant(i));
                if (ace->isNullValue() || isa<UndefValue>(ace))
                    continue;
                allZeros = false;
                break;
            }

            for (unsigned int i = 0; i < cda->getNumElements(); i++) {
                createDataDefinition(                    
                    addressSpaceId, addr, cda->getElementAsConstant(i), 
                    !allZeros);
            }
        }
    } else {
        // LLVM does not include dump() when built in non-debug mode.
        // cv->dump();
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
    int addressSpaceId, unsigned& addr, const ConstantInt* ci, 
    bool isPointer) {

    assert(addr % (dl_->getABITypeAlignment(ci->getType())) == 0 && 
           "Invalid alignment for constant int!");

    std::vector<MinimumAddressableUnit> maus;

    unsigned sz = ((ci->getBitWidth() + MAU_BITS-1) / MAU_BITS);

    if (isPointer) {
        sz = POINTER_SIZE;
    }

    if (!(sz == 1 || sz == 2 || sz == 4 || sz == 8)) {

//        std::cerr << "## int with size " << sz << "!" << std::endl;
    }

    TTAMachine::AddressSpace& aSpace = 
        addressSpaceById(addressSpaceId);
    TTAProgram::DataMemory& dmem = dataMemoryForAddressSpace(aSpace);

    TTAProgram::Address start(addr, aSpace);

    // FIXME: Assuming 8bit MAU.
    union {
        int64_t d;
        char bytes[8];
    } u;

    u.d = ci->getZExtValue();
    
    TTAProgram::DataDefinition* def;

    if (!mach_->isLittleEndian()) {
        for (unsigned i = 0; i < sz; i++) {
            maus.push_back(u.bytes[sz - i - 1]);
        }
    } else {
        for (unsigned i = 0; i < sz; i++) {
            maus.push_back(u.bytes[i]);
        }
    }

    def = new TTAProgram::DataDefinition(start, maus, mach_->isLittleEndian());
    addr += def->size();
    dmem.addDataDefinition(def);
}


/**
 * Creates data definition of a floating point constant.
 */
void
LLVMTCEBuilder::createFPDataDefinition(
    int addressSpaceId, unsigned& addr, const ConstantFP* cfp) {

    assert(addr % (dl_->getABITypeAlignment(cfp->getType())) == 0 
           && "Invalid alignment for constant fp!");

    TTAMachine::AddressSpace& aSpace = 
        addressSpaceById(addressSpaceId);
    TTAProgram::DataMemory& dmem = dataMemoryForAddressSpace(aSpace);

    TTAProgram::Address start(addr, aSpace);
    std::vector<MinimumAddressableUnit> maus;

    TYPE_CONST Type* type = cfp->getType();
    unsigned sz = dl_->getTypeStoreSize(type);

    TTAProgram::DataDefinition* def = NULL;

    if (type->getTypeID() == Type::DoubleTyID) {

        double val = cfp->getValueAPF().convertToDouble();
        assert(sz == 8);
        union {
            double d;
            char bytes[8];
        } u;

        u.d = val;
        if (!mach_->isLittleEndian()) {
            for (unsigned i = 0; i < sz; i++) {
                maus.push_back(u.bytes[sz - i - 1]);
            }
        } else {
            for (unsigned i = 0; i < sz; i++) {
                maus.push_back(u.bytes[i]);
            }
        }
        def = new TTAProgram::DataDefinition(
            start, maus, mach_->isLittleEndian());
    } else if (type->getTypeID() == Type::FloatTyID) {

        float val = cfp->getValueAPF().convertToFloat();
        assert(sz == 4);
        union {
            float f;
            char bytes[4];
        } u;

        u.f = val;
        if (!mach_->isLittleEndian()) {
            for (unsigned i = 0; i < sz; i++) {
                maus.push_back(u.bytes[sz - i - 1]);
            }
        } else {
            for (unsigned i = 0; i < sz; i++) {
                maus.push_back(u.bytes[i]);
            }
        }
        def = new TTAProgram::DataDefinition(
            start, maus, mach_->isLittleEndian());
    } else if (type->getTypeID() == Type::HalfTyID) {

        APFloat apf = cfp->getValueAPF();
        bool inexact;
#if LLVM_OLDER_THAN_4_0
        apf.convert(APFloat::IEEEhalf, APFloat::rmNearestTiesToEven, &inexact);
#else
        apf.convert(APFloat::IEEEhalf(),
            APFloat::rmNearestTiesToEven, &inexact);
#endif
        APInt api = apf.bitcastToAPInt();
        assert(sz == 2);
        union {
            uint64_t i;
            char bytes[4];
        } u;

        u.i = api.getRawData()[0];
        for (unsigned i = sz; i < 4; i++) {
            maus.push_back(0);
        }
        if (!mach_->isLittleEndian()) {
            for (unsigned i = 0; i < sz; i++) {
                maus.push_back(u.bytes[sz - i - 1]);
            }
        } else {
            for (unsigned i = 0; i < sz; i++) {
                maus.push_back(u.bytes[i]);
            }
        }
        def = new TTAProgram::DataDefinition(
            start, maus, mach_->isLittleEndian());
    } else {
        assert(false && "Unknown floating point typeID!");
    }
    addr += def->size();
    dmem.addDataDefinition(def);
}


/**
 * Creates data definition for a global value reference.
 *
 * @param addr Address where the reference is to be defined.
 * @param gv Global value reference.
 */
void
LLVMTCEBuilder::createGlobalValueDataDefinition(
    int addressSpaceId, unsigned& addr, const GlobalValue* gv, int offset) {

    assert(addr % POINTER_SIZE == 0 &&
           "Invalid alignment for gv reference!");

    TYPE_CONST Type* type = gv->getType();

    unsigned sz = dl_->getTypeStoreSize(type);

    assert(sz == POINTER_SIZE && "Unexpected pointer size!");

    SmallString<256> Buffer;
    mang_->getNameWithPrefix(Buffer, gv, false);
    TCEString label(Buffer.c_str());

    TTAMachine::AddressSpace& aSpace = 
        addressSpaceById(addressSpaceId);
    TTAProgram::DataMemory& dmem = dataMemoryForAddressSpace(aSpace);

    TTAProgram::Address start(addr, aSpace);

    TTAProgram::DataDefinition* def = NULL;
    if (codeLabels_.find(label) != codeLabels_.end()) {
        assert(
            offset == 0 &&
            "Instruction reference with an offset not supported yet.");

        TTAProgram::Instruction* instr = codeLabels_[label];
        TTAProgram::InstructionReference ref =
            prog_->instructionReferenceManager().createReference(*instr);

        def = new TTAProgram::DataInstructionAddressDef(
            start, sz, ref, mach_->isLittleEndian());
    } else if (dataLabels_.find(label) != dataLabels_.end()) {
        TTAProgram::Address ref(
            (dataLabels_[label] + offset), aSpace);

        def = new TTAProgram::DataAddressDef(
            start, sz, ref, mach_->isLittleEndian());
    } else {
        assert(false && "Global value label not found!");
    }
    addr += def->size();
    dmem.addDataDefinition(def);
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
    int addressSpaceId, unsigned& addr, const ConstantExpr* ce, int offset) {

    assert(addr % (dl_->getABITypeAlignment(ce->getType())) == 0 && 
           "Invalid alignment for constant expr!");

    unsigned opcode = ce->getOpcode();
    if (opcode == Instruction::GetElementPtr) {
        const Constant* ptr = ce->getOperand(0);
        SmallVector<Value*, 8> idxVec(ce->op_begin() + 1, ce->op_end());

#ifdef LLVM_OLDER_THAN_3_9
        int64_t ptrOffset = offset + dl_->getIndexedOffset(
            ptr->getType(), idxVec);
#else
        APInt offsetAI(dl_->getPointerTypeSizeInBits(ce->getType()), 0);
        bool success = cast<GEPOperator>(ce)->accumulateConstantOffset(
            *dl_, offsetAI);
        assert(success); // Fails if GEP is not all-constant.
        int64_t ptrOffset = offset + offsetAI.getSExtValue();
#endif

        if (const GlobalValue* gv = dyn_cast<GlobalValue>(ptr)) {
            createGlobalValueDataDefinition(
                addressSpaceId, addr, gv, ptrOffset);
        } else if (const ConstantExpr* ce =
                   dyn_cast<ConstantExpr>(ptr)) {
            createExprDataDefinition(
                addressSpaceId, addr, ce, ptrOffset);
        } else {
            assert(false && "Unsuported getElementPtr target!");
        }
    } else if (opcode == Instruction::BitCast) {
        const Constant* ptr = ce->getOperand(0);
        if (const ConstantExpr* ce  = dyn_cast<ConstantExpr>(ptr)) {
            createExprDataDefinition(
                addressSpaceId, addr, ce, offset);
        } else if (const GlobalValue* gv = dyn_cast<GlobalValue>(ptr)) {
            createGlobalValueDataDefinition(
                addressSpaceId, addr, gv, offset);
        } else {
            assert(offset == 0);
#ifndef NDEBUG
            unsigned dataAddr = 
                createDataDefinition(
                    addressSpaceId, addr, ptr);
            // Data should have been padded already:
            assert(dataAddr == addr);
#else
            createDataDefinition(addressSpaceId, addr, ptr);
#endif
        }
    } else if (opcode == Instruction::IntToPtr) {
        assert(offset == 0);
        const ConstantInt* ci = dyn_cast<ConstantInt>(ce->getOperand(0));
        assert(ci != NULL);
        createIntDataDefinition(addressSpaceId, addr, ci, true);
    } else if (opcode == Instruction::PtrToInt) {
        assert(offset == 0);
        // Data should have been padded already:
#ifndef NDEBUG
        unsigned dataAddr = 
#endif
        createDataDefinition(addressSpaceId, addr, ce->getOperand(0));
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
 * Sets the value the stack pointer should be initialized to.
 */
void
LLVMTCEBuilder::setInitialStackPointerValue(unsigned value) {
    initialStackPointerValue_ = value;
}


//        std::min(addressSpaceById(0).end() & 0xfffffff8, value);

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
    
    SmallString<256> Buffer;
#ifdef LLVM_OLDER_THAN_6_0
    mang_->getNameWithPrefix(Buffer, mf.getFunction(), false);
#else
    mang_->getNameWithPrefix(Buffer, &mf.getFunction(), false);
#endif
    TCEString fnName(Buffer.c_str());

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
#ifdef DEBUG_LLVMTCEBUILDER
            std::cerr << "### converting: ";
            std::cerr << std::endl;
#endif
#if LLVM_OLDER_THAN_4_0
            instr = emitInstruction(j, proc);
#else
            instr = emitInstruction(&*j, proc);
#endif

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
    return false;
}

/**
 * Finalizes the POM building.
 *
 * Creates data initializers.
 * Fixes dummy code references to point the actual instructions.
 */
bool
LLVMTCEBuilder::doFinalization(Module& m) {

    // get machine dce analysis
    MachineDCE& MDCE = getAnalysis<MachineDCE>();
    // For some reason this is not otherwise ran _before_ this pass,
    // even though this pass depends on it.
    MDCE.doFinalization(m);

    for (MachineDCE::UnusedFunctionsList::iterator i = 
             MDCE.removeableFunctions.begin();
         i != MDCE.removeableFunctions.end(); i++) {        
        std::string name = *i;
#if 0
        Application::logStream() 
            <<  "Deleting unused function from pom: " 
            << name << std::endl;
#endif

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

    TTAMachine::AddressSpace& aSpace = addressSpaceById(0);  
    unsigned& dataEndPos = dataEnd(aSpace);

    // Create new _end symbol at the end of the data memory definitions of
    // the default address space. This is used by malloc() to determine the
    // beginning of the heap.
    DataDef def;
    def.name = END_SYMBOL_NAME;
    def.address = dataEndPos;
    def.addressSpaceId = 0;
    def.alignment = 1;
    def.size = 1;
    def.initialize = false;
    emitDataDef(def);

    TTAProgram::Address endAddr(dataEndPos, aSpace);
    TTAProgram::DataLabel* label = new TTAProgram::DataLabel(
        END_SYMBOL_NAME, endAddr, prog_->globalScope());

    prog_->globalScope().addDataLabel(label);

    // Fix references to _end symbol.
    unsigned i = 0;
    for (; i < endReferences_.size(); i++) {
        SimValue endLoc(32);
        endLoc = dataEndPos;
        TTAProgram::TerminalAddress* ea =
            new TTAProgram::TerminalAddress(endLoc, aSpace);

        endReferences_[i]->setSource(ea);
    }

    for (DataMemIndex::const_iterator i = dmemIndex_.begin(); 
         i != dmemIndex_.end(); ++i) {
        prog_->addDataMemory((*i).second);
    }
    
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

    /* In case both the program and the machine use multiple
       data address spaces, ensure all memory operations are
       annotated with the candidate LSUs. In this scenario
       no memory operation is freely schedulable to any LSU otherwise
       random memory accesses happen.

       In case the machine contains only a single data address space,
       all accesses (regardless of their id in the code) are mapped to
       it so the annotations are not needed.
    */
    if (multiAddrSpacesFound_ && multiDataMemMachine_) {
        TTAProgram::Program::InstructionVector instrs = 
            prog_->instructionVector();
        for (TTAProgram::Program::InstructionVector::const_iterator i = 
                 instrs.begin(); i != instrs.end(); ++i) {
            if ((*i)->moveCount() == 0) continue;
            TTAProgram::Move& m = (*i)->move(0);
            TTAProgram::Terminal& t = m.destination();
            if (t.isFUPort() && !t.isRA() &&
                t.hintOperation().operand(t.operationIndex()).isAddress() &&
                !m.hasAnnotations(
                    TTAProgram::ProgramAnnotation::ANN_ALLOWED_UNIT_DST)) {
                std::cerr
                    << "The program refers to multiple data address spaces, "
                    << "the machine contains multiple data address spaces and "
                    << "an ambigious memory accessing move '" << m.toString()
                    << "' found." << std::endl;
                abort();
            }
        }
    }
    return false;
}

const TTAMachine::HWOperation&
LLVMTCEBuilder::getHWOperation(std::string opName) {
    const TTAMachine::FunctionUnit* fu = NULL;

    if (UniversalMachine::instance().controlUnit()->hasOperation(opName)) {
        fu = UniversalMachine::instance().controlUnit();
    } else if (UniversalMachine::instance().universalFunctionUnit().
               hasOperation(opName)) {
        fu = &UniversalMachine::instance().universalFunctionUnit();
    } else {
        abortWithError(
            TCEString("ERROR: Operation '") + opName +
            "' not found in the machine.");
    }

    // Check that the target machine supports this instruction.
    if (opset_.find(StringTools::stringToLower(opName)) == opset_.end()) {
        std::cerr << "ERROR: Operation '" << opName
                  << "' is required by the program but not found "
                  << "in the machine." << std::endl;
        abortWithError("Cannot proceed.");
    }
    return *fu->operation(opName);
}

/**
 * Creates POM instructions from a LLVM MachineInstruction.
 *
 * One POM instruction per Move are created.
 *
 * @param mi Machine instruction to emit to the POM.
 * @param proc POM procedure to append the instruction to.
 * @return First of the POM instructions emitted.
 */
TTAProgram::Instruction*
LLVMTCEBuilder::emitInstruction(
    const MachineInstr* mi, TTAProgram::CodeSnippet* proc) {

    bool isSpill = false;
    bool isRaSlot = false;
    bool isFpSlot = false;
    const llvm::MCInstrDesc* opDesc = &mi->getDesc();
    unsigned opc = mi->getDesc().getOpcode();

    // when the -g option turn on, this will come up opc with this, therefore
    // add this to ignore however, it is uncertain whether the debug "-g" will
    // generate more opc, need to verify
    if (opc == TargetOpcode::DBG_VALUE) {
        return NULL;
    }	

    if (opc == TargetOpcode::KILL) {
        return NULL;
    }

    std::string opName = "";

    bool hasGuard = false;
    bool trueGuard = true;
    if (dynamic_cast<const TCETargetMachine*>(&targetMachine()) != NULL) {

        if (opDesc->isReturn()) {
            // in case of TTA targets, the return node needs to be
            // converted to ra -> jump here as it does not map 1:1
            // with the DAG names and we map LLVM DAG names to OSAL
            // operations
            // FIXME: this is the wrong way to do this:
            // should LowerReturn to RA -> JUMP.1 instead and process just
            // like any other operation here.
            return emitReturn(mi, proc);
        }
        opName = operationName(*mi);
        
        // Pseudo instructions don't require any actual instructions.
        if (opName == "PSEUDO" || opName == "DEBUG_LABEL") {
            return NULL;
        }
        
        if (opName[0] == '?') {
            hasGuard = true;
            opName = opName.substr(1);
        }

        if (opName[0] == '!') {
            hasGuard = true;
            trueGuard = false;
            opName = opName.substr(1);
        }

        if (opName == "MOVE") {
            return emitMove(mi, proc, hasGuard, trueGuard);
        }

        // TODO: guarded also for these
        if (opName == "INLINEASM") {
            return emitInlineAsm(mi, proc);
        }

        if (opName == "CMOV_SELECT") {
            return emitSelect(mi, proc);
        }
        
        if (opName.find("_BUILD_") == 0) {
            std::vector<std::string> res;
            StringTools::chopString(opName, "_", res);
            int elemCount = Conversion::toInt(res.at(2));
            return emitVectorBuild(elemCount, mi, proc);
        }

        if (opName.find("_EXTRACT_") == 0) {
            return emitVectorExtract(mi, proc);
        }

        if (opName.find("_INSERT_") == 0) {
            std::vector<std::string> res;
            StringTools::chopString(opName, "_", res);
            int elemCount = Conversion::toInt(res.at(2));
            return emitVectorInsert(elemCount, mi, proc);
        }

        if (opName.find("_VECTOR_MOV") == 0) {
            std::vector<std::string> res;
            StringTools::chopString(opName, "_", res);
            int elemCount = Conversion::toInt(res.at(3));
            return emitVectorMov(elemCount, mi, proc);
        }
    } else {
        opName = operationName(*mi);
    }

    // format is _VECTOR_##__opname
    // where ## is number of elements
    if (opName.find("_VECTOR_") == 0) {
        std::vector<std::string> res;
        StringTools::chopString(opName, "_", res);
        int elemCount = Conversion::toInt(res.at(2));
        return emitVectorInstruction(res.at(3), elemCount, mi, proc);
    }

    const HWOperation& op = getHWOperation(opName);

    Bus& bus = UniversalMachine::instance().universalBus();

    OperationPool pool;
    const Operation& operation = pool.operation(opName.c_str());

    std::vector<TTAProgram::Instruction*> operandMoves;
    std::vector<TTAProgram::Instruction*> resultMoves;

    int inputOperand = 0;
    int outputOperand = operation.numberOfInputs();
#ifdef DEBUG_LLVMTCEBUILDER
    PRINT_VAR(operation.numberOfInputs());
    PRINT_VAR(operation.numberOfOutputs());
    Application::logStream() << " mi->getNumOperands() = " 
    	<< mi->getNumOperands() << std::endl;
#endif
    TTAProgram::MoveGuard* guard = NULL;
    int guardOperandIndex = -1;

    if (hasGuard) {
        for (unsigned o = 0; o < mi->getNumOperands(); o++) {
            const MachineOperand& mo = mi->getOperand(o);
	
            // Guarded operations have the guarded element as the first
            // operand.

            if (mo.isReg() && mo.isUse()) {
                guardOperandIndex = o;
                // Create move from the condition operand register to bool register
                // which is used by the guard.
                TTAProgram::Terminal *t = createTerminal(mo);
                // inv guards not yet supported
                guard = createGuard(t, trueGuard);
                delete t;
                assert(guard != NULL);
                break;
            }
        }
    }


    for (unsigned o = 0; o < mi->getNumOperands(); o++) {
        if ((int)o == guardOperandIndex) {
            continue;
        }

        const MachineOperand& mo = mi->getOperand(o);
        TTAProgram::Terminal* src = NULL;
        TTAProgram::Terminal* dst = NULL;

        // if vector operand, expand one MachineOperand into
        // multiple TCE operands.
        int vectorWidth = vectorOperandSize(mo);
        if (vectorWidth > 1) {
            // TODO: does not work with bigger than 8 size vectors
            int nameIndex = strlen("_VECTOR_") + 2;
            for (int i = 0; i < vectorWidth; i++) {
                int dRegNum = mo.getReg();
                int idx = registerIndex(dRegNum);
                std::string vectorRfName = registerFileName(dRegNum);
                int nextNameIndex = vectorRfName.find('+', nameIndex);
                int len = nextNameIndex - nameIndex;
                std::string rfName = vectorRfName.substr(nameIndex, len);
                nameIndex = nextNameIndex + 1;
                
                TTAProgram::TerminalRegister* tr = 
                    createTerminalRegister(rfName, idx);
		
                // input
                if (mo.isUse() || operation.numberOfOutputs() == 0) {
                    ++inputOperand;
                    // something messed up?
                    if (inputOperand > operation.numberOfInputs()) {
                        if (mo.isMetadata()) {
                            const MDNode* mdNode = mo.getMetadata();
                            for (unsigned int i = 0; i < mdNode->getNumOperands(); i++) {
                                const MDOperand & oper = mdNode->getOperand(i);
                                if (llvm::MDString* mds = dyn_cast<llvm::MDString>(oper)) {
                                    TCEString s = mds->getString().str();
                                    if (s == "AA_CATEGORY_STACK_SLOT") {
                                        isSpill = true;
                                    } else if (s == "AA_CATEGORY_RA_SAVE_SLOT") {
                                        isRaSlot = true;
                                    } else if (s == "AA_CATEGORY_FP_SAVE_SLOT") {
                                        isFpSlot = true;
                                    }
                                }
                            }
                        }
                        continue;
                    }
		    
                    TTAProgram::Terminal* dst = 
                        new TTAProgram::TerminalFUPort(op, inputOperand);
                    auto move = createMove(tr, dst, bus);
                    TTAProgram::Instruction* ins = 
                        new TTAProgram::Instruction();
                    ins->addMove(move);
                    operandMoves.push_back(ins);
                    debugDataToAnnotations(mi, *move);
                } else {
                    // output
                    ++outputOperand;
                    
                    if (operation.operand(outputOperand).isNull())
                        continue;
		    
                    assert(operation.operand(outputOperand).isOutput() &&
                           !operation.operand(outputOperand).isAddress() &&
                           "Operand mismatch.");
		    
                    TTAProgram::Terminal* src = 
                        new TTAProgram::TerminalFUPort(op, outputOperand);
		    
                    auto move = createMove(src, tr, bus);
                    TTAProgram::Instruction* ins = 
                        new TTAProgram::Instruction();
                    ins->addMove(move);
                    resultMoves.push_back(ins);
                    debugDataToAnnotations(mi, *move);
                }
            }
            continue;
        } 

        if (!mo.isReg() || mo.isUse() || operation.numberOfOutputs() == 0) {
            ++inputOperand;
            if (inputOperand > operation.numberOfInputs()) {

                if (mo.isMetadata()) {
                    const MDNode* mdNode = mo.getMetadata();
                    for (unsigned int i = 0; i < mdNode->getNumOperands(); i++) {
                        const MDOperand & oper = mdNode->getOperand(i);
                        if (llvm::MDString* mds = dyn_cast<llvm::MDString>(oper)) {
                            TCEString s = mds->getString().str();
                            if (s == "AA_CATEGORY_STACK_SLOT") {
                                isSpill = true;
                            } else if (s == "AA_CATEGORY_RA_SAVE_SLOT") {
                                isRaSlot = true;
                            } else if (s == "AA_CATEGORY_FP_SAVE_SLOT") {
                                isFpSlot = true;
                            }
                        }
                    }
                }
                continue;
            }

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
                dst = new TTAProgram::TerminalFUPort(op, inputOperand);
                TTAProgram::MoveGuard* guardCopy = 
                    guard == NULL ? NULL : guard->copy();
		
                auto move = createMove(src, dst, bus, guardCopy);
                TTAProgram::Instruction* instr = new TTAProgram::Instruction();
                instr->addMove(move);
            
                operandMoves.push_back(instr);

                debugDataToAnnotations(mi, *move);
                addPointerAnnotations(mi, *move);
                o += 1;                  
                const MachineOperand& offset = mi->getOperand(o);
                if (isBaseOffsetMemOperation(operation)) {
                    ++inputOperand;
                    // the offset is always the 2nd operand for the standard
                    // base+offset ops
                    assert(inputOperand == 2);

                    // create the offset operand move
                    src = createTerminal(offset);                
                    dst = new TTAProgram::TerminalFUPort(op, inputOperand);
                    TTAProgram::MoveGuard* guardCopy = 
                        guard == NULL ? NULL : guard->copy();
                    
                    move = createMove(src, dst, bus, guardCopy);
                    instr = new TTAProgram::Instruction();
                    instr->addMove(move);
                    operandMoves.push_back(instr);
                    debugDataToAnnotations(mi, *move);
                } else {
                    assert(offset.getImm() == 0);
                }
            } else {
                src = createTerminal(mo);
                dst = new TTAProgram::TerminalFUPort(op, inputOperand);
                TTAProgram::MoveGuard* guardCopy = 
                    guard == NULL ? NULL : guard->copy();

                auto move = createMove(src, dst, bus, guardCopy);
                TTAProgram::Instruction* instr = new TTAProgram::Instruction();
                instr->addMove(move);
#ifdef DEBUG_LLVMTCEBUILDER
                Application::logStream()
                    << "adding " << move->toString() << std::endl;
#endif
                operandMoves.push_back(instr);
                debugDataToAnnotations(mi, *move);
            }
        } else {
            ++outputOperand;
            if (operation.operand(outputOperand).isNull())
                continue;

            assert(operation.operand(outputOperand).isOutput() &&
                   !operation.operand(outputOperand).isAddress() &&
                   "Operand mismatch.");

            src = new TTAProgram::TerminalFUPort(op, outputOperand);
            dst = createTerminal(mo);

            TTAProgram::MoveGuard* guardCopy = 
                guard == NULL ? NULL : guard->copy();

            auto move = createMove(src, dst, bus, guardCopy);
            TTAProgram::Instruction* instr = new TTAProgram::Instruction();
            instr->addMove(move);
#ifdef DEBUG_LLVMTCEBUILDER
            Application::logStream()
                << "adding " << move->toString() << std::endl;
#endif
            resultMoves.push_back(instr);
            debugDataToAnnotations(mi, *move);
        }
    }

    for (unsigned int i = 0; i < resultMoves.size();i++) {
        TTAProgram::Instruction& resultIns = *resultMoves[i];
        for (int j = 0; j < resultIns.moveCount(); j++) {
            TTAProgram::Move& resultMove = resultIns.move(j);
            // if some operand was mem operand, copy the addr space annotations from that operand
            copyFUAnnotations(operandMoves, resultMove);
        }
    }
    // Return the first instruction of the whole operation.
    TTAProgram::Instruction* first = NULL;
    if (!operandMoves.empty()) {
        first = operandMoves[0];
    } else if (!resultMoves.empty()) {
        first = resultMoves[0];
    } else if (opDesc->isReturn() && mi->getNumOperands() == 0) {
        // LLVM allows RET without any paramters and with defined return value. 
        // RET with return value is converted above but not the other one.
        // To convert it to move, we just write 0 as source terminal. 
        // LLVM already  generated code to put return address on a top of the stack, 
        // so no point explicitely writing ra -> ret.1.
        const TTAMachine::HWOperation* jump =  &getHWOperation(opName);
        TTAProgram::TerminalFUPort* dst = 
        	new TTAProgram::TerminalFUPort(*jump, 1);
        int width = 32; // FIXME
        SimValue val(0, width);
                    
        auto move = createMove(
        	new TTAProgram::TerminalImmediate(val), dst, bus);
        TTAProgram::Instruction* instr = new TTAProgram::Instruction();
        instr->addMove(move);
        operandMoves.push_back(instr);
        first = instr;
        debugDataToAnnotations(mi, *move);
    } else {
        assert(false && "No moves?");
    }
    boost::shared_ptr<ProgramOperation> po(
    	new ProgramOperation(operation, mi));

    for (unsigned i = 0; i < operandMoves.size(); i++) {
        TTAProgram::Instruction* instr = operandMoves[i];
        auto m = instr->movePtr(0);

        // create the memory category annotations
        if (isSpill) {
            m->addAnnotation(
                TTAProgram::ProgramAnnotation(
                    TTAProgram::ProgramAnnotation::ANN_STACKUSE_SPILL));
        } else if (isRaSlot) {
            m->addAnnotation(
               TTAProgram::ProgramAnnotation(
                    TTAProgram::ProgramAnnotation::ANN_STACKUSE_RA_SAVE));
        } else if (isFpSlot) {
            m->addAnnotation(
                TTAProgram::ProgramAnnotation(
                    TTAProgram::ProgramAnnotation::ANN_STACKUSE_FP_SAVE));
        }

        proc->add(instr);
        createMoveNode(po, m, true);
    }
    for (unsigned i = 0; i < resultMoves.size(); i++) {
        TTAProgram::Instruction* instr = resultMoves[i];
        auto m = instr->movePtr(0);
        // create the memory category annotations
        if (isSpill) {
            m->addAnnotation(
                TTAProgram::ProgramAnnotation(
                    TTAProgram::ProgramAnnotation::ANN_STACKUSE_SPILL));
        } else if (isRaSlot) {
            m->addAnnotation(
               TTAProgram::ProgramAnnotation(
                    TTAProgram::ProgramAnnotation::ANN_STACKUSE_RA_SAVE));
        } else if (isFpSlot) {
            m->addAnnotation(
                TTAProgram::ProgramAnnotation(
                    TTAProgram::ProgramAnnotation::ANN_STACKUSE_FP_SAVE));
        }

        proc->add(instr);
        createMoveNode(po, m, false);
    }

    if (guard != NULL) {
	delete guard;
    }
    return first;
}

void
LLVMTCEBuilder::copyFUAnnotations(
    const std::vector<TTAProgram::Instruction*>& operandMoves, 
    TTAProgram::Move& move) const {
    for (unsigned int i = 0; i < operandMoves.size(); i++) {
        TTAProgram::Move& operandMove = operandMoves[i]->move(0);
        for (int j = 0; j < operandMove.annotationCount(); j++) {
            TTAProgram::ProgramAnnotation anno = operandMove.annotation(j);
            if (anno.id() == 
                TTAProgram::ProgramAnnotation::ANN_ALLOWED_UNIT_DST) {
                move.addAnnotation(
                    TTAProgram::ProgramAnnotation(
                        TTAProgram::ProgramAnnotation::ANN_ALLOWED_UNIT_SRC,
                        anno.payload()));
            }
        }
    }
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
 * Adds annotations to a pointer-register move to assist the
 * TCE-side alias analysis.
 */
void
LLVMTCEBuilder::addPointerAnnotations(
    const llvm::MachineInstr* mi, TTAProgram::Move& move) {
     
    // copy some pointer data to Move annotations
#if 0
    if (mi->memoperands_begin() == mi->memoperands_end()) {
        Application::logStream() << "move " << move->toString()
                                 << " does not have mem operands!"
                                 << std::endl;
    }
#endif

    int addrSpaceId = 0;
    // TODO: why this is a loop actually?? It only handles a single 
    // Move anyways --Pekka
    for (MachineInstr::mmo_iterator i = mi->memoperands_begin();
         i != mi->memoperands_end(); i++) {
        
        // annotate only RF -> {ld,st}.1 moves
        // @todo make this more foolproof
        if (!(move.source().isGPR() &&
              move.destination().operationIndex() == 1))
            continue;
            
        const llvm::Value* memOpValue = (*i)->getValue();

        if (memOpValue != NULL) {
            std::string pointerName = "";
            // can we get the name right away or have to through
            // GetElemntPtrInst
                
            if (memOpValue->hasName()) {
                pointerName = memOpValue->getName();
            } else if (isa<GetElementPtrInst>(memOpValue)) {
                memOpValue =
                    cast<GetElementPtrInst>(
                        memOpValue)->getPointerOperand();
                if (memOpValue->hasName()) {
                    pointerName = memOpValue->getName();
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
                move.addAnnotation(progAnnotation);
            }

            // try to find the origin for the pointer which can be
            // a function argument with 'noalias' attribute set
            const llvm::Value* originMemOpValue = memOpValue;
            while (originMemOpValue != NULL) {
                TCEString currentPointerName = 
                    (std::string)originMemOpValue->getName();

                // Query metadata of the memory operands to find work item
                // identifiers for OpenCL memory operands.
                if (dyn_cast<Instruction>(originMemOpValue) &&
                    dyn_cast<Instruction>(originMemOpValue)->getMetadata("wi")) {
                    const MDNode* md = 
                        cast<Instruction>(originMemOpValue)->getMetadata("wi");
                    const MDNode* XYZ = dyn_cast<MDNode>(md->getOperand(2));
                    assert(XYZ->getNumOperands() == 4);                
                    ConstantInt *CX = dyn_cast<ConstantInt>(
                        dyn_cast<llvm::ConstantAsMetadata>(XYZ->getOperand(1))->getValue());
                    ConstantInt *CY = dyn_cast<ConstantInt>(
                        dyn_cast<llvm::ConstantAsMetadata>(XYZ->getOperand(2))->getValue());
                    ConstantInt *CZ = dyn_cast<ConstantInt>(
                        dyn_cast<llvm::ConstantAsMetadata>(XYZ->getOperand(3))->getValue());

                    int id = (CZ->getZExtValue() & 0x0FF)
                            | ((CY->getZExtValue() & 0x0FF) << 8)
                            | ((CX->getZExtValue() & 0x0FF) << 16);
                    TTAProgram::ProgramAnnotation progAnnotation(
                        TTAProgram::ProgramAnnotation::
                        ANN_OPENCL_WORK_ITEM_ID, id);
                    move.addAnnotation(progAnnotation);
                    // In case the memory operand is BitCastInst, we may be
                    // looking at the vector memory access.
                    // Find the type of the accessed element and if it is 
                    // vector add second annotation marking the last work
                    // it id the vector is accessing.
                    // Without this information there DDGBuilder can not
                    // correctly create edges.
                    if (isa<BitCastInst>(originMemOpValue)) {
                        llvm::Type* type = 
                            dyn_cast<BitCastInst>(originMemOpValue)->getDestTy();
                        if (type->isPointerTy()) {
                            llvm::Type* typeElem = 
                                cast<PointerType>(type)->getElementType();
                            if (typeElem->isVectorTy()) {
                                int numElems = 
                                    cast<VectorType>(typeElem)->getNumElements();
                                int idLast = (CZ->getZExtValue() & 0x0FF)
                                        | ((CY->getZExtValue() & 0x0FF) << 8)
                                        | (((CX->getZExtValue() 
                                            + numElems) & 0x0FF) << 16);
                                TTAProgram::ProgramAnnotation progAnnotation(
                                    TTAProgram::ProgramAnnotation::
                                    ANN_OPENCL_WORK_ITEM_ID_LAST, idLast);
                                move.addAnnotation(progAnnotation);
                            }
                        }
                    }                                                
                }
                    
                if (isa<Argument>(originMemOpValue) && 
                    cast<Argument>(originMemOpValue)->hasNoAliasAttr()) {
                    TTAProgram::ProgramAnnotation progAnnotation(
                        TTAProgram::ProgramAnnotation::
                        ANN_POINTER_NOALIAS, 1);
                    move.addAnnotation(progAnnotation);
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

                    pointerName = originMemOpValue->getName();
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
                move.addAnnotation(pointerAnn);
            }

            addrSpaceId =
                cast<PointerType>(memOpValue->getType())->
                getAddressSpace();

            if (addrSpaceId != 0) {
                // this annotation is used only for alias analysis as
                // the address spaces are assumed to be always disjoint
                std::string addressSpace =
                    (boost::format("%d") % addrSpaceId).str();
                TTAProgram::ProgramAnnotation progAnnotation(
                    TTAProgram::ProgramAnnotation::ANN_POINTER_ADDR_SPACE, 
                    addressSpace);
                move.addAnnotation(progAnnotation);
                multiAddrSpacesFound_ = true;
            }                
        }
    }
    if (multiDataMemMachine_) {
        // annotate all memory moves with FU candidate sets
        // so the memory operations are assigned to the correct
        // load-store units in the multimemory machine     
 
        // for stack accesses, there is no LLVM pointer info in which
        // case we add the default addr space id 0 
        addCandidateLSUAnnotations(addrSpaceId, move);
    }
}

/**
 * Helper method that converts LLVM debug data markers to Move annotations.
 */
void
LLVMTCEBuilder::debugDataToAnnotations(
    const llvm::MachineInstr* mi, TTAProgram::Move& move) {

    // annotate the moves generated from known ra saves.
    if (mi->getFlag(MachineInstr::FrameSetup)) {
            TTAProgram::ProgramAnnotation progAnnotation(
                TTAProgram::ProgramAnnotation::ANN_STACKUSE_RA_SAVE);
            move.setAnnotation(progAnnotation);
    }

    DebugLoc dl = mi->getDebugLoc();
    if (!dl)
        return;

    // TODO: nobody currently generates these
    // spill line number kludges, this is deprecated.
    // annotate the moves generated from known spill instructions
    if (dl.getLine() == 0xFFFFFFF0) {
        TTAProgram::ProgramAnnotation progAnnotation(
            TTAProgram::ProgramAnnotation::ANN_STACKUSE_SPILL);
        move.setAnnotation(progAnnotation);
        ++spillMoveCount_;
    } else {
        // handle file+line number debug info

        bool hasDebugInfo = false;
        hasDebugInfo = dl.getScope() != NULL;
        if (hasDebugInfo) {
            
            int sourceLineNumber = -1;
            TCEString sourceFileName = "";
                
            // inspired from lib/codegen/MachineInstr.cpp
            sourceLineNumber = dl.getLine();
            sourceFileName = 
                static_cast<TCEString>(
                    cast<DIScope>(dl.getScope())->getFilename());

            if (sourceFileName.size() >
                TPEF::InstructionAnnotation::MAX_ANNOTATION_BYTES) {
                sourceFileName = 
                    sourceFileName.substr(
                        sourceFileName.size() - 
                        TPEF::InstructionAnnotation::MAX_ANNOTATION_BYTES,
                        TPEF::InstructionAnnotation::MAX_ANNOTATION_BYTES);
            }
            TTAProgram::ProgramAnnotation progAnnotation(
                TTAProgram::ProgramAnnotation::ANN_DEBUG_SOURCE_CODE_LINE, 
                sourceLineNumber);
            move.addAnnotation(progAnnotation);
                       
            if (sourceFileName != "") {
                TTAProgram::ProgramAnnotation progAnnotation(
                    TTAProgram::ProgramAnnotation::
                    ANN_DEBUG_SOURCE_CODE_PATH, 
                    sourceFileName);
                move.addAnnotation(progAnnotation);
            }
        }
    }
}

TTAProgram::TerminalRegister*
LLVMTCEBuilder::createTerminalRegister(
    const std::string& rfName, int idx) {

    if (!mach_->registerFileNavigator().hasItem(rfName)) {
	TCEString msg = "regfile ";
	msg << rfName << " not found.";
    }
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
}

/**
 * Creates a POM source terminal from an LLVM machine operand.
 *
 * @param mo LLVM machine operand.
 * @return POM terminal.
 */
TTAProgram::Terminal*
LLVMTCEBuilder::createTerminal(const MachineOperand& mo, int bitLimit) {

    if (mo.isReg()) {
        unsigned dRegNum = mo.getReg();

        // is it the RA register?
        if (isTTATarget() && dRegNum == raPortDRegNum()) {
            return new TTAProgram::TerminalFUPort(
                *UniversalMachine::instance().controlUnit()->
                returnAddressPort());
        }

        // an FU port register?
        TTAProgram::Terminal* term = createFUTerminal(mo);
        if (term != NULL)
            return term;

        // a general purpose register?
        std::string rfName = registerFileName(dRegNum);
        int idx = registerIndex(dRegNum);
        return createTerminalRegister(rfName, idx);
    } else if (mo.isFPImm()) {
        const APFloat& apf = mo.getFPImm()->getValueAPF();
#if LLVM_OLDER_THAN_4_0
        if (&apf.getSemantics() == &APFloat::IEEEhalf) { //Half float
#else
        if (&apf.getSemantics() == &APFloat::IEEEhalf()) { //Half float
#endif
            APInt api = apf.bitcastToAPInt();
            uint16_t binary = (uint16_t)api.getRawData()[0];
            SimValue val(32);
            val = HalfFloatWord( binary );
            return new TTAProgram::TerminalImmediate(val);
        } else {
            float fval = apf.convertToFloat();
            SimValue val(32);
            val = fval;
            return new TTAProgram::TerminalImmediate(val);
        }
    } else if (mo.isImm()) {
        int width = bitLimit;
        SimValue val(mo.getImm(), width);
        return new TTAProgram::TerminalImmediate(val);
    } else if (mo.isMBB() || mo.isBlockAddress()) {
        return createMBBReference(mo);
    } else if (mo.isFI()) {
        std::cerr << " Frame index source operand NOT IMPLEMENTED!"
                  << std::endl;
        assert(false);
    } else if (mo.isCPI()) {
        if (!functionAtATime_) {
            int width = 32; // FIXME
            unsigned idx = mo.getIndex();
            assert(currentFnCP_.find(idx) != currentFnCP_.end() &&
               "CPE not found!");
            unsigned addr = currentFnCP_[idx];
            SimValue cpeAddr(addr, width);
            return new TTAProgram::TerminalImmediate(cpeAddr);
        } else {          
            // Constant Pool Index is converted to dummy
            // symbol reference. Will be converted back
            // when doing POM->LLVM transfer.
            // Format of reference is ".CP_INDEX_OFFSET".
            TCEString ref(".CP_");
            ref <<  mo.getIndex() << "_" << mo.getOffset();
            return createSymbolReference(ref);
      }
    } else if (mo.isJTI()) {
    	TCEString ref(".JTI_");
        ref << mo.getIndex();
        return createSymbolReference(ref);    
    } else if (mo.isGlobal()) {
        unsigned aSpaceId = 
            cast<PointerType>(mo.getGlobal()->getType())->getAddressSpace();

        TTAMachine::AddressSpace& aSpace = 
            addressSpaceById(aSpaceId);

        SmallString<256> Buffer;
        mang_->getNameWithPrefix(Buffer, mo.getGlobal(), false);
        TCEString name(Buffer.c_str());

        if (name == END_SYMBOL_NAME) {
            return createSymbolReference(name);
        } else if (dataLabels_.find(name) != dataLabels_.end()) {
            SimValue address(dataLabels_[name] + mo.getOffset(), 32);
            return new TTAProgram::TerminalAddress(address, aSpace);

        } else {
            // TODO: this lacks offset??
            return createSymbolReference(name);
        }
    } else if (mo.isJTI()) {
        std::cerr << " Jump table index operand NOT IMPLEMENTED!\n";
        assert(false);
    } else if (mo.isSymbol()) {
        return createSymbolReference(mo);
    } else if (mo.isMCSymbol()) {
        return createProgramOperationReference(mo);
    } else {
        std::cerr << "Unknown src operand type!" << std::endl;
        // LLVM does not include dump() when built in non-debug mode.
        // mo.getParent()->dump();
        assert(false);
    }
    abortWithError("Should not get here!");
    return NULL;
}

TTAProgram::Terminal* 
LLVMTCEBuilder::createProgramOperationReference(
    const MachineOperand& mo) {
    llvm::MCSymbol* symbol = mo.getMCSymbol();
    TTAProgram::TerminalProgramOperation* term = 
        new TTAProgram::TerminalProgramOperation(symbol->getName().str());
    symbolicPORefs_.insert(term);
    return term;
}

/**
 * Fixes the symbolic ProgramOperation references to point to the
 * now created real ProgramOperations.
 *
 * Assumes the POs are found in the label index.
 */
void
LLVMTCEBuilder::fixProgramOperationReferences() {
    for (std::set<TTAProgram::TerminalProgramOperation*>::const_iterator i = 
             symbolicPORefs_.begin(); i != symbolicPORefs_.end(); ++i) {
        TTAProgram::TerminalProgramOperation* term = *i;
        if (term->isProgramOperationKnown()) continue;
        ProgramOperationPtr po = labeledPOs_[term->label()];
        assert(po.get() != NULL);
        term->setProgramOperation(po);
    }
}

TTAProgram::Terminal*
LLVMTCEBuilder::createMBBReference(const MachineOperand& mo) {
    TTAProgram::InstructionReference dummy(NULL);
    
    TTAProgram::TerminalInstructionReference* ref =
        new TTAProgram::TerminalInstructionReference(dummy);
    mbbReferences_[ref] = mbbName(*mo.getMBB());
    return ref;
}

TTAProgram::Terminal*
LLVMTCEBuilder::createSymbolReference(const MachineOperand& mo) {
    //} else if (mo.isExternalSymbol()) {
    
    /**
     * NOTE: Hack to get code compiling even if llvm falsely makes libcalls to
     *       external functions even if they are found from currently lowered program.
     *
     *       http://llvm.org/bugs/show_bug.cgi?id=2673
     *
     *       Should be removed after fix is applied to llvm.. (maybe never...)
     */
    return createSymbolReference(mo.getSymbolName());
}

TTAProgram::Terminal*
LLVMTCEBuilder::createSymbolReference(const TCEString& name) {
    if (name == END_SYMBOL_NAME) {
        return NULL;
    }

    TTAProgram::InstructionReference* dummy =
        new TTAProgram::InstructionReference(NULL);
    
    
    TTAProgram::TerminalInstructionReference* ref =
        new TTAProgram::TerminalInstructionReference(
            *dummy);
    
    codeLabelReferences_[ref] = name;
    return ref;
    /**
     * END OF HACK
     */
    
}

/**
 * Emits data definitions for a function constant pool.
 *
 * The constant pool is appended to the end of the default data memory.
 *
 * @param mcp Constant pool to emit.
 */
void
LLVMTCEBuilder::emitConstantPool(const MachineConstantPool& mcp) {

    currentFnCP_.clear();

    const std::vector<MachineConstantPoolEntry>& cp = mcp.getConstants();

    unsigned& dataEndPos = dataEnd(addressSpaceById(0));      
    for (unsigned i = 0, e = cp.size(); i != e; ++i) {
        assert(!(cp[i].isMachineConstantPoolEntry()) && "NOT SUPPORTED");
        currentFnCP_[i] = 
            createDataDefinition(0, dataEndPos, cp[i].Val.ConstVal);
    }
}


/**
 * Creates POM instruction for a move.
 *
 * @param src The source operand.
 * @param dst The dst operand.
 * @return POM Move.
 */
std::shared_ptr<TTAProgram::Move>
LLVMTCEBuilder::createMove(
    const MachineOperand& src, const MachineOperand& dst, 
    TTAProgram::MoveGuard* guard) {
    assert(!src.isReg() || src.isUse());
    assert(dst.isDef());
    
    // eliminate register-to-itself moves
    if (dst.isReg() && src.isReg() && dst.getReg() == src.getReg()) {
        return NULL;
    }

    Bus& bus = UniversalMachine::instance().universalBus();
    TTAProgram::Terminal* dstTerm = createTerminal(dst);
    auto move = createMove(
        createTerminal(src, dstTerm->port().width()), dstTerm, bus, guard);

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
    const MachineInstr* mi, TTAProgram::CodeSnippet* proc, 
    bool conditional, bool trueGuard) {

    unsigned int operandCount = conditional ? 3 : 2;
    if (mi->getNumOperands() > operandCount) {
        for (unsigned int i = operandCount; i < mi->getNumOperands(); i++) {
            assert(mi->getOperand(i).isImplicit());
        }
    }

    assert(mi->getNumOperands() >= operandCount); // src, dst

    const MachineOperand& dst = mi->getOperand(0);
    const MachineOperand& src = mi->getOperand(operandCount - 1);
    TTAProgram::MoveGuard* guard = NULL;
    if (conditional) {
        const MachineOperand& gmo = mi->getOperand(1);
        assert (gmo.isReg() && gmo.isUse());
        // Create move from the condition operand register to bool register
        // which is used by the guard.
        TTAProgram::Terminal *t = createTerminal(gmo);
        // inv guards not yet supported
        guard = createGuard(t, trueGuard);
    }
    auto move = createMove(src, dst, guard);

    if (move == NULL) {
        return NULL;
    }
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
    const MachineInstr*  mi , TTAProgram::CodeSnippet* proc) {

    Bus& bus = UniversalMachine::instance().universalBus();
    TTAProgram::TerminalFUPort* src = new TTAProgram::TerminalFUPort(
        *UniversalMachine::instance().controlUnit()->returnAddressPort());

    TTAMachine::HWOperation& jump =
        *UniversalMachine::instance().controlUnit()->operation("jump");

    OperationPool pool;
    const Operation& operation = pool.operation("jump");    

    TTAProgram::TerminalFUPort* dst = new TTAProgram::TerminalFUPort(jump, 1);
    auto move = createMove(src, dst, bus);
    TTAProgram::Instruction* instr = new TTAProgram::Instruction();
    instr->addMove(move);
    proc->add(instr);
    // Create ProgramOperation also for return so DDGBuilder does not have
    // to do that.
    boost::shared_ptr<ProgramOperation> po(
        new ProgramOperation(operation, mi));    
    createMoveNode(po, move, true);
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
    const MachineInstr* mi, TTAProgram::CodeSnippet* proc) {

// 0 = dest?

    const MachineOperand& guardMo = mi->getOperand(1);

    // Create move from the condition operand register to bool register
    // which is used by the guard.
    TTAProgram::Terminal *guardTerminal = createTerminal(guardMo);

    assert(guardTerminal != NULL);

    TTAProgram::Terminal* dstT = createTerminal(mi->getOperand(0));
    TTAProgram::Terminal* dstF = createTerminal(mi->getOperand(0));
    TTAProgram::Terminal* srcT = createTerminal(mi->getOperand(2));
    TTAProgram::Terminal* srcF = createTerminal(mi->getOperand(3));

    Bus& bus = UniversalMachine::instance().universalBus();
    TTAProgram::Instruction *firstIns = NULL;

    // do no create X -> X moves.
    if (dstT->equals(*srcT)) {
        delete srcT;
        delete dstT;
    } else {
        TTAProgram::MoveGuard* trueGuard = createGuard(guardTerminal, true);
        assert(trueGuard != NULL);
        auto trueMove = createMove(srcT, dstT, bus, trueGuard);
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
        auto falseMove = createMove(srcF, dstF, bus, falseGuard);
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
    SmallString<256> Buffer;
#ifdef LLVM_OLDER_THAN_6_0
    mang_->getNameWithPrefix(Buffer, mbb.getParent()->getFunction(), false);
#else
    mang_->getNameWithPrefix(Buffer, &mbb.getParent()->getFunction(), false);
#endif
    TCEString name(Buffer.c_str());
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

    return true;
}

/**
 * Emit stack pointer initialization move to the begining of the program.
 */
void
LLVMTCEBuilder::emitSPInitialization() {
    TTAProgram::Instruction& first = prog_->firstInstruction();

    TTAProgram::Procedure& proc =
        dynamic_cast<TTAProgram::Procedure&>(first.parent());

    emitSPInitialization(proc);
}

void
LLVMTCEBuilder::emitSPInitialization(TTAProgram::CodeSnippet& target) {

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

    unsigned ival = initialStackPointerValue_;

    if (initialStackPointerValue_ == 0 || 
        initialStackPointerValue_ >
        (addressSpaceById(0).end() & 0xfffffff8))
        ival = addressSpaceById(0).end() & 0xfffffff8;

    SimValue val(ival, 32);
    TTAProgram::TerminalImmediate* src =
        new TTAProgram::TerminalImmediate(val);

    Bus& bus = UniversalMachine::instance().universalBus();
    auto move = createMove(src, dst, bus);
    TTAProgram::Instruction* spInit = new TTAProgram::Instruction();
    spInit->addMove(move);
    
    if (target.instructionCount() > 0) {
        TTAProgram::Instruction& first = target.firstInstruction(); 
        target.insertBefore(first, spInit);
        if (prog_->instructionReferenceManager().hasReference(first)) {
            prog_->instructionReferenceManager().replace(first, *spInit);
        }
    } else {
        target.add(spInit);
    }
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
    const MachineInstr* mi, TTAProgram::CodeSnippet* proc) {

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

    std::vector<std::string> addressedFUs;
    // test this is an addressable instruction
    bool addressedOp = StringTools::containsChar(opName, '.');

    if (addressedOp) {
	if (opName.substr(0,3) == "_AS") {
	    int addressSpaceId = -1;
	    std::string foo;
	    std::string addressedAS;
	    std::istringstream iss(opName);
	    std::getline(iss, foo, '.');
	    std::getline(iss, addressedAS, '.');
	    std::getline(iss, opName, '.');
	    char *end;

	    AddressSpace* targetAS = NULL;

	    if (addressedAS.size() && addressedAS[0] == '#') {
		addressSpaceId = strtol(addressedAS.c_str()+1, &end, 10);
		if (end == addressedAS.c_str()+1) {
		    std::cerr << "ERROR: Address space id following # not a number '" << addressedAS
			      << std::endl;
		} else {
		    // find addresspace with given id
		    const TTAMachine::Machine::AddressSpaceNavigator& nav = mach_->addressSpaceNavigator();
		    for (int i = 0; i < nav.count(); i++) {
			AddressSpace* as = nav.item(i);
			if (as->hasNumericalId(addressSpaceId)) {
			    targetAS = as;
			    break;
			}
		    }
		}
	    } else {
		const TTAMachine::Machine::AddressSpaceNavigator& nav = mach_->addressSpaceNavigator();
		for (int i = 0; i < nav.count(); i++) {
		    AddressSpace* as = nav.item(i);
		    if (as->name() == addressedAS) {
			targetAS = as;
			break;
		    }
		}
	    }

	    if (targetAS == NULL) {
		std::cerr << "ERROR: Address space '" << addressedAS
			  << "' not found."
			  << std::endl;
		assert(false);
	    }
	    
            // find function units with given as.
	    const TTAMachine::Machine::FunctionUnitNavigator& fuNav = mach_->functionUnitNavigator();
	    for (int j = 0; j < fuNav.count(); j++) {
		const FunctionUnit* fu = fuNav.item(j);
		if (fu->addressSpace() == targetAS) {
		    addressedFUs.push_back(fu->name());
		}
	    }
	} else {
	    // Split the string to get the FU and the operation
	    std::string addressedFU;

	    std::istringstream iss(opName);
	    std::getline(iss, addressedFU, '.');
	    std::getline(iss, opName, '.');
	    if (!mach_->functionUnitNavigator().hasItem(addressedFU)) {
		std::cerr << "ERROR: Function Unit '" << addressedFU
			  << "' not found."
			  << std::endl;
		assert(false);
	    }
	    addressedFUs.push_back(addressedFU);
        }
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

    const UniversalFunctionUnit& fu = UniversalMachine::instance().universalFunctionUnit();
    if (!fu.hasOperation(opName)) {
        std::cerr 
            << "ERROR: Explicitly executed operation '" 
            << opName << "' not found."
            << std::endl;        
        assert(false);
    }

    HWOperation* op = fu.operation(opName);

    Bus& bus = UniversalMachine::instance().universalBus();
    std::vector<TTAProgram::Instruction*> operandMoves;
    std::vector<TTAProgram::Instruction*> resultMoves;
    ExecutionPipeline::OperandSet useOps = op->pipeline()->readOperands();
    ExecutionPipeline::OperandSet defOps = op->pipeline()->writtenOperands();


    OperationPool pool;
    const Operation& operation = pool.operation(opName.c_str());
    int inputOperand = 0;

    // go through the operands (0 is the chain, 1 is the asm string)
    for (unsigned o = 3; o < mi->getNumOperands(); o++) {
        const MachineOperand& mo = mi->getOperand(o);
        if (!(mo.isReg() || mo.isImm() || mo.isGlobal()))   {
            // All operands should be in registers. Everything else is ignored.
            std::cerr << "Ignoring an operand of " << opName << std::endl;
            continue;
        }

        TTAProgram::Terminal* src = NULL;
        TTAProgram::Terminal* dst = NULL;
        if (mo.isImm() || mo.isGlobal() || mo.isUse()) {
            // implicit usage of whole vector when one element used.
            if (useOps.empty()) {
                // ignore implicit defs that are too many.
                if (mo.isImplicit()) {
                    continue;
                }
                std::cerr << std::endl;
                std::cerr <<"ERROR: Too many input operands for custom "
                          << "operation '" << opName << "'." << std::endl;
                assert(false);
            }
            src = createTerminal(mo);
            dst = new TTAProgram::TerminalFUPort(*op, (*useOps.begin()));
            useOps.erase(useOps.begin());
            ++inputOperand;
        } else {
            if (defOps.empty()) {
                // ignore implicit defs that are too many.
                if (mo.isImplicit()) {
                    continue;
                }
                std::cerr << std::endl;
                std::cerr << "ERROR: Too many output operands for custom "
                          << "operation '" << opName << "'." << std::endl;

                assert(false);
            }
            src = new TTAProgram::TerminalFUPort(*op, (*defOps.begin()));
            dst = createTerminal(mo);
            defOps.erase(defOps.begin());

        }

        auto move = createMove(src, dst, bus);
        TTAProgram::Instruction* instr = new TTAProgram::Instruction();
        instr->addMove(move);

        if (mo.isImm() || mo.isGlobal() || mo.isUse()) {
            operandMoves.push_back(instr);

            // Custom memory accessing operation? Assume absolute addr for now.
            if (operation.operand(inputOperand).isAddress()) {
                debugDataToAnnotations(mi, *move);
                addPointerAnnotations(mi, *move);
            }
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
        if (addressedOp) {
            // remove other allowed fu annotations, they are not allowed
            operandMoves[i]->move(0).removeAnnotations(
                TTAProgram::ProgramAnnotation::ANN_ALLOWED_UNIT_DST);
            
            for (unsigned int j = 0; j < addressedFUs.size(); j++) {
                TTAProgram::ProgramAnnotation dstCandidate(
                    TTAProgram::ProgramAnnotation::ANN_ALLOWED_UNIT_DST,
                    addressedFUs[j]);
                operandMoves[i]->move(0).addAnnotation(dstCandidate);
            }
        }
    }

    for (unsigned i = 0; i < resultMoves.size(); i++) {
        proc->add(resultMoves[i]);
        if (addressedOp) {
            // remove other allowed fu annotations, they are not allowed
            resultMoves[i]->move(0).removeAnnotations(
                TTAProgram::ProgramAnnotation::ANN_ALLOWED_UNIT_SRC);
            
            for (unsigned int j = 0; j < addressedFUs.size(); j++) {
                TTAProgram::ProgramAnnotation srcCandidate(
                    TTAProgram::ProgramAnnotation::ANN_ALLOWED_UNIT_SRC,
                    addressedFUs[j]);
                resultMoves[i]->move(0).addAnnotation(srcCandidate);
            }
        }
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
    const std::string op, const MachineInstr* mi, 
    TTAProgram::CodeSnippet* proc) {

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
    const MachineInstr* mi, TTAProgram::CodeSnippet* proc) {

    if (mi->getNumOperands() != 5) {
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

    CodeGenerator codeGenerator(*mach_);

    TTAProgram::Terminal* srcTerminal =
        codeGenerator.createTerminalRegister(sp, false);

    const MachineOperand& dest = mi->getOperand(3);
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
    const MachineInstr* mi, TTAProgram::CodeSnippet* proc) {

    if (mi->getNumOperands() != 6) {
        Application::logStream() 
            << "got " << mi->getNumOperands() << " operands" << std::endl;
        abortWithError(
            "ERROR: wrong number of operands in \".pointer_category\"");
    }

    TTAProgram::Instruction& lastInstruction =
        proc->lastInstruction();

    CodeGenerator codeGenerator(*mach_);

    TTAProgram::Terminal* srcTerminal = 
        createTerminal(mi->getOperand(5));

    TTAProgram::Terminal* dstTerminal = 
        createTerminal(mi->getOperand(3));
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
    const MachineInstr* mi, TTAProgram::CodeSnippet* proc) {

    if (mi->getNumOperands() != 7) {
        std::cerr << "ERROR: wrong number of operands in "".setjmp"""
            << std::endl;
        assert(false);
    }

    const MachineOperand& val = mi->getOperand(3);
    const MachineOperand& env = mi->getOperand(5);

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

    CodeGenerator codeGenerator(*mach_);

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
    const MachineInstr* /*mi*/, TTAProgram::CodeSnippet* proc,
    bool constructors) {

    std::string globalName = 
        constructors ? 
        ("llvm.global_ctors") : ("llvm.global_dtors");

    TTAProgram::Instruction* firstInstruction = NULL;

    // find the _llvm.global_Xtors global with the
    // function pointers and priorities
    for (Module::const_global_iterator i = mod_->global_begin();
         i != mod_->global_end(); i++) {

#ifdef LLVM_OLDER_THAN_3_8        
        const GlobalVariable* gv = i;
#else
        const GlobalVariable* gv = &(*i);
#endif

        if (gv->getName() == globalName && gv->use_empty()) {
            // The initializer should be an array of '{ int, void ()* }' 
            // structs for LLVM 3.4 and lower, and an array of
            // '{ int, void ()*, i8* }' structs for LLVM 3.5.
            // The first value is the init priority, which we ignore.
            const ConstantArray* initList = cast<const ConstantArray>
                (gv->getInitializer());
            for (unsigned i = 0, e = initList->getNumOperands(); i != e; ++i) {
                if (ConstantStruct* cs = 
                    dyn_cast<ConstantStruct>(initList->getOperand(i))) {

                    // LLVM 3.5 introduced an additional field, so test for
                    // an array of 3-element structs.
                    if (cs->getNumOperands() != 3) {
                        return firstInstruction;
                    }

                     // Found a null terminator, exit printing.
                    if (cs->getOperand(1)->isNullValue()) {
                        return firstInstruction;
                    }

                    // Emit the call.
                    GlobalValue* gv =  dyn_cast<GlobalValue>(
                        cs->getOperand(1));
                    assert(gv != NULL&&"global constructor name not constv");

                    SmallString<256> Buffer;
                    mang_->getNameWithPrefix(Buffer, gv, false);
                    TCEString name(Buffer.c_str());
                    
                    TTAProgram::Terminal* xtorRef = NULL;

                    // cannot use instr. refs in the new builder as the
                    // instructions won't belong in a procedure before
                    // they have been fully scheduled.
                    xtorRef = new TTAProgram::TerminalSymbolReference(name);

                    CodeGenerator codeGenerator(*mach_); 

                    auto ctrCall =
                        std::make_shared<TTAProgram::Move>(
                            xtorRef, codeGenerator.createTerminalFUPort("call", 1),
                            UniversalMachine::instance().universalBus());

                    OperationPool opPool;
                    
                    // Create ProgramOperation also for return so DDGBuilder does not have
                    // to do that.
                    boost::shared_ptr<ProgramOperation> po(
                        new ProgramOperation(opPool.operation("call")));
                    createMoveNode(po, ctrCall, true);

                    TTAProgram::Instruction* newInstr =
                        new TTAProgram::Instruction(
                            TTAMachine::NullInstructionTemplate::instance());

                    newInstr->addMove(ctrCall);
                    proc->add(newInstr);
                    
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
    const MachineInstr* mi, TTAProgram::CodeSnippet* proc) {

    if (mi->getNumOperands() != 7) {
        std::cerr << "ERROR: wrong number of operands in "".longjmp"""
            << std::endl;
        assert(false);
    }

    const MachineOperand& env = mi->getOperand(3);
    const MachineOperand& val = mi->getOperand(5);

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

    CodeGenerator codeGenerator(*mach_);

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
 * Emits a vector build instruction.
 *
 * Creates moves from all source operands into all elements of the
 * destintion vector
 */
TTAProgram::Instruction*
LLVMTCEBuilder::emitVectorBuild(
    int elementCount,
    const MachineInstr* mi, TTAProgram::CodeSnippet* proc) {

    Bus& bus = UniversalMachine::instance().universalBus();
    TTAProgram::Instruction* first = NULL;
    MachineOperand dstOperand = mi->getOperand(0);

    int dRegNum = dstOperand.getReg();
    int idx = registerIndex(dRegNum);
    std::string vectorRfName = registerFileName(dRegNum);
    // TODO: does not work with more than 8-sized vector.
    int nameIndex = strlen("_VECTOR_") +2;
    for (int i = 0; i < elementCount; i++) {
	int nextNameIndex = vectorRfName.find('+', nameIndex);
	int len = nextNameIndex - nameIndex;
	std::string rfName = vectorRfName.substr(nameIndex, len);
	nameIndex = nextNameIndex + 1;

	TTAProgram::Terminal* dst = createTerminalRegister(rfName, idx);
	TTAProgram::Terminal* src = createTerminal(mi->getOperand(i+1));
	auto move = createMove(src, dst, bus);

	TTAProgram::Instruction* instr = new TTAProgram::Instruction();
	if (first == NULL) {
	    first = instr;
	}

	instr->addMove(move);
	proc->add(instr);
    }
    return first;
}

/**
 * Emits a vector move instruction.
 *
 * Creates moves from all vector lanes.
 */
TTAProgram::Instruction*
LLVMTCEBuilder::emitVectorMov(
    int elementCount,
    const MachineInstr* mi, TTAProgram::CodeSnippet* proc) {

    Bus& bus = UniversalMachine::instance().universalBus();
    TTAProgram::Instruction* first = NULL;
    MachineOperand dstOperand = mi->getOperand(0);
    MachineOperand srcOperand = mi->getOperand(1);

    int sRegNum = srcOperand.getReg();
    int dRegNum = dstOperand.getReg();
    int sIdx = registerIndex(sRegNum);
    int dIdx = registerIndex(dRegNum);

    std::string srcVectorRfName = registerFileName(sRegNum);
    std::string dstVectorRfName = registerFileName(dRegNum);
    // TODO: does not work with more than 8-sized vector.
    int srcNameIndex = strlen("_VECTOR_") +2;
    int dstNameIndex = strlen("_VECTOR_") +2;
    for (int i = 0; i < elementCount; i++) {
	int nextSrcNameIndex = srcVectorRfName.find('+', srcNameIndex);
	int nextDstNameIndex = dstVectorRfName.find('+', dstNameIndex);
	int sLen = nextSrcNameIndex - srcNameIndex;
	int dLen = nextDstNameIndex - dstNameIndex;
	std::string srcRfName = srcVectorRfName.substr(srcNameIndex, sLen);
	std::string dstRfName = dstVectorRfName.substr(dstNameIndex, dLen);
	srcNameIndex = nextSrcNameIndex + 1;
	dstNameIndex = nextDstNameIndex + 1;

	TTAProgram::Terminal* dst = createTerminalRegister(dstRfName, dIdx);
	TTAProgram::Terminal* src = createTerminalRegister(srcRfName, sIdx);
	auto move = createMove(src, dst, bus);

	TTAProgram::Instruction* instr = new TTAProgram::Instruction();
	if (first == NULL) {
	    first = instr;
	}

	instr->addMove(move);
	proc->add(instr);
    }
    return first;
}
/**
 *
 * Emits an llvm vector extract instruction.
 *
 * Reads one element from a vector to a scalar registers.
 *
 */
TTAProgram::Instruction*
LLVMTCEBuilder::emitVectorExtract(
    const MachineInstr* mi, TTAProgram::CodeSnippet* proc) {

    MachineOperand srcMO = mi->getOperand(1);
    MachineOperand dstMO = mi->getOperand(0);
    MachineOperand elementIndexMO = mi->getOperand(2);
    
    int dRegNum = srcMO.getReg();
    int idx = registerIndex(dRegNum);
    std::string vectorRfName = registerFileName(dRegNum);
    // TODO: does not work with more than 8-sized vectors
    int nameIndex = strlen("_VECTOR_") +2;
    int nextNameIndex = vectorRfName.find('+', nameIndex);
    
    // this for loop just searches for the correct rf name from the string
    assert(elementIndexMO.isImm());
    int elementIndex = elementIndexMO.getImm();
    for (int i = 0; i < elementIndex; i++) {
	nameIndex = nextNameIndex + 1;
	nextNameIndex = vectorRfName.find('+', nameIndex);
    }
    int len = nextNameIndex - nameIndex;
    std::string rfName = vectorRfName.substr(nameIndex, len);
    
    TTAProgram::Terminal* src = createTerminalRegister(rfName, idx);
    TTAProgram::Terminal* dst = createTerminal(dstMO);
    Bus& bus = UniversalMachine::instance().universalBus();

    auto move = createMove(src, dst, bus);
    
    TTAProgram::Instruction* instr = new TTAProgram::Instruction();
    instr->addMove(move);
    proc->add(instr);
    return instr;
}

/**
 * Emits an llvm vector insert instruction.
 * 
 * This instruction copies a vector into different vector, taking one element
 * from a scalar.
 * If the source and destinations are same, skip the copying of the vector,
 * only create the scalar move into the element of the vector
 */
TTAProgram::Instruction*
LLVMTCEBuilder::emitVectorInsert(
    int elementCount, 
    const MachineInstr* mi, 
    TTAProgram::CodeSnippet* proc) {
    
    MachineOperand dstVectorMO = mi->getOperand(0);
    MachineOperand srcVectorMO = mi->getOperand(1);
    MachineOperand srcScalarMO = mi->getOperand(2);
    MachineOperand elementIndexMO = mi->getOperand(3);
    
    int dRegNumDst = dstVectorMO.getReg();
    int dstIdx = registerIndex(dRegNumDst);
    std::string dstVectorRfName = registerFileName(dRegNumDst);
    // TODO: does not work with more than 8-sized vectors
    int dstNameIndex = strlen("_VECTOR_") +2;
    int dstNextNameIndex = dstVectorRfName.find('+', dstNameIndex);

    assert(elementIndexMO.isImm());
    int elementIndex = elementIndexMO.getImm();
    assert(elementIndex < elementCount);
    for (int i = 0; i < elementIndex; i++) {
        dstNameIndex = dstNextNameIndex + 1;
        dstNextNameIndex = dstVectorRfName.find('+', dstNameIndex);
    }
    int dlen = dstNextNameIndex - dstNameIndex;
    std::string dstRfName = dstVectorRfName.substr(dstNameIndex, dlen);
    
    TTAProgram::Terminal* src = createTerminal(srcScalarMO);
    TTAProgram::Terminal* dst = createTerminalRegister(dstRfName, dstIdx);
    Bus& bus = UniversalMachine::instance().universalBus();

    auto move = createMove(src, dst, bus);
    
    TTAProgram::Instruction* instr = new TTAProgram::Instruction();
    instr->addMove(move);
    proc->add(instr);

    // if not same registers, need to copy whole vector excludng the 1 element.
    if (srcVectorMO.getReg() != dstVectorMO.getReg()) {
	int dRegNumSrc = srcVectorMO.getReg();
	int srcIdx = registerIndex(dRegNumSrc);
	std::string srcVectorRfName = registerFileName(dRegNumSrc);
        // TODO: does not work with more than 8-sized vectors
	int srcNameIndex = strlen("_VECTOR_")+2;
	int srcNextNameIndex = srcVectorRfName.find('+', srcNameIndex);

        // TODO: does not work with more than 8-sized vectors
	dstNameIndex = strlen("_VECTOR_")+2;
	dstNextNameIndex = dstVectorRfName.find('+', dstNameIndex);
	for (int i = 0; i < elementCount; i++) {
	    if (i != elementIndex) {
		int dlen = dstNextNameIndex - dstNameIndex;
		std::string dstRfName = 
		    dstVectorRfName.substr(dstNameIndex, dlen);
	    
		int slen = srcNextNameIndex - srcNameIndex;
		std::string srcRfName = 
		    srcVectorRfName.substr(srcNameIndex, slen);

		TTAProgram::Terminal* src = 
		    createTerminalRegister(srcRfName, srcIdx);
		TTAProgram::Terminal* dst = 
		    createTerminalRegister(dstRfName, dstIdx);
		Bus& bus = UniversalMachine::instance().universalBus();
		auto move = createMove(src, dst, bus);
		
		TTAProgram::Instruction* instr = new TTAProgram::Instruction();
		instr->addMove(move);
		proc->add(instr);
            }
	    srcNameIndex = srcNextNameIndex + 1;
	    srcNextNameIndex = srcVectorRfName.find('+', srcNameIndex);

	    dstNameIndex = dstNextNameIndex + 1;
	    dstNextNameIndex = dstVectorRfName.find('+', dstNameIndex);
	}
    }
    return instr;
}

/**
 * Emits generic vector calculation instruction.
 *
 * Emits operation which operates between 2 vectors.
 * These operations are expanded into multiple tce operations.
 */
TTAProgram::Instruction*
LLVMTCEBuilder::emitVectorInstruction(
    const std::string& opName,
    int elementCount,
    const MachineInstr* mi, TTAProgram::CodeSnippet* proc) {

    TTAProgram::Instruction* firstIns = NULL;
    Bus& bus = UniversalMachine::instance().universalBus();

    int operandCount = mi->getNumOperands();
    // TODO: does not work with more than 8-sized vectors
    std::vector<int> regNameStringIndex(operandCount, strlen("_VECTOR_") +2);
    const HWOperation& hwop = getHWOperation(opName);

    OperationPool pool;
    const Operation& operation = pool.operation(opName.c_str());

    for (int i = 0; i < elementCount; i++) {
	int inputOperand = 0;
	int outputOperand = operation.numberOfInputs();
	std::vector<TTAProgram::Instruction*> operandMoves;
	std::vector<TTAProgram::Instruction*> resultMoves;

	for (unsigned o = 0; o < mi->getNumOperands(); o++) {
	    const MachineOperand* mo = &mi->getOperand(o);

	    TTAProgram::Terminal* tr = NULL;
	    if (vectorOperandSize(*mo) > 1) {
		int dRegNum = mo->getReg();
		int idx = registerIndex(dRegNum);
		std::string vectorRfName = registerFileName(dRegNum);
		int nameIndex = regNameStringIndex[o];
		int nextNameIndex = vectorRfName.find('+', nameIndex);
		int len = nextNameIndex - nameIndex;
		std::string rfName = vectorRfName.substr(nameIndex, len);
		regNameStringIndex[o] = nextNameIndex + 1;
		tr = createTerminalRegister(rfName, idx);
	    } else {
		o += i;
		mo = &mi->getOperand(o);
		assert(vectorOperandSize(*mo) == 1);
		tr = createTerminal(*mo);
	    }

	    // input
	    if (!mo->isReg() || mo->isUse() || 
		operation.numberOfOutputs() == 0) {
		++inputOperand;

		// something messed up?
		if (inputOperand > operation.numberOfInputs()) 
		    continue;

		TTAProgram::Terminal* dst = 
		    new TTAProgram::TerminalFUPort(hwop, inputOperand);
		auto move = createMove(tr, dst, bus);
		TTAProgram::Instruction* ins = new TTAProgram::Instruction();
		if (firstIns == NULL) {
		    firstIns = ins;
		}
		ins->addMove(move);
		operandMoves.push_back(ins);
	    } else {
		// output
		++outputOperand;

		if (operation.operand(outputOperand).isNull())
		    continue;

		assert(operation.operand(outputOperand).isOutput() &&
		       !operation.operand(outputOperand).isAddress() &&
		       "Operand mismatch.");

		TTAProgram::Terminal* src = 
		    new TTAProgram::TerminalFUPort(hwop, outputOperand);
		
		auto move = createMove(src, tr, bus);
		TTAProgram::Instruction* ins = new TTAProgram::Instruction();
		ins->addMove(move);
		resultMoves.push_back(ins);
	    }
	    // skip to next operand
	    if (vectorOperandSize(*mo) == 1) {
		o += (1-i);
	    }
	}
	boost::shared_ptr<ProgramOperation> po(
	    new ProgramOperation(operation, mi));
	
	for (unsigned i = 0; i < operandMoves.size(); i++) {
	    TTAProgram::Instruction* instr = operandMoves[i];
	    auto m = instr->movePtr(0);
	    proc->add(instr);
	    createMoveNode(po, m, true);
	}
	for (unsigned i = 0; i < resultMoves.size(); i++) {
	    TTAProgram::Instruction* instr = resultMoves[i];
	    auto m = instr->movePtr(0);
	    proc->add(instr);
	    createMoveNode(po, m, false);
	}
    }
    return firstIns;
}

/**
 * Checks if machineOperand is a vector register.
 * 
 * @TODO: is there some llvm way of doing this?
 */
int LLVMTCEBuilder::vectorOperandSize(const MachineOperand& mo) {
    if (!mo.isReg()) {
        return 1;
    }
    unsigned int dRegNum = mo.getReg();
    if (dRegNum == raPortDRegNum()) {
	return 1;
    }
    
    std::string origRfName = registerFileName(dRegNum);
    if (origRfName.length() < 8) {
	return 1;
    }
    if (origRfName.substr(0,8) == "_VECTOR_") {
        size_t sizeEndIndex = origRfName.find('_', 8);
        if (sizeEndIndex != std::string::npos) {
            return Conversion::toInt(
                origRfName.substr(8, (sizeEndIndex-8)));
        } else {
            return 1;
        }
    } else {
        return 1;
    }

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
std::shared_ptr<TTAProgram::Move>
LLVMTCEBuilder::createMove(
    TTAProgram::Terminal* src,
    TTAProgram::Terminal* dst,
    const TTAMachine::Bus& bus,
    TTAProgram::MoveGuard* guard) {

    std::shared_ptr<TTAProgram::Move> move = NULL;

    bool endRef = false;

    if (src == NULL) {
        // Create a dummy source Terminal so the move can be added to an
        // instruction.
        SimValue val(0, 32);
        src = new TTAProgram::TerminalImmediate(val);
        endRef = true;
    }

    if (guard == NULL) {
        move = std::make_shared<TTAProgram::Move>(src, dst, bus);
    } else {
        move = std::make_shared<TTAProgram::Move>(src, dst, bus, guard);
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
LLVMTCEBuilder::result() {
    return prog_;
}

/**
 * Creates a register guard to given guard register.
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
    std::cerr << "Warning: Could not find suitable guard from any bus in the"
              << "processor. Did you forget to add guards to the processor?"
              << std::endl;
    return NULL;
}

TTAMachine::AddressSpace&
LLVMTCEBuilder::addressSpaceById(unsigned id) {

    if (!multiDataMemMachine_)
        return *defaultDataAddressSpace_;

    const TTAMachine::Machine::AddressSpaceNavigator asNav =
        mach_->addressSpaceNavigator();
    for (int i = 0; i < asNav.count(); i++) {
        TTAMachine::AddressSpace& aSpace = *asNav.item(i);
        if (aSpace.hasNumericalId(id))
            return aSpace;
    }
    Application::logStream()
        << "Address space with numerical id " << id << " not found." 
        << std::endl;
    abort();
}


/**
 * Returns the position after the highest written data symbol for the
 * given address space.
 */
unsigned&
LLVMTCEBuilder::dataEnd(TTAMachine::AddressSpace& aSpace) {
    if (!MapTools::containsKey(dataEnds_, &aSpace)) {
        unsigned end = aSpace.start();
        /* Avoid placing data to address 0, as it may break some null pointer
           tests. Waste a valuable word of memory. Add a dummy word to prevent 
           writing bytes to 1,2,3 addresses and thus then reading valid data 
           from 0. */
        if (end == 0) {
            const TCETargetMachine* tm = 
                dynamic_cast<const TCETargetMachine*>(tm_);
            assert(tm != NULL);
            end = tm->getMaxMemoryAlignment();
        }
        dataEnds_[&aSpace] = end;

    }
    return dataEnds_[&aSpace];   
}

/**
 * Returns the "layout array" for the data memory of the given address
 * space.
 */
TTAProgram::DataMemory&
LLVMTCEBuilder::dataMemoryForAddressSpace(TTAMachine::AddressSpace& aSpace) {
    if (!MapTools::containsKey(dmemIndex_, &aSpace)) {
        dmemIndex_[&aSpace] = new TTAProgram::DataMemory(aSpace);
    }
    return *dmemIndex_[&aSpace];
}

/**
 * Adds annotations to the given move that limit the choice of the
 * load-store unit to only those that support the given address space.
 */
void 
LLVMTCEBuilder::addCandidateLSUAnnotations(
    unsigned asNum, TTAProgram::Move& move) {

    bool foundLSU = false;
    const TTAMachine::Machine::FunctionUnitNavigator fuNav =
        mach_->functionUnitNavigator();
    for (int i = 0; i < fuNav.count(); i++) {
        const TTAMachine::FunctionUnit& fu = *fuNav.item(i);
        if (fu.hasAddressSpace()) {
            if (fu.addressSpace()->hasNumericalId(asNum)) {
                TTAProgram::ProgramAnnotation progAnnotation(
                    TTAProgram::ProgramAnnotation::
                    ANN_ALLOWED_UNIT_DST, fu.name());
                move.addAnnotation(progAnnotation);
                foundLSU = true;
            }
        }
    }
    if (!foundLSU) {
        Application::logStream()
            << "ERROR: no candidate LSU found for address space id " 
            << asNum << std::endl;
        abort();
    }
}
