/* LLVM LOCAL begin (ENTIRE FILE!)  */
/* Processor ABI customization hooks
Copyright (C) 2005, 2006, 2007 Free Software Foundation, Inc.
Contributed by Chris Lattner (sabre@nondot.org)

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
// This is a C++ header file that specifies how argument values are passed and
// returned from function calls.  This allows the target to specialize handling
// of things like how structures are passed by-value.
//===----------------------------------------------------------------------===//

#ifndef LLVM_ABI_H
#define LLVM_ABI_H

#include "llvm-internal.h"
#include "llvm/Constants.h"
#include "llvm/DerivedTypes.h"
#include "llvm/ParameterAttributes.h"
#include "llvm/Target/TargetData.h"

namespace llvm {
  class BasicBlock;
}
extern "C" {
#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "tree.h"
}  

/// DefaultABIClient - This is a simple implementation of the ABI client
/// interface that can be subclassed.
struct DefaultABIClient {
  bool isStructReturn() { return false; }
  
  /// HandleScalarResult - This callback is invoked if the function returns a
  /// simple scalar result value.
  void HandleScalarResult(const Type *RetTy) {}

  /// HandleAggregateResultAsScalar - This callback is invoked if the function
  /// returns an aggregate value by bit converting it to the specified scalar
  /// type and returning that.
  void HandleAggregateResultAsScalar(const Type *ScalarTy) {}
  
  /// HandleAggregateShadowArgument - This callback is invoked if the function
  /// returns an aggregate value by using a "shadow" first parameter.  If RetPtr
  /// is set to true, the pointer argument itself is returned from the function.
  void HandleAggregateShadowArgument(const PointerType *PtrArgTy, bool RetPtr){}
  
  
  /// HandleScalarArgument - This is the primary callback that specifies an LLVM
  /// argument to pass.
  void HandleScalarArgument(const llvm::Type *LLVMTy, tree argTreeType) {}
  
  /// EnterField - Called when we're about the enter the field of a struct
  /// or union.  FieldNo is the number of the element we are entering in the
  /// LLVM Struct, StructTy is the LLVM type of the struct we are entering.
  void EnterField(unsigned FieldNo, const llvm::Type *StructTy) {}
  void ExitField() {}
};

/// isAggregateTreeType - Return true if the specified GCC type is an aggregate
/// that cannot live in an LLVM register.
static bool isAggregateTreeType(tree type) {
  return TREE_CODE(type) == RECORD_TYPE || TREE_CODE(type) == ARRAY_TYPE ||
         TREE_CODE(type) == UNION_TYPE  || TREE_CODE(type) == QUAL_UNION_TYPE ||
         TREE_CODE(type) == COMPLEX_TYPE;
}

/// isSingleElementStructOrArray - If this is (recursively) a structure with one
/// field or an array with one element, return the field type, otherwise return
/// null.
static tree isSingleElementStructOrArray(tree type) {
  // Scalars are good.
  if (!isAggregateTreeType(type)) return type;
  
  tree FoundField = 0;
  switch (TREE_CODE(type)) {
  case QUAL_UNION_TYPE:
  case UNION_TYPE:     // Single element unions don't count.
  case COMPLEX_TYPE:   // Complex values are like 2-element records.
  default:
    return 0;
  case RECORD_TYPE:
    // If this record has variable length, reject it.
    if (TREE_CODE(TYPE_SIZE(type)) != INTEGER_CST)
      return 0;

    for (tree Field = TYPE_FIELDS(type); Field; Field = TREE_CHAIN(Field))
      if (TREE_CODE(Field) == FIELD_DECL) {
        if (!FoundField)
          FoundField = getDeclaredType(Field);
        else
          return 0;   // More than one field.
      }
    return FoundField ? isSingleElementStructOrArray(FoundField) : 0;
  case ARRAY_TYPE:
    const ArrayType *Ty = dyn_cast<ArrayType>(ConvertType(type));
    if (!Ty || Ty->getNumElements() != 1)
      return 0;
    return isSingleElementStructOrArray(TREE_TYPE(type));
  }
}

// LLVM_SHOULD_PASS_AGGREGATE_IN_INTEGER_REGS - Return true if this aggregate
// value should be passed in integer registers.  By default, we do this for all
// values that are not single-element structs.  This ensures that things like
// {short,short} are passed in one 32-bit chunk, not as two arguments (which
// would often be 64-bits).
#ifndef LLVM_SHOULD_PASS_AGGREGATE_IN_INTEGER_REGS
#define LLVM_SHOULD_PASS_AGGREGATE_IN_INTEGER_REGS(X) \
   !isSingleElementStructOrArray(X)
#endif

// LLVM_SHOULD_PASS_AGGREGATE_USING_BYVAL_ATTR - Return true if this aggregate
// value should be passed by value, i.e. passing its address with the byval
// attribute bit set. The default is false.
#ifndef LLVM_SHOULD_PASS_AGGREGATE_USING_BYVAL_ATTR
#define LLVM_SHOULD_PASS_AGGREGATE_USING_BYVAL_ATTR(X) \
    false
#endif

/// DefaultABI - This class implements the default LLVM ABI where structures are
/// passed by decimating them into individual components and unions are passed
/// by passing the largest member of the union.
///
template<typename Client>
class DefaultABI {
protected:
  Client &C;
public:
  DefaultABI(Client &c) : C(c) {}

  bool isStructReturn() const { return C.isStructReturn(); }
  
  /// HandleReturnType - This is invoked by the target-independent code for the
  /// return type. It potentially breaks down the argument and invokes methods
  /// on the client that indicate how its pieces should be handled.  This
  /// handles things like returning structures via hidden parameters.
  void HandleReturnType(tree type) {
    const Type *Ty = ConvertType(type);
    if (Ty->isFirstClassType() || Ty == Type::VoidTy) {
      // Return scalar values normally.
      C.HandleScalarResult(Ty);
    } else if (TYPE_SIZE(type) && TREE_CODE(TYPE_SIZE(type)) == INTEGER_CST &&
               !aggregate_value_p(type, current_function_decl) &&
               // FIXME: this is a hack around returning 'complex double' by-val
               // which returns in r3/r4/r5/r6 on PowerPC.
               TREE_INT_CST_LOW(TYPE_SIZE_UNIT(type)) <= 8) {
      if (tree SingleElt = isSingleElementStructOrArray(type)) {
        C.HandleAggregateResultAsScalar(ConvertType(SingleElt));
      } else {
        // Otherwise return as an integer value large enough to hold the entire
        // aggregate.
        unsigned Size = TREE_INT_CST_LOW(TYPE_SIZE_UNIT(type));
        if (Size == 0)
          C.HandleAggregateResultAsScalar(Type::VoidTy);
        else if (Size == 1)
          C.HandleAggregateResultAsScalar(Type::Int8Ty);
        else if (Size == 2)
          C.HandleAggregateResultAsScalar(Type::Int16Ty);
        else if (Size <= 4)
          C.HandleAggregateResultAsScalar(Type::Int32Ty);
        else if (Size <= 8)
          C.HandleAggregateResultAsScalar(Type::Int64Ty);
        else {
          assert(0 && "Cannot return this aggregate as a scalar!");
          abort();
        }
      }
    } else {
      // If the function is returning a struct or union, we pass the pointer to
      // the struct as the first argument to the function.

      // FIXME: should return the hidden first argument for some targets
      // (e.g. ELF i386).
      C.HandleAggregateShadowArgument(PointerType::getUnqual(Ty), false);
    }
  }
  
  /// HandleArgument - This is invoked by the target-independent code for each
  /// argument type passed into the function.  It potentially breaks down the
  /// argument and invokes methods on the client that indicate how its pieces
  /// should be handled.  This handles things like decimating structures into
  /// their fields.
  void HandleArgument(tree type, uint16_t *Attributes = NULL) {
    const Type *Ty = ConvertType(type);

    if (isPassedByInvisibleReference(type)) { // variable size -> by-ref.
      C.HandleScalarArgument(PointerType::getUnqual(Ty), type);
    } else if (Ty->isFirstClassType()) {
      C.HandleScalarArgument(Ty, type);
    } else if (LLVM_SHOULD_PASS_AGGREGATE_USING_BYVAL_ATTR(type)) {
      C.HandleByValArgument(Ty, type);
      if (Attributes)
        *Attributes |= ParamAttr::ByVal;
    } else if (LLVM_SHOULD_PASS_AGGREGATE_IN_INTEGER_REGS(type)) {
      PassInIntegerRegisters(type, Ty);
    } else if (TREE_CODE(type) == RECORD_TYPE) {
      for (tree Field = TYPE_FIELDS(type); Field; Field = TREE_CHAIN(Field))
        if (TREE_CODE(Field) == FIELD_DECL) {
          unsigned FNo = GetFieldIndex(Field);
          assert(FNo != ~0U && "Case not handled yet!");

          C.EnterField(FNo, Ty);
          HandleArgument(getDeclaredType(Field));
          C.ExitField();
        }
    } else if (TREE_CODE(type) == COMPLEX_TYPE) {
      C.EnterField(0, Ty);
      HandleArgument(TREE_TYPE(type));
      C.ExitField();
      C.EnterField(1, Ty);
      HandleArgument(TREE_TYPE(type));
      C.ExitField();
    } else if ((TREE_CODE(type) == UNION_TYPE) ||
               (TREE_CODE(type) == QUAL_UNION_TYPE)) {
      HandleUnion(type);
    } else if (TREE_CODE(type) == ARRAY_TYPE) {
      const ArrayType *ATy = cast<ArrayType>(Ty);
      for (unsigned i = 0, e = ATy->getNumElements(); i != e; ++i) {
        C.EnterField(i, Ty);
        HandleArgument(TREE_TYPE(type));
        C.ExitField();
      }
    } else {
      assert(0 && "unknown aggregate type!");
      abort();
    }
  }

  /// HandleUnion - Handle a UNION_TYPE or QUAL_UNION_TYPE tree.
  ///
  void HandleUnion(tree type) {
    if (TYPE_TRANSPARENT_UNION(type)) {
      tree Field = TYPE_FIELDS(type);
      assert(Field && "Transparent union must have some elements!");
      while (TREE_CODE(Field) != FIELD_DECL) {
        Field = TREE_CHAIN(Field);
        assert(Field && "Transparent union must have some elements!");
      }
      
      HandleArgument(TREE_TYPE(Field));
    } else {
      // Unions pass the largest element.
      unsigned MaxSize = 0;
      tree MaxElt = 0;
      for (tree Field = TYPE_FIELDS(type); Field; Field = TREE_CHAIN(Field)) {
        if (TREE_CODE(Field) == FIELD_DECL) {
          // Skip fields that are known not to be present.
          if (TREE_CODE(type) == QUAL_UNION_TYPE &&
              integer_zerop(DECL_QUALIFIER(Field)))
              continue;

          tree SizeTree = TYPE_SIZE(TREE_TYPE(Field));
          unsigned Size = ((unsigned)TREE_INT_CST_LOW(SizeTree)+7)/8;
          if (Size > MaxSize) {
            MaxSize = Size;
            MaxElt = Field;
          }

          // Skip remaining fields if this one is known to be present.
          if (TREE_CODE(type) == QUAL_UNION_TYPE &&
              integer_onep(DECL_QUALIFIER(Field)))
              break;
        }
      }
      
      if (MaxElt)
        HandleArgument(TREE_TYPE(MaxElt));
    }
  }
    
  /// PassInIntegerRegisters - Given an aggregate value that should be passed in
  /// integer registers, convert it to a structure containing ints and pass all
  /// of the struct elements in.
  void PassInIntegerRegisters(tree type, const Type *Ty) {
    unsigned Size = TREE_INT_CST_LOW(TYPE_SIZE(type))/8;

    // FIXME: We should preserve all aggregate value alignment information.
    // Work around to preserve some aggregate value alignment information:
    // don't bitcast aggregate value to Int64 if its alignment is different
    // from Int64 alignment. ARM backend needs this.
    unsigned Align = TYPE_ALIGN(type)/8;
    unsigned Int64Align = getTargetData().getABITypeAlignment(Type::Int64Ty);
    bool UseInt64 = (Align >= Int64Align);

    // FIXME: In cases where we can, we should use the original struct.
    // Consider cases like { int, int } and {int, short} for example!  This will
    // produce far better LLVM code!
    std::vector<const Type*> Elts;

    unsigned ElementSize = UseInt64 ? 8:4;
    unsigned ArraySize = Size / ElementSize;

    const Type *ATy = NULL;
    const Type *ArrayElementType = NULL;
    if (ArraySize) {
      Size = Size % ElementSize;
      ArrayElementType = (UseInt64)?Type::Int64Ty:Type::Int32Ty;
      ATy = ArrayType::get(ArrayElementType, ArraySize);
      Elts.push_back(ATy);
    }

    if (Size >= 4) {
      Elts.push_back(Type::Int32Ty);
      Size -= 4;
    }
    if (Size >= 2) {
      Elts.push_back(Type::Int16Ty);
      Size -= 2;
    }
    if (Size >= 1) {
      Elts.push_back(Type::Int8Ty);
      Size -= 1;
    }
    assert(Size == 0 && "Didn't cover value?");
    const StructType *STy = StructType::get(Elts, false);

    unsigned i = 0;
    if (ArraySize) {
      C.EnterField(0, STy);
      for (unsigned j = 0; j < ArraySize; ++j) {
        C.EnterField(j, ATy);
        C.HandleScalarArgument(ArrayElementType, 0);
        C.ExitField();
      }
      C.ExitField();
      ++i;
    }
    for (unsigned e = Elts.size(); i != e; ++i) {
      C.EnterField(i, STy);
      C.HandleScalarArgument(Elts[i], 0);
      C.ExitField();
    }
  }
};

/// TheLLVMABI - This can be defined by targets if they want total control over
/// ABI decisions.
///
#ifndef TheLLVMABI
#define TheLLVMABI DefaultABI
#endif

#endif
/* LLVM LOCAL end (ENTIRE FILE!)  */
