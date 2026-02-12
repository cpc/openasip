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
 * @file LowerMissingInstructions.cc
 *
 * Convert instruction which are not supported by the machine to
 * function calls.
 *
 * We perform this because there is no binary linker in the compiler chain
 * which would be required by the regular libcall conversion in the LLVM
 * code generation.
 *
 * NOTE: Right now system is limited to replace only those operations, which
 *       use only one bitwidth integers for example i1.icmp.i32.i32 cannot
 *       be lowered to function call.
 *       However i16.mul.i16.i16 works.
 *
 *       Maybe better way would be to take llvm footprints which must be
 *       emulated and create function prototype directly for them.
 * @author Mikael Lepisto 2008-2009 (mikael.lepisto-no.spam-tut.fi)
 * @author Pekka Jaaskelainen 2010
 * @note reting: red
 */

#include "CompilerWarnings.hh"
IGNORE_COMPILER_WARNING("-Wunused-parameter")

#define DEBUG_TYPE "lowermissing"

#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils/UnifyFunctionExitNodes.h"
#include "tce_config.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/Compiler.h"
#include "llvm/IR/AbstractCallSite.h"
#include "llvm/IR/InstrTypes.h" // CreateIntegerCast()
#include "llvm/ADT/Twine.h"

#include "llvm/ADT/STLExtras.h" // array_endof
#include "llvm/Support/CommandLine.h" // cl::opt


// TCE headers
// tce_config.h defines these. this undef to avoid warning.
// TODO: how to do this in tce_config.h???
#ifdef LLVM_LIBDIR
#undef LLVM_LIBDIR
#endif

#include "Machine.hh"
#include "OperationDAGSelector.hh"
#include "MachineInfo.hh"
#include "Operand.hh"
#include "Operation.hh"
#include "OperationPool.hh"
#include "TCEString.hh"
using TTAMachine::Machine;

#include "LLVMBackend.hh" // llvmRequiredOps()

POP_COMPILER_DIAGS

using namespace llvm;

STATISTIC(NumLowered, "Number of instructions lowered");

#include <iostream>

#define ARGLIST_CONST
#define TYPE_CONST

namespace {
    class LowerMissingInstructions : public FunctionPass {
        std::map< std::string, Function*> replaceFunctions;
        const TTAMachine::Machine* mach_;
        Module* dstModule_;
        
    public:
        static char ID; // Pass ID, replacement for typeid       
        LowerMissingInstructions(const TTAMachine::Machine& mach);

        
        // from llvm::Pass:
        bool doInitialization(Module &M) override;
        bool doFinalization (Module &M) override;

        // to suppress Clang warnings
        using llvm::FunctionPass::doInitialization;
        using llvm::FunctionPass::doFinalization;

        bool runOnBasicBlock(BasicBlock &BB);
        bool runOnFunction(Function &F);

    virtual StringRef getPassName() const override {
            return "TCE: LowerMissingInstructions";
        }

        void addFunctionForFootprints(
            Module& M, FunctionType* fType, Operation& op, 
            std::string suffix);

    private:
        std::string stringType(const Type* type) const;

        ARGLIST_CONST Type* getLLVMType(
            Operand::OperandType type, ARGLIST_CONST Type* llvmIntegerType);

        std::string getFootprint(Instruction& I);

        // getGlobalContext() was removed form LLVM.
        LLVMContext& getGlobalContext() const {
            return dstModule_->getContext();
        }
    };

    char LowerMissingInstructions::ID = 0;    

// When we got another way than parameter 
// for passing machine we can register pass to manager
//    RegisterPass<LowerMissingInstructions>
//    X("lowermissing", "Lower missing instructions to libcalls");
}

// - Interface to this file...
Pass* createLowerMissingInstructionsPass(const TTAMachine::Machine& mach) {
    return new LowerMissingInstructions(mach);
}

LowerMissingInstructions::LowerMissingInstructions(
    const TTAMachine::Machine& mach) : 
    FunctionPass(ID),
    mach_(&mach) {
}

// convert type name to string
std::string 
LowerMissingInstructions::stringType(const Type* type) const {
    LLVMContext& context = getGlobalContext();
    if (type == Type::getInt64Ty(context)) {
        return "i64";
    } else if (type == Type::getInt32Ty(context)) {
        return "i32";
    }  else if (type == Type::getInt16Ty(context)) {
        return "i16";
    }  else if (type == Type::getInt8Ty(context)) {
        return "i8";
    }  else if (type == Type::getInt1Ty(context)) {
        return "i1";
    }  else if (type == Type::getHalfTy(context)) {
        return "f16";
    }  else if (type == Type::getFloatTy(context)) {
        return "f32";
    }  else if (type == Type::getDoubleTy(context)) {
        return "f64";
    }  else if (type == Type::getLabelTy(context)) {
        return "label";
    }  else if (type == Type::getVoidTy(context)) {
        return "void";
    } else {
        return "unknown";
    }
}

ARGLIST_CONST Type* 
LowerMissingInstructions::getLLVMType(
    Operand::OperandType type, ARGLIST_CONST Type* llvmIntegerType) {
    switch (type) {
    case Operand::SINT_WORD:
        return llvmIntegerType;
    case Operand::UINT_WORD:
        return llvmIntegerType;
    case Operand::SLONG_WORD:
        return Type::getInt64Ty(getGlobalContext());
    case Operand::ULONG_WORD:
        return Type::getInt64Ty(getGlobalContext());
    case Operand::FLOAT_WORD:
        return Type::getFloatTy(getGlobalContext());
    case Operand::HALF_FLOAT_WORD:
        return Type::getHalfTy(getGlobalContext());
    case Operand::DOUBLE_WORD:
        return Type::getDoubleTy(getGlobalContext());
    default:
        return Type::getVoidTy(getGlobalContext());
    }
}

const std::vector<std::string>& llvmFootprints(std::string tceOp) {
    static bool init = true;
    static std::map<std::string, std::vector<std::string> > footprints;
    
    // NOTE: .i32 is default footprint suffix if there is no i16, i8 or i1
    //       versions.. e.g. .i32 is suffix for f32.fadd.f32.f32
    if (init) {
//         footprints["DIVU.i32"].push_back("i32.udiv.i32.i32");
//         footprints["DIVU.i16"].push_back("i16.udiv.i16.i16");
//         footprints["DIVU.i8"].push_back("i8.udiv.i8.i8");

//         footprints["DIV.i32"].push_back("i32.sdiv.i32.i32");
//         footprints["DIV.i16"].push_back("i16.sdiv.i16.i16");
//         footprints["DIV.i8"].push_back("i8.sdiv.i8.i8");

//         footprints["MODU.i32"].push_back("i32.urem.i32.i32");
//         footprints["MODU.i16"].push_back("i16.urem.i16.i16");
//         footprints["MODU.i8"].push_back("i8.urem.i8.i8");

//         footprints["MOD.i32"].push_back("i32.rem.i32.i32");
//         footprints["MOD.i16"].push_back("i16.rem.i16.i16");
//         footprints["MOD.i8"].push_back("i8.rem.i8.i8");
        
//         footprints["MUL.i32"].push_back("i32.mul.i32.i32");
//         footprints["MUL.i16"].push_back("i16.mul.i16.i16");
//         footprints["MUL.i8"].push_back("i8.mul.i8.i8");
        footprints["ADDH.i32"].push_back("f16.fadd.f16.f16");
        footprints["SUBH.i32"].push_back("f16.fsub.f16.f16");
        footprints["NEGH.i32"].push_back("f16.fneg.f16");
        footprints["MULH.i32"].push_back("f16.fmul.f16.f16");
        footprints["DIVH.i32"].push_back("f16.fdiv.f16.f16");
        footprints["SQRTH.i32"].push_back("f16.sqrt.f16");

        footprints["ADDF.i32"].push_back("f32.fadd.f32.f32");
        footprints["SUBF.i32"].push_back("f32.fsub.f32.f32");
        footprints["NEGF.i32"].push_back("f32.fneg.f32");
        footprints["MULF.i32"].push_back("f32.fmul.f32.f32");
        footprints["DIVF.i32"].push_back("f32.fdiv.f32.f32");
        footprints["SQRTF.i32"].push_back("f32.sqrt.f32");

        footprints["ADDD.i64"].push_back("f64.fadd.f64.f64");
        footprints["SUBD.i64"].push_back("f64.fsub.f64.f64");
        footprints["NEGD.i64"].push_back("f64.fneg.f64");
        footprints["MULD.i64"].push_back("f64.fmul.f64.f64");
        footprints["DIVD.i64"].push_back("f64.fdiv.f64.f64");
        footprints["SQRTD.i64"].push_back("f64.sqrt.f64");

        footprints["CFI.i32"].push_back("i32.fptosi.f32");
        footprints["CFIU.i32"].push_back("i32.fptoui.f32");
        footprints["CIF.i32"].push_back("f32.sitofp.i32");
        footprints["CIFU.i32"].push_back("f32.uitofp.i32");

        footprints["CFI.i16"].push_back("i16.fptosi.f32");
        footprints["CFIU.i16"].push_back("i16.fptoui.f32");
        footprints["CIF.i16"].push_back("f32.sitofp.i16");
        footprints["CIFU.i16"].push_back("f32.uitofp.i16");

        footprints["CFI.i8"].push_back("i8.fptosi.f32");
        footprints["CFIU.i8"].push_back("i8.fptoui.f32");
        footprints["CIF.i8"].push_back("f32.sitofp.i8");
        footprints["CIFU.i8"].push_back("f32.uitofp.i8");

        footprints["CDL.i64"].push_back("i64.fptosi.f64");
        footprints["CDLU.i64"].push_back("i64.fptoui.f64");
        footprints["CLD.i64"].push_back("f64.sitofp.i64");
        footprints["CLDU.i64"].push_back("f64.uitofp.i64");

        footprints["CDL.i32"].push_back("i32.fptosi.f64");
        footprints["CDLU.i32"].push_back("i32.fptoui.f64");
        footprints["CLD.i32"].push_back("f64.sitofp.i32");
        footprints["CLDU.i32"].push_back("f64.uitofp.i32");

        footprints["CDL.i16"].push_back("i16.fptosi.f64");
        footprints["CDLU.i16"].push_back("i16.fptoui.f64");
        footprints["CLD.i16"].push_back("f64.sitofp.i16");
        footprints["CLDU.i16"].push_back("f64.uitofp.i16");

        footprints["CDL.i8"].push_back("i8.fptosi.f64");
        footprints["CDLU.i8"].push_back("i8.fptoui.f64");
        footprints["CLD.i8"].push_back("f64.sitofp.i8");
        footprints["CLDU.i8"].push_back("f64.uitofp.i8");

        footprints["EQF.i1"].push_back("i1.fcmp.oeq.f32.f32");
        footprints["NEF.i1"].push_back("i1.fcmp.one.f32.f32");
        footprints["LTF.i1"].push_back("i1.fcmp.olt.f32.f32");
        footprints["LEF.i1"].push_back("i1.fcmp.ole.f32.f32");
        footprints["GTF.i1"].push_back("i1.fcmp.ogt.f32.f32");
        footprints["GEF.i1"].push_back("i1.fcmp.oge.f32.f32");
        
        footprints["EQUF.i1"].push_back("i1.fcmp.ueq.f32.f32");
        footprints["NEUF.i1"].push_back("i1.fcmp.une.f32.f32");
        footprints["LTUF.i1"].push_back("i1.fcmp.ult.f32.f32");
        footprints["LEUF.i1"].push_back("i1.fcmp.ule.f32.f32");
        footprints["GTUF.i1"].push_back("i1.fcmp.ugt.f32.f32");
        footprints["GEUF.i1"].push_back("i1.fcmp.uge.f32.f32");

        footprints["EQF.i32"].push_back("i32.fcmp.oeq.f32.f32");
        footprints["NEF.i32"].push_back("i32.fcmp.one.f32.f32");
        footprints["LTF.i32"].push_back("i32.fcmp.olt.f32.f32");
        footprints["LEF.i32"].push_back("i32.fcmp.ole.f32.f32");
        footprints["GTF.i32"].push_back("i32.fcmp.ogt.f32.f32");
        footprints["GEF.i32"].push_back("i32.fcmp.oge.f32.f32");
        
        footprints["EQUF.i32"].push_back("i32.fcmp.ueq.f32.f32");
        footprints["NEUF.i32"].push_back("i32.fcmp.une.f32.f32");
        footprints["LTUF.i32"].push_back("i32.fcmp.ult.f32.f32");
        footprints["LEUF.i32"].push_back("i32.fcmp.ule.f32.f32");
        footprints["GTUF.i32"].push_back("i32.fcmp.ugt.f32.f32");
        footprints["GEUF.i32"].push_back("i32.fcmp.uge.f32.f32");

        footprints["ORDF.i1"].push_back("i1.fcmp.ord.f32.f32");
        footprints["ORDF.i32"].push_back("i32.fcmp.ord.f32.f32");
        footprints["UORDF.i1"].push_back("i1.fcmp.uno.f32.f32");
        footprints["UORDF.i32"].push_back("i32.fcmp.uno.f32.f32");

        footprints["CFD.i64"].push_back("f64.fpext.f32");

        footprints["EQD.i1"].push_back("i1.fcmp.oeq.f64.f64");
        footprints["NED.i1"].push_back("i1.fcmp.one.f64.f64");
        footprints["LTD.i1"].push_back("i1.fcmp.olt.f64.f64");
        footprints["GED.i1"].push_back("i1.fcmp.oge.f64.f64");
        footprints["LED.i1"].push_back("i1.fcmp.ole.f64.f64");
        footprints["GTD.i1"].push_back("i1.fcmp.ogt.f64.f64");

        footprints["EQD.i64"].push_back("i64.fcmp.oeq.f64.f64");
        footprints["NED.i64"].push_back("i64.fcmp.one.f64.f64");
        footprints["LTD.i64"].push_back("i64.fcmp.olt.f64.f64");
        footprints["GED.i64"].push_back("i64.fcmp.oge.f64.f64");
        footprints["LED.i64"].push_back("i64.fcmp.ole.f64.f64");
        footprints["GTD.i64"].push_back("i64.fcmp.ogt.f64.f64");

        footprints["EQUD.i1"].push_back("i1.fcmp.ueq.f64.f64");
        footprints["NEUD.i1"].push_back("i1.fcmp.une.f64.f64");
        footprints["GEUD.i1"].push_back("i1.fcmp.uge.f64.f64");
        footprints["LEUD.i1"].push_back("i1.fcmp.ule.f64.f64");
        footprints["LTUD.i1"].push_back("i1.fcmp.ult.f64.f64");
        footprints["GTUD.i1"].push_back("i1.fcmp.ugt.f64.f64");

        footprints["EQUD.i64"].push_back("i64.fcmp.ueq.f64.f64");
        footprints["NEUD.i64"].push_back("i64.fcmp.une.f64.f64");
        footprints["LEUD.i64"].push_back("i64.fcmp.ule.f64.f64");
        footprints["GEUD.i64"].push_back("i64.fcmp.uge.f64.f64");
        footprints["LTUD.i64"].push_back("i64.fcmp.ult.f64.f64");
        footprints["GTUD.i64"].push_back("i64.fcmp.ugt.f64.f64");

        footprints["ORDD.i1"].push_back("i1.fcmp.ord.f64.f64");
        footprints["ORDD.i64"].push_back("i64.fcmp.ord.f64.f64");
        footprints["UORDD.i1"].push_back("i1.fcmp.uno.f64.f64");
        footprints["UORDD.i64"].push_back("i64.fcmp.uno.f64.f64");

        init = false;                
    }

    return footprints[tceOp];
}

std::string 
LowerMissingInstructions::getFootprint(Instruction& I) {
    
    std::string footPrint = stringType(I.getType());

    switch (I.getOpcode()) {

    case Instruction::FCmp: {
        FCmpInst* cmpInst = dyn_cast<FCmpInst>(&I);
        footPrint += std::string(".") + cmpInst->getOpcodeName() + ".";
        
        switch (cmpInst->getPredicate()) {
        case FCmpInst::FCMP_FALSE:
            footPrint += "false";
            break;
        case FCmpInst::FCMP_OEQ:
            footPrint += "oeq";
            break;
        case FCmpInst::FCMP_OGT:
            footPrint += "ogt";
            break;
        case FCmpInst::FCMP_OGE:
            footPrint += "oge";
            break;
        case FCmpInst::FCMP_OLT:
            footPrint += "olt";
            break;
        case FCmpInst::FCMP_OLE:
            footPrint += "ole";
            break;
        case FCmpInst::FCMP_ONE:
            footPrint += "one";
            break;
        case FCmpInst::FCMP_ORD:
            footPrint += "ord";
            break;
        case FCmpInst::FCMP_UNO:
            footPrint += "uno";
            break;
        case FCmpInst::FCMP_UEQ:
            footPrint += "ueq";
            break;
        case FCmpInst::FCMP_UGT:
            footPrint += "ugt";
            break;
        case FCmpInst::FCMP_UGE:
            footPrint += "uge";
            break;
        case FCmpInst::FCMP_ULT:
            footPrint += "ult";
            break;
        case FCmpInst::FCMP_ULE:
            footPrint += "ule";
            break;
        case FCmpInst::FCMP_UNE:
            footPrint += "une";
            break;
        case FCmpInst::FCMP_TRUE:
            footPrint += "true";
            break;
        default:
            footPrint += "PREDFAIL";
        }
    } break;

    case Instruction::Call: {
        if (!isa<CallInst>(&I) ||
            dyn_cast<CallInst>(&I)->getCalledFunction() == NULL)
            break;
        std::string calledName = 
            dyn_cast<CallInst>(&I)->getCalledFunction()->getName().str();
        if (calledName == "llvm.sqrt.f32") {
            return "f32.sqrt.f32";
        }
        
    } break;
    
    default:
        footPrint += std::string(".") + I.getOpcodeName();
    }

    for (unsigned int i = 0; i < I.getNumOperands();i++) {
        footPrint += "." + stringType(I.getOperand(i)->getType());
    }
    
    return footPrint;
}

void LowerMissingInstructions::addFunctionForFootprints(
    Module& M, FunctionType* /*fType*/, Operation& op, std::string suffix) {

    // set replace footprints for operations to emulate
    // (there might be multiple different footprints for the same
    //  emulation function)
    const std::vector<std::string> footprints = llvmFootprints(op.name() + suffix);

    for (unsigned int j = 0; j < footprints.size(); j++) {
        Function* func = M.getFunction(op.emulationFunctionName());
        replaceFunctions[footprints[j]] = func;

#if 0
        std::cerr << "Operation: " << op.name()
                  << " is emulated with: " << op.emulationFunctionName() 
                  << " footprint: " << footprints[j]
                  << std::endl;
#endif
    }
}

bool LowerMissingInstructions::doInitialization(Module &M) {        

    if (Application::verboseLevel() > Application::VERBOSE_LEVEL_DEFAULT) {
        Application::logStream()
            << std::endl
            << "---- LowerMissingInstructions ----"
            << std::endl;
    }
    
    dstModule_ = &M;    

    bool retVal = true;

    OperationDAGSelector::OperationSet 
        opSet = MachineInfo::getOpset(*mach_);
    
    OperationDAGSelector::OperationSet
        requiredSet = LLVMBackend::llvmRequiredOpset(true, mach_->isLittleEndian(), mach_->is64bit());
    
    OperationPool osal;
    
    // Check required set, which must be lowered to function calls..
    for (OperationDAGSelector::OperationSet::iterator i = requiredSet.begin();
         i != requiredSet.end(); i++) {
        
        if (opSet.find(*i) == opSet.end() && 
            OperationDAGSelector::findDags(*i, opSet).empty()) {

            Operation& op = osal.operation((*i).c_str());
            
            if (&op == &NullOperation::instance()) {
                std::cerr << "Error: Cant find operation: " << *i 
                          << " from OSAL"
                          << std::endl;

                return false;                
            }

            if (op.numberOfOutputs() != 1) {
                if (Application::verboseLevel() > 0) {
                    Application::errorStream() << "Cannot lower missing instruction:" 
                                               <<  *i << std::endl;
                }
                continue;
            }
	    
            // Make parameter list for operation with all needed integer 
            // widths. If pure floating point just i32 is used. 
            // 
            // If there is also 
            // IntWord or UIntWord parameters all vectors are filled.
            std::vector<ARGLIST_CONST Type*> argList_i64;
            TYPE_CONST Type* retVal_i64 = NULL;

            std::vector<ARGLIST_CONST Type*> argList_i32;
            TYPE_CONST Type* retVal_i32 = NULL;

            std::vector<ARGLIST_CONST Type*> argList_i16;
            TYPE_CONST Type* retVal_i16 = NULL;

            std::vector<ARGLIST_CONST Type*> argList_i8;
            TYPE_CONST Type* retVal_i8 = NULL;

            std::vector<ARGLIST_CONST Type*> argList_i1;
            TYPE_CONST Type* retVal_i1 = NULL;

            TYPE_CONST Type* defaultIntegerType =
                mach_->is64bit() ?
                Type::getInt64Ty(getGlobalContext()) :
                Type::getInt32Ty(getGlobalContext());

            std::vector<ARGLIST_CONST Type*>& argList_default =
                mach_->is64bit() ? argList_i64 : argList_i32;

            TYPE_CONST Type*& retVal_default =
                mach_->is64bit() ? retVal_i64 : retVal_i32;

            const char* defaultIntegerSuffix =
                mach_->is64bit() ? ".i64" : ".i32";
            
            bool useInt = false;
            for (int j = 1; j <= op.numberOfInputs(); j++) { 
                Operand& operand = op.operand(j);
                ARGLIST_CONST Type* llvmOp = getLLVMType(
                    operand.type(), //Type::getInt32Ty(getGlobalContext()));
                    defaultIntegerType);

                argList_default.push_back(llvmOp);

                if (llvmOp == defaultIntegerType) {
                    useInt = true;
                }               
            }
            
            Operand& outputOperand = op.operand(op.numberOfInputs() + 1);
            retVal_default = getLLVMType(outputOperand.type(), defaultIntegerType);

            // TODO: should also create smaller bit widths for long type operands??
            if (retVal_default == defaultIntegerType) { //Type::getInt32Ty(getGlobalContext())) {
                useInt = true;
            }

            FunctionType* fType_default =
                FunctionType::get(retVal_default, argList_default, false);

            if (retVal_default == Type::getInt32Ty(getGlobalContext()) ||
                retVal_default == Type::getFloatTy(getGlobalContext())) {

                FunctionType* fType_i32 =
                    FunctionType::get(Type::getInt32Ty(getGlobalContext()), argList_i32, false);
                addFunctionForFootprints(M, fType_i32, op, ".i32"); //".i32");
            } else {
                addFunctionForFootprints(M, fType_default, op, defaultIntegerSuffix); //".i32");
            }

            // create other function protos for other integer bitwidths
            if (useInt) {
                if (retVal_default == defaultIntegerType) {
                    if (mach_->is64bit()) {
                        retVal_i32 = Type::getInt32Ty(getGlobalContext());
                    }
                    retVal_i16 = Type::getInt16Ty(getGlobalContext());
                    retVal_i8 = Type::getInt8Ty(getGlobalContext());
                    retVal_i1 = Type::getInt1Ty(getGlobalContext());
                } else {
                    if (mach_->is64bit()) {
                        retVal_i32 = retVal_default;
                    }
                    retVal_i16 = retVal_default;
                    retVal_i8 = retVal_default;
                    retVal_i1 = retVal_default;
                }

                for (unsigned int j = 0; j < argList_default.size(); j++) {
                    ARGLIST_CONST Type* currArg = argList_default[j];
                    if (currArg == defaultIntegerType) {
                        if (mach_->is64bit()) {
                            argList_i32.push_back(Type::getInt32Ty(getGlobalContext()));
                        }
                        argList_i16.push_back(Type::getInt16Ty(getGlobalContext()));
                        argList_i8.push_back(Type::getInt8Ty(getGlobalContext()));
                        argList_i1.push_back(Type::getInt1Ty(getGlobalContext()));
                    } else {
                        if (mach_->is64bit()) {
                            argList_i16.push_back(currArg);
                        }
                        argList_i16.push_back(currArg);
                        argList_i8.push_back(currArg);
                        argList_i1.push_back(currArg);
                    }
                }

                if (mach_->is64bit()) {
                    FunctionType* fType_i32 =
                        FunctionType::get(retVal_i32, argList_i32, false);
                    addFunctionForFootprints(M, fType_i32, op, ".i32");
                }

                FunctionType* fType_i16 = 
                    FunctionType::get(retVal_i16, argList_i16, false);
                
                FunctionType* fType_i8 = 
                    FunctionType::get(retVal_i8, argList_i8, false);
                
                FunctionType* fType_i1 = 
                    FunctionType::get(retVal_i1, argList_i1, false);
                
                addFunctionForFootprints(M, fType_i16, op, ".i16");
                addFunctionForFootprints(M, fType_i8, op, ".i8");
                addFunctionForFootprints(M, fType_i1, op, ".i1");
                
            }           
        }
    }

    return retVal;
}

bool LowerMissingInstructions::doFinalization(Module& /* M */) {
    if (Application::verboseLevel() > Application::VERBOSE_LEVEL_DEFAULT) {
        Application::logStream()
            << std::endl
            << "---- DONE: LowerMissingInstructions ----"
            << std::endl;
    }
    return true;
}

// runOnBasicBlock - This method does the actual work of converting
// instructions over, assuming that the pass has already been initialized.
//
bool LowerMissingInstructions::runOnBasicBlock(BasicBlock &BB) {
    bool Changed = false;
    #ifdef LLVM_OLDER_THAN_16
    BasicBlock::InstListType &BBIL = BB.getInstList();
    #endif

    // Loop over all of the instructions, looking for instructions to lower
    // instructions
    for (BasicBlock::iterator I = BB.begin(), E = BB.end(); I != E; ++I) {
        
        // get footprint of instruction
        std::string footPrint = getFootprint(*I);
        
        std::map<std::string, Function*>::iterator
            replaceFunc =  replaceFunctions.find(footPrint);        
 
        // std::cerr << "Footprint: " << footPrint << "\n";

        if (replaceFunc != replaceFunctions.end()) {
            if (replaceFunc->second == NULL) {
                // this should leak down to llvm-tce
                std::cerr
                    << (boost::format(
                            "ERROR: emulation function for footprint"
                            "'%s' wasn't found. Floating point"
                            " emulation required but --swfp was not given?") % 
                        footPrint).str() << std::endl;
                /* TODO: we have to exit() here as the Exception does not
                   propagate down to the llvm-tce with all distributions.
                   Should fail more gracefully as this is library code. */
                exit(1);            
            }
            if (Application::verboseLevel() >
                Application::VERBOSE_LEVEL_DEFAULT) {
                Application::logStream()
                    << "Replacing: " << footPrint
                    << " with emulation function." << std::endl;
            }
            std::vector<Value*> args;            

            for (unsigned j = 0; j < I->getNumOperands(); j++) {

                if (I->getOperand(j)->getType() == Type::getInt16Ty(getGlobalContext()) ||
                    I->getOperand(j)->getType() == Type::getInt8Ty(getGlobalContext()) ||
                    (I->getOperand(j)->getType() == Type::getInt32Ty(getGlobalContext()) &&
                     mach_->is64bit())) {

                    // Emulated operations with i1/i8/i16 operands need
                    // their operands extended to 32 bits. However, there's
                    // no easy way to see if the llvm operand requires
                    // sign or zero extension, so the correct extension is
                    // currently determined directly from the footprint.
                    if (footPrint == "f32.sitofp.i16" ||
                        footPrint == "f32.sitofp.i8") {

                        // sign extension needed
                        args.push_back(llvm::CastInst::CreateIntegerCast(
                            I->getOperand(j),
                            Type::getInt32Ty(getGlobalContext()), true, "",
                            I));
                    } else if (footPrint == "f32.uitofp.i16" ||
                               footPrint == "f32.uitofp.i8") {
                        // zero extension needed
                        args.push_back(llvm::CastInst::CreateIntegerCast(
                            I->getOperand(j),
                            Type::getInt32Ty(getGlobalContext()), false, "",
                            I));
                    } else if (footPrint == "f64.sitofp.i32" && mach_->is64bit()) {
                        args.push_back(llvm::CastInst::CreateIntegerCast(
                            I->getOperand(j),
                            Type::getInt64Ty(getGlobalContext()), true, "",
                            I));

                    } else if (footPrint == "f64.uitofp.i32" && mach_->is64bit()) {
                        args.push_back(llvm::CastInst::CreateIntegerCast(
                            I->getOperand(j),
                            Type::getInt64Ty(getGlobalContext()), false, "",
                            I));
                    } else {
                        // might not need ext after all.
                        args.push_back(I->getOperand(j));
                    }
                } else if (I->getOpcode() == llvm::Instruction::Call
                           && j == 0) {
                    // the first operand of a Call is the called function pointer, 
                    // ignore it
                    continue;
                } else {
                    args.push_back(I->getOperand(j));
                }
            }
            CallInst* NewCall = CallInst::Create(
                FunctionCallee(replaceFunc->second), args, Twine(""), I);
            NewCall->setTailCall();

            // Replace all uses of the instruction with call instruction
            if (I->getType() != NewCall->getType()) {

                Value* MCast;
                Instruction::CastOps castOps =
                    llvm::CastInst::getCastOpcode(
                        NewCall, false, I->getType(), false);
                MCast = llvm::CastInst::Create(
                    castOps, NewCall, I->getType(), Twine(""), I);
                I->replaceAllUsesWith(MCast);

            } else {
                I->replaceAllUsesWith(NewCall);
            }

#ifdef LLVM_OLDER_THAN_16
            I = --BBIL.erase(I);
            #else
            BB.erase(I, I);
            #endif
            Changed = true;
            
            NumLowered++;
        } 
    }
    return Changed;
}

bool
LowerMissingInstructions::runOnFunction(Function &F) {
    for (BasicBlock &BB : F) {
        runOnBasicBlock(BB);
    }
    return true;
}
