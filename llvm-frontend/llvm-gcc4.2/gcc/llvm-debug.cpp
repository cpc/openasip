/* LLVM LOCAL begin (ENTIRE FILE!)  */
/* High-level LLVM backend interface 
Copyright (C) 2005 Free Software Foundation, Inc.
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
// This is a C++ source file that implements the debug information gathering.
//===----------------------------------------------------------------------===//

#include "llvm-debug.h"

#include "llvm-abi.h"
#include "llvm-internal.h"
#include "llvm/Constants.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Instructions.h"
#include "llvm/Intrinsics.h"
#include "llvm/Module.h"
#include "llvm/Support/Dwarf.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/MachineModuleInfo.h"

extern "C" {
#include "langhooks.h"
#include "toplev.h"
#include "tree.h"
#include "version.h"
}

using namespace llvm;
using namespace llvm::dwarf;

#ifndef LLVMTESTDEBUG
#define DEBUGASSERT(S) ((void)0)
#else
#define DEBUGASSERT(S) assert(S)
#endif


/// DirectoryAndFile - Extract the directory and file name from a path.  If no
/// directory is specified, then use the source working directory.
static void DirectoryAndFile(const std::string &FullPath,
                             std::string &Directory, std::string &FileName) {
  // Look for the directory slash.
  size_t Slash = FullPath.rfind('/');
  
  // If no slash
  if (Slash == std::string::npos) {
    // The entire path is the file name.
    Directory = "";
    FileName = FullPath;
  } else {
    // Separate the directory from the file name.
    Directory = FullPath.substr(0, Slash);
    FileName = FullPath.substr(Slash + 1);
  }
  
  // If no directory present then use source working directory.
  if (Directory.empty() || Directory[0] != '/') {
    Directory = std::string(get_src_pwd()) + "/" + Directory;
  }
}

/// NodeSizeInBits - Returns the size in bits stored in a tree node regardless
/// of whether the node is a TYPE or DECL.
static uint64_t NodeSizeInBits(tree Node) {
  if (TREE_CODE(Node) == ERROR_MARK) {
    return BITS_PER_WORD;
  } else if (TYPE_P(Node)) {
    if (TYPE_SIZE(Node) == NULL_TREE)
      return 0;
    else if (isInt64(TYPE_SIZE(Node), 1))
      return getInt64(TYPE_SIZE(Node), 1);
    else
      return TYPE_ALIGN(Node);
  } else if (DECL_P(Node)) {
    if (DECL_SIZE(Node) == NULL_TREE)
      return 0;
    else if (isInt64(DECL_SIZE(Node), 1))
      return getInt64(DECL_SIZE(Node), 1);
    else
      return DECL_ALIGN(Node);
  }
  
  return 0;
}

/// NodeAlignInBits - Returns the alignment in bits stored in a tree node
/// regardless of whether the node is a TYPE or DECL.
static uint64_t NodeAlignInBits(tree Node) {
  if (TREE_CODE(Node) == ERROR_MARK) return BITS_PER_WORD;
  if (TYPE_P(Node)) return TYPE_ALIGN(Node);
  if (DECL_P(Node)) return DECL_ALIGN(Node);
  return BITS_PER_WORD;
}

/// FieldType - Returns the type node of a structure member field.
///
static tree FieldType(tree Field) {
  if (TREE_CODE (Field) == ERROR_MARK) return integer_type_node;
  return getDeclaredType(Field);
}

/// GetNodeName - Returns the name stored in a node regardless of whether the
/// node is a TYPE or DECL.
static const char *GetNodeName(tree Node) {
  tree Name = NULL;
  
  if (DECL_P(Node)) {
    Name = DECL_NAME(Node);
  } else if (TYPE_P(Node)) {
    Name = TYPE_NAME(Node);
  }

  if (Name) {
    if (TREE_CODE(Name) == IDENTIFIER_NODE) {
      return IDENTIFIER_POINTER(Name);
    } else if (TREE_CODE(Name) == TYPE_DECL && DECL_NAME(Name) &&
               !DECL_IGNORED_P(Name)) {
      return IDENTIFIER_POINTER(DECL_NAME(Name));
    }
  }
  
  return "";
}

/// GetNodeLocation - Returns the location stored in a node  regardless of
/// whether the node is a TYPE or DECL.  UseStub is true if we should consider
/// the type stub as the actually location (ignored in struct/unions/enums.)
static expanded_location GetNodeLocation(tree Node, bool UseStub = true) {
  expanded_location Location = { NULL, 0 };

  tree Name = NULL;
  
  if (DECL_P(Node)) {
    Name = DECL_NAME(Node);
  } else if (TYPE_P(Node)) {
    Name = TYPE_NAME(Node);
  }
  
  if (Name) {
    if (TYPE_STUB_DECL(Name)) {
      tree Stub = TYPE_STUB_DECL(Name);
      Location = expand_location(DECL_SOURCE_LOCATION(Stub));
    } else if (DECL_P(Name)) {
      Location = expand_location(DECL_SOURCE_LOCATION(Name));
    }
  }
  
  if (!Location.line) {
    if (UseStub && TYPE_STUB_DECL(Node)) {
      tree Stub = TYPE_STUB_DECL(Node);
      Location = expand_location(DECL_SOURCE_LOCATION(Stub));
    } else if (DECL_P(Node)) {
      Location = expand_location(DECL_SOURCE_LOCATION(Node));
    }
  }
  
  return Location;
}

/// GetGlobalNames - Sets the names for a global descriptor.
///
static void GetGlobalNames(tree Node, GlobalDesc *Global) {
  tree decl_name = DECL_NAME(Node);
  if (decl_name != NULL && IDENTIFIER_POINTER (decl_name) != NULL) {
    Global->setName(lang_hooks.decl_printable_name (Node, 0));
    Global->setFullName(lang_hooks.decl_printable_name (Node, 1));
    
    if (TREE_PUBLIC(Node) &&
        DECL_ASSEMBLER_NAME(Node) != DECL_NAME(Node) && 
        !DECL_ABSTRACT(Node)) {
        Global->setLinkageName(IDENTIFIER_POINTER(DECL_ASSEMBLER_NAME(Node)));
    }
  }
}  

DebugInfo::DebugInfo(Module *m)
: M(m)
, SR()
, CurFullPath("")
, CurLineNo(0)
, PrevFullPath("")
, PrevLineNo(0)
, PrevBB(NULL)
, CompileUnitCache()
, StopPointFn(NULL)
, FuncStartFn(NULL)
, RegionStartFn(NULL)
, RegionEndFn(NULL)
, DeclareFn(NULL)
, CompileUnitAnchor(NULL)
, GlobalVariableAnchor(NULL)
, SubprogramAnchor(NULL)
, RegionStack()
, Subprogram(NULL)
{
  // uint64_t and int64_t are used in the debug descriptors for sizes and
  // offsets.  So, we should make sure we don't lose any accuracy. 
  DEBUGASSERT(sizeof(HOST_WIDE_INT) <= sizeof(int64_t) &&
              "Bit size exceeds values stored in debug descriptors");
         
  // Let the debug serializer know where the module lives.
  SR.setModule(M);
}

/// getValueFor - Return a llvm representation for a given debug information
/// descriptor.
Value *DebugInfo::getValueFor(DebugInfoDesc *DD) {
  return SR.Serialize(DD);
}

/// getCastValueFor - Return a llvm representation for a given debug information
/// descriptor cast to an empty struct pointer.
Value *DebugInfo::getCastValueFor(DebugInfoDesc *DD) {
  return TheFolder->CreateBitCast(SR.Serialize(DD), SR.getEmptyStructPtrType());
}

/// EmitFunctionStart - Constructs the debug code for entering a function -
/// "llvm.dbg.func.start."
void DebugInfo::EmitFunctionStart(tree FnDecl, Function *Fn,
                                  BasicBlock *CurBB) {
  PrevFullPath = "";
  PrevLineNo = 0;
  PrevBB = NULL;
  
  // Create subprogram descriptor.
  Subprogram = new SubprogramDesc();
  
  // Make sure we have an anchor.
  if (!SubprogramAnchor) {
    SubprogramAnchor = new AnchorDesc(Subprogram);
  }

  // Get name information.
  GetGlobalNames(FnDecl, Subprogram);
  
  // Gather location information.
  CompileUnitDesc *Unit = getOrCreateCompileUnit(CurFullPath);
  
  // Get function type.
  TypeDesc *SPTy = getOrCreateType(TREE_TYPE(TREE_TYPE(FnDecl)), Unit);

  Subprogram->setAnchor(SubprogramAnchor);
  Subprogram->setContext(Unit);
  Subprogram->setFile(Unit);
  Subprogram->setLine(CurLineNo);
  Subprogram->setType(SPTy);
  Subprogram->setIsStatic(Fn->hasInternalLinkage());
  Subprogram->setIsDefinition(true);
  
  // Lazily construct llvm.dbg.func.start.
  if (!FuncStartFn)
    FuncStartFn = Intrinsic::getDeclaration(M, Intrinsic::dbg_func_start);

  // Call llvm.dbg.func.start which also implicitly calls llvm.dbg.stoppoint.
  CallInst::Create(FuncStartFn, getCastValueFor(Subprogram), "", CurBB);
  
  // Push function on region stack.
  RegionStack.push_back(Subprogram);
}

/// EmitRegionStart- Constructs the debug code for entering a declarative
/// region - "llvm.dbg.region.start."
void DebugInfo::EmitRegionStart(Function *Fn, BasicBlock *CurBB) {
  BlockDesc *Block = new BlockDesc();
  Block->setContext(RegionStack.back());
  RegionStack.push_back(Block);

  // Lazily construct llvm.dbg.region.start function.
  if (!RegionStartFn)
    RegionStartFn = Intrinsic::getDeclaration(M, Intrinsic::dbg_region_start);
  
  // Call llvm.dbg.func.start.
  CallInst::Create(RegionStartFn, getCastValueFor(Block), "", CurBB);
}

/// EmitRegionEnd - Constructs the debug code for exiting a declarative
/// region - "llvm.dbg.region.end."
void DebugInfo::EmitRegionEnd(Function *Fn, BasicBlock *CurBB) {
  // Lazily construct llvm.dbg.region.end function.
  if (!RegionEndFn)
    RegionEndFn = Intrinsic::getDeclaration(M, Intrinsic::dbg_region_end);
  
  // Provide an region stop point.
  EmitStopPoint(Fn, CurBB);
  
  // Call llvm.dbg.func.end.
  CallInst::Create(RegionEndFn, getCastValueFor(RegionStack.back()), "", CurBB);
  RegionStack.pop_back();
}

/// EmitDeclare - Constructs the debug code for allocation of a new variable.
/// region - "llvm.dbg.declare."
void DebugInfo::EmitDeclare(tree decl, unsigned Tag, const char *Name,
                            tree type, Value *AI, BasicBlock *CurBB) {
  // Ignore compiler generated temporaries.
  if (DECL_IGNORED_P(decl))
    return;

  // Lazily construct llvm.dbg.declare function.
  const PointerType *EmpPtr = SR.getEmptyStructPtrType();
  if (!DeclareFn)
    DeclareFn = Intrinsic::getDeclaration(M, Intrinsic::dbg_declare);

  // Get type information.
  CompileUnitDesc *Unit = getOrCreateCompileUnit(CurFullPath);
  TypeDesc *TyDesc = getOrCreateType(type, Unit);

  expanded_location Loc = GetNodeLocation(decl, false);
  CompileUnitDesc *File = Loc.line ? getOrCreateCompileUnit(Loc.file) : NULL;

  // Construct variable.
  VariableDesc *Variable = new VariableDesc(Tag);
  Variable->setContext(RegionStack.back());
  Variable->setName(Name);
  Variable->setFile(File);
  Variable->setLine(Loc.line);
  Variable->setType(TyDesc);

  // Cast the AllocA result to a {}* for the call to llvm.dbg.declare. Since
  // only pointer types are involved, this is always a BitCast
  Value *AllocACast = new BitCastInst(AI, EmpPtr, Name, CurBB);

  // Call llvm.dbg.declare.
  Value *Args[2] = {
    AllocACast,
    getCastValueFor(Variable)
  };
  CallInst::Create(DeclareFn, Args, Args + 2, "", CurBB);
}

/// EmitStopPoint - Emit a call to llvm.dbg.stoppoint to indicate a change of 
/// source line - "llvm.dbg.stoppoint."
void DebugInfo::EmitStopPoint(Function *Fn, BasicBlock *CurBB) {
  // Don't bother if things are the same as last time.
  if (PrevLineNo == CurLineNo &&
      PrevBB == CurBB &&
      (PrevFullPath == CurFullPath ||
       !strcmp(PrevFullPath, CurFullPath))) return;
  if (!CurFullPath[0] || CurLineNo == 0) return;
  
  // Update last state.
  PrevFullPath = CurFullPath;
  PrevLineNo = CurLineNo;
  PrevBB = CurBB;
  
  // Get the appropriate compile unit.
  CompileUnitDesc *Unit = getOrCreateCompileUnit(CurFullPath);
  
  // Lazily construct llvm.dbg.stoppoint function.
  if (!StopPointFn)
    StopPointFn = Intrinsic::getDeclaration(M, Intrinsic::dbg_stoppoint);
  
  // Invoke llvm.dbg.stoppoint
  Value *Args[3] = {
    ConstantInt::get(Type::Int32Ty, CurLineNo),
    ConstantInt::get(Type::Int32Ty, 0),
    getCastValueFor(Unit)
  };
  CallInst::Create(StopPointFn, Args, Args+3, "", CurBB);
}

/// EmitGlobalVariable - Emit information about a global variable.
///
void DebugInfo::EmitGlobalVariable(GlobalVariable *GV, tree decl) {
  // FIXME - lots to do here.
  // Create global variable debug descriptor.
  GlobalVariableDesc *Global = new GlobalVariableDesc();

  // Make sure we have an anchor.
  if (!GlobalVariableAnchor) {
    GlobalVariableAnchor = new AnchorDesc(Global);
  }
  
  // Get name information.
  GetGlobalNames(decl, Global);

  // Gather location information.
  expanded_location location = expand_location(DECL_SOURCE_LOCATION(decl));
  CompileUnitDesc *Unit = getOrCreateCompileUnit(location.file);
  
  TypeDesc *TyD = getOrCreateType(TREE_TYPE(decl), Unit);
  
  // Fill in the blanks.
  Global->setAnchor(GlobalVariableAnchor);
  Global->setContext(Unit);
  Global->setFile(Unit);
  Global->setLine(location.line);
  Global->setType(TyD);
  Global->setIsDefinition(true);
  Global->setIsStatic(GV->hasInternalLinkage());
  Global->setGlobalVariable(GV);
  
  // Make sure global is created if needed.
  getValueFor(Global);
}

/// AddTypeQualifiers - Add const/volatile qualifiers prior to the type
/// descriptor.
static TypeDesc *AddTypeQualifiers(tree_node *type, CompileUnitDesc *Unit,
                                   TypeDesc *TyDesc) {
  if (TYPE_READONLY(type)) {
    DerivedTypeDesc *DerivedTy = new DerivedTypeDesc(DW_TAG_const_type);
    DerivedTy->setContext(Unit);
    DerivedTy->setFromType(TyDesc);
    TyDesc = DerivedTy;
  }
  if (TYPE_VOLATILE(type)) {
    DerivedTypeDesc *DerivedTy = new DerivedTypeDesc(DW_TAG_volatile_type);
    DerivedTy->setContext(Unit);
    DerivedTy->setFromType(TyDesc);
    TyDesc = DerivedTy;
  }
  
  // FIXME - Add private/public/protected.
  
  return TyDesc;
}

/// getOrCreateType - Get the type from the cache or create a new type if
/// necessary.
/// FIXME - I hate jumbo methods - split up.
TypeDesc *DebugInfo::getOrCreateType(tree type, CompileUnitDesc *Unit) {
  DEBUGASSERT(type != NULL_TREE && type != error_mark_node &&
              "Not a type.");
  if (type == NULL_TREE || type == error_mark_node) return NULL;

  // Ignore variants such as const, volatile, or restrict.
  type = TYPE_MAIN_VARIANT(type);

  // Should only be void if a pointer/reference/return type.  Returning NULL
  // allows the caller to produce a non-derived type.
  if (TREE_CODE(type) == VOID_TYPE) return NULL;
  
  // Check to see if the compile unit already has created this type.
  TypeDesc *Slot = TypeCache[type];
  if (Slot) return Slot;
  
  // Ty will have contain the resulting type.
  TypeDesc *Ty = NULL;
  
  // Get the name and location early to assist debugging.
  const char *TypeName = GetNodeName(type);
  expanded_location Loc = GetNodeLocation(type);
  
  // Bit size, align and offset of the type.
  uint64_t Size = NodeSizeInBits(type);
  uint64_t Align = NodeAlignInBits(type);
  uint64_t Offset = 0;
  
  // Do we have a typedef?
  if (tree Name = TYPE_NAME(type)) {
    if (TREE_CODE(Name) == TYPE_DECL &&  DECL_ORIGINAL_TYPE(Name)) {
      // typedefs are derived from some other type.
      DerivedTypeDesc *DerivedTy = new DerivedTypeDesc(DW_TAG_typedef);
      // Set the slot early to prevent recursion difficulties.
      TypeCache[type] = Ty = DerivedTy;
      // Handle derived type.
      TypeDesc *FromTy = getOrCreateType(DECL_ORIGINAL_TYPE(Name), Unit);
      DerivedTy->setFromType(FromTy);
      // typedefs size should be fetched from the derived type.
      Size = Align = 0;
    }
  }
  
  // If no result so far.
  if (!Ty) {
    // Work out details of type.
    switch (TREE_CODE(type)) {
    case ERROR_MARK:
    case LANG_TYPE:
    case TRANSLATION_UNIT_DECL:
    default: {
      DEBUGASSERT(0 && "Unsupported type");
      return NULL;
    }

    case POINTER_TYPE:
    case REFERENCE_TYPE:
    case BLOCK_POINTER_TYPE: {
      // type* and type&
      // FIXME: Should BLOCK_POINTER_TYP have its own DW_TAG?
      unsigned T = (TREE_CODE(type) == POINTER_TYPE ||
                    TREE_CODE(type) == BLOCK_POINTER_TYPE) ?
        DW_TAG_pointer_type :
        DW_TAG_reference_type;
      DerivedTypeDesc *DerivedTy = new DerivedTypeDesc(T);
      Ty = DerivedTy;
      // Set the slot early to prevent recursion difficulties.
      // Any other use of the type should include the qualifiers.
      TypeCache[type] = AddTypeQualifiers(type, Unit, DerivedTy);
      // Handle the derived type.
      TypeDesc *FromTy = getOrCreateType(TREE_TYPE(type), Unit);
      DerivedTy->setFromType(FromTy);
      break;
    }
    
    case OFFSET_TYPE: {
      // gen_type_die(TYPE_OFFSET_BASETYPE(type), context_die);
      
      // gen_type_die(TREE_TYPE(type), context_die);
      
      // gen_ptr_to_mbr_type_die(type, context_die);
      break;
    }

    case FUNCTION_TYPE:
    case METHOD_TYPE: {
      CompositeTypeDesc *SubrTy = new CompositeTypeDesc(DW_TAG_subroutine_type);
      Ty = SubrTy;
      // Set the slot early to prevent recursion difficulties.
      // Any other use of the type should include the qualifiers.
      TypeCache[type] = AddTypeQualifiers(type, Unit, SubrTy);
      
      // Prepare to add the arguments for the subroutine.
      std::vector<DebugInfoDesc *> &Elements = SubrTy->getElements();
      // Get result type.
      TypeDesc *ArgTy = getOrCreateType(TREE_TYPE(type), Unit);
      Elements.push_back(ArgTy);

      // Set up remainder of arguments.
      for (tree arg = TYPE_ARG_TYPES(type); arg; arg = TREE_CHAIN(arg)) {
        tree formal_type = TREE_VALUE(arg);
        
        if (formal_type == void_type_node) break;
        
        ArgTy = getOrCreateType(formal_type, Unit);
        Elements.push_back(ArgTy);
      }
      
      break;
    }
      
    case VECTOR_TYPE:
    case ARRAY_TYPE: {
      // type[n][m]...[p]
      if (TYPE_STRING_FLAG(type) && TREE_CODE(TREE_TYPE(type)) == INTEGER_TYPE){
        DEBUGASSERT(0 && "Don't support pascal strings");
        return NULL;
      }
      
      CompositeTypeDesc *ArrayTy;
      
      if (TREE_CODE(type) == VECTOR_TYPE) {
        Ty = ArrayTy = new CompositeTypeDesc(DW_TAG_vector_type);
        // Set the slot early to prevent recursion difficulties.
        // Any other use of the type should include the qualifiers.
        TypeCache[type] = AddTypeQualifiers(type, Unit, ArrayTy);
        // Use the element type of the from this point.
        type = TREE_TYPE(TYPE_FIELDS(TYPE_DEBUG_REPRESENTATION_TYPE(type)));
      } else {
        Ty = ArrayTy = new CompositeTypeDesc(DW_TAG_array_type);
        // Set the slot early to prevent recursion difficulties.
        // Any other use of the type should include the qualifiers.
        TypeCache[type] = AddTypeQualifiers(type, Unit, ArrayTy);
      }

      // Prepare to add the dimensions of the array.
      std::vector<DebugInfoDesc *> &Elements = ArrayTy->getElements();

      // There will be ARRAY_TYPE nodes for each rank.  Followed by the derived
      // type.
      for (; TREE_CODE(type) == ARRAY_TYPE; type = TREE_TYPE(type)) {
        tree Domain = TYPE_DOMAIN(type);
        SubrangeDesc *Subrange = new SubrangeDesc();

        if (Domain) {
          // FIXME - handle dynamic ranges
          tree MinValue = TYPE_MIN_VALUE(Domain);
          tree MaxValue = TYPE_MAX_VALUE(Domain);
          if (MinValue && MaxValue &&
              isInt64(MinValue, 0) && isInt64(MaxValue, 0)) {
            Subrange->setLo(getInt64(MinValue, 0));
            Subrange->setHi(getInt64(MaxValue, 0));
          }
        }
        
        Elements.push_back(Subrange);
      }
      
      // Now handle the derived type.
      ArrayTy->setFromType(getOrCreateType(type, Unit));
      break;
    }
    
    case ENUMERAL_TYPE: {
      // enum { a, b, ..., z };
      CompositeTypeDesc *Enum = new CompositeTypeDesc(DW_TAG_enumeration_type);
      Ty = Enum;
      // Any other use of the type should include the qualifiers.
      TypeCache[type] = AddTypeQualifiers(type, Unit, Enum);
      // Prepare to add the enumeration values.
      std::vector<DebugInfoDesc *> &Elements = Enum->getElements();

      if (TYPE_SIZE(type)) {
        for (tree Link = TYPE_VALUES(type); Link; Link = TREE_CHAIN(Link)) {
          EnumeratorDesc *EnumDesc = new EnumeratorDesc();

          tree EnumValue = TREE_VALUE(Link);
          int64_t Value = getInt64(EnumValue, tree_int_cst_sgn(EnumValue) > 0);
          const char *EnumName = IDENTIFIER_POINTER(TREE_PURPOSE(Link));
          EnumDesc->setName(EnumName);
          EnumDesc->setValue(Value);
        
          Elements.push_back(EnumDesc);
        }
      }
      break;
    }
    
    case RECORD_TYPE:
    case QUAL_UNION_TYPE:
    case UNION_TYPE: {
      // struct { a; b; ... z; }; | union { a; b; ... z; };
      unsigned Tag = TREE_CODE(type) == RECORD_TYPE ? DW_TAG_structure_type :
                                                      DW_TAG_union_type;
      CompositeTypeDesc *StructTy = new CompositeTypeDesc(Tag);
      Ty = StructTy;
      // Set the slot early to prevent recursion difficulties.
      // Any other use of the type should include the qualifiers.
      TypeCache[type] = AddTypeQualifiers(type, Unit, StructTy);
      // Prepare to add the fields.
      std::vector<DebugInfoDesc *> &Elements = StructTy->getElements();
      
      if (tree binfo = TYPE_BINFO(type)) {
        VEC (tree, gc) *accesses = BINFO_BASE_ACCESSES (binfo);
        
        for (unsigned i = 0, e = BINFO_N_BASE_BINFOS(binfo); i != e; ++i) {
          tree BInfo = BINFO_BASE_BINFO(binfo, i);
          tree BInfoType = BINFO_TYPE (BInfo);
          TypeDesc *BaseClass = getOrCreateType(BInfoType, Unit);
          DerivedTypeDesc *MemberDesc = new DerivedTypeDesc(DW_TAG_inheritance);
          MemberDesc->setFromType(BaseClass);
          
          if (accesses) {
            tree access = VEC_index(tree, accesses, i);
            if (access == access_protected_node) {
              MemberDesc->setIsProtected();
            } else if (access == access_private_node) {
              MemberDesc->setIsPrivate();
            }
          }
          
          MemberDesc->setOffset(getInt64(BINFO_OFFSET(BInfo), 0));
          Elements.push_back(MemberDesc);
        }
      }

      // Now add members of this class.
      for (tree Member = TYPE_FIELDS(type); Member;
                Member = TREE_CHAIN(Member)) {
        // Should we skip.
        if (DECL_P(Member) && DECL_IGNORED_P(Member)) continue;

        // Get the location of the member.
        expanded_location MemLoc = GetNodeLocation(Member, false);
        CompileUnitDesc *MemFile = MemLoc.line ?
                                   getOrCreateCompileUnit(MemLoc.file) :
                                   NULL;

        if (TREE_CODE(Member) == FIELD_DECL) {
          if (DECL_FIELD_OFFSET(Member) == 0 ||
              TREE_CODE(DECL_FIELD_OFFSET(Member)) != INTEGER_CST)
            // FIXME: field with variable position, skip it for now.
            continue;

          DerivedTypeDesc *MemberDesc = new DerivedTypeDesc(DW_TAG_member);
          // Field type is the declared type of the field.
          tree FieldNodeType = FieldType(Member);
          TypeDesc *MemberType = getOrCreateType(FieldNodeType, Unit);
          const char *MemberName = GetNodeName(Member);
          
          MemberDesc->setName(MemberName);
          MemberDesc->setFile(MemFile);
          MemberDesc->setLine(MemLoc.line);
          MemberDesc->setFromType(MemberType);
          MemberDesc->setSize(NodeSizeInBits(Member));
          MemberDesc->setAlign(NodeAlignInBits(FieldNodeType));
          MemberDesc->setOffset(int_bit_position(Member));
          
          if (TREE_PROTECTED(Member)) {
            MemberDesc->setIsProtected();
          } else if (TREE_PRIVATE(Member)) {
            MemberDesc->setIsPrivate();
          }

          Elements.push_back(MemberDesc);
        } else if (TREE_CODE(Member) == VAR_DECL) {
          GlobalVariableDesc *Static = new GlobalVariableDesc();

          // Get name information.
          GetGlobalNames(Member, Static);

          TypeDesc *TyD = getOrCreateType(TREE_TYPE(Member), Unit);
          
          // Fill in the blanks.
          Static->setContext(Unit);
          Static->setFile(MemFile);
          Static->setLine(MemLoc.line);
          Static->setType(TyD);
          Static->setIsDefinition(false);
          Static->setIsStatic(!TREE_PUBLIC(Member));

          Elements.push_back(Static);
        } else {
          // FIXME - ignoring others for the time being.
        }
      }
      for (tree Member = TYPE_METHODS(type); Member;
                Member = TREE_CHAIN(Member)) {
                
        if (DECL_ABSTRACT_ORIGIN (Member)) continue;
                
        // Create subprogram descriptor.
        Subprogram = new SubprogramDesc();
        
        // Get name information.
        GetGlobalNames(Member, Subprogram);
        
        // Get function type.
        TypeDesc *SPTy = getOrCreateType(TREE_TYPE(Member), Unit);

        if (TREE_PROTECTED(Member)) {
          SPTy->setIsProtected();
        } else if (TREE_PRIVATE(Member)) {
          SPTy->setIsPrivate();
        }

        Subprogram->setContext(Unit);
        Subprogram->setFile(Unit);
        Subprogram->setLine(CurLineNo);
        Subprogram->setType(SPTy);
        Subprogram->setIsStatic(!TREE_PUBLIC(Member));
        Subprogram->setIsDefinition(false);

        Elements.push_back(Subprogram);
      }
      
      break;
    }

    case INTEGER_TYPE:
    case REAL_TYPE:   
    case COMPLEX_TYPE:
    case BOOLEAN_TYPE: {
      // char, short, int, long long, bool, float, double.
      BasicTypeDesc *BTy = new BasicTypeDesc();
      Ty = BTy;
      // Any other use of the type should include the qualifiers.
      TypeCache[type] = AddTypeQualifiers(type, Unit, BTy);
      // The encoding specific to the type.
      unsigned Encoding = 0;

      switch (TREE_CODE(type)) {
      case INTEGER_TYPE:
        if (TYPE_STRING_FLAG (type)) {
          if (TYPE_UNSIGNED (type))
            Encoding = DW_ATE_unsigned_char;
          else
            Encoding = DW_ATE_signed_char;
        }
        else if (TYPE_UNSIGNED (type))
          Encoding = DW_ATE_unsigned;
        else
          Encoding = DW_ATE_signed;
        break;
      case REAL_TYPE:
        Encoding = DW_ATE_float;
        break;
      case COMPLEX_TYPE:
        Encoding = TREE_CODE(TREE_TYPE(type)) == REAL_TYPE ?
                             DW_ATE_complex_float : DW_ATE_lo_user;
        break;
      case BOOLEAN_TYPE:
        Encoding = DW_ATE_boolean;
        break;
      default: { 
        DEBUGASSERT(0 && "Basic type case missing");
        Encoding = DW_ATE_signed;
        Size = BITS_PER_WORD;
        Align = BITS_PER_WORD;
        break;
      }
      }
      
      BTy->setEncoding(Encoding);
      // Don't associate basic type with a location.
      Loc.line = 0;
    }
    }
  }

  // If the type is defined, fill in teh details.
  if (Ty) {
    CompileUnitDesc *File = Loc.line ? getOrCreateCompileUnit(Loc.file) : NULL;
 
    Ty->setContext(Unit);
    Ty->setName(TypeName);
    Ty->setFile(File);
    Ty->setLine(Loc.line);
    Ty->setSize(Size);
    Ty->setAlign(Align);
    Ty->setOffset(Offset);
  }

  DEBUGASSERT(TypeCache[type] && "Unimplemented type");
  return TypeCache[type];
}

/// getOrCreateCompileUnit - Get the compile unit from the cache or create a new
/// one if necessary.
CompileUnitDesc *DebugInfo::getOrCreateCompileUnit(const std::string &FullPath){
  // See if this compile unit has been used before.
  CompileUnitDesc *&Slot = CompileUnitCache[FullPath];
  if (Slot) return Slot;

  // Create new compile unit.
  CompileUnitDesc *Unit = new CompileUnitDesc();

  // Make sure we have an anchor.
  if (!CompileUnitAnchor) {
    CompileUnitAnchor = new AnchorDesc(Unit);
  }

  // Get source file information.
  std::string Directory;
  std::string FileName;
  DirectoryAndFile(FullPath, Directory, FileName);
  
  Unit->setAnchor(CompileUnitAnchor);
  Unit->setFileName(FileName);
  Unit->setDirectory(Directory);
  
  // Set up producer name.
  Unit->setProducer(version_string);

  // Set up Language number.
  unsigned Language;
  const std::string LanguageName(lang_hooks.name);
  if (LanguageName == "GNU C")
    Unit->setLanguage(DW_LANG_C89);
  else if (LanguageName == "GNU C++")
    Unit->setLanguage(DW_LANG_C_plus_plus);
  else if (LanguageName == "GNU Ada")
    Unit->setLanguage(DW_LANG_Ada95);
  else if (LanguageName == "GNU F77")
    Unit->setLanguage(DW_LANG_Fortran77);
  else if (LanguageName == "GNU Pascal")
    Unit->setLanguage(DW_LANG_Pascal83);
  else if (LanguageName == "GNU Java")
    Unit->setLanguage(DW_LANG_Java);
  else
    Unit->setLanguage(DW_LANG_C89);
    
  // Update cache.
  Slot = Unit;
  
  return Unit;
}

/// readLLVMDebugInfo - Read debug info from PCH file. TheModule already
/// represents module read from PCH file. Restore AnchorDesc from PCH file.
void DebugInfo::readLLVMDebugInfo() {
  MachineModuleInfo MMI;
  MMI.AnalyzeModule(*TheModule);

  std::vector<SubprogramDesc *> Subprograms;
  MMI.getAnchoredDescriptors<SubprogramDesc>(*TheModule, Subprograms);

  if (!Subprograms.empty())
    SubprogramAnchor = Subprograms[0]->getAnchor();

  std::vector<CompileUnitDesc *> CUs;
  MMI.getAnchoredDescriptors<CompileUnitDesc>(*TheModule, CUs);

  if (!CUs.empty())
    CompileUnitAnchor = CUs[0]->getAnchor();

  std::vector<GlobalVariableDesc *> GVs;
  MMI.getAnchoredDescriptors<GlobalVariableDesc>(*TheModule, GVs);

  if (!GVs.empty())
    GlobalVariableAnchor = GVs[0]->getAnchor();

  const std::map<GlobalVariable *, DebugInfoDesc *> &GlobalDescs
    = MMI.getDIDeserializer()->getGlobalDescs();
  for (std::map<GlobalVariable *, DebugInfoDesc *>::const_iterator 
         I = GlobalDescs.begin(), E = GlobalDescs.end(); I != E; ++I) 
    SR.addDescriptor(I->second, I->first);
}
  
/* LLVM LOCAL end (ENTIRE FILE!)  */

