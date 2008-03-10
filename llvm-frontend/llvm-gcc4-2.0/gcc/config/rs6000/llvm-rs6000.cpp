/* APPLE LOCAL begin LLVM (ENTIRE FILE!)  */
/* High-level LLVM backend interface 
Copyright (C) 2007 Free Software Foundation, Inc.
Contributed by Jim Laskey (jlaskey@apple.com)

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */

//===----------------------------------------------------------------------===//
// This is a C++ source file that implements specific llvm powerpc ABI.
//===----------------------------------------------------------------------===//

#include "llvm-abi.h"
#include "llvm-internal.h"
#include "llvm/Instructions.h"
#include "llvm/Intrinsics.h"
#include "llvm/Module.h"

extern "C" {
#include "toplev.h"
}

// MergeIntPtrOperand - This merges the int and pointer operands of a GCC
// intrinsic into a single operand for the LLVM intrinsic.  For example, this
// turns LVX(4, p) -> llvm.lvx(gep P, 4).  OPNUM specifies the operand number
// of the integer to contract with its following pointer and NAME specifies the
// name of the resultant intrinsic.
static void MergeIntPtrOperand(TreeToLLVM *TTL,
                               unsigned OpNum, Intrinsic::ID IID,
                               const Type *ResultType,
                               std::vector<Value*> &Ops,
                               LLVMBuilder &Builder, Value *&Result) {
  const Type *VoidPtrTy = PointerType::get(Type::Int8Ty);
  
  Function *IntFn = Intrinsic::getDeclaration(TheModule, IID);
  
  Value *Offset = Ops[OpNum];
  Value *Ptr = Ops[OpNum + 1];
  Ptr = TTL->CastToType(Instruction::BitCast, Ptr, VoidPtrTy);
  
  if (!isa<Constant>(Offset) || !cast<Constant>(Offset)->isNullValue())
    Ptr = Builder.CreateGEP(Ptr, Offset, "tmp");
    
  Ops.erase(Ops.begin() + OpNum);
  Ops[OpNum] = Ptr;
  Value *V = Builder.CreateCall(IntFn, &Ops[0], &Ops[0]+Ops.size());
  
  if (V->getType() != Type::VoidTy) {
    V->setName("tmp");
    Result = V;
  }
}

// GetAltivecTypeNumFromType - Given an LLVM type, return a unique ID for
// the type in the range 0-3.
static int GetAltivecTypeNumFromType(const Type *Ty) {
  return ((Ty == Type::Int32Ty) ? 0 : \
          ((Ty == Type::Int16Ty) ? 1 : \
           ((Ty == Type::Int8Ty) ? 2 : \
            ((Ty == Type::FloatTy) ? 3 : -1))));
}

// TargetIntrinsicLower - To handle builtins, we want to expand the
//invocation into normal LLVM code.  If the target can handle the builtin, this
//function should emit the expanded code and return true.
//
bool TreeToLLVM::TargetIntrinsicLower(tree exp,
                                      unsigned FnCode,
                                      Value *DestLoc,
                                      Value *&Result,
                                      const Type *ResultType,
                                      std::vector<Value*> &Ops) {
  switch (FnCode) {
  default: break;
  case ALTIVEC_BUILTIN_VADDFP:
  case ALTIVEC_BUILTIN_VADDUBM:
  case ALTIVEC_BUILTIN_VADDUHM:
  case ALTIVEC_BUILTIN_VADDUWM:
    Result = Builder.CreateAdd(Ops[0], Ops[1], "tmp");
    return true;
  case ALTIVEC_BUILTIN_VSUBFP:
  case ALTIVEC_BUILTIN_VSUBUBM:
  case ALTIVEC_BUILTIN_VSUBUHM:
  case ALTIVEC_BUILTIN_VSUBUWM:
    Result = Builder.CreateSub(Ops[0], Ops[1], "tmp");
    return true;
  case ALTIVEC_BUILTIN_VAND:
    Result = Builder.CreateAnd(Ops[0], Ops[1], "tmp");
    return true;
  case ALTIVEC_BUILTIN_VANDC:
    Ops[1] = Builder.CreateNot(Ops[1], "tmp");
    Result = Builder.CreateAnd(Ops[0], Ops[1], "tmp");
    return true;
  case ALTIVEC_BUILTIN_VOR:
    Result = Builder.CreateOr(Ops[0], Ops[1], "tmp");
    return true;
  case ALTIVEC_BUILTIN_VNOR:
    Result = Builder.CreateOr(Ops[0], Ops[1], "tmp");
    Result = Builder.CreateNot(Result, "tmp");
    return true;
  case ALTIVEC_BUILTIN_VXOR:
    Result = Builder.CreateXor(Ops[0], Ops[1], "tmp");
    return true;
  case ALTIVEC_BUILTIN_LVSL:
    MergeIntPtrOperand(this, 0, Intrinsic::ppc_altivec_lvsl,
                       ResultType, Ops, Builder, Result);
    return true;
  case ALTIVEC_BUILTIN_LVSR:
    MergeIntPtrOperand(this, 0, Intrinsic::ppc_altivec_lvsr,
                       ResultType, Ops, Builder, Result);
    return true;
  case ALTIVEC_BUILTIN_LVX:
    MergeIntPtrOperand(this, 0, Intrinsic::ppc_altivec_lvx,
                       ResultType, Ops, Builder, Result);
    return true;
  case ALTIVEC_BUILTIN_LVXL:
    MergeIntPtrOperand(this, 0, Intrinsic::ppc_altivec_lvxl,
                         ResultType, Ops, Builder, Result);
    return true;
  case ALTIVEC_BUILTIN_LVEBX:
    MergeIntPtrOperand(this, 0, Intrinsic::ppc_altivec_lvebx,
                       ResultType, Ops, Builder, Result);
    return true;
  case ALTIVEC_BUILTIN_LVEHX:
    MergeIntPtrOperand(this, 0, Intrinsic::ppc_altivec_lvehx,
                       ResultType, Ops, Builder, Result);
    return true;
  case ALTIVEC_BUILTIN_LVEWX:
    MergeIntPtrOperand(this, 0, Intrinsic::ppc_altivec_lvewx,
                       ResultType, Ops, Builder, Result);
    return true;
  case ALTIVEC_BUILTIN_STVX:
    MergeIntPtrOperand(this, 1, Intrinsic::ppc_altivec_stvx,
                       ResultType, Ops, Builder, Result);
    return true;
  case ALTIVEC_BUILTIN_STVEBX:
    MergeIntPtrOperand(this, 1, Intrinsic::ppc_altivec_stvebx,
                       ResultType, Ops, Builder, Result);
    return true;
  case ALTIVEC_BUILTIN_STVEHX:
    MergeIntPtrOperand(this, 1, Intrinsic::ppc_altivec_stvehx,
                       ResultType, Ops, Builder, Result);
    return true;
  case ALTIVEC_BUILTIN_STVEWX:
    MergeIntPtrOperand(this, 1, Intrinsic::ppc_altivec_stvewx,
                       ResultType, Ops, Builder, Result);
    return true;
  case ALTIVEC_BUILTIN_STVXL:
    MergeIntPtrOperand(this, 1, Intrinsic::ppc_altivec_stvxl,
                       ResultType, Ops, Builder, Result);
    return true;
  case ALTIVEC_BUILTIN_VSPLTISB:
    if (Constant *Elt = dyn_cast<ConstantInt>(Ops[0])) {
      Elt = ConstantExpr::getIntegerCast(Elt, Type::Int8Ty, true);
      Result = BuildVector(Elt, Elt, Elt, Elt,  Elt, Elt, Elt, Elt,
                           Elt, Elt, Elt, Elt,  Elt, Elt, Elt, Elt, NULL);
    } else {
      error("%Helement must be an immediate", &EXPR_LOCATION(exp));
      Result = UndefValue::get(VectorType::get(Type::Int8Ty, 16));
    }
    return true;
  case ALTIVEC_BUILTIN_VSPLTISH:
    if (Constant *Elt = dyn_cast<ConstantInt>(Ops[0])) {
      Elt = ConstantExpr::getIntegerCast(Elt, Type::Int16Ty, true);
      Result = BuildVector(Elt, Elt, Elt, Elt,  Elt, Elt, Elt, Elt, NULL);
    } else {
      error("%Helement must be an immediate", &EXPR_LOCATION(exp));
      Result = UndefValue::get(VectorType::get(Type::Int16Ty, 8));
    }
    return true;
  case ALTIVEC_BUILTIN_VSPLTISW:
    if (Constant *Elt = dyn_cast<ConstantInt>(Ops[0])) {
      Elt = ConstantExpr::getIntegerCast(Elt, Type::Int32Ty, true);
      Result = BuildVector(Elt, Elt, Elt, Elt, NULL);
    } else {
      error("%Hmask must be an immediate", &EXPR_LOCATION(exp));
      Result = UndefValue::get(VectorType::get(Type::Int32Ty, 4));
    }
    return true;
  case ALTIVEC_BUILTIN_VSPLTB:
    if (ConstantInt *Elt = dyn_cast<ConstantInt>(Ops[1])) {
      int EV = Elt->getZExtValue();
      Result = BuildVectorShuffle(Ops[0], Ops[0],
                                  EV, EV, EV, EV, EV, EV, EV, EV,
                                  EV, EV, EV, EV, EV, EV, EV, EV);
    } else {
      error("%Helement number must be an immediate", &EXPR_LOCATION(exp));
      Result = Ops[0];
    }
    return true;
  case ALTIVEC_BUILTIN_VSPLTH:
    if (ConstantInt *Elt = dyn_cast<ConstantInt>(Ops[1])) {
      int EV = Elt->getZExtValue();
      Result = BuildVectorShuffle(Ops[0], Ops[0],
                                  EV, EV, EV, EV, EV, EV, EV, EV);
    } else {
      error("%Helement number must be an immediate", &EXPR_LOCATION(exp));
      Result = Ops[0];
    }
    return true;
  case ALTIVEC_BUILTIN_VSPLTW:
    if (ConstantInt *Elt = dyn_cast<ConstantInt>(Ops[1])) {
      int EV = Elt->getZExtValue();
      Result = BuildVectorShuffle(Ops[0], Ops[0], EV, EV, EV, EV);
    } else {
      error("%Helement number must be an immediate", &EXPR_LOCATION(exp));
      Result = Ops[0];
    }
    return true;
  case ALTIVEC_BUILTIN_VSLDOI_16QI:
  case ALTIVEC_BUILTIN_VSLDOI_8HI:
  case ALTIVEC_BUILTIN_VSLDOI_4SI:
  case ALTIVEC_BUILTIN_VSLDOI_4SF:
    if (ConstantInt *Elt = dyn_cast<ConstantInt>(Ops[2])) {
      /* Map all of these to a shuffle. */
      unsigned Amt = Elt->getZExtValue() & 15;
      VectorType *v16i8 = VectorType::get(Type::Int8Ty, 16);
      Ops[0] = Builder.CreateBitCast(Ops[0], v16i8, "tmp");
      Ops[1] = Builder.CreateBitCast(Ops[1], v16i8, "tmp");
      Result = BuildVectorShuffle(Ops[0], Ops[1],
                                  Amt, Amt+1, Amt+2, Amt+3,
                                  Amt+4, Amt+5, Amt+6, Amt+7,
                                  Amt+8, Amt+9, Amt+10, Amt+11,
                                  Amt+12, Amt+13, Amt+14, Amt+15);
    } else {
      error("%Hshift amount must be an immediate", &EXPR_LOCATION(exp));
      Result = Ops[0];
    }
    return true;
  case ALTIVEC_BUILTIN_VPKUHUM:
    Ops[0] = Builder.CreateBitCast(Ops[0], ResultType, "tmp");
    Ops[1] = Builder.CreateBitCast(Ops[1], ResultType, "tmp");
    Result = BuildVectorShuffle(Ops[0], Ops[1], 1, 3, 5, 7, 9, 11, 13, 15,
                                17, 19, 21, 23, 25, 27, 29, 31);
    return true;
  case ALTIVEC_BUILTIN_VPKUWUM:
    Ops[0] = Builder.CreateBitCast(Ops[0], ResultType, "tmp");
    Ops[1] = Builder.CreateBitCast(Ops[1], ResultType, "tmp");
    Result = BuildVectorShuffle(Ops[0], Ops[1], 1, 3, 5, 7, 9, 11, 13, 15);
    return true;
  case ALTIVEC_BUILTIN_VMRGHB:
    Result = BuildVectorShuffle(Ops[0], Ops[1],
                                0, 16, 1, 17, 2, 18, 3, 19,
                                4, 20, 5, 21, 6, 22, 7, 23);
    return true;
  case ALTIVEC_BUILTIN_VMRGHH:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 0, 8, 1, 9, 2, 10, 3, 11);
    return true;
  case ALTIVEC_BUILTIN_VMRGHW:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 0, 4, 1, 5);
    return true;
  case ALTIVEC_BUILTIN_VMRGLB:
    Result = BuildVectorShuffle(Ops[0], Ops[1],
                                 8, 24,  9, 25, 10, 26, 11, 27,
                                12, 28, 13, 29, 14, 30, 15, 31);
    return true;
  case ALTIVEC_BUILTIN_VMRGLH:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 4, 12, 5, 13, 6, 14, 7, 15);
    return true;
  case ALTIVEC_BUILTIN_VMRGLW:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 2, 6, 3, 7);
    return true;
  case ALTIVEC_BUILTIN_ABS_V4SF: {
    // and out sign bits
    VectorType *v4i32 = VectorType::get(Type::Int32Ty, 4);
    Ops[0] = Builder.CreateBitCast(Ops[0], v4i32, "tmp");
    Constant *C = ConstantInt::get(Type::Int32Ty, 0x7FFFFFFF);
    C = ConstantVector::get(std::vector<Constant*>(4, C));
    Result = Builder.CreateAnd(Ops[0], C, "tmp");
    Result = Builder.CreateBitCast(Result, ResultType, "tmp");
    return true;
  }
  case ALTIVEC_BUILTIN_ABS_V4SI:
  case ALTIVEC_BUILTIN_ABS_V8HI:
  case ALTIVEC_BUILTIN_ABS_V16QI: { // iabs(x) -> smax(x, 0-x)
    Result = Builder.CreateNeg(Ops[0], "tmp");
    // get the right smax intrinsic.
    static const Intrinsic::ID smax_iid[3] = {
      Intrinsic::ppc_altivec_vmaxsw,
      Intrinsic::ppc_altivec_vmaxsh,
      Intrinsic::ppc_altivec_vmaxsb
    };
    const VectorType *PTy = cast<VectorType>(ResultType);
    unsigned N = GetAltivecTypeNumFromType(PTy->getElementType());
    Function *smax = Intrinsic::getDeclaration(TheModule, smax_iid[N]);
    Value *ActualOps[] = { Ops[0], Result };
    Result = Builder.CreateCall(smax, ActualOps, ActualOps+2, "tmp");
    return true;
  }
  case ALTIVEC_BUILTIN_ABSS_V4SI:
  case ALTIVEC_BUILTIN_ABSS_V8HI:
  case ALTIVEC_BUILTIN_ABSS_V16QI: { // iabss(x) -> smax(x, satsub(0,x))
    // get the right smax/subs intrinsics.
    static const Intrinsic::ID smax_iid[3] = {
      Intrinsic::ppc_altivec_vmaxsw,
      Intrinsic::ppc_altivec_vmaxsh,
      Intrinsic::ppc_altivec_vmaxsb
    };
    static const Intrinsic::ID subss_iid[3] = {
      Intrinsic::ppc_altivec_vsubsws,
      Intrinsic::ppc_altivec_vsubshs,
      Intrinsic::ppc_altivec_vsubsbs
    };
    
    // get the right satsub intrinsic.
    const VectorType *PTy = cast<VectorType>(ResultType);
    unsigned N = GetAltivecTypeNumFromType(PTy->getElementType());
    Function *smax = Intrinsic::getDeclaration(TheModule, smax_iid[N]);
    Function *subss = Intrinsic::getDeclaration(TheModule, subss_iid[N]);

    Value *ActualOps[] = { Constant::getNullValue(ResultType), Ops[0] };
    Result = Builder.CreateCall(subss, ActualOps, ActualOps+2, "tmp");
    ActualOps[0] = Ops[0];
    ActualOps[1] = Result;
    Result = Builder.CreateCall(smax, ActualOps, ActualOps+2, "tmp");
    return true;
  }
  }

  return false;
}

/* APPLE LOCAL end LLVM (ENTIRE FILE!)  */

