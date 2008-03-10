/* APPLE LOCAL begin LLVM (ENTIRE FILE!)  */
/* Internal interfaces between the LLVM backend components
Copyright (C) 2005, 2006, 2007 Free Software Foundation, Inc.
Contributed by Chris Lattner  (sabre@nondot.org)

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
// This is a C++ header file that defines the internal interfaces shared among
// the llvm-*.cpp files.
//===----------------------------------------------------------------------===//

#ifndef LLVM_INTERNAL_H
#define LLVM_INTERNAL_H

#include <vector>
#include <cassert>
#include <map>
#include <string>
#include "llvm/Intrinsics.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/Support/DataTypes.h"
#include "llvm/Support/LLVMBuilder.h"
#include "llvm/Support/Streams.h"

extern "C" {
#include "llvm.h"
}

namespace llvm {
  class Module;
  class GlobalVariable;
  class Function;
  class GlobalValue;
  class BasicBlock;
  class Instruction;
  class AllocaInst;
  class BranchInst;
  class Value;
  class Constant;
  class ConstantInt;
  class Type;
  class FunctionType;
  class TargetMachine;
  class TargetData;
  class DebugInfo;
}
using namespace llvm;

/// TheModule - This is the current global module that we are compiling into.
///
extern llvm::Module *TheModule;

/// TheDebugInfo - This object is responsible for gather all debug information.
/// If it's value is NULL then no debug information should be gathered.
extern llvm::DebugInfo *TheDebugInfo;

/// TheTarget - The current target being compiled for.
///
extern llvm::TargetMachine *TheTarget;

/// getTargetData - Return the current TargetData object from TheTarget.
const TargetData &getTargetData();

/// AsmOutFile - A C++ ostream wrapper around asm_out_file.
///
extern llvm::OStream *AsmOutFile;

/// StaticCtors/StaticDtors - The static constructors and destructors that we
/// need to emit.
extern std::vector<std::pair<Function*, int> > StaticCtors, StaticDtors;

/// AttributeUsedGlobals - The list of globals that are marked attribute(used).
extern SmallSetVector<Constant *,32> AttributeUsedGlobals;

/// AttributeNoinlineFunctions - The list of functions that are 
/// marked attribute(noinline)
extern std::vector<Constant*> AttributeNoinlineFunctions;

extern Constant* ConvertMetadataStringToGV(const char* str);

/// AddAnnotateAttrsToGlobal - Adds decls that have a
/// annotate attribute to a vector to be emitted later.
extern void AddAnnotateAttrsToGlobal(GlobalValue *GV, union tree_node* decl);

void changeLLVMValue(Value *Old, Value *New);
void readLLVMTypesStringTable();
void writeLLVMTypesStringTable();
void readLLVMValues();
void writeLLVMValues();
void clearTargetBuiltinCache();

struct StructTypeConversionInfo;

/// Return true if and only if field no. N from struct type T is a padding
/// element added to match llvm struct type size and gcc struct type size.
bool isPaddingElement(const Type *T, unsigned N);

/// TypeConverter - Implement the converter from GCC types to LLVM types.
///
class TypeConverter {
  /// ConvertingStruct - If we are converting a RECORD or UNION to an LLVM type
  /// we set this flag to true.
  bool ConvertingStruct;
  
  /// PointersToReresolve - When ConvertingStruct is true, we handling of
  /// POINTER_TYPE and REFERENCE_TYPE is changed to return opaque*'s instead of
  /// recursively calling ConvertType.  When this happens, we add the
  /// POINTER_TYPE to this list.
  ///
  std::vector<tree_node*> PointersToReresolve;
public:
  TypeConverter() : ConvertingStruct(false) {}
  
  const Type *ConvertType(tree_node *type);
  
  /// GCCTypeOverlapsWithLLVMTypePadding - Return true if the specified GCC type
  /// has any data that overlaps with structure padding in the specified LLVM
  /// type.
  static bool GCCTypeOverlapsWithLLVMTypePadding(tree_node *t, const Type *Ty);
  
  
  /// ConvertFunctionType - Convert the specified FUNCTION_TYPE or METHOD_TYPE
  /// tree to an LLVM type.  This does the same thing that ConvertType does, but
  /// it also returns the function's LLVM calling convention.
  const FunctionType *ConvertFunctionType(tree_node *type,
                                          tree_node *decl,
                                          tree_node *static_chain,
                                          unsigned &CallingConv);
  
  /// ConvertArgListToFnType - Given a DECL_ARGUMENTS list on an GCC tree,
  /// return the LLVM type corresponding to the function.  This is useful for
  /// turning "T foo(...)" functions into "T foo(void)" functions.
  const FunctionType *ConvertArgListToFnType(tree_node *retty,
                                             tree_node *arglist,
                                             tree_node *static_chain,
                                             unsigned &CallingConv);
  
private:
  const Type *ConvertRECORD(tree_node *type, tree_node *orig_type);
  const Type *ConvertUNION(tree_node *type, tree_node *orig_type);
  void DecodeStructFields(tree_node *Field, StructTypeConversionInfo &Info);
  void DecodeStructBitField(tree_node *Field, StructTypeConversionInfo &Info);
};

extern TypeConverter *TheTypeConverter;

/// ConvertType - Convert the specified tree type to an LLVM type.
///
inline const Type *ConvertType(tree_node *type) {
  return TheTypeConverter->ConvertType(type);
}

/// isInt64 - Return true if t is an INTEGER_CST that fits in a 64 bit integer.
/// If Unsigned is false, returns whether it fits in a int64_t.  If Unsigned is
/// true, returns whether the value is non-negative and fits in a uint64_t.
/// Always returns false for overflowed constants.
bool isInt64(tree_node *t, bool Unsigned);

/// getInt64 - Extract the value of an INTEGER_CST as a 64 bit integer.  If
/// Unsigned is false, the value must fit in a int64_t.  If Unsigned is true,
/// the value must be non-negative and fit in a uint64_t.  Must not be used on
/// overflowed constants.  These conditions can be checked by calling isInt64.
uint64_t getInt64(tree_node *t, bool Unsigned);

/// isPassedByInvisibleReference - Return true if the specified type should be
/// passed by 'invisible reference'. In other words, instead of passing the
/// thing by value, pass the address of a temporary.
bool isPassedByInvisibleReference(tree_node *type);

/// isSequentialCompatible - Return true if the specified gcc array or pointer
/// type and the corresponding LLVM SequentialType lay out their components
/// identically in memory.
bool isSequentialCompatible(tree_node *type);

/// isArrayCompatible - Return true if the specified gcc array or pointer type
/// corresponds to an LLVM array type.
bool isArrayCompatible(tree_node *type);

/// arrayLength - Return a tree expressing the number of elements in an array
/// of the specified type, or NULL if the type does not specify the length.
tree_node *arrayLength(tree_node *type);

/// ValidateRegisterVariable - Check that a static "asm" variable is
/// well-formed.  If not, emit error messages and return true.  If so, return
/// false.
bool ValidateRegisterVariable(tree_node *decl);

/// LValue - This struct represents an lvalue in the program.  In particular,
/// the Ptr member indicates the memory that the lvalue lives in.  If this is
/// a bitfield reference, BitStart indicates the first bit in the memory that
/// is part of the field and BitSize indicates the extent.
///
/// "LValue" is intended to be a light-weight object passed around by-value.
struct LValue {
  Value *Ptr;
  unsigned char BitStart;
  unsigned char BitSize;
  
  LValue(Value *P) : Ptr(P), BitStart(255), BitSize(255) {}
  LValue(Value *P, unsigned BSt, unsigned BSi) 
    : Ptr(P), BitStart(BSt), BitSize(BSi) {
      assert(BitStart == BSt && BitSize == BSi &&
             "Bit values larger than 256?");
  }
  
  bool isBitfield() const { return BitStart != 255; }
};

/// TreeToLLVM - An instance of this class is created and used to convert the
/// body of each function to LLVM.
///
class TreeToLLVM {
  // State that is initialized when the function starts.
  const TargetData &TD;
  tree_node *FnDecl;
  Function *Fn;
  BasicBlock *ReturnBB;
  BasicBlock *UnwindBB;
  
  // State that changes as the function is emitted.

  /// Builder - Instruction creator, the location to insert into is always the
  /// same as &Fn->back().
  LLVMBuilder Builder;

  // AllocaInsertionPoint - Place to insert alloca instructions.  Lazily created
  // and managed by CreateTemporary.
  Instruction *AllocaInsertionPoint;
  
  //===-------------- Exception / Finally Block Handling ------------------===//
  
  struct BranchFixup {
    /// SrcBranch - This is the unconditional branch instruction that we are 
    /// fixing up.  The destination of the fixup is the dest of the uncond
    /// branch.
    BranchInst *SrcBranch;
    
    /// isExceptionEdge - True if this fixup is for an exception.  If not for
    /// an exception, cleanups that only apply to exceptions don't get emitted
    /// for this fixup.
    bool isExceptionEdge;
    
    BranchFixup(BranchInst *srcBranch, bool IsExceptionEdge)
      : SrcBranch(srcBranch), isExceptionEdge(IsExceptionEdge) {}
  };

  enum CatchTypes { Unknown = 0, CatchList, FilterExpr };

  /// EHScope - One of these scopes is maintained for each TRY_CATCH_EXPR and
  /// TRY_FINALLY_EXPR blocks that we are currently in.
  struct EHScope {
    /// CatchExpr - Contains the cleanup code for a TRY_CATCH_EXPR, and NULL for
    /// a TRY_FINALLY_EXPR.
    tree_node *CatchExpr;

    /// UnwindBlock - A basic block in this scope that branches to the unwind
    /// destination.  This is lazily created by the first invoke in this scope.
    BasicBlock *UnwindBlock;

    // The basic blocks that are directly in this region.
    std::vector<BasicBlock*> Blocks;

    /// BranchFixups - This is a list of fixups we need to process in this scope
    /// or in a parent scope.
    std::vector<BranchFixup> BranchFixups;

    /// InfosType - The nature of the type infos CatchExpr contains: a list of
    /// CATCH_EXPR (-> CatchList) or an EH_FILTER_EXPR (-> FilterExpr).  Equal
    /// to Unknown if type info information has not yet been gathered.
    CatchTypes InfosType;

    /// TypeInfos - The type infos corresponding to the catches or filter in
    /// CatchExpr.  If InfosType is Unknown then this information has not yet
    /// been gathered.
    std::vector<Constant *> TypeInfos;

    EHScope(tree_node *expr) :
        CatchExpr(expr), UnwindBlock(0), InfosType(Unknown) {}
  };
  
  /// CurrentEHScopes - The current stack of exception scopes we are
  /// maintaining.
  std::vector<EHScope> CurrentEHScopes;
  void dumpEHScopes() const;
  
  /// BlockEHScope - If a block is in an exception scope, it is added to the
  /// list of blocks maintained by the scope and the scope number is added to
  /// this map.
  std::map<BasicBlock*, unsigned> BlockEHScope;

  /// CleanupFilter - Lazily created EH_FILTER_EXPR wrapped in a STATEMENT_LIST
  /// used to catch exceptions thrown by the finally part of a TRY_FINALLY_EXPR.
  /// The handler code is specified by the lang_protect_cleanup_actions langhook
  /// (which returns a call to "terminate" in the case of C++).
  tree_node *CleanupFilter;

  /// ExceptionValue - Is the local to receive the current exception.
  /// 
  Value *ExceptionValue;
  
  /// ExceptionSelectorValue - Is the local to receive the current exception
  /// selector.
  Value *ExceptionSelectorValue;
  
  /// FuncEHException - Function used to receive the exception.
  ///
  Function *FuncEHException;
  
  /// FuncEHSelector - Function used to receive the exception selector.
  ///
  Function *FuncEHSelector;
  
  /// FuncEHGetTypeID - Function used to return type id for give typeinfo.
  ///
  Function *FuncEHGetTypeID;
  
  /// FuncCPPPersonality - Function handling c++ personality.
  ///
  Value *FuncCPPPersonality;
  
  /// FuncUnwindResume - Function used to continue exception unwinding.
  ///
  Value *FuncUnwindResume;
  
  /// FinallyStack - Stack for nested try exit points.
  ///
  std::vector<BasicBlock *> FinallyStack;
  
  /// NumAddressTakenBlocks - Count the number of labels whose addresses are
  /// taken.
  uint64_t NumAddressTakenBlocks;

  /// AddressTakenBBNumbers - For each label with its address taken, we keep 
  /// track of its unique ID.
  std::map<BasicBlock*, ConstantInt*> AddressTakenBBNumbers;
  
  /// IndirectGotoBlock - If non-null, the block that indirect goto's in this
  /// function branch to.
  BasicBlock *IndirectGotoBlock;
  
  /// IndirectGotoValue - This is set to be the alloca temporary that the
  /// indirect goto block switches on.
  Value *IndirectGotoValue;
  
public:
  TreeToLLVM(tree_node *fndecl);
  ~TreeToLLVM();
  
  /// getFUNCTION_DECL - Return the FUNCTION_DECL node for the current function
  /// being compiled.
  tree_node *getFUNCTION_DECL() const { return FnDecl; }
  
  /// StartFunctionBody - Start the emission of 'fndecl', outputing all
  /// declarations for parameters and setting things up.
  void StartFunctionBody();
  
  /// Emit - Convert the specified tree node to LLVM code.  If the node is an
  /// expression that fits into an LLVM scalar value, the result is returned. If
  /// the result is an aggregate, it is stored into the location specified by
  /// DestLoc.
  Value *Emit(tree_node *exp, Value *DestLoc);
  
  /// EmitLV - Convert the specified l-value tree node to LLVM code, returning
  /// the address of the result.
  LValue EmitLV(tree_node *exp);

  /// FinishFunctionBody - Once the body of the function has been emitted, this
  /// cleans up and returns the result function.
  Function *FinishFunctionBody();
  
  /// getIndirectGotoBlockNumber - Return the unique ID of the specified basic
  /// block for uses that take the address of it.
  Constant *getIndirectGotoBlockNumber(BasicBlock *BB);
  
  /// getIndirectGotoBlock - Get (and potentially lazily create) the indirect
  /// goto block.
  BasicBlock *getIndirectGotoBlock();
  
  void TODO(tree_node *exp = 0);
  
  /// CastToType - Cast the specified value to the specified type if it is
  /// not already that type.
  Value *CastToType(unsigned opcode, Value *V, const Type *Ty);
  Value *CastToType(unsigned opcode, Value *V, tree_node *type) {
    return CastToType(opcode, V, ConvertType(type));
  }

  /// CastToAnyType - Cast the specified value to the specified type regardless
  /// of the types involved. This is an inferred cast.
  Value *CastToAnyType (Value *V, bool VSigned, const Type* Ty, bool TySigned);

  /// CastToUIntType - Cast the specified value to the specified type assuming
  /// that V's type and Ty are integral types. This arbitrates between BitCast,
  /// Trunc and ZExt.
  Value *CastToUIntType(Value *V, const Type* Ty);

  /// CastToSIntType - Cast the specified value to the specified type assuming
  /// that V's type and Ty are integral types. This arbitrates between BitCast,
  /// Trunc and SExt.
  Value *CastToSIntType(Value *V, const Type* Ty);

  /// CastToFPType - Cast the specified value to the specified type assuming
  /// that V's type and Ty are floating point types. This arbitrates between
  /// BitCast, FPTrunc and FPExt.
  Value *CastToFPType(Value *V, const Type* Ty);

  /// NOOPCastToType - Insert a BitCast from V to Ty if needed. This is just a
  /// convenience function for CastToType(Instruction::BitCast, V, Ty);
  Value *BitCastToType(Value *V, const Type *Ty);

  /// CreateTemporary - Create a new alloca instruction of the specified type,
  /// inserting it into the entry block and returning it.  The resulting
  /// instruction's type is a pointer to the specified type.
  AllocaInst *CreateTemporary(const Type *Ty);
  
private: // Helper functions.

  /// EmitBlock - Add the specified basic block to the end of the function.  If
  /// the previous block falls through into it, add an explicit branch.  Also,
  /// manage fixups for EH info.
  void EmitBlock(BasicBlock *BB);
  
  /// EmitAggregateCopy - Copy the elements from SrcPtr to DestPtr, using the
  /// GCC type specified by GCCType to know which elements to copy.
  void EmitAggregateCopy(Value *DestPtr, Value *SrcPtr, tree_node *GCCType,
                         bool isDstVolatile, bool isSrcVolatile,
                         unsigned Alignment);
  /// EmitAggregateZero - Zero the elements of DestPtr.
  ///
  void EmitAggregateZero(Value *DestPtr, tree_node *GCCType);
                         
  /// EmitMemCpy/EmitMemMove/EmitMemSet - Emit an llvm.memcpy/llvm.memmove or
  /// llvm.memset call with the specified operands.
  void EmitMemCpy(Value *DestPtr, Value *SrcPtr, Value *Size, unsigned Align);
  void EmitMemMove(Value *DestPtr, Value *SrcPtr, Value *Size, unsigned Align);
  void EmitMemSet(Value *DestPtr, Value *SrcVal, Value *Size, unsigned Align);

  /// EmitBranchInternal - Emit an unconditional branch to the specified basic
  /// block, running cleanups if the branch exits scopes.  The argument specify
  /// how to handle these cleanups.
  void EmitBranchInternal(BasicBlock *Dest, bool IsExceptionEdge);

  /// AddBranchFixup - Add the specified unconditional branch to the fixup list
  /// for the outermost exception scope, merging it if there is already a fixup
  /// that works.
  void AddBranchFixup(BranchInst *BI, bool isExceptionEdge);
private:
  void EmitAutomaticVariableDecl(tree_node *decl);
  
  /// isNoopCast - Return true if a cast from V to Ty does not change any bits.
  ///
  static bool isNoopCast(Value *V, const Type *Ty);

  void HandleMultiplyDefinedGCCTemp(tree_node *var);
  
  /// EmitAnnotateIntrinsic - Emits call to annotate attr intrinsic
  void EmitAnnotateIntrinsic(Value *V, tree_node *decl);
  
private:
  /// GatherTypeInfo - Walk through the expression gathering all the
  /// typeinfos that are used.
  void GatherTypeInfo(tree_node *exp, std::vector<Constant *> &TypeInfos);

  /// AddLandingPad - Insert code to fetch and save the exception and exception
  /// selector.
  void AddLandingPad();

  /// CreateExceptionValues - Create values used internally by exception handling.
  ///
  void CreateExceptionValues();

  // Emit* - These are delgates from Emit, and have the same parameter
  // characteristics.
    
  // Basic lists and binding scopes.
  Value *EmitBIND_EXPR(tree_node *exp, Value *DestLoc);
  Value *EmitSTATEMENT_LIST(tree_node *exp, Value *DestLoc);

  // Helpers for exception handling.
  void   EmitProtectedCleanups(tree_node *cleanups);
  Value *EmitTryInternal(tree_node *inner, tree_node *handler, bool isCatch);

  // Control flow.
  Value *EmitLABEL_EXPR(tree_node *exp);
  Value *EmitGOTO_EXPR(tree_node *exp);
  Value *EmitRETURN_EXPR(tree_node *exp, Value *DestLoc);
  Value *EmitCOND_EXPR(tree_node *exp);
  Value *EmitSWITCH_EXPR(tree_node *exp);
  Value *EmitTRY_EXPR(tree_node *exp);
  Value *EmitCATCH_EXPR(tree_node *exp);
  Value *EmitEXC_PTR_EXPR(tree_node *exp);
  Value *EmitEH_FILTER_EXPR(tree_node *exp);

  // Expressions.
  void   EmitINTEGER_CST_Aggregate(tree_node *exp, Value *DestLoc);
  Value *EmitLoadOfLValue(tree_node *exp, Value *DestLoc);
  Value *EmitOBJ_TYPE_REF(tree_node *exp, Value *DestLoc);
  Value *EmitADDR_EXPR(tree_node *exp);
  Value *EmitOBJ_TYPE_REF(tree_node *exp);
  Value *EmitCALL_EXPR(tree_node *exp, Value *DestLoc);
  Value *EmitCallOf(Value *Callee, tree_node *exp, Value *DestLoc);
  Value *EmitMODIFY_EXPR(tree_node *exp, Value *DestLoc);
  Value *EmitNOP_EXPR(tree_node *exp, Value *DestLoc);
  Value *EmitCONVERT_EXPR(tree_node *exp, Value *DestLoc);
  Value *EmitVIEW_CONVERT_EXPR(tree_node *exp, Value *DestLoc);
  Value *EmitNEGATE_EXPR(tree_node *exp, Value *DestLoc);
  Value *EmitCONJ_EXPR(tree_node *exp, Value *DestLoc);
  Value *EmitABS_EXPR(tree_node *exp);
  Value *EmitBIT_NOT_EXPR(tree_node *exp);
  Value *EmitTRUTH_NOT_EXPR(tree_node *exp);
  Value *EmitEXACT_DIV_EXPR(tree_node *exp, Value *DestLoc);
  Value *EmitCompare(tree_node *exp, unsigned UIPred, unsigned SIPred, 
                     unsigned FPOpc);
  Value *EmitBinOp(tree_node *exp, Value *DestLoc, unsigned Opc);
  Value *EmitPtrBinOp(tree_node *exp, unsigned Opc);
  Value *EmitTruthOp(tree_node *exp, unsigned Opc);
  Value *EmitShiftOp(tree_node *exp, Value *DestLoc, unsigned Opc);
  Value *EmitRotateOp(tree_node *exp, unsigned Opc1, unsigned Opc2);
  Value *EmitMinMaxExpr(tree_node *exp, unsigned UIPred, unsigned SIPred, 
                        unsigned Opc);
  Value *EmitFLOOR_MOD_EXPR(tree_node *exp, Value *DestLoc);
  Value *EmitCEIL_DIV_EXPR(tree_node *exp);
  Value *EmitROUND_DIV_EXPR(tree_node *exp);

  // Inline Assembly and Register Variables.
  Value *EmitASM_EXPR(tree_node *exp);
  Value *EmitReadOfRegisterVariable(tree_node *vardecl, Value *DestLoc);
  void EmitModifyOfRegisterVariable(tree_node *vardecl, Value *RHS);

  // Helpers for Builtin Function Expansion.
  Value *BuildVector(const std::vector<Value*> &Elts);
  Value *BuildVector(Value *Elt, ...);
  Value *BuildVectorShuffle(Value *InVec1, Value *InVec2, ...);

  // Builtin Function Expansion.
  bool EmitBuiltinCall(tree_node *exp, tree_node *fndecl, 
                       Value *DestLoc, Value *&Result);
  bool EmitFrontendExpandedBuiltinCall(tree_node *exp, tree_node *fndecl,
                                       Value *DestLoc, Value *&Result);
  bool EmitBuiltinUnaryIntOp(Value *InVal, Value *&Result, Intrinsic::ID Id);
  Value *EmitBuiltinUnaryFPOp(Value *Amt, Intrinsic::ID F32ID,
                              Intrinsic::ID F64ID);
  Value *EmitBuiltinUnaryFPOp(Value *InVal, const char *F32Name,
                              const char *F64Name);
  Value *EmitBuiltinPOWI(tree_node *exp);

  bool EmitBuiltinConstantP(tree_node *exp, Value *&Result);
  bool EmitBuiltinAlloca(tree_node *exp, Value *&Result);
  bool EmitBuiltinExpect(tree_node *exp, Value *DestLoc, Value *&Result);
  bool EmitBuiltinExtendPointer(tree_node *exp, Value *&Result);
  bool EmitBuiltinVAStart(tree_node *exp);
  bool EmitBuiltinVAEnd(tree_node *exp);
  bool EmitBuiltinVACopy(tree_node *exp);
  bool EmitBuiltinMemCopy(tree_node *exp, Value *&Result, bool isMemMove);
  bool EmitBuiltinMemSet(tree_node *exp, Value *&Result);
  bool EmitBuiltinBZero(tree_node *exp, Value *&Result);
  bool EmitBuiltinPrefetch(tree_node *exp);
  bool EmitBuiltinReturnAddr(tree_node *exp, Value *&Result, bool isFrame);
  bool EmitBuiltinExtractReturnAddr(tree_node *exp, Value *&Result);
  bool EmitBuiltinFrobReturnAddr(tree_node *exp, Value *&Result);
  bool EmitBuiltinStackSave(tree_node *exp, Value *&Result);
  bool EmitBuiltinStackRestore(tree_node *exp);
  bool EmitBuiltinDwarfCFA(tree_node *exp, Value *&Result);
  bool EmitBuiltinDwarfSPColumn(tree_node *exp, Value *&Result);
  bool EmitBuiltinEHReturnDataRegno(tree_node *exp, Value *&Result);
  bool EmitBuiltinEHReturn(tree_node *exp, Value *&Result);
  bool EmitBuiltinInitDwarfRegSizes(tree_node *exp, Value *&Result);
  bool EmitBuiltinUnwindInit(tree_node *exp, Value *&Result);
  bool EmitBuiltinInitTrampoline(tree_node *exp, Value *&Result);

  // Complex Math Expressions.
  void EmitLoadFromComplex(Value *&Real, Value *&Imag, Value *SrcComplex,
                           bool isVolatile);
  void EmitStoreToComplex(Value *DestComplex, Value *Real, Value *Imag,
                          bool isVolatile);
  void EmitCOMPLEX_CST(tree_node *exp, Value *DestLoc);
  void EmitCOMPLEX_EXPR(tree_node *exp, Value *DestLoc);
  Value *EmitComplexBinOp(tree_node *exp, Value *DestLoc);

  // L-Value Expressions.
  LValue EmitLV_DECL(tree_node *exp);
  LValue EmitLV_ARRAY_REF(tree_node *exp);
  LValue EmitLV_COMPONENT_REF(tree_node *exp);
  LValue EmitLV_BIT_FIELD_REF(tree_node *exp);
  LValue EmitLV_XXXXPART_EXPR(tree_node *exp, unsigned Idx);
  LValue EmitLV_VIEW_CONVERT_EXPR(tree_node *exp);

  // Constant Expressions.
  Value *EmitINTEGER_CST(tree_node *exp);
  Value *EmitREAL_CST(tree_node *exp);
  Value *EmitCONSTRUCTOR(tree_node *exp, Value *DestLoc);

  // Optional target defined builtin intrinsic expanding function.
  bool TargetIntrinsicLower(tree_node *exp,
                            unsigned FnCode,
                            Value *DestLoc,
                            Value *&Result,
                            const Type *ResultType,
                            std::vector<Value*> &Ops);
};

/// TreeConstantToLLVM - An instance of this class is created and used to 
/// convert tree constant values to LLVM.  This is primarily for things like
/// global variable initializers.
///
class TreeConstantToLLVM {
public:
  // Constant Expressions
  static Constant *Convert(tree_node *exp);
  static Constant *ConvertINTEGER_CST(tree_node *exp);
  static Constant *ConvertREAL_CST(tree_node *exp);
  static Constant *ConvertVECTOR_CST(tree_node *exp);
  static Constant *ConvertSTRING_CST(tree_node *exp);
  static Constant *ConvertCOMPLEX_CST(tree_node *exp);
  static Constant *ConvertNOP_EXPR(tree_node *exp);
  static Constant *ConvertCONVERT_EXPR(tree_node *exp);
  static Constant *ConvertBinOp_CST(tree_node *exp);
  static Constant *ConvertCONSTRUCTOR(tree_node *exp);
  static Constant *ConvertArrayCONSTRUCTOR(tree_node *exp);
  static Constant *ConvertRecordCONSTRUCTOR(tree_node *exp);
  static Constant *ConvertUnionCONSTRUCTOR(tree_node *exp);
  
  // Constant Expression l-values.
  static Constant *EmitLV(tree_node *exp);
  static Constant *EmitLV_Decl(tree_node *exp);
  static Constant *EmitLV_LABEL_DECL(tree_node *exp);
  static Constant *EmitLV_STRING_CST(tree_node *exp);
  static Constant *EmitLV_COMPONENT_REF(tree_node *exp);
  static Constant *EmitLV_ARRAY_REF(tree_node *exp);
  
};

#endif
/* APPLE LOCAL end LLVM (ENTIRE FILE!)  */
