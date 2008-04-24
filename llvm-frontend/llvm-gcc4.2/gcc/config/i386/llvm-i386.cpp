/* LLVM LOCAL begin (ENTIRE FILE!)  */
/* High-level LLVM backend interface 
Copyright (C) 2005 Free Software Foundation, Inc.
Contributed by Evan Cheng (evan.cheng@apple.com)

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
// This is a C++ source file that implements specific llvm IA-32 ABI.
//===----------------------------------------------------------------------===//

#include "llvm-abi.h"
#include "llvm-internal.h"
#include "llvm/Instructions.h"
#include "llvm/Intrinsics.h"
#include "llvm/Module.h"

extern "C" {
#include "toplev.h"
}

/* TargetIntrinsicLower - For builtins that we want to expand to normal LLVM
 * code, emit the code now.  If we can handle the code, this macro should emit
 * the code, return true.
 */
bool TreeToLLVM::TargetIntrinsicLower(tree exp,
                                      unsigned FnCode,
                                      const MemRef *DestLoc,
                                      Value *&Result,
                                      const Type *ResultType,
                                      std::vector<Value*> &Ops) {
  switch (FnCode) {
  default: break;
  case IX86_BUILTIN_ADDPS:
  case IX86_BUILTIN_ADDPD:
  case IX86_BUILTIN_PADDB:
  case IX86_BUILTIN_PADDW:
  case IX86_BUILTIN_PADDD:
  case IX86_BUILTIN_PADDQ:
  case IX86_BUILTIN_PADDB128:
  case IX86_BUILTIN_PADDW128:
  case IX86_BUILTIN_PADDD128:
  case IX86_BUILTIN_PADDQ128:
    Result = Builder.CreateAdd(Ops[0], Ops[1], "tmp");
    return true;
  case IX86_BUILTIN_SUBPS:
  case IX86_BUILTIN_SUBPD:
  case IX86_BUILTIN_PSUBB:
  case IX86_BUILTIN_PSUBW:
  case IX86_BUILTIN_PSUBD:
  case IX86_BUILTIN_PSUBQ:
  case IX86_BUILTIN_PSUBB128:
  case IX86_BUILTIN_PSUBW128:
  case IX86_BUILTIN_PSUBD128:
  case IX86_BUILTIN_PSUBQ128:
    Result = Builder.CreateSub(Ops[0], Ops[1], "tmp");
    return true;
  case IX86_BUILTIN_MULPS:
  case IX86_BUILTIN_MULPD:
  case IX86_BUILTIN_PMULLW:
  case IX86_BUILTIN_PMULLW128:
    Result = Builder.CreateMul(Ops[0], Ops[1], "tmp");
    return true;
  case IX86_BUILTIN_PSLLWI: {
    Function *psllw =
      Intrinsic::getDeclaration(TheModule, Intrinsic::x86_mmx_psll_w);
    Ops[1] = Builder.CreateZExt(Ops[1], Type::Int64Ty, "zext");
    Ops[1] = Builder.CreateBitCast(Ops[1], 
                                   VectorType::get(Type::Int64Ty, 1), 
                                   "bitcast");      
    Result = Builder.CreateCall(psllw, Ops.begin(), Ops.begin()+2, "tmp");
    Result = Builder.CreateBitCast(Result, ResultType, "tmp");
    return true;
  }
  case IX86_BUILTIN_PSLLWI128: {
    Function *psllw =
      Intrinsic::getDeclaration(TheModule, Intrinsic::x86_sse2_psll_w);
    Value *Undef = UndefValue::get(Type::Int32Ty);
    Ops[1] = BuildVector(Ops[1], Undef, Undef, Undef, NULL);
    Ops[1] = Builder.CreateBitCast(Ops[1], Ops[0]->getType(), "tmp");
    Result = Builder.CreateCall(psllw, Ops.begin(), Ops.begin()+2, "tmp");
    Result = Builder.CreateBitCast(Result, ResultType, "tmp");
    return true;
  }
  case IX86_BUILTIN_PSLLDI: {
    Function *pslld =
      Intrinsic::getDeclaration(TheModule, Intrinsic::x86_mmx_psll_d);
    Ops[1] = Builder.CreateZExt(Ops[1], Type::Int64Ty, "zext");
    Ops[1] = Builder.CreateBitCast(Ops[1], 
                                   VectorType::get(Type::Int64Ty, 1), 
                                   "bitcast");      
    Result = Builder.CreateCall(pslld, Ops.begin(), Ops.begin()+2, "tmp");
    Result = Builder.CreateBitCast(Result, ResultType, "tmp");
    return true;
  }
  case IX86_BUILTIN_PSLLDI128: {
    Function *pslld
      = Intrinsic::getDeclaration(TheModule, Intrinsic::x86_sse2_psll_d);
    Value *Undef = UndefValue::get(Type::Int32Ty);
    Ops[1] = BuildVector(Ops[1], Undef, Undef, Undef, NULL);
    Result = Builder.CreateCall(pslld, Ops.begin(), Ops.begin()+2, "tmp");
    Result = Builder.CreateBitCast(Result, ResultType, "tmp");
    return true;
  }
  case IX86_BUILTIN_PSLLQI: {
    Function *psllq =
      Intrinsic::getDeclaration(TheModule, Intrinsic::x86_mmx_psll_q);
    Ops[1] = Builder.CreateZExt(Ops[1], Type::Int64Ty, "zext");
    Ops[1] = Builder.CreateBitCast(Ops[1], 
                                   VectorType::get(Type::Int64Ty, 1), 
                                   "bitcast");      
    Result = Builder.CreateCall(psllq, Ops.begin(), Ops.begin()+2, "tmp");
    Result = Builder.CreateBitCast(Result, ResultType, "tmp");
    return true;
  }
  case IX86_BUILTIN_PSLLQI128: {
    Function *psllq =
      Intrinsic::getDeclaration(TheModule, Intrinsic::x86_sse2_psll_q);
    Value *Undef = UndefValue::get(Type::Int32Ty);
    Ops[1] = BuildVector(Ops[1], Undef, Undef, Undef, NULL);
    Ops[1] = Builder.CreateBitCast(Ops[1], Ops[0]->getType(), "tmp");
    Result = Builder.CreateCall(psllq, Ops.begin(), Ops.begin()+2, "tmp");
    Result = Builder.CreateBitCast(Result, ResultType, "tmp");
    return true;
  }
  case IX86_BUILTIN_PSRLWI: {
    Function *psrlw =
      Intrinsic::getDeclaration(TheModule, Intrinsic::x86_mmx_psrl_w);
    Ops[1] = Builder.CreateZExt(Ops[1], Type::Int64Ty, "zext");
    Ops[1] = Builder.CreateBitCast(Ops[1], 
                                   VectorType::get(Type::Int64Ty, 1), 
                                   "bitcast");      
    Result = Builder.CreateCall(psrlw, Ops.begin(), Ops.begin()+2, "tmp");
    Result = Builder.CreateBitCast(Result, ResultType, "tmp");
    return true;
  }
  case IX86_BUILTIN_PSRLWI128: {
    Function *psrlw =
      Intrinsic::getDeclaration(TheModule, Intrinsic::x86_sse2_psrl_w);
    Value *Undef = UndefValue::get(Type::Int32Ty);
    Ops[1] = BuildVector(Ops[1], Undef, Undef, Undef, NULL);
    Ops[1] = Builder.CreateBitCast(Ops[1], Ops[0]->getType(), "tmp");
    Result = Builder.CreateCall(psrlw, Ops.begin(), Ops.begin()+2, "tmp");
    Result = Builder.CreateBitCast(Result, ResultType, "tmp");
    return true;
  }
  case IX86_BUILTIN_PSRLDI: {
    Function *psrld =
      Intrinsic::getDeclaration(TheModule, Intrinsic::x86_mmx_psrl_d);
    Ops[1] = Builder.CreateZExt(Ops[1], Type::Int64Ty, "zext");
    Ops[1] = Builder.CreateBitCast(Ops[1], 
                                   VectorType::get(Type::Int64Ty, 1), 
                                   "bitcast");      
    Result = Builder.CreateCall(psrld, Ops.begin(), Ops.begin()+2, "tmp");
    Result = Builder.CreateBitCast(Result, ResultType, "tmp");
    return true;
  }
  case IX86_BUILTIN_PSRLDI128: {
    Function *psrld =
      Intrinsic::getDeclaration(TheModule, Intrinsic::x86_sse2_psrl_d);
    Value *Undef = UndefValue::get(Type::Int32Ty);
    Ops[1] = BuildVector(Ops[1], Undef, Undef, Undef, NULL);
    Result = Builder.CreateCall(psrld, Ops.begin(), Ops.begin()+2, "tmp");
    Result = Builder.CreateBitCast(Result, ResultType, "tmp");
    return true;
  }
  case IX86_BUILTIN_PSRLQI: {
    Function *psrlq =
      Intrinsic::getDeclaration(TheModule, Intrinsic::x86_mmx_psrl_q);
    Ops[1] = Builder.CreateZExt(Ops[1], Type::Int64Ty, "zext");
    Ops[1] = Builder.CreateBitCast(Ops[1], 
                                   VectorType::get(Type::Int64Ty, 1),
                                   "bitcast");      
    Result = Builder.CreateCall(psrlq, Ops.begin(), Ops.begin()+2, "tmp");
    Result = Builder.CreateBitCast(Result, ResultType, "tmp");
    return true;
  }
  case IX86_BUILTIN_PSRLQI128: {
    Function *psrlq =
      Intrinsic::getDeclaration(TheModule, Intrinsic::x86_sse2_psrl_q);
    Value *Undef = UndefValue::get(Type::Int32Ty);
    Ops[1] = BuildVector(Ops[1], Undef, Undef, Undef, NULL);
    Ops[1] = Builder.CreateBitCast(Ops[1], Ops[0]->getType(), "tmp");
    Result = Builder.CreateCall(psrlq, Ops.begin(), Ops.begin()+2, "tmp");
    Result = Builder.CreateBitCast(Result, ResultType, "tmp");
    return true;
  }
  case IX86_BUILTIN_PSRAWI: {
    Function *psraw =
      Intrinsic::getDeclaration(TheModule, Intrinsic::x86_mmx_psra_w);
    Ops[1] = Builder.CreateZExt(Ops[1], Type::Int64Ty, "zext");
    Ops[1] = Builder.CreateBitCast(Ops[1], 
                                   VectorType::get(Type::Int64Ty, 1),
                                   "bitcast");      
    Result = Builder.CreateCall(psraw, Ops.begin(), Ops.begin()+2, "tmp");
    Result = Builder.CreateBitCast(Result, ResultType, "tmp");
    return true;
  }
  case IX86_BUILTIN_PSRAWI128: {
    Function *psraw =
      Intrinsic::getDeclaration(TheModule, Intrinsic::x86_sse2_psra_w);
    Value *Undef = UndefValue::get(Type::Int32Ty);
    Ops[1] = BuildVector(Ops[1], Undef, Undef, Undef, NULL);
    Ops[1] = Builder.CreateBitCast(Ops[1], Ops[0]->getType(), "tmp");
    Result = Builder.CreateCall(psraw, Ops.begin(), Ops.begin()+2, "tmp");
    Result = Builder.CreateBitCast(Result, ResultType, "tmp");
    return true;
  }
  case IX86_BUILTIN_PSRADI: {
    Function *psrad =
      Intrinsic::getDeclaration(TheModule, Intrinsic::x86_mmx_psra_d);
    Ops[1] = Builder.CreateZExt(Ops[1], Type::Int64Ty, "zext");
    Ops[1] = Builder.CreateBitCast(Ops[1], 
                                   VectorType::get(Type::Int64Ty, 1),
                                   "bitcast");      
    Result = Builder.CreateCall(psrad, Ops.begin(), Ops.begin()+2, "tmp");
    Result = Builder.CreateBitCast(Result, ResultType, "tmp");
    return true;
  }
  case IX86_BUILTIN_PSRADI128: {
    Function *psrad =
      Intrinsic::getDeclaration(TheModule, Intrinsic::x86_sse2_psra_d);
    Value *Undef = UndefValue::get(Type::Int32Ty);
    Ops[1] = BuildVector(Ops[1], Undef, Undef, Undef, NULL);
    Result = Builder.CreateCall(psrad, Ops.begin(), Ops.begin()+2, "tmp");
    Result = Builder.CreateBitCast(Result, ResultType, "tmp");
    return true;
  }
  case IX86_BUILTIN_DIVPS:
  case IX86_BUILTIN_DIVPD:
    Result = Builder.CreateFDiv(Ops[0], Ops[1], "tmp");
    return true;
  case IX86_BUILTIN_PAND:
  case IX86_BUILTIN_PAND128:
    Result = Builder.CreateAnd(Ops[0], Ops[1], "tmp");
    return true;
  case IX86_BUILTIN_PANDN:
  case IX86_BUILTIN_PANDN128:
    Ops[0] = Builder.CreateNot(Ops[0], "tmp");
    Result = Builder.CreateAnd(Ops[0], Ops[1], "tmp");
    return true;
  case IX86_BUILTIN_POR:
  case IX86_BUILTIN_POR128:
    Result = Builder.CreateOr(Ops[0], Ops[1], "tmp");
    return true;
  case IX86_BUILTIN_PXOR:
  case IX86_BUILTIN_PXOR128:
    Result = Builder.CreateXor(Ops[0], Ops[1], "tmp");
    return true;
  case IX86_BUILTIN_ANDPS:
  case IX86_BUILTIN_ORPS:
  case IX86_BUILTIN_XORPS:
  case IX86_BUILTIN_ANDNPS:
  case IX86_BUILTIN_ANDPD:
  case IX86_BUILTIN_ORPD:
  case IX86_BUILTIN_XORPD:
  case IX86_BUILTIN_ANDNPD:
    if (cast<VectorType>(ResultType)->getNumElements() == 4)  // v4f32
      Ops[0] = Builder.CreateBitCast(Ops[0], VectorType::get(Type::Int32Ty, 4),
                                     "tmp");
    else                                                      // v2f64
      Ops[0] = Builder.CreateBitCast(Ops[0], VectorType::get(Type::Int64Ty, 2),
                                     "tmp");
    
    Ops[1] = Builder.CreateBitCast(Ops[1], Ops[0]->getType(), "tmp");
    switch (FnCode) {
      case IX86_BUILTIN_ANDPS:
      case IX86_BUILTIN_ANDPD:
        Result = Builder.CreateAnd(Ops[0], Ops[1], "tmp");
        break;
      case IX86_BUILTIN_ORPS:
      case IX86_BUILTIN_ORPD:
        Result = Builder.CreateOr (Ops[0], Ops[1], "tmp");
         break;
      case IX86_BUILTIN_XORPS:
      case IX86_BUILTIN_XORPD:
        Result = Builder.CreateXor(Ops[0], Ops[1], "tmp");
        break;
      case IX86_BUILTIN_ANDNPS:
      case IX86_BUILTIN_ANDNPD:
        Ops[0] = Builder.CreateNot(Ops[0], "tmp");
        Result = Builder.CreateAnd(Ops[0], Ops[1], "tmp");
        break;
    }
    Result = Builder.CreateBitCast(Result, ResultType, "tmp");
    return true;
  case IX86_BUILTIN_SHUFPS:
    if (ConstantInt *Elt = dyn_cast<ConstantInt>(Ops[2])) {
      int EV = Elt->getZExtValue();
      Result = BuildVectorShuffle(Ops[0], Ops[1],
                                  ((EV & 0x03) >> 0),   ((EV & 0x0c) >> 2),
                                  ((EV & 0x30) >> 4)+4, ((EV & 0xc0) >> 6)+4);
    } else {
      error("%Hmask must be an immediate", &EXPR_LOCATION(exp));
      Result = Ops[0];
    }
    return true;
  case IX86_BUILTIN_PSHUFW:
  case IX86_BUILTIN_PSHUFD:
    if (ConstantInt *Elt = dyn_cast<ConstantInt>(Ops[1])) {
      int EV = Elt->getZExtValue();
      Result = BuildVectorShuffle(Ops[0], Ops[0],
                                  ((EV & 0x03) >> 0),   ((EV & 0x0c) >> 2),
                                  ((EV & 0x30) >> 4),   ((EV & 0xc0) >> 6));
    } else {
      error("%Hmask must be an immediate", &EXPR_LOCATION(exp));
      Result = Ops[0];
    }
    return true;
  case IX86_BUILTIN_PSHUFHW:
    if (ConstantInt *Elt = dyn_cast<ConstantInt>(Ops[1])) {
      int EV = Elt->getZExtValue();
      Result = BuildVectorShuffle(Ops[0], Ops[0],
                                  0, 1, 2, 3,
                                  ((EV & 0x03) >> 0)+4, ((EV & 0x0c) >> 2)+4,
                                  ((EV & 0x30) >> 4)+4, ((EV & 0xc0) >> 6)+4);
      return true;
    }
    return false;
  case IX86_BUILTIN_PSHUFLW:
    if (ConstantInt *Elt = dyn_cast<ConstantInt>(Ops[1])) {
      int EV = Elt->getZExtValue();
      Result = BuildVectorShuffle(Ops[0], Ops[0],
                                  ((EV & 0x03) >> 0),   ((EV & 0x0c) >> 2),
                                  ((EV & 0x30) >> 4),   ((EV & 0xc0) >> 6),
                                  4, 5, 6, 7);
    } else {
      error("%Hmask must be an immediate", &EXPR_LOCATION(exp));
      Result = Ops[0];
    }
    
    return true;
  case IX86_BUILTIN_PUNPCKHBW:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 4, 12, 5, 13,
                                                6, 14, 7, 15);
    return true;
  case IX86_BUILTIN_PUNPCKHWD:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 2, 6, 3, 7);
    return true;
  case IX86_BUILTIN_PUNPCKHDQ:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 1, 3);
    return true;
  case IX86_BUILTIN_PUNPCKLBW:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 0,  8, 1,  9,
                                                2, 10, 3, 11);
    return true;
  case IX86_BUILTIN_PUNPCKLWD:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 0, 4, 1, 5);
    return true;
  case IX86_BUILTIN_PUNPCKLDQ:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 0, 2);
    return true;
  case IX86_BUILTIN_PUNPCKHBW128:
    Result = BuildVectorShuffle(Ops[0], Ops[1],  8, 24,  9, 25,
                                                10, 26, 11, 27,
                                                12, 28, 13, 29,
                                                14, 30, 15, 31);
    return true;
  case IX86_BUILTIN_PUNPCKHWD128:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 4, 12, 5, 13, 6, 14, 7, 15);
    return true;
  case IX86_BUILTIN_PUNPCKHDQ128:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 2, 6, 3, 7);
    return true;
  case IX86_BUILTIN_PUNPCKLBW128:
    Result = BuildVectorShuffle(Ops[0], Ops[1],  0, 16,  1, 17,
                                                 2, 18,  3, 19,
                                                 4, 20,  5, 21,
                                                 6, 22,  7, 23);
    return true;
  case IX86_BUILTIN_PUNPCKLWD128:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 0, 8, 1, 9, 2, 10, 3, 11);
    return true;
  case IX86_BUILTIN_PUNPCKLDQ128:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 0, 4, 1, 5);
    return true;
  case IX86_BUILTIN_UNPCKHPS:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 2, 6, 3, 7);
    return true;
  case IX86_BUILTIN_UNPCKLPS:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 0, 4, 1, 5);
    return true;
  case IX86_BUILTIN_MOVHLPS:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 6, 7, 2, 3);
    return true;
  case IX86_BUILTIN_MOVLHPS:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 0, 1, 4, 5);
    return true;
  case IX86_BUILTIN_MOVSS:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 4, 1, 2, 3);
    return true;
  case IX86_BUILTIN_MOVQ: {
    Value *Zero = ConstantInt::get(Type::Int32Ty, 0);
    Ops[1] = BuildVector(Zero, Zero, Zero, Zero, NULL);
    Result = BuildVectorShuffle(Ops[1], Ops[0], 4, 5, 2, 3);
    return true;
  }
  case IX86_BUILTIN_LOADQ: {
    PointerType *f64Ptr = PointerType::getUnqual(Type::DoubleTy);
    Value *Zero = ConstantFP::get(Type::DoubleTy, APFloat(0.0));
    Ops[0] = Builder.CreateBitCast(Ops[0], f64Ptr, "tmp");
    Ops[0] = Builder.CreateLoad(Ops[0], "tmp");
    Result = BuildVector(Ops[0], Zero, NULL);
    Result = Builder.CreateBitCast(Result, ResultType, "tmp");
    return true;
  }
  case IX86_BUILTIN_LOADHPS: {
    PointerType *f64Ptr = PointerType::getUnqual(Type::DoubleTy);
    Ops[1] = Builder.CreateBitCast(Ops[1], f64Ptr, "tmp");
    Value *Load = Builder.CreateLoad(Ops[1], "tmp");
    Ops[1] = BuildVector(Load, UndefValue::get(Type::DoubleTy), NULL);
    Ops[1] = Builder.CreateBitCast(Ops[1], ResultType, "tmp");
    Result = BuildVectorShuffle(Ops[0], Ops[1], 0, 1, 4, 5);
    Result = Builder.CreateBitCast(Result, ResultType, "tmp");
    return true;
  }
  case IX86_BUILTIN_LOADLPS: {
    PointerType *f64Ptr = PointerType::getUnqual(Type::DoubleTy);
    Ops[1] = Builder.CreateBitCast(Ops[1], f64Ptr, "tmp");
    Value *Load = Builder.CreateLoad(Ops[1], "tmp");
    Ops[1] = BuildVector(Load, UndefValue::get(Type::DoubleTy), NULL);
    Ops[1] = Builder.CreateBitCast(Ops[1], ResultType, "tmp");
    Result = BuildVectorShuffle(Ops[0], Ops[1], 4, 5, 2, 3);
    Result = Builder.CreateBitCast(Result, ResultType, "tmp");
    return true;
  }
  case IX86_BUILTIN_STOREHPS: {
    VectorType *v2f64 = VectorType::get(Type::DoubleTy, 2);
    PointerType *f64Ptr = PointerType::getUnqual(Type::DoubleTy);
    Ops[0] = Builder.CreateBitCast(Ops[0], f64Ptr, "tmp");
    Value *Idx = ConstantInt::get(Type::Int32Ty, 1);
    Ops[1] = Builder.CreateBitCast(Ops[1], v2f64, "tmp");
    Ops[1] = Builder.CreateExtractElement(Ops[1], Idx, "tmp");
    Result = Builder.CreateStore(Ops[1], Ops[0]);
    return true;
  }
  case IX86_BUILTIN_STORELPS: {
    VectorType *v2f64 = VectorType::get(Type::DoubleTy, 2);
    PointerType *f64Ptr = PointerType::getUnqual(Type::DoubleTy);
    Ops[0] = Builder.CreateBitCast(Ops[0], f64Ptr, "tmp");
    Value *Idx = ConstantInt::get(Type::Int32Ty, 0);
    Ops[1] = Builder.CreateBitCast(Ops[1], v2f64, "tmp");
    Ops[1] = Builder.CreateExtractElement(Ops[1], Idx, "tmp");
    Result = Builder.CreateStore(Ops[1], Ops[0]);
    return true;
  }
  case IX86_BUILTIN_MOVSHDUP:
    Result = BuildVectorShuffle(Ops[0], Ops[0], 1, 1, 3, 3);
    return true;
  case IX86_BUILTIN_MOVSLDUP:
    Result = BuildVectorShuffle(Ops[0], Ops[0], 0, 0, 2, 2);
    return true;
  case IX86_BUILTIN_VEC_INIT_V2SI:
    Result = BuildVector(Ops[0], Ops[1], NULL);
    return true;
  case IX86_BUILTIN_VEC_INIT_V4HI:
    // Sometimes G++ promotes arguments to int.
    for (unsigned i = 0; i != 4; ++i)
      Ops[i] = Builder.CreateIntCast(Ops[i], Type::Int16Ty, false, "tmp");
    Result = BuildVector(Ops[0], Ops[1], Ops[2], Ops[3], NULL);
    return true;
  case IX86_BUILTIN_VEC_INIT_V8QI:
    // Sometimes G++ promotes arguments to int.
    for (unsigned i = 0; i != 8; ++i)
      Ops[i] = Builder.CreateIntCast(Ops[i], Type::Int8Ty, false, "tmp");
    Result = BuildVector(Ops[0], Ops[1], Ops[2], Ops[3],
                         Ops[4], Ops[5], Ops[6], Ops[7], NULL);
    return true;
  case IX86_BUILTIN_VEC_EXT_V2SI:
  case IX86_BUILTIN_VEC_EXT_V4HI:
  case IX86_BUILTIN_VEC_EXT_V2DF:
  case IX86_BUILTIN_VEC_EXT_V2DI:
  case IX86_BUILTIN_VEC_EXT_V4SI:
  case IX86_BUILTIN_VEC_EXT_V4SF:
  case IX86_BUILTIN_VEC_EXT_V8HI:
    Result = Builder.CreateExtractElement(Ops[0], Ops[1], "tmp");
    return true;
  case IX86_BUILTIN_VEC_SET_V4HI:
  case IX86_BUILTIN_VEC_SET_V8HI:
    // GCC sometimes doesn't produce the right element type.
    Ops[1] = Builder.CreateIntCast(Ops[1], Type::Int16Ty, false, "tmp");
    Result = Builder.CreateInsertElement(Ops[0], Ops[1], Ops[2], "tmp");
    return true;
  case IX86_BUILTIN_CMPEQPS:
  case IX86_BUILTIN_CMPLTPS:
  case IX86_BUILTIN_CMPLEPS:
  case IX86_BUILTIN_CMPGTPS:
  case IX86_BUILTIN_CMPGEPS:
  case IX86_BUILTIN_CMPNEQPS:
  case IX86_BUILTIN_CMPNLTPS:
  case IX86_BUILTIN_CMPNLEPS:
  case IX86_BUILTIN_CMPNGTPS:
  case IX86_BUILTIN_CMPNGEPS:
  case IX86_BUILTIN_CMPORDPS:
  case IX86_BUILTIN_CMPUNORDPS: {
    Function *cmpps =
      Intrinsic::getDeclaration(TheModule, Intrinsic::x86_sse_cmp_ps);
    bool flip = false;
    unsigned PredCode;
    switch (FnCode) {
    default: assert(0 && "Unknown fncode!");
    case IX86_BUILTIN_CMPEQPS: PredCode = 0; break;
    case IX86_BUILTIN_CMPLTPS: PredCode = 1; break;
    case IX86_BUILTIN_CMPGTPS: PredCode = 1; flip = true; break;
    case IX86_BUILTIN_CMPLEPS: PredCode = 2; break;
    case IX86_BUILTIN_CMPGEPS: PredCode = 2; flip = true; break;
    case IX86_BUILTIN_CMPUNORDPS: PredCode = 3; break;
    case IX86_BUILTIN_CMPNEQPS: PredCode = 4; break;
    case IX86_BUILTIN_CMPNLTPS: PredCode = 5; break;
    case IX86_BUILTIN_CMPNGTPS: PredCode = 5; flip = true; break;
    case IX86_BUILTIN_CMPNLEPS: PredCode = 6; break;
    case IX86_BUILTIN_CMPNGEPS: PredCode = 6; flip = true; break;
    case IX86_BUILTIN_CMPORDPS: PredCode = 7; break;
    }
    Value *Pred = ConstantInt::get(Type::Int8Ty, PredCode);
    Value *Arg0 = Ops[0];
    Value *Arg1 = Ops[1];
    if (flip) std::swap(Arg0, Arg1);
    Value *CallOps[3] = { Arg0, Arg1, Pred };
    Result = Builder.CreateCall(cmpps, CallOps, CallOps+3, "tmp");
    Result = Builder.CreateBitCast(Result, ResultType, "tmp");
    return true;
  }
  case IX86_BUILTIN_CMPEQSS:
  case IX86_BUILTIN_CMPLTSS:
  case IX86_BUILTIN_CMPLESS:
  case IX86_BUILTIN_CMPNEQSS:
  case IX86_BUILTIN_CMPNLTSS:
  case IX86_BUILTIN_CMPNLESS:
  case IX86_BUILTIN_CMPNGTSS:
  case IX86_BUILTIN_CMPNGESS:
  case IX86_BUILTIN_CMPORDSS:
  case IX86_BUILTIN_CMPUNORDSS: {
    Function *cmpss =
      Intrinsic::getDeclaration(TheModule, Intrinsic::x86_sse_cmp_ss);
    unsigned PredCode;
    switch (FnCode) {
    default: assert(0 && "Unknown fncode");
    case IX86_BUILTIN_CMPEQSS:    PredCode = 0; break;
    case IX86_BUILTIN_CMPLTSS:    PredCode = 1; break;
    case IX86_BUILTIN_CMPLESS:    PredCode = 2; break;
    case IX86_BUILTIN_CMPUNORDSS: PredCode = 3; break;
    case IX86_BUILTIN_CMPNEQSS:   PredCode = 4; break;
    case IX86_BUILTIN_CMPNLTSS:   PredCode = 5; break;
    case IX86_BUILTIN_CMPNLESS:   PredCode = 6; break;
    case IX86_BUILTIN_CMPORDSS:   PredCode = 7; break;
    }
    Value *Pred = ConstantInt::get(Type::Int8Ty, PredCode);
    Value *CallOps[3] = { Ops[0], Ops[1], Pred };
    Result = Builder.CreateCall(cmpss, CallOps, CallOps+3, "tmp");
    Result = Builder.CreateBitCast(Result, ResultType, "tmp");
    return true;
  }
  case IX86_BUILTIN_CMPEQPD:
  case IX86_BUILTIN_CMPLTPD:
  case IX86_BUILTIN_CMPLEPD:
  case IX86_BUILTIN_CMPGTPD:
  case IX86_BUILTIN_CMPGEPD:
  case IX86_BUILTIN_CMPNEQPD:
  case IX86_BUILTIN_CMPNLTPD:
  case IX86_BUILTIN_CMPNLEPD:
  case IX86_BUILTIN_CMPNGTPD:
  case IX86_BUILTIN_CMPNGEPD:
  case IX86_BUILTIN_CMPORDPD:
  case IX86_BUILTIN_CMPUNORDPD: {
    Function *cmppd =
      Intrinsic::getDeclaration(TheModule, Intrinsic::x86_sse2_cmp_pd);
    bool flip = false;
    unsigned PredCode;
    switch (FnCode) {
    default: assert(0 && "Unknown fncode!");
    case IX86_BUILTIN_CMPEQPD:    PredCode = 0; break;
    case IX86_BUILTIN_CMPLTPD:    PredCode = 1; break;
    case IX86_BUILTIN_CMPGTPD:    PredCode = 1; flip = true; break;
    case IX86_BUILTIN_CMPLEPD:    PredCode = 2; break;
    case IX86_BUILTIN_CMPGEPD:    PredCode = 2; flip = true; break;
    case IX86_BUILTIN_CMPUNORDPD: PredCode = 3; break;
    case IX86_BUILTIN_CMPNEQPD:   PredCode = 4; break;
    case IX86_BUILTIN_CMPNLTPD:   PredCode = 5; break;
    case IX86_BUILTIN_CMPNGTPD:   PredCode = 5; flip = true; break;
    case IX86_BUILTIN_CMPNLEPD:   PredCode = 6; break;
    case IX86_BUILTIN_CMPNGEPD:   PredCode = 6; flip = true; break;
    case IX86_BUILTIN_CMPORDPD:   PredCode = 7; break;
    }
    Value *Pred = ConstantInt::get(Type::Int8Ty, PredCode);
    Value *Arg0 = Ops[0];
    Value *Arg1 = Ops[1];
    if (flip) std::swap(Arg0, Arg1);

    Value *CallOps[3] = { Arg0, Arg1, Pred };
    Result = Builder.CreateCall(cmppd, CallOps, CallOps+3, "tmp");
    Result = Builder.CreateBitCast(Result, ResultType, "tmp");
    return true;
  }
  case IX86_BUILTIN_CMPEQSD:
  case IX86_BUILTIN_CMPLTSD:
  case IX86_BUILTIN_CMPLESD:
  case IX86_BUILTIN_CMPNEQSD:
  case IX86_BUILTIN_CMPNLTSD:
  case IX86_BUILTIN_CMPNLESD:
  case IX86_BUILTIN_CMPORDSD:
  case IX86_BUILTIN_CMPUNORDSD: {
    Function *cmpsd =
      Intrinsic::getDeclaration(TheModule, Intrinsic::x86_sse2_cmp_sd);
    unsigned PredCode;
    switch (FnCode) {
      default: assert(0 && "Unknown fncode");
    case IX86_BUILTIN_CMPEQSD:    PredCode = 0; break;
    case IX86_BUILTIN_CMPLTSD:    PredCode = 1; break;
    case IX86_BUILTIN_CMPLESD:    PredCode = 2; break;
    case IX86_BUILTIN_CMPUNORDSD: PredCode = 3; break;
    case IX86_BUILTIN_CMPNEQSD:   PredCode = 4; break;
    case IX86_BUILTIN_CMPNLTSD:   PredCode = 5; break;
    case IX86_BUILTIN_CMPNLESD:   PredCode = 6; break;
    case IX86_BUILTIN_CMPORDSD:   PredCode = 7; break;
    }
    Value *Pred = ConstantInt::get(Type::Int8Ty, PredCode);
    Value *CallOps[3] = { Ops[0], Ops[1], Pred };
    Result = Builder.CreateCall(cmpsd, CallOps, CallOps+3, "tmp");
    Result = Builder.CreateBitCast(Result, ResultType, "tmp");
    return true;
  }
  case IX86_BUILTIN_LDMXCSR: {
    Function *ldmxcsr =
      Intrinsic::getDeclaration(TheModule, Intrinsic::x86_sse_ldmxcsr);
    Value *Ptr = CreateTemporary(Type::Int32Ty);
    Builder.CreateStore(Ops[0], Ptr);
    Ptr = Builder.CreateBitCast(Ptr, PointerType::getUnqual(Type::Int8Ty), "tmp");
    Result = Builder.CreateCall(ldmxcsr, Ptr);
    return true;
  }
  case IX86_BUILTIN_STMXCSR: {
    Function *stmxcsr =
      Intrinsic::getDeclaration(TheModule, Intrinsic::x86_sse_stmxcsr);
    Value *Ptr  = CreateTemporary(Type::Int32Ty);
    Value *BPtr = Builder.CreateBitCast(Ptr, PointerType::getUnqual(Type::Int8Ty),
                                        "tmp");
    Builder.CreateCall(stmxcsr, BPtr);
    
    Result = Builder.CreateLoad(Ptr, "tmp");
    return true;
  }
  }

  return false;
}

/* These are defined in i386.c */
extern "C" enum machine_mode ix86_getNaturalModeForType(tree);
extern "C" int ix86_HowToPassArgument(enum machine_mode, tree, int, int*, int*);

/* Target hook for llvm-abi.h. It returns true if an aggregate of the
   specified type should be passed in memory. This is only called for
   x86-64. */
bool llvm_x86_64_should_pass_aggregate_in_memory(tree type) {
  int IntRegs, SSERegs;
  enum machine_mode Mode = ix86_getNaturalModeForType(type);
  /* If ix86_HowToPassArgument return 0, then it's passed byval in memory.*/
  return !ix86_HowToPassArgument(Mode, type, 0, &IntRegs, &SSERegs);
}

/* LLVM LOCAL end (ENTIRE FILE!)  */
