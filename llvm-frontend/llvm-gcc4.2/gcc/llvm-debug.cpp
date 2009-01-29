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

static const char *getLinkageName(tree Node) {

  tree decl_name = DECL_NAME(Node);
  if (decl_name != NULL && IDENTIFIER_POINTER (decl_name) != NULL) {
    if (TREE_PUBLIC(Node) &&
        DECL_ASSEMBLER_NAME(Node) != DECL_NAME(Node) && 
        !DECL_ABSTRACT(Node)) {
      return IDENTIFIER_POINTER(DECL_ASSEMBLER_NAME(Node));
    } 
  }
  return "";
}

DebugInfo::DebugInfo(Module *m)
: M(m)
, DebugFactory(*m)
, CurFullPath("")
, CurLineNo(0)
, PrevFullPath("")
, PrevLineNo(0)
, PrevBB(NULL)
, StopPointFn(NULL)
, FuncStartFn(NULL)
, RegionStartFn(NULL)
, RegionEndFn(NULL)
, DeclareFn(NULL)
, RegionStack()
{
  MainCompileUnit = createCompileUnit(main_input_filename);
}

/// EmitFunctionStart - Constructs the debug code for entering a function -
/// "llvm.dbg.func.start."
void DebugInfo::EmitFunctionStart(tree FnDecl, Function *Fn,
                                  BasicBlock *CurBB) {
  // Gather location information.
  expanded_location Loc = GetNodeLocation(FnDecl, false);
  std::string Filename, Directory;
  DirectoryAndFile(Loc.file, Directory, Filename);
  const char *LinkageName = getLinkageName(FnDecl);

  tree func_type = TREE_TYPE(FnDecl);
  llvm::SmallVector<llvm::DIDescriptor, 16> ArgTys;
  // Add the result type at least.
  ArgTys.push_back(getOrCreateType(TREE_TYPE(func_type)));

  // Set up remainder of arguments.
  for (tree arg = TYPE_ARG_TYPES(func_type); arg; arg = TREE_CHAIN(arg)) {
    tree formal_type = TREE_VALUE(arg);
    if (formal_type == void_type_node) break;
   ArgTys.push_back(getOrCreateType(formal_type));
  }
      
  llvm::DIArray FnTypeArray =
    DebugFactory.GetOrCreateArray(&ArgTys[0], ArgTys.size());

  llvm::DICompositeType  FnTy = 
    DebugFactory.CreateCompositeType(llvm::dwarf::DW_TAG_subroutine_type,
                                     MainCompileUnit, "", 
                                     MainCompileUnit, 0, 0, 0, 0, 0,
                                     llvm::DIType(), FnTypeArray);

  DISubprogram SP = DebugFactory.CreateSubprogram(MainCompileUnit, 
                                                  Fn->getNameStr(),
                                                  Fn->getNameStr(), LinkageName,
                                                  MainCompileUnit, CurLineNo, 
                                                  FnTy, 
                                                  Fn->hasInternalLinkage(),
                                                  true /*definition*/,
                                                  &Filename, &Directory);

  DebugFactory.InsertSubprogramStart(SP, CurBB);

  // Push function on region stack.
  RegionStack.push_back(SP);
}

/// EmitRegionStart- Constructs the debug code for entering a declarative
/// region - "llvm.dbg.region.start."
void DebugInfo::EmitRegionStart(Function *Fn, BasicBlock *CurBB) {
  llvm::DIDescriptor D;
  if (!RegionStack.empty())
    D = RegionStack.back();
  D = DebugFactory.CreateBlock(D);
  RegionStack.push_back(D);
  DebugFactory.InsertRegionStart(D, CurBB);
}

/// EmitRegionEnd - Constructs the debug code for exiting a declarative
/// region - "llvm.dbg.region.end."
void DebugInfo::EmitRegionEnd(Function *Fn, BasicBlock *CurBB) {

  assert(!RegionStack.empty() && "Region stack mismatch, stack empty!");

  // Provide an region stop point.
  EmitStopPoint(Fn, CurBB);
  
  DebugFactory.InsertRegionEnd(RegionStack.back(), CurBB);
  RegionStack.pop_back();
}

/// EmitDeclare - Constructs the debug code for allocation of a new variable.
/// region - "llvm.dbg.declare."
void DebugInfo::EmitDeclare(tree decl, unsigned Tag, const char *Name,
                            tree type, Value *AI, BasicBlock *CurBB) {
  // Ignore compiler generated temporaries.
  if (DECL_IGNORED_P(decl))
    return;

  assert(!RegionStack.empty() && "Region stack mismatch, stack empty!");

  expanded_location Loc = GetNodeLocation(decl, false);
  std::string Filename, Directory;
  DirectoryAndFile(Loc.file, Directory, Filename);

  // Construct variable.
  llvm::DIVariable D =
    DebugFactory.CreateVariable(Tag, RegionStack.back(), Name, MainCompileUnit,
                                Loc.line, getOrCreateType(type), &Filename,
                                &Directory);

  // Insert an llvm.dbg.declare into the current block.
  DebugFactory.InsertDeclare(AI, D, CurBB);
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
  
  DebugFactory.InsertStopPoint(MainCompileUnit, CurLineNo, 0 /*column no. */,
                              CurBB);
}

/// EmitGlobalVariable - Emit information about a global variable.
///
void DebugInfo::EmitGlobalVariable(GlobalVariable *GV, tree decl) {
  // Gather location information.
  expanded_location Loc = expand_location(DECL_SOURCE_LOCATION(decl));
  std::string Filename, Directory;
  DirectoryAndFile(Loc.file, Directory, Filename);
  DIType TyD = getOrCreateType(TREE_TYPE(decl));
    DebugFactory.CreateGlobalVariable(MainCompileUnit, GV->getNameStr(), 
                                      GV->getNameStr(), getLinkageName(decl),
                                      MainCompileUnit, Loc.line,
                                      TyD, GV->hasInternalLinkage(),
                                      true/*definition*/, GV,
                                      &Filename, &Directory);

}

/// getOrCreateType - Get the type from the cache or create a new type if
/// necessary.
/// FIXME - I hate jumbo methods - split up.
DIType DebugInfo::getOrCreateType(tree type) {
  DEBUGASSERT(type != NULL_TREE && type != error_mark_node &&
              "Not a type.");
  if (type == NULL_TREE || type == error_mark_node) return DIType();

  // Should only be void if a pointer/reference/return type.  Returning NULL
  // allows the caller to produce a non-derived type.
  if (TREE_CODE(type) == VOID_TYPE) return DIType();
  
  // Check to see if the compile unit already has created this type.
  DIType &Slot = TypeCache[type];
  if (!Slot.isNull())
    return Slot;
  
  DIType MainTy;
  if (type != TYPE_MAIN_VARIANT(type)) {
    if (TYPE_NEXT_VARIANT(type) && type != TYPE_NEXT_VARIANT(type))
      MainTy = getOrCreateType(TYPE_NEXT_VARIANT(type));
    else if (TYPE_MAIN_VARIANT(type))
      MainTy = getOrCreateType(TYPE_MAIN_VARIANT(type));
  }

  // Get the name and location early to assist debugging.
  const char *TypeName = GetNodeName(type);
  expanded_location Loc = GetNodeLocation(type);
  
  // Bit size and align of the type.
  uint64_t Size = NodeSizeInBits(type);
  uint64_t Align = NodeAlignInBits(type);

  DIType Ty;
  // Do we have a typedef?
  if (tree Name = TYPE_NAME(type)) {
    if (TREE_CODE(Name) == TYPE_DECL &&  DECL_ORIGINAL_TYPE(Name)) {
      expanded_location TypeDefLoc = GetNodeLocation(Name);
      std::string Filename, Directory;
      DirectoryAndFile(TypeDefLoc.file, Directory, Filename);
      Ty = DebugFactory.CreateDerivedType(DW_TAG_typedef, MainCompileUnit,
                                          GetNodeName(Name), 
                                          MainCompileUnit, TypeDefLoc.line,
                                          0 /*size*/,
                                          0 /*align*/,
                                          0 /*offset */, 
                                          0 /*flags*/, 
                                          MainTy, &Filename, &Directory);
      // Set the slot early to prevent recursion difficulties.
      TypeCache[type] = Ty;
      return Ty;
    }
  }

  if (TYPE_VOLATILE(type)) {
    Ty = DebugFactory.CreateDerivedType(DW_TAG_volatile_type, MainCompileUnit, "", 
                                        MainCompileUnit, 0 /*line no*/, 
                                        NodeSizeInBits(type),
                                        NodeAlignInBits(type),
                                        0 /*offset */, 
                                        0 /* flags */, 
                                        MainTy);
    MainTy = Ty;
  }

  if (TYPE_READONLY(type)) 
    Ty =  DebugFactory.CreateDerivedType(DW_TAG_const_type, MainCompileUnit, "", 
                                         MainCompileUnit, 0 /*line no*/, 
                                         NodeSizeInBits(type),
                                         NodeAlignInBits(type),
                                         0 /*offset */, 
                                         0 /* flags */, 
                                         MainTy);
  
  if (TYPE_VOLATILE(type) || TYPE_READONLY(type)) {
    TypeCache[type] = Ty;
    return Ty;
  }

  // If, for some reason, main type varaint type is seen then use it.
  if (!MainTy.isNull())
    return MainTy;

  // Work out details of type.
  switch (TREE_CODE(type)) {
    case ERROR_MARK:
    case LANG_TYPE:
    case TRANSLATION_UNIT_DECL:
    default: {
      DEBUGASSERT(0 && "Unsupported type");
      return DIType();
    }
    
    case POINTER_TYPE:
    case REFERENCE_TYPE:
    case BLOCK_POINTER_TYPE: {

      DIType FromTy = getOrCreateType(TREE_TYPE(type));
      // type* and type&
      // FIXME: Should BLOCK_POINTER_TYP have its own DW_TAG?
      unsigned Tag = (TREE_CODE(type) == POINTER_TYPE ||
                      TREE_CODE(type) == BLOCK_POINTER_TYPE) ?
        DW_TAG_pointer_type :
        DW_TAG_reference_type;
      Ty =  DebugFactory.CreateDerivedType(Tag, MainCompileUnit, "", 
                                           MainCompileUnit, 0 /*line no*/, 
                                           NodeSizeInBits(type),
                                           NodeAlignInBits(type),
                                           0 /*offset */, 
                                           0 /* flags */, 
                                           FromTy);
      
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
      llvm::SmallVector<llvm::DIDescriptor, 16> EltTys;
    
      // Add the result type at least.
      EltTys.push_back(getOrCreateType(TREE_TYPE(type)));
      
      // Set up remainder of arguments.
      for (tree arg = TYPE_ARG_TYPES(type); arg; arg = TREE_CHAIN(arg)) {
        tree formal_type = TREE_VALUE(arg);
        if (formal_type == void_type_node) break;
        EltTys.push_back(getOrCreateType(formal_type));
      }
      
      llvm::DIArray EltTypeArray =
        DebugFactory.GetOrCreateArray(&EltTys[0], EltTys.size());
      
      Ty = DebugFactory.CreateCompositeType(llvm::dwarf::DW_TAG_subroutine_type,
                                            MainCompileUnit, "", 
                                            MainCompileUnit, 0, 0, 0, 0, 0,
                                            llvm::DIType(), EltTypeArray);
      break;
    }
      
    case VECTOR_TYPE:
    case ARRAY_TYPE: {
      // type[n][m]...[p]
      if (TYPE_STRING_FLAG(type) && TREE_CODE(TREE_TYPE(type)) == INTEGER_TYPE){
        DEBUGASSERT(0 && "Don't support pascal strings");
        return DIType();
      }
      
      unsigned Tag = 0;
      
      if (TREE_CODE(type) == VECTOR_TYPE) 
        Tag = DW_TAG_vector_type;
      else
        Tag = DW_TAG_array_type;

      // Add the dimensions of the array.  FIXME: This loses CV qualifiers from
      // interior arrays, do we care?  Why aren't nested arrays represented the
      // obvious/recursive way?
      llvm::SmallVector<llvm::DIDescriptor, 8> Subscripts;

      // There will be ARRAY_TYPE nodes for each rank.  Followed by the derived
      // type.
      tree atype = type;
      tree EltTy = TREE_TYPE(atype);
      for (; TREE_CODE(atype) == ARRAY_TYPE; atype = TREE_TYPE(atype)) {
        tree Domain = TYPE_DOMAIN(atype);
        if (Domain) {
          // FIXME - handle dynamic ranges
          tree MinValue = TYPE_MIN_VALUE(Domain);
          tree MaxValue = TYPE_MAX_VALUE(Domain);
          if (MinValue && MaxValue &&
              isInt64(MinValue, 0) && isInt64(MaxValue, 0)) {
            uint64_t Low = getInt64(MinValue, 0);
            uint64_t Hi = getInt64(MaxValue, 0);
            Subscripts.push_back(DebugFactory.GetOrCreateSubrange(Low, Hi));
          }
        }
        EltTy = TREE_TYPE(atype);
      }

      llvm::DIArray SubscriptArray =
        DebugFactory.GetOrCreateArray(&Subscripts[0], Subscripts.size());

      Ty = DebugFactory.CreateCompositeType(llvm::dwarf::DW_TAG_array_type,
                                            MainCompileUnit, "", 
                                            MainCompileUnit,
                                            0, Size, Align, 0, 0,
                                            getOrCreateType(EltTy),
                                            SubscriptArray);
      break;
    }
    
    case ENUMERAL_TYPE: {
      // enum { a, b, ..., z };
      llvm::SmallVector<llvm::DIDescriptor, 32> Elements;

      if (TYPE_SIZE(type)) {
        for (tree Link = TYPE_VALUES(type); Link; Link = TREE_CHAIN(Link)) {
          tree EnumValue = TREE_VALUE(Link);
          int64_t Value = getInt64(EnumValue, tree_int_cst_sgn(EnumValue) > 0);
          const char *EnumName = IDENTIFIER_POINTER(TREE_PURPOSE(Link));
          Elements.push_back(DebugFactory.CreateEnumerator(EnumName, Value));
        }
      }

      llvm::DIArray EltArray =
        DebugFactory.GetOrCreateArray(&Elements[0], Elements.size());

      expanded_location Loc = GetNodeLocation(TREE_CHAIN(type), false);
      std::string Filename, Directory;
      DirectoryAndFile(Loc.file, Directory, Filename);
      Ty = DebugFactory.CreateCompositeType(llvm::dwarf::DW_TAG_enumeration_type,
                                            MainCompileUnit, TypeName, 
                                            MainCompileUnit, Loc.line,
                                            Size, Align, 0, 0,
                                            llvm::DIType(), EltArray,
                                            &Filename, &Directory);
      break;
    }
    
    case RECORD_TYPE:
    case QUAL_UNION_TYPE:
    case UNION_TYPE: {
      // struct { a; b; ... z; }; | union { a; b; ... z; };
      unsigned Tag = TREE_CODE(type) == RECORD_TYPE ? DW_TAG_structure_type :
                                                      DW_TAG_union_type;

      // Records and classes and unions can all be recursive.  To handle them,
      // we first generate a debug descriptor for the struct as a forward 
      // declaration. Then (if it is a definition) we go through and get debug 
      // info for all of its members.  Finally, we create a descriptor for the
      // complete type (which may refer to the forward decl if the struct is 
      // recursive) and replace all  uses of the forward declaration with the 
      // final definition. 
      expanded_location Loc = GetNodeLocation(TREE_CHAIN(type), false);
      std::string Filename, Directory;
      DirectoryAndFile(Loc.file, Directory, Filename);
      llvm::DIType FwdDecl =
        DebugFactory.CreateCompositeType(Tag, MainCompileUnit, TypeName, 
                                         MainCompileUnit, Loc.line, 
                                         0, 0, 0, llvm::DIType::FlagFwdDecl,
                                         llvm::DIType(), llvm::DIArray(),
                                         &Filename, &Directory);
  

      // forward declaration, 
      if (TYPE_SIZE(type) == 0) {
        Ty = FwdDecl;
        break;
      }

      // Insert into the TypeCache so that recursive uses will find it.
      TypeCache[type] =  FwdDecl;

      // Convert all the elements.
      llvm::SmallVector<llvm::DIDescriptor, 16> EltTys;

      if (tree binfo = TYPE_BINFO(type)) {
        VEC (tree, gc) *accesses = BINFO_BASE_ACCESSES (binfo);
        
        for (unsigned i = 0, e = BINFO_N_BASE_BINFOS(binfo); i != e; ++i) {
          tree BInfo = BINFO_BASE_BINFO(binfo, i);
          tree BInfoType = BINFO_TYPE (BInfo);
          DIType BaseClass = getOrCreateType(BInfoType);
         
          // FIXME : name, size, align etc...
          DIType DTy = 
            DebugFactory.CreateDerivedType(DW_TAG_inheritance, 
                                           MainCompileUnit,"", 
                                           MainCompileUnit, 0,0,0, 
                                           getInt64(BINFO_OFFSET(BInfo), 0),
                                           0, BaseClass);
          EltTys.push_back(DTy);
        }
      }

      // Now add members of this class.
      for (tree Member = TYPE_FIELDS(type); Member;
                Member = TREE_CHAIN(Member)) {
        // Should we skip.
        if (DECL_P(Member) && DECL_IGNORED_P(Member)) continue;

        if (TREE_CODE(Member) == FIELD_DECL) {

          if (DECL_FIELD_OFFSET(Member) == 0 ||
              TREE_CODE(DECL_FIELD_OFFSET(Member)) != INTEGER_CST)
            // FIXME: field with variable position, skip it for now.
            continue;

          /* Ignore nameless fields.  */
          if (DECL_NAME (Member) == NULL_TREE)
            continue;
          
          // Get the location of the member.
          expanded_location MemLoc = GetNodeLocation(Member, false);
          std::string MemFilename, MemDirectory;
          DirectoryAndFile(MemLoc.file, MemDirectory, MemFilename);

          // Field type is the declared type of the field.
          tree FieldNodeType = FieldType(Member);
          DIType MemberType = getOrCreateType(FieldNodeType);
          const char *MemberName = GetNodeName(Member);
          unsigned Flags = 0;
          if (TREE_PROTECTED(Member))
            Flags = llvm::DIType::FlagProtected;
          else if (TREE_PRIVATE(Member))
            Flags = llvm::DIType::FlagPrivate;

          DIType DTy =
            DebugFactory.CreateDerivedType(DW_TAG_member, MainCompileUnit, 
                                           MemberName, MainCompileUnit,
                                           MemLoc.line, NodeSizeInBits(Member),
                                           NodeAlignInBits(FieldNodeType),
                                           int_bit_position(Member), 
                                           Flags, MemberType,
                                           &MemFilename, &MemDirectory);
          EltTys.push_back(DTy);
        } else {
          DEBUGASSERT(0 && "Unsupported member tree code!");
        }
      }

      for (tree Member = TYPE_METHODS(type); Member;
                Member = TREE_CHAIN(Member)) {
                
        if (DECL_ABSTRACT_ORIGIN (Member)) continue;

        // Get the location of the member.
        expanded_location MemLoc = GetNodeLocation(Member, false);
        std::string MemFilename, MemDirectory;
        DirectoryAndFile(MemLoc.file, MemDirectory, MemFilename);
        
        const char *MemberName = GetNodeName(Member);                
        DIType SPTy = getOrCreateType(TREE_TYPE(Member));
        DISubprogram SP = 
          DebugFactory.CreateSubprogram(MainCompileUnit, MemberName, MemberName,
                                        MemberName, MainCompileUnit, 
                                        MemLoc.line, SPTy, false, false,
                                        &MemFilename, &MemDirectory);

        EltTys.push_back(SP);
      }

      llvm::DIArray Elements =
        DebugFactory.GetOrCreateArray(&EltTys[0], EltTys.size());
      
      llvm::DIType RealDecl =
        DebugFactory.CreateCompositeType(Tag, MainCompileUnit, TypeName, 
                                         MainCompileUnit, Loc.line, Size,
                                         Align, 0, 0, llvm::DIType(), Elements,
                                         &Filename, &Directory);

      // Now that we have a real decl for the struct, replace anything using the
      // old decl with the new one.  This will recursively update the debug info.
      FwdDecl.getGV()->replaceAllUsesWith(RealDecl.getGV());
      FwdDecl.getGV()->eraseFromParent();
      Ty = RealDecl;

      break;
    }

    case INTEGER_TYPE:
    case REAL_TYPE:   
    case COMPLEX_TYPE:
    case BOOLEAN_TYPE: {

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
      Ty = DebugFactory.CreateBasicType(MainCompileUnit, TypeName, 
                                        MainCompileUnit, 0, Size, Align,
                                        0, 0, Encoding);
    }
  }
  TypeCache[type] = Ty;
  return Ty;
}

/// createCompileUnit - Get the compile unit from the cache or create a new
/// one if necessary.
DICompileUnit DebugInfo::createCompileUnit(const std::string &FullPath){
  // Get source file information.
  std::string Directory;
  std::string FileName;
  DirectoryAndFile(FullPath, Directory, FileName);
  
  // Set up Language number.
  unsigned LangTag;
  const std::string LanguageName(lang_hooks.name);
  if (LanguageName == "GNU C")
    LangTag = DW_LANG_C89;
  else if (LanguageName == "GNU C++")
    LangTag = DW_LANG_C_plus_plus;
  else if (LanguageName == "GNU Ada")
    LangTag = DW_LANG_Ada95;
  else if (LanguageName == "GNU F77")
    LangTag = DW_LANG_Fortran77;
  else if (LanguageName == "GNU Pascal")
    LangTag = DW_LANG_Pascal83;
  else if (LanguageName == "GNU Java")
    LangTag = DW_LANG_Java;
  else if (LanguageName == "GNU Objective-C")
    LangTag = DW_LANG_ObjC;
  else if (LanguageName == "GNU Objective-C++") 
    LangTag = DW_LANG_ObjC_plus_plus;
  else
    LangTag = DW_LANG_C89;

  return DebugFactory.CreateCompileUnit(LangTag, FileName, Directory, 
                                        version_string);
}

/* LLVM LOCAL end (ENTIRE FILE!)  */
