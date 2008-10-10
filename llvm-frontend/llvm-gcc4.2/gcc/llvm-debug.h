/* LLVM LOCAL begin (ENTIRE FILE!)  */
/* Internal interfaces between the LLVM backend components
Copyright (C) 2006 Free Software Foundation, Inc.
Contributed by Jim Laskey  (jlaskey@apple.com)

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
// This is a C++ header file that defines the debug interfaces shared among
// the llvm-*.cpp files.
//===----------------------------------------------------------------------===//

#ifndef LLVM_DEBUG_H
#define LLVM_DEBUG_H

#include "llvm/CodeGen/MachineModuleInfo.h"

extern "C" {
#include "llvm.h"
}  

#include <string>
#include <map>

namespace llvm {

// Forward declarations
class AllocaInst;
class BasicBlock;
class CallInst;
class Function;
class Module;

/// DebugInfo - This class gathers all debug information during compilation and
/// is responsible for emitting to llvm globals or pass directly to the backend.
class DebugInfo {
private:
  Module *M;                            // The current module.
  DISerializer SR;                      // Debug information serializer.
  const char *CurFullPath;              // Previous location file encountered.
  int CurLineNo;                        // Previous location line# encountered.
  const char *PrevFullPath;             // Previous location file encountered.
  int PrevLineNo;                       // Previous location line# encountered.
  BasicBlock *PrevBB;                   // Last basic block encountered.
  std::map<std::string, CompileUnitDesc *> CompileUnitCache;
                                        // Cache of previously constructed 
                                        // CompileUnits.
  DenseMap<tree_node *, TypeDesc *> TypeCache;
                                        // Cache of previously constructed 
                                        // Types.
  Function *StopPointFn;                // llvm.dbg.stoppoint
  Function *FuncStartFn;                // llvm.dbg.func.start
  Function *RegionStartFn;              // llvm.dbg.region.start
  Function *RegionEndFn;                // llvm.dbg.region.end
  Function *DeclareFn;                  // llvm.dbg.declare
  AnchorDesc *CompileUnitAnchor;        // Anchor for compile units.
  AnchorDesc *GlobalVariableAnchor;     // Anchor for global variables.
  AnchorDesc *SubprogramAnchor;         // Anchor for subprograms.
  std::vector<DebugInfoDesc *> RegionStack;
                                        // Stack to track declarative scopes.
  SubprogramDesc *Subprogram;           // Current subprogram.                                        
  
public:
  DebugInfo(Module *m);

  // Accessors.
  void setLocationFile(const char *FullPath) { CurFullPath = FullPath; }
  void setLocationLine(int LineNo)           { CurLineNo = LineNo; }
  
  /// getValueFor - Return a llvm representation for a given debug information
  /// descriptor.
  Value *getValueFor(DebugInfoDesc *DD);
  
  /// getCastValueFor - Return a llvm representation for a given debug 
  /// information descriptor cast to an empty struct pointer.
  Value *getCastValueFor(DebugInfoDesc *DD);

  /// EmitFunctionStart - Constructs the debug code for entering a function -
  /// "llvm.dbg.func.start."
  void EmitFunctionStart(tree_node *FnDecl, Function *Fn, BasicBlock *CurBB);

  /// EmitRegionStart- Constructs the debug code for entering a declarative
  /// region - "llvm.dbg.region.start."
  void EmitRegionStart(Function *Fn, BasicBlock *CurBB);

  /// EmitRegionEnd - Constructs the debug code for exiting a declarative
  /// region - "llvm.dbg.region.end."
  void EmitRegionEnd(Function *Fn, BasicBlock *CurBB);

  /// EmitDeclare - Constructs the debug code for allocation of a new variable.
  /// region - "llvm.dbg.declare."
  void EmitDeclare(tree_node *decl, unsigned Tag, const char *Name,
                   tree_node *type, Value *AI,
                   BasicBlock *CurBB);

  /// EmitStopPoint - Emit a call to llvm.dbg.stoppoint to indicate a change of 
  /// source line.
  void EmitStopPoint(Function *Fn, BasicBlock *CurBB);
                     
  /// EmitGlobalVariable - Emit information about a global variable.
  ///
  void EmitGlobalVariable(GlobalVariable *GV, tree_node *decl);

  /// getOrCreateType - Get the type from the cache or create a new type if
  /// necessary.
  TypeDesc *getOrCreateType(tree_node *type, CompileUnitDesc *Unit);

  /// getOrCreateCompileUnit - Get the compile unit from the cache or create a
  /// new one if necessary.
  CompileUnitDesc *getOrCreateCompileUnit(const std::string &FullPath);

  /// readLLVMDebugInfo - Read debug info from PCH file.
  void readLLVMDebugInfo();
};

} // end namespace llvm

#endif
/* LLVM LOCAL end (ENTIRE FILE!)  */
