/**
 * @file TCETargetLowering.cpp
 *
 * Implementation of TCETargetLowering class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel@cs.tut.fi)
 */

#include <assert.h>
#include <string>
#include <llvm/Function.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Target/TargetLowering.h>
#include <llvm/CodeGen/SelectionDAG.h>
#include <llvm/CodeGen/SSARegMap.h>
#include <llvm/CodeGen/MachineFrameInfo.h>
#include <llvm/CodeGen/MachineInstrBuilder.h>
#include <llvm/ADT/VectorExtras.h>
#include <llvm/Intrinsics.h>

#include "TCEPlugin.hh"
#include "TCERegisterInfo.hh"
#include "TCETargetMachine.hh"
#include "TCESubtarget.hh"
#include "TCETargetLowering.hh"

#include <iostream> // DEBUG

#define USE_ARG_REGS false

using namespace llvm;


/**
 * Returns target node opcode names for debugging purposes.
 *
 * @param opcode Opcode to convert to string.
 * @return Opcode name.
 */
const char*
TCETargetLowering::getTargetNodeName(unsigned opcode) const {
    switch (opcode) {
    default: return NULL;
    case TCEISD::CALL: return "TCEISD::CALL";
    case TCEISD::RET_FLAG: return "TCEISD::RET_FLAG";
    case TCEISD::GLOBAL_ADDR: return "TCEISD::GLOBAL_ADDR";
    case TCEISD::CONST_POOL: return "TCEISD::CONST_POOL";

    case TCEISD::SELECT_I1: return "TCEISD::SELECT_I1";
    case TCEISD::SELECT_I8: return "TCEISD::SELECT_I8";
    case TCEISD::SELECT_I16: return "TCEISD::SELECT_I16";
    case TCEISD::SELECT_I32: return "TCEISD::SELECT_I32";
    case TCEISD::SELECT_I64: return "TCEISD::SELECT_I64";
    case TCEISD::SELECT_F32: return "TCEISD::SELECT_F32";
    case TCEISD::SELECT_F64: return "TCEISD::SELECT_F64";
    }
}

/**
 * The Constructor.
 *
 * Initializes the target lowering.
 */
TCETargetLowering::TCETargetLowering(TCETargetMachine& tm) :
    TargetLowering(tm), tm_(tm) {

    addRegisterClass(MVT::i32, TCE::I32RegsRegisterClass);
    addRegisterClass(MVT::f32, TCE::F32RegsRegisterClass);

    //setLoadXAction(ISD::EXTLOAD, MVT::f32, Expand);
    setLoadXAction(ISD::EXTLOAD, MVT::i1   , Promote);
    setLoadXAction(ISD::ZEXTLOAD, MVT::i1   , Expand);

    setOperationAction(ISD::UINT_TO_FP, MVT::i1   , Promote);
    setOperationAction(ISD::UINT_TO_FP, MVT::i8   , Promote);
    setOperationAction(ISD::UINT_TO_FP, MVT::i16  , Promote);

    setOperationAction(ISD::SINT_TO_FP, MVT::i1   , Promote);
    setOperationAction(ISD::SINT_TO_FP, MVT::i8   , Promote);
    setOperationAction(ISD::SINT_TO_FP, MVT::i16  , Promote);

    setOperationAction(ISD::FP_TO_UINT, MVT::i1   , Promote);
    setOperationAction(ISD::FP_TO_UINT, MVT::i8   , Promote);
    setOperationAction(ISD::FP_TO_UINT, MVT::i16  , Promote);

    setOperationAction(ISD::FP_TO_SINT, MVT::i1   , Promote);
    setOperationAction(ISD::FP_TO_SINT, MVT::i8   , Promote);
    setOperationAction(ISD::FP_TO_SINT, MVT::i16  , Promote);

    setOperationAction(ISD::GlobalAddress, MVT::i32, Custom);
    setOperationAction(ISD::ConstantPool , MVT::i32, Custom);
    setOperationAction(ISD::RET,           MVT::Other, Custom);

    // Selects are custom lowered to a pseudo instruction that
    // is later converted to a diamond control flow pattern.
    setOperationAction(ISD::SELECT, MVT::i1, Promote);
    setOperationAction(ISD::SELECT, MVT::i8, Promote);
    setOperationAction(ISD::SELECT, MVT::i16, Promote);
    setOperationAction(ISD::SELECT, MVT::i32, Custom);
    setOperationAction(ISD::SELECT, MVT::i64, Custom);
    setOperationAction(ISD::SELECT, MVT::f32, Custom);
    setOperationAction(ISD::SELECT, MVT::f64, Custom);

    // SELECT is used instead of SELECT_CC
    setOperationAction(ISD::SELECT_CC, MVT::Other, Expand);

    setSetCCResultType(MVT::i32);

    setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i1 , Expand);

    // Expand indirect branches.
    setOperationAction(ISD::BRIND, MVT::Other, Expand);
    // Expand jumptable branches.
    setOperationAction(ISD::BR_JT, MVT::Other, Expand);
    // Expand conditional branches.
    setOperationAction(ISD::BR_CC, MVT::Other, Expand);

    setOperationAction(ISD::MULHU,  MVT::i32, Expand);
    setOperationAction(ISD::MULHS,  MVT::i32, Expand);
    setOperationAction(ISD::MEMMOVE, MVT::Other, Expand);
    setOperationAction(ISD::MEMSET, MVT::Other, Expand);
    setOperationAction(ISD::MEMCPY, MVT::Other, Expand);
    setOperationAction(ISD::SHL_PARTS, MVT::i32, Expand);
    setOperationAction(ISD::SRA_PARTS, MVT::i32, Expand);
    setOperationAction(ISD::SRL_PARTS, MVT::i32, Expand);

    setOperationAction(ISD::LOCATION, MVT::Other, Expand);
    setOperationAction(ISD::DEBUG_LOC, MVT::Other, Expand);
    setOperationAction(ISD::LABEL, MVT::Other, Expand);

    setOperationAction(ISD::VASTART           , MVT::Other, Custom);
    setOperationAction(ISD::VAARG             , MVT::Other, Expand);
    setOperationAction(ISD::VACOPY            , MVT::Other, Expand);
    setOperationAction(ISD::VAEND             , MVT::Other, Expand);

    setOperationAction(ISD::STACKSAVE         , MVT::Other, Expand);
    setOperationAction(ISD::STACKRESTORE      , MVT::Other, Expand);
    setOperationAction(ISD::DYNAMIC_STACKALLOC, MVT::i32  , Expand);

    setOperationAction(ISD::FCOPYSIGN, MVT::f64, Expand);
    setOperationAction(ISD::FCOPYSIGN, MVT::f32, Expand);

    setOperationAction(ISD::ConstantFP, MVT::f64, Expand);
    setOperationAction(ISD::ConstantFP, MVT::f32, Expand);

    setStackPointerRegisterToSaveRestore(TCE::SP);

    computeRegisterProperties();
}


/**
 * The Destructor.
 */
TCETargetLowering::~TCETargetLowering() {
}


/**
 * Handles custom operation lowerings.
 */
SDOperand
TCETargetLowering::LowerOperation(SDOperand op, SelectionDAG& dag) {

    switch(op.getOpcode()) {
    case ISD::RET: return LowerRET(op, dag);
    case ISD::GlobalAddress: {
        GlobalValue* gv = cast<GlobalAddressSDNode>(op)->getGlobal();
        SDOperand ga = dag.getTargetGlobalAddress(gv, MVT::i32);
        return dag.getNode(TCEISD::GLOBAL_ADDR, MVT::i32, ga);
    }
    case ISD::DYNAMIC_STACKALLOC: {
        assert(false && "Dynamic stack allocation not yet implemented.");
    }
    case ISD::SELECT: {
        return lowerSELECT(op, dag);
    }
    case ISD::VASTART: {
        MVT::ValueType ptrVT = dag.getTargetLoweringInfo().getPointerTy();
        SDOperand fr = dag.getFrameIndex(varArgsFrameIndex_, ptrVT);
        SrcValueSDNode* sv = cast<SrcValueSDNode>(op.getOperand(2));
        return dag.getStore(
            op.getOperand(0), fr, op.getOperand(1), sv->getValue(),
            sv->getOffset());

    }
    case ISD::ConstantPool: {
        // TODO: Check this.
        MVT::ValueType ptrVT = op.getValueType();
        ConstantPoolSDNode* cp = cast<ConstantPoolSDNode>(op);
        SDOperand res;
        if (cp->isMachineConstantPoolEntry()) {
            res = dag.getTargetConstantPool(
                cp->getMachineCPVal(), ptrVT,
                cp->getAlignment());
        } else {
            res = dag.getTargetConstantPool(
                cp->getConstVal(), ptrVT,
                cp->getAlignment());
        }
        return dag.getNode(TCEISD::CONST_POOL, MVT::i32, res);
    }
    }
    op.Val->dump(&dag);
    assert(0 && "Custom lowerings not implemented!");
}

/**
 * Lowers return operation.
 */
SDOperand
TCETargetLowering::LowerRET(SDOperand op, SelectionDAG& dag) {
    
    SDOperand copy;
    switch (op.getNumOperands()) {
    default: {
        std::cerr  << "Do not know how to return "
                   << op.getNumOperands() << " operands!"
                   << std::endl;

        assert(false);
    }
    case 1: {
        return SDOperand();
    }
    case 3: {
        unsigned argReg;
        switch(op.getOperand(1).getValueType()) {
        default: assert(0 && "Unknown type to return");
        case MVT::i32: argReg = TCE::IRES0; break;
        case MVT::f32: argReg = TCE::FRES0; break;
	 //case MVT::i64: argReg = TCE::DIRES0; break;
	 //case MVT::f64: argReg = TCE::DRES0; break;
        }
        copy = dag.getCopyToReg(
            op.getOperand(0), argReg, op.getOperand(1),
            SDOperand());
        break;
    }
    case 5: {
        // TODO: i64 return value is now split in two registers:
        // lo part in IRES0 and hi part in KLUDGE_REGISTER
        assert(op.getOperand(1).getValueType() == MVT::i32);
        assert(op.getOperand(3).getValueType() == MVT::i32);

        copy = dag.getCopyToReg(
            op.getOperand(0), TCE::IRES0, op.getOperand(3),
            SDOperand());

        copy = dag.getCopyToReg(
            copy, TCE::KLUDGE_REGISTER, op.getOperand(1),
            copy.getValue(1));

        break;
    }
    }
    return dag.getNode(
        TCEISD::RET_FLAG, MVT::Other, copy, copy.getValue(1));

}

/**
 * Lowers FORMAL_ARGUMENTS node.
 */
std::vector<SDOperand>
TCETargetLowering::LowerArguments(Function& f, SelectionDAG& dag) {
 
    MachineFunction& mf = dag.getMachineFunction();
    SSARegMap* regMap = mf.getSSARegMap();
    std::vector<SDOperand> argValues;

    unsigned argOffset = 0;

    static const unsigned iArgRegs[] = {
       0//TCE::IARG1, TCE::IARG2, TCE::IARG3, TCE::IARG4
    };

    const unsigned* curIArgReg = iArgRegs;
    const unsigned* iArgRegEnd = iArgRegs + 0;
    SDOperand root = dag.getRoot();

    std::vector<unsigned> argVRegs;
    std::vector<unsigned> argPRegs;

    Function::arg_iterator iter = f.arg_begin();
    for (; iter != f.arg_end(); iter++) {
        MVT::ValueType objectVT = getValueType(iter->getType());

        switch (objectVT) {
        default: assert(
            false &&
            "TCETargetLowering::LowerArguments(): unhandled argument type");

        case MVT::i1:
            // fall through
        case MVT::i8:
            // fall through
        case MVT::i16:
            // fall through
        case MVT::i32: {
            if (USE_ARG_REGS && iter->use_empty()) {
                // Argument is dead.
                if (curIArgReg < iArgRegEnd) ++curIArgReg;
                argValues.push_back(dag.getNode(ISD::UNDEF, objectVT));
            } else if (USE_ARG_REGS && curIArgReg < iArgRegEnd) {

                // Argument in register.
                unsigned vreg = regMap->createVirtualRegister(
                    &TCE::I32RegsRegClass);

                //mf.addLiveIn(*curIArgReg++, vreg);
                argVRegs.push_back(vreg);
                argPRegs.push_back(*curIArgReg);
                curIArgReg++;

                SDOperand arg = dag.getCopyFromReg(root, vreg, MVT::i32);
                if (objectVT != MVT::i32) {
                    unsigned assertOp = ISD::AssertSext;
                    arg = dag.getNode(assertOp, MVT::i32, arg,
                                      dag.getValueType(objectVT));

                    arg = dag.getNode(ISD::TRUNCATE, objectVT, arg);

                }
                argValues.push_back(arg);
            } else {

                // Argument in stack.
                int frameIdx =
                    mf.getFrameInfo()->CreateFixedObject(4, argOffset);
                SDOperand fiPtr = dag.getFrameIndex(frameIdx, MVT::i32);
                SDOperand load;
                if (objectVT == MVT::i32) {
                    load = dag.getLoad(MVT::i32, root, fiPtr, NULL, 0);
                } else {
                    ISD::LoadExtType loadOp = ISD::SEXTLOAD;
                    unsigned offset = 0;
                    if (objectVT == MVT::i1 || objectVT == MVT::i8) {
                        offset = 3;
                    } else if (objectVT == MVT::i16) {
                        offset = 2;
                    } else {
                        assert(false);
                    }
                    fiPtr = dag.getNode(
                        ISD::ADD, MVT::i32, fiPtr,
                        dag.getConstant(offset, MVT::i32));
                    
                    load = dag.getExtLoad(loadOp, MVT::i32, root, fiPtr,
                                          NULL, 0, objectVT);
                    
                    load = dag.getNode(ISD::TRUNCATE, objectVT, load);
                }
                argValues.push_back(load);
                argOffset += 4;
            }
            break;
        }
        case MVT::i64: {
            // Following sparc example: i64 is split into lo/hi parts.
            if (USE_ARG_REGS && iter->use_empty()) { // Argument is dead.

                if (curIArgReg < iArgRegEnd) ++curIArgReg;
                if (curIArgReg < iArgRegEnd) ++curIArgReg;
                argValues.push_back(dag.getNode(ISD::UNDEF, objectVT));
            } else {
                SDOperand hiVal;
                if (USE_ARG_REGS && curIArgReg < iArgRegEnd) {

                    // Lives in an incoming GPR
                    unsigned vRegHi =
                        regMap->createVirtualRegister(&TCE::I32RegsRegClass);

                    //mf.addLiveIn(*curIArgReg++, vRegHi);
                    argVRegs.push_back(vRegHi);
                    argPRegs.push_back(*curIArgReg);
                    curIArgReg++;

                    hiVal = dag.getCopyFromReg(root, vRegHi, MVT::i32);
                } else {

                    int frameIdx =
                        mf.getFrameInfo()->CreateFixedObject(4, argOffset);

                    SDOperand fiPtr = dag.getFrameIndex(frameIdx, MVT::i32);

                    hiVal = dag.getLoad(MVT::i32, root, fiPtr, NULL, 0);
                }

                SDOperand loVal;
                if (USE_ARG_REGS && curIArgReg < iArgRegEnd) {

                    // Lives in an incoming GPR
                    unsigned vRegLo =
                        regMap->createVirtualRegister(&TCE::I32RegsRegClass);

                    //mf.addLiveIn(*curIArgReg++, vRegLo);
                    argVRegs.push_back(vRegLo);
                    argPRegs.push_back(*curIArgReg);
                    curIArgReg++;

                    loVal = dag.getCopyFromReg(root, vRegLo, MVT::i32);
                } else {

                    int frameIdx =
                        mf.getFrameInfo()->CreateFixedObject(4, argOffset+4);

                    SDOperand fiPtr = dag.getFrameIndex(frameIdx, MVT::i32);
                    loVal = dag.getLoad(MVT::i32, root, fiPtr, NULL, 0);
                }

                // Compose the two halves together into an i64 unit.
                SDOperand wholeValue =
                    dag.getNode(ISD::BUILD_PAIR, MVT::i64, loVal, hiVal);

                argValues.push_back(wholeValue);
                argOffset += 8;
            }
            break;
        }
        case MVT::f32: {
            if (USE_ARG_REGS && iter->use_empty()) {
                // Argument is dead.
                argValues.push_back(dag.getNode(ISD::UNDEF, objectVT));
            } else {
                // For now, f32s are always passed in the stack.
                int frameIdx =
                    mf.getFrameInfo()->CreateFixedObject(4, argOffset);

                SDOperand fiPtr = dag.getFrameIndex(frameIdx, MVT::i32);
                SDOperand load = dag.getLoad(MVT::f32, root, fiPtr, NULL, 0);
                argValues.push_back(load);
                argOffset += 4;
                break;
            }
        }
        case MVT::f64:
            if (USE_ARG_REGS && iter->use_empty()) {
                // Argument is dead.
                argValues.push_back(dag.getNode(ISD::UNDEF, objectVT));
	    } else {
	       SDOperand hiVal;
	       SDOperand loVal;

	       int frameIdxHi =
		 mf.getFrameInfo()->CreateFixedObject(4, argOffset);

	       SDOperand hiFiPtr = dag.getFrameIndex(frameIdxHi, MVT::i32);

	       hiVal = dag.getLoad(MVT::i32, root, hiFiPtr, NULL, 0);
	       
	       int frameIdxLo =
		 mf.getFrameInfo()->CreateFixedObject(4, argOffset+4);

	       SDOperand loFiPtr = dag.getFrameIndex(frameIdxLo, MVT::i32);
	       loVal = dag.getLoad(MVT::i32, root, loFiPtr, NULL, 0);

	       // Compose the two halves together into an i64 unit.
	       SDOperand wholeValue =
		 dag.getNode(ISD::BUILD_PAIR, MVT::f64, loVal, hiVal);

	       argValues.push_back(wholeValue);
	       argOffset += 8;
            }
	    break;
        }
    }

    // In case of a variadic argument function, all parameters are passed
    // in the stack.
    if(f.isVarArg()) {
        varArgsFrameIndex_ =
            mf.getFrameInfo()->CreateFixedObject(4, argOffset);
    }

    // Copy vregs to physical argument registers.
    if (USE_ARG_REGS) {
        const TargetInstrInfo* tii = getTargetMachine().getInstrInfo();
        MachineBasicBlock& bb = mf.front();
        for (unsigned i = 0; i < argVRegs.size(); ++i) {
            BuildMI(&bb, tii->get(TCE::MOVI32rr), argVRegs[i])
                .addReg(argPRegs[i]);

            //TODO: Save arg regs if this is a vararg function?
            /*
              MachineFrameInfo* mfi = mf.getFrameInfo();
              if(f.isVarArg()) {
              // if this is a varargs function,
              // we copy the input registers to the stack
              std::cerr << " store @offset: " << argOffset;
              int fi = mfi->CreateFixedObject(4, argOffset);
              argOffset+=4;
              BuildMI(&bb, tii->get(TCE::MOVI32ri), TCE::KLUDGE_REGISTER)
              .addFrameIndex(fi);
              
              BuildMI(&bb, tii->get(TCE::STWrr))
              .addReg(TCE::KLUDGE_REGISTER).addImm(0).addReg(argPRegs[i]);
            */
        }
    }

    switch (getValueType(f.getReturnType())) {
    default: assert(0 && "Unknown type!");
    case MVT::isVoid: break;
    case MVT::i1:
    case MVT::i8:
    case MVT::i16:
    case MVT::i32:
        mf.addLiveOut(TCE::IRES0);
        break;
    case MVT::f32:
        mf.addLiveOut(TCE::FRES0);
        break;
    case MVT::i64:
    case MVT::f64: {
        mf.addLiveOut(TCE::IRES0);
        mf.addLiveOut(TCE::KLUDGE_REGISTER);
        break;
    }
    }
    return argValues;
}


/**
 * Lowers call pseudo instruction to target specific sequence.
 */
std::pair<SDOperand, SDOperand>
TCETargetLowering::LowerCallTo(
    SDOperand chain, const Type* retTy, bool retTyIsSigned,
    bool isVarArg, unsigned cc, bool isTailCall, SDOperand callee,
    ArgListTy& args, SelectionDAG& dag) {

    unsigned argsSize = 0;
    for (unsigned i = 0; i < args.size(); i++) {
        switch(getValueType(args[i].Ty)) {
        default: assert(false && "Unknown value type!");
        case MVT::i1: {
            argsSize += 4;
            break;
        }
        case MVT::i8: {
            argsSize += 4;
            break;
        }
        case MVT::i16: {
            argsSize += 4;
            break;
        }
        case MVT::f32: {
            argsSize += 4;
            break;
        }
        case MVT::i32: {
            argsSize += 4;
            break;
        }
        case MVT::i64: {
            argsSize += 8;
            break;
        }
        case MVT::f64: {
            argsSize += 8;
            break;
        }
        }
    }

    if (USE_ARG_REGS) {
        if (argsSize > 16) {
            argsSize -= 16; // Values in registers.
        } else {
            argsSize = 0;
        }
    }


    // Keep stack frames 4-byte aligned.
    argsSize = (argsSize+3) & ~3;

    chain = dag.getCALLSEQ_START(
        chain, dag.getConstant(argsSize, getPointerTy()));

    SDOperand stackPtr;
    std::vector<SDOperand> stores;
    std::vector<SDOperand> iRegValuesToPass;
    std::vector<SDOperand> fpRegValuesToPass;
    unsigned argOffset = 0;
    unsigned numIArgRegs = 0; // Number of available integer arg regs.

    for (unsigned i = 0; i < args.size(); i++) {
        SDOperand val = args[i].Node;
        MVT::ValueType objectVT = val.getValueType();
        SDOperand valToStore(0, 0);
        unsigned objSize = 0;

        switch (objectVT) {
        default: assert(false && "Unhandled argument type!");
        case MVT::i1:
        case MVT::i8:
        case MVT::i16: {
            // Promote the integer to 32-bits.
            ISD::NodeType ext = ISD::ANY_EXTEND;
            if (args[i].isSExt) {
                ext = ISD::SIGN_EXTEND;
            } else if (args[i].isZExt) {
                ext = ISD::ZERO_EXTEND;
            }
            val = dag.getNode(ext, MVT::i32, val);
            // FALL THROUGH
        }
        case MVT::i32: {
            objSize = 4;
            if (USE_ARG_REGS && iRegValuesToPass.size() < numIArgRegs) {
                iRegValuesToPass.push_back(val);
            } else {
                valToStore = val;
            }
            break;
        }
        case MVT::i64: {
            // Following sparc example: split i64 to low/hi parts.
            objSize = 8;
            if (!USE_ARG_REGS && iRegValuesToPass.size() >= numIArgRegs) {
                valToStore = val;    // Pass in stack.
                break;
            }

            // Split the value into top and bottom part.
            // Top part goes in a reg.
            SDOperand hi = dag.getNode(
                ISD::EXTRACT_ELEMENT, getPointerTy(), val,
                dag.getConstant(1, MVT::i32));

            SDOperand lo = dag.getNode(
                ISD::EXTRACT_ELEMENT, getPointerTy(), val,
                dag.getConstant(0, MVT::i32));

            iRegValuesToPass.push_back(hi);

            if (USE_ARG_REGS && iRegValuesToPass.size() >= numIArgRegs) {
                valToStore = lo;
                argOffset += 4;
                objSize = 4;
            } else {
                iRegValuesToPass.push_back(lo);
            }
            break;
        }
        case MVT::f32: {
            objSize = 4;
            valToStore = val;
            break;
            }
        case MVT::f64: {
            objSize = 8;
            valToStore = val;
            break;
        }
        }

        if (valToStore.Val) {
            if (!stackPtr.Val) {
                stackPtr = dag.getRegister(TCE::SP, MVT::i32);
            }
            SDOperand ptrOff = dag.getConstant(argOffset, getPointerTy());
            ptrOff = dag.getNode(ISD::ADD, MVT::i32, stackPtr, ptrOff);
            stores.push_back(
                dag.getStore(chain, valToStore, ptrOff, NULL, 0));

            argOffset += objSize;
        }
    }

    
    // Emit all stores, make sure the occur before any copies into physregs.
    if (!stores.empty())
        chain = dag.getNode(
            ISD::TokenFactor, MVT::Other, &stores[0], stores.size());

    // Outgoing args in int registers
    static const unsigned iArgRegs[] = {
       0//TCE::IARG1, TCE::IARG2, TCE::IARG3, TCE::IARG4
    };

    SDOperand inFlag;
    for (unsigned i = 0; i < iRegValuesToPass.size(); i++) {
       assert(false);
        chain =
            dag.getCopyToReg(chain, iArgRegs[i], iRegValuesToPass[i], inFlag);

        inFlag = chain.getValue(1);

    }

    // Turn global addressses to target global adresses.
    if (GlobalAddressSDNode* g = dyn_cast<GlobalAddressSDNode>(callee))
        callee = dag.getTargetGlobalAddress(g->getGlobal(), MVT::i32);
    else if (ExternalSymbolSDNode* e = dyn_cast<ExternalSymbolSDNode>(callee))
        callee = dag.getTargetExternalSymbol(e->getSymbol(), MVT::i32);

    std::vector<MVT::ValueType> nodeTys;
    nodeTys.push_back(MVT::Other);
    nodeTys.push_back(MVT::Flag);
    SDOperand ops[] = { chain, callee, inFlag };
    chain = dag.getNode(TCEISD::CALL, nodeTys, ops, inFlag.Val ? 3 : 2);
    inFlag = chain.getValue(1);

    // Return value
    MVT::ValueType retTyVT = getValueType(retTy);
    SDOperand retVal;
    if (retTyVT != MVT::isVoid) {
        switch (retTyVT) {
        default: assert(false && "Unsupported return value type.");
        case MVT::i1:
        case MVT::i8:
        case MVT::i16: {
            retVal = dag.getCopyFromReg(chain, TCE::IRES0, MVT::i32, inFlag);
            chain = retVal.getValue(1);
            retVal = dag.getNode(ISD::TRUNCATE, retTyVT, retVal);
            break;
        }
        case MVT::i32: {
            retVal = dag.getCopyFromReg(chain, TCE::IRES0, MVT::i32, inFlag);
            chain = retVal.getValue(1);
            break;
        }
        case MVT::i64: {
            SDOperand lo = dag.getCopyFromReg(
                chain, TCE::IRES0, MVT::i32, inFlag);

            SDOperand hi = dag.getCopyFromReg(
                lo.getValue(1), TCE::KLUDGE_REGISTER, MVT::i32, 
                lo.getValue(2));

            retVal = dag.getNode(ISD::BUILD_PAIR, MVT::i64, lo, hi);
            chain = hi.getValue(1);
            break;
        }
        case MVT::f32: {
            retVal = dag.getCopyFromReg(chain, TCE::FRES0, MVT::f32, inFlag);
            chain = retVal.getValue(1);
            break;
        }
        case MVT::f64: {
            SDOperand lo = dag.getCopyFromReg(
                chain, TCE::IRES0, MVT::i32, inFlag);

            SDOperand hi = dag.getCopyFromReg(
                lo.getValue(1), TCE::KLUDGE_REGISTER, MVT::i32, 
                lo.getValue(2));

            retVal = dag.getNode(ISD::BUILD_PAIR, MVT::i64, lo, hi);
            chain = hi.getValue(1);
            break;
        }
        }
    }

    chain = dag.getNode(ISD::CALLSEQ_END, MVT::Other, chain,
                        dag.getConstant(argsSize, getPointerTy()));

    return std::make_pair(retVal, chain);
}

/**
 * Handles TCE SELECT nodes by inserting a diamond control flow pattern.
 *
 * @param mbb Machine basic block containing the SELECT node.
 * @param mi SELECT instruction.
 * @return Machine basic block with the SELECT pseudo instruction replaced.
 */
MachineBasicBlock*
TCETargetLowering::InsertAtEndOfBasicBlock(
    MachineInstr* mi, MachineBasicBlock* mbb) {

    // TODO CHECK THIS
    // ---------------------------------------
    //  Copied from PPCISelLowering
    // ---------------------------------------

    const TargetInstrInfo* tii = getTargetMachine().getInstrInfo();
    assert((mi->getOpcode() == TCE::SELECT_I1 ||
            mi->getOpcode() == TCE::SELECT_I8 ||
            mi->getOpcode() == TCE::SELECT_I16 ||
            mi->getOpcode() == TCE::SELECT_I32 ||
            mi->getOpcode() == TCE::SELECT_I64 ||
            mi->getOpcode() == TCE::SELECT_F32 ||
            mi->getOpcode() == TCE::SELECT_F64) &&
           "Unexpected instr type to insert");
  
    //assert(false);
    // To "insert" a SELECT instruction, we actually have to insert
    // the diamond
    // control-flow pattern.  The incoming instruction knows the
    // destination vreg
    // to set, the condition code register to branch on,
    // the true/false values to
    // select between, and a branch opcode to use.
    const BasicBlock* LLVM_BB = mbb->getBasicBlock();
    ilist<MachineBasicBlock>::iterator It = mbb;
    ++It;
  
    //  thisMBB:
    //  ...
    //   TrueVal = ...
    //   cmpTY ccX, r1, r2
    //   bCC copy1MBB
    //   fallthrough --> copy0MBB
    MachineBasicBlock* thisMBB = mbb;
    MachineBasicBlock* copy0MBB = new MachineBasicBlock(LLVM_BB);
    MachineBasicBlock* sinkMBB = new MachineBasicBlock(LLVM_BB);
    //unsigned SelectPred = mi->getOperand(4).getImm();
    BuildMI(mbb, tii->get(TCE::TCEBRCOND))
        .addReg(mi->getOperand(3).getReg()).addMBB(sinkMBB);

    MachineFunction* F = mbb->getParent();
    F->getBasicBlockList().insert(It, copy0MBB);
    F->getBasicBlockList().insert(It, sinkMBB);
    // Update machine-CFG edges by first adding all successors of the current
    // block to the new block which will contain the Phi node for the select.
    for(MachineBasicBlock::succ_iterator i = mbb->succ_begin(), 
            e = mbb->succ_end(); i != e; ++i)
        sinkMBB->addSuccessor(*i);
    // Next, remove all successors of the current block, and add the true
    // and fallthrough blocks as its successors.
    while(!mbb->succ_empty())
        mbb->removeSuccessor(mbb->succ_begin());

    mbb->addSuccessor(copy0MBB);
    mbb->addSuccessor(sinkMBB);
    
    //  copy0MBB:
    //   %FalseValue = ...
    //   # fallthrough to sinkMBB
    mbb = copy0MBB;
    
    // Update machine-CFG edges
    mbb->addSuccessor(sinkMBB);
    
    //  sinkMBB:
    //   %Result = phi [ %FalseValue, copy0MBB ], [ %TrueValue, thisMBB ]
    //  ...
    mbb = sinkMBB;
    BuildMI(mbb, tii->get(TCE::PHI), mi->getOperand(0).getReg())
        .addReg(mi->getOperand(2).getReg()).addMBB(copy0MBB)
        .addReg(mi->getOperand(1).getReg()).addMBB(thisMBB);

    delete mi;   // The pseudo instruction is gone now.
    return mbb;    
}

/**
 * Converts SELECT nodes to TCE-specific pseudo instructions.
 */
SDOperand
TCETargetLowering::lowerSELECT(
    SDOperand op, SelectionDAG& dag) {

    SDOperand cond = op.getOperand(0);
    SDOperand trueVal = op.getOperand(1);
    SDOperand falseVal = op.getOperand(2);

    unsigned opcode = 0;
    switch(trueVal.getValueType()) {
    default: assert(0 && "Unknown type to select.");
    case MVT::i1: {
        opcode = TCEISD::SELECT_I1;
        break;
    }
    case MVT::i8: {
        opcode = TCEISD::SELECT_I8;
        break;
    }
    case MVT::i16: {
        opcode = TCEISD::SELECT_I16;
        break;
    }
    case MVT::i32: {
        opcode = TCEISD::SELECT_I32;
        break;
    }
    case MVT::i64: {
        opcode = TCEISD::SELECT_I64;
        break;
    }
    case MVT::f32: {
        opcode = TCEISD::SELECT_F32;
        break;
    }
    case MVT::f64: {
        opcode = TCEISD::SELECT_F64;
        break;
    }
    }

    return dag.getNode(
        opcode,
        trueVal.getValueType(),
        trueVal, falseVal, cond);
}

/**
 * Inline ASM constraint support for custom op definitions.
 */
std::pair<unsigned, const TargetRegisterClass*>
TCETargetLowering::getRegForInlineAsmConstraint(
    const std::string& constraint,
    MVT::ValueType vt) const {

    if (constraint == "r" && vt == MVT::i32) {
        return std::make_pair(0U, TCE::I32RegsRegisterClass);
    }

    if (constraint == "f" && vt == MVT::f32) {
        return std::make_pair(0U, TCE::F32RegsRegisterClass);
    }

    return TargetLowering::getRegForInlineAsmConstraint(constraint, vt);
}

/*
std::vector<unsigned>
TCETargetLowering::getRegClassForInlineAsmConstraint(
    const std::string& constraint,
    MVT::ValueType vt) const {

    std::cerr << "getRegClassForInlineAsmConstraing(" << constraint << ", "
              << vt << ")"<<  std::endl;

    if (constraint == "r" && vt == MVT::i32) {
        std::vector<unsigned> rc;
        rc.push_back(TCE::I32RegsRegisterClass);
        return rc;
    }

    return std::vector<unsigned>();
}
*/
