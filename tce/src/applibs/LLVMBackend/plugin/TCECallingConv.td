//===- TCECallingConv.td - Calling Conventions TCE ---------*- tablegen -*-===//
// 
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
// 
//===----------------------------------------------------------------------===//
//
// This describes the calling conventions for the TCE architectures.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Return Value Calling Conventions
//===----------------------------------------------------------------------===//

def RetCC_TCE : CallingConv<[
  CCIfType<[i32], CCAssignToReg<[IRES0]>>,
  CCIfType<[f32], CCAssignToReg<[IRES0]>>,
  CCAssignToStack<4, 4>
]>;

// Check MIPS CC_MipsEABI it's nice...
def CC_TCE : CallingConv<[
  CCIfType<[i1, i8, i16], CCPromoteToType<i32>>,

    CCIfType<[i32], CCAssignToReg<[IRES0]>>,
  // Integer values get stored in stack slots that are 4 bytes in
  // size and 4-byte aligned.
  CCIfType<[i32, f32], CCAssignToStack<4, 4>>,

  // Integer values get stored in stack slots that are 8 bytes in
  // size and 8-byte aligned.
  CCIfType<[f64], CCAssignToStack<8, 8>>
]>;
