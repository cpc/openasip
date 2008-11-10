/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
//===- LowerMissingInstructions.cpp - 
//
// Convert instruction which are not supported by the machine to
// function calls.
//
// NOTE: Right now system is limited to replace only those operations, which
//       use only one bitwidth integers for example i1.icmp.i32.i32 cannot
//       be lowered to function call.
//       However i16.mul.i16.i16 works.
//
//       Maybe better way would be to take llvm footprints which must be
//       emulated and create function prototype directly for them.
//
//===----------------------------------------------------------------------===

#define DEBUG_TYPE "lowermissing"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils/UnifyFunctionExitNodes.h"
#include "llvm/Module.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Instructions.h"
#include "llvm/Constants.h"
// #include "llvm/Pass.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Support/Compiler.h"

// #include "llvm/ParameterAttributes.h"
#include "llvm/Support/CallSite.h"
#include "llvm/ADT/STLExtras.h" // array_endof
#include "llvm/Support/CommandLine.h" // cl::opt

// Stuff needed by linker feature
#include "llvm/Linker.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/System/Path.h"

// TCE headers
#include "Machine.hh"
#include "OperationDAGSelector.hh"
#include "MachineInfo.hh"
#include "Operand.hh"
#include "OperationPool.hh"
#include "TCEString.hh"
using TTAMachine::Machine;

using namespace llvm;

STATISTIC(NumLowered, "Number of instructions lowered");

#include <iostream>

namespace {
    // ADF file which for optimization is done
    cl::opt<std::string>
    ADFFile("adf-file", cl::value_desc("filename"),
            cl::desc("A machine which for program is optimized"));

    cl::opt<std::string>
    EmulationFunctionsFile("emulation-lib", cl::value_desc("filename"),
                           cl::desc("Bytecode functions for emulating "
                                    "missing instructions. If not given, "
                                    "pass will assume that needed functions "
                                    "are already found from program. NOTE: "
                                    "right now switch does not have any effect"));
    
    /// LowerAllocations - Turn malloc and free instructions into %malloc and
    /// %free calls.
    ///
    class VISIBILITY_HIDDEN LowerMissingInstructions : public BasicBlockPass {
        std::map< std::string, Constant*> replaceFunctions;        
        const TTAMachine::Machine* mach_;
        Module* dstModule_;
        
    public:
        static char ID; // Pass ID, replacement for typeid       
        LowerMissingInstructions(const TTAMachine::Machine& mach);

        virtual void getAnalysisUsage(AnalysisUsage &AU) const {
            AU.addRequired<TargetData>();
            AU.setPreservesCFG();

            // This is a cluster of orthogonal Transforms:
            AU.addPreserved<UnifyFunctionExitNodes>();
            AU.addPreservedID(PromoteMemoryToRegisterID);
            //in LLVM 2.2:
            //AU.addPreservedID(LowerSelectID);
            AU.addPreservedID(LowerSwitchID);
            AU.addPreservedID(LowerInvokePassID);
        }

        /// doPassInitialization - For the lower allocations pass, this 
        /// ensures that a module contains a declaration for a malloc and 
        /// a free function.
        ///
        bool doInitialization(Module &M);
        bool doFinalization (Module &M);

        virtual bool doInitialization(Function &F) {
            return BasicBlockPass::doInitialization(F);
        }

        /// runOnBasicBlock - This method does the actual work of converting
        /// instructions over, assuming that the pass has already been 
        /// initialized.
        ///
        bool runOnBasicBlock(BasicBlock &BB);

        void addFunctionForFootprints(
            Module& M, FunctionType* fType, Operation& op, 
            std::string suffix);
    };

    char LowerMissingInstructions::ID = 0;    
//    RegisterPass<LowerMissingInstructions>
//    X("lowermissing", "Lower missing instructions to libcalls");
}

// Publically exposed interface to pass...
// const PassInfo* LowerMissingInstructionsID = X.getPassInfo();

// - Interface to this file...
Pass* createLowerMissingInstructionsPass(const TTAMachine::Machine& mach) {
    return new LowerMissingInstructions(mach);
}

LowerMissingInstructions::LowerMissingInstructions(const TTAMachine::Machine& mach) : 
    BasicBlockPass((intptr_t)&ID), mach_(&mach) {
}

// convert type name to string
std::string stringType(const Type* type) {
    if (type == Type::Int64Ty) {
        return "i64";
    } else if (type == Type::Int32Ty) {
        return "i32";
    }  else if (type == Type::Int16Ty) {
        return "i16";
    }  else if (type == Type::Int8Ty) {
        return "i8";
    }  else if (type == Type::Int1Ty) {
        return "i1";
    }  else if (type == Type::FloatTy) {
        return "f32";
    }  else if (type == Type::DoubleTy) {
        return "f64";
    }  else if (type == Type::LabelTy) {
        return "label";
    }  else if (type == Type::VoidTy) {
        return "void";
    } else {
        return "unknown";
    }
}

const Type* getLLVMType(
    Operand::OperandType type, const Type* llvmIntegerType) {
    switch (type) {
    case Operand::SINT_WORD:
        return llvmIntegerType;
    case Operand::UINT_WORD:
        return llvmIntegerType;
    case Operand::FLOAT_WORD:
        return Type::FloatTy;
    case Operand::DOUBLE_WORD:
        return Type::DoubleTy;
    default:
        return Type::VoidTy;
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

        footprints["ADDF.i32"].push_back("f32.add.f32.f32");
        footprints["SUBF.i32"].push_back("f32.sub.f32.f32");
        footprints["NEGF.i32"].push_back("f32.fneg.f32");
        footprints["MULF.i32"].push_back("f32.mul.f32.f32");
        footprints["DIVF.i32"].push_back("f32.fdiv.f32.f32");

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

        init = false;                
    }

    return footprints[tceOp];
}

std::string getFootprint(Instruction& I) {
    
    std::string footPrint = stringType(I.getType());

    switch (I.getOpcode()) {

    case Instruction::FCmp: {
        FCmpInst* cmpInst = dynamic_cast<FCmpInst*>(&I);
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
    const std::vector<std::string>& 
        footprints = llvmFootprints(op.name() + suffix);
                       
    for (unsigned int j = 0; j < footprints.size(); j++) {                
        Function* func = M.getFunction(op.emulationFunctionName());
        replaceFunctions[footprints[j]] = func;

//        replaceFunctions[footprints[j]] = 
//            M.getOrInsertFunction(op.emulationFunctionName(), fType);

#if 0
       std::cerr << "Operation: " << op.name()
                  << " is emulated with: " << op.emulationFunctionName() 
                  << " footprint: " << footprints[j]
                  << std::endl;
#endif
        if (replaceFunctions[footprints[j]] == NULL) {
            std::cerr << " ERROR: suitable function wasn't found" << std::endl;
        }
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
        requiredSet = OperationDAGSelector::llvmRequiredOpset();
    
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
                        
            assert (op.numberOfOutputs() == 1 && 
                    "LowerMissing supports only 1 output instructions.");

            // Make parameter list for operation with all needed integer 
            // widths if pure floatingpoint just i32 is used. 
            // 
            // If there is also 
            // IntWord or UIntWord parameters all vectors are filled.
            std::vector<const Type*> argList_i32;
            const Type* retVal_i32 = NULL;

            std::vector<const Type*> argList_i16;
            const Type* retVal_i16 = NULL;

            std::vector<const Type*> argList_i8;
            const Type* retVal_i8 = NULL;

            std::vector<const Type*> argList_i1;
            const Type* retVal_i1 = NULL;
            
            bool useInt = false;
            for (int j = 1; j <= op.numberOfInputs(); j++) { 
                Operand& operand = op.operand(j);                
                const Type* llvmOp = getLLVMType(operand.type(), 
                                                 Type::Int32Ty);
                argList_i32.push_back(llvmOp);
                if (llvmOp == Type::Int32Ty) {
                    useInt = true;
                }               
            }
            
            Operand& outputOperand = op.operand(op.numberOfInputs() + 1);
            retVal_i32 = getLLVMType(outputOperand.type(), Type::Int32Ty);

            if (retVal_i32 == Type::Int32Ty) {
                useInt = true;
            }
                       
            FunctionType* fType_i32 = 
                FunctionType::get(retVal_i32, argList_i32, false);
             
            addFunctionForFootprints(M, fType_i32, op, ".i32");
            
            // create other function protos for other integer bitwidths
            if (useInt) {
                if (retVal_i32 == Type::Int32Ty) {
                    retVal_i16 = Type::Int16Ty;
                    retVal_i8 = Type::Int8Ty;
                    retVal_i1 = Type::Int1Ty;
                } else {
                    retVal_i16 = retVal_i32;
                    retVal_i8 = retVal_i32;
                    retVal_i1 = retVal_i32;
                }

                for (unsigned int j = 0; j < argList_i32.size(); j++) {
                    const Type* currArg = argList_i32[j];
                    if (currArg == Type::Int32Ty) {
                        argList_i16.push_back(Type::Int16Ty);
                        argList_i8.push_back(Type::Int8Ty);
                        argList_i1.push_back(Type::Int1Ty);
                    } else {
                        argList_i16.push_back(currArg);
                        argList_i8.push_back(currArg);
                        argList_i1.push_back(currArg);
                    }
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
    
    BasicBlock::InstListType &BBIL = BB.getInstList();

    // Loop over all of the instructions, looking for instructions to lower
    // instructions
    for (BasicBlock::iterator I = BB.begin(), E = BB.end(); I != E; ++I) {
        
        // get footprint of instruction
        std::string footPrint = getFootprint(*I);

        std::map<std::string, Constant*>::iterator 
            replaceFunc =  replaceFunctions.find(footPrint);        

        if (replaceFunc != replaceFunctions.end()) {

            if (Application::verboseLevel() >
                Application::VERBOSE_LEVEL_DEFAULT) {

                Application::logStream()
                    << "Replacing: " << footPrint
                    << " with emulation function." << std::endl;
            }
            std::vector<Value*> args;            

            for (unsigned j = 0; j < I->getNumOperands(); j++) {

                if (I->getOperand(j)->getType() == Type::Int16Ty ||
		    I->getOperand(j)->getType() == Type::Int8Ty) {

                    // Emulated operations with i1/i8/i16 operands need
                    // their operands extended to 32 bits. However, there's
                    // no easy way to see if the llvm operand requires
                    // sign or zero extension, so the correct extension is
                    // currently determined directly from the footprint.
                    if (footPrint == "f32.sitofp.i16" ||
                        footPrint == "f32.sitofp.i8") {

                        // sign extension needed
                        args.push_back(
                            llvm::CastInst::createIntegerCast(
                                I->getOperand(j), Type::Int32Ty, true, "", I));

                    } else if (footPrint == "f32.uitofp.i16" ||
                               footPrint == "f32.uitofp.i8") {

                        // zero extension needed
                        args.push_back(
                            llvm::CastInst::createIntegerCast(
                                I->getOperand(j), Type::Int32Ty,
                                false, "", I));
                    } else {
                        // unknown extension needed
                        assert(false && "Unknown operation footprint "
                               "requiring operand extension.");
                    }
                } else {
                    args.push_back(I->getOperand(j));
                }
            }

            CallInst *NewCall = 
                CallInst::Create(
                    replaceFunc->second, args.begin(), args.end(), "", I);
            
            NewCall->setTailCall();    

            // Replace all uses of the instruction with call instruction
            if (I->getType() != NewCall->getType()) {
                assert(llvm::CastInst::isCastable(
                           NewCall->getType(), I->getType()));

                Value *MCast;                
                Instruction::CastOps castOps =
                    llvm::CastInst::getCastOpcode(
                        NewCall, false, I->getType(), false);

                MCast = llvm::CastInst::create(
                    castOps ,NewCall, I->getType(), "", I);
                I->replaceAllUsesWith(MCast);                                

            } else {
                I->replaceAllUsesWith(NewCall);
            }
                        
            
            I = --BBIL.erase(I);
            Changed = true;
            
            NumLowered++;
        }
    }

    return Changed;
}

