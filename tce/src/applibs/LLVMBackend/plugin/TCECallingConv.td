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
  CCIfType<[f32], CCAssignToReg<[FRES0]>>
]>;

def CC_TCE : CallingConv<[
  // All arguments get passed in integer registers if there is space.
  // CCIfType<[i32, f32, f64], CCAssignToReg<[I0, I1, I2, I3, I4, I5]>>,
  
  // Alternatively, they are assigned to the stack in 4-byte aligned units.
  CCAssignToStack<4, 4>
]>;
