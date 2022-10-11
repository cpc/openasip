// Copyright (c) 2012 Vinogradov Viacheslav
//
// This file is part of TTA-Based Codesign Environment (TCE).
// 
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal input the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included input
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. input NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER input AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, output OF OR input CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS input THE SOFTWARE.
//////////////////////////////////////////////////////////////////////////////
// Title      : Load store unit
//////////////////////////////////////////////////////////////////////////////
// File       : lsu.v
// Author     : Vinogradov Viacheslav
//////////////////////////////////////////////////////////////////////////////
// Description: Add/Logic functional unit for TTA
//              -FU core logic as a separate component
//              -Supports SVTL pipelining discipline
//              opcode  0 add o1+t1
//                      1 and o1&t1
//                      2 equ o1==t1
//                      3 gt  o1>t1
//                      4 gtu o1>t1
//                      5 ior o1|t1
//                      6 shl o1<<<t1
//                      7 shr o1>>>t1
//                      8 shru o1>>t1
//                      9 sub o1-t1
//                      10 sxhw o1 t1
//                      11 sxqw o1 t1
//                      12 xor o1^t1
//
//////////////////////////////////////////////////////////////////////////////
// Revisions  :
// 2012-04-04  1.0  Vinogradov 
//////////////////////////////////////////////////////////////////////////////

module add_and_eq_gt_gtu_ior_shl_shr_shru_sub_sxhw_sxqw_xor_arith
#(
    parameter dataw = 32,
    parameter shiftw = 5
)
(
    input  [dataw-1 : 0]A,
    input  [3 : 0]     opc,
    input  [dataw-1 : 0]B,
    output reg[dataw-1 : 0]Z
);

    localparam OPC_ADD = 0;
    localparam OPC_AND = 1;
    localparam OPC_EQ  = 2;
    localparam OPC_GT  = 3;
    localparam OPC_GTU = 4;
    localparam OPC_IOR = 5;
    localparam OPC_SHL = 6;
    localparam OPC_SHR = 7;
    localparam OPC_SHRU= 8;
    localparam OPC_SUB = 9;
    localparam OPC_SXHW= 10;
    localparam OPC_SXQW= 11;
    localparam OPC_XOR = 12;

    function[dataw-1:0] shift_func;
    input[dataw-1:0]    d_in;
    input[shiftw-1:0]   shft_amount;
    input integer opc;
    begin
        if(opc==OPC_SHR)
            shift_func= $signed(d_in)>>>shft_amount;
        else if(opc==OPC_SHRU)
            shift_func= d_in>>shft_amount;
        else//OPC_SHL
            shift_func= d_in<<shft_amount;
    end
    endfunction

    always@(*)
    case(opc)
       OPC_ADD:Z = $signed(A) + $signed(B);
       OPC_AND: Z = A & B;
       OPC_IOR: Z = A | B;
       OPC_EQ :
            if(A == B)
                Z = $unsigned(1);
            else
                Z = $unsigned(0);
       OPC_GT:
            if($signed(A) > $signed(B))
                Z = $unsigned(1);
            else
                Z = $unsigned(0);
       OPC_GTU:
            // operation GTU;
            if($unsigned(A) > $unsigned(B))
                Z = $unsigned(1);
            else
                Z = $unsigned(0);
       OPC_SHL:
            Z = shift_func(B,A[shiftw-1 : 0],OPC_SHL);
       OPC_SHR:
            Z = shift_func(B,A[shiftw-1 : 0],OPC_SHR);
       OPC_SHRU:
            Z = shift_func(B,A[shiftw-1 : 0],OPC_SHRU);
        OPC_SUB:
            Z =  $signed(A) - $signed(B);
       OPC_SXQW:
            Z = $signed(A[7 : 0]);
       OPC_SXHW:
            Z = $signed(A[dataw/2-1 : 0]);
        
       default: Z = A ^ B;
    endcase
endmodule

//////////////////////////////////////////////////////////////////////////////-
// Add/logic unit latency 1
//////////////////////////////////////////////////////////////////////////////-
module fu_add_and_eq_gt_gtu_ior_shl_shr_shru_sub_sxhw_sxqw_xor_always_1
#(
    parameter dataw = 32,
    parameter shiftw = 32
)
(
    input[dataw-1 : 0]t1data,
    input[3 : 0]t1opcode,
    input t1load,

    input[dataw-1 : 0]o1data,
    input o1load,

    output[dataw-1 : 0]r1data,
    input  glock,
    input  rstx,
    input  clk
);
    reg[3 : 0] opc1reg;
    reg[dataw-1 : 0] t1reg;
    reg[dataw-1 : 0] o1reg;
    reg[dataw-1 : 0] o1temp;
    
    wire[dataw-1 : 0] r1;
    wire[1:0] control = {o1load,t1load};
  
    always@(posedge clk or negedge rstx)
    if(!rstx)
    begin
        t1reg   <= {dataw{1'b0}};
        opc1reg <= {4{1'b0}};
        o1reg   <= {dataw{1'b0}};
        o1temp  <= {dataw{1'b0}};
    end
    else
    if(!glock)
        case(control)
            3:
            begin
                o1reg   <= o1data;
                o1temp  <= o1data;
                t1reg   <= t1data;
                opc1reg <= t1opcode;
            end
            2:o1temp <= o1data;
            1:
            begin
                o1reg   <= o1temp;
                t1reg   <= t1data;
                opc1reg <= t1opcode;
            end
            default:
            begin
            end
        endcase

    add_and_eq_gt_gtu_ior_shl_shr_shru_sub_sxhw_sxqw_xor_arith
    #(
        .dataw(dataw),
        .shiftw(shiftw)
    )
    fu_arch
    (
        .A   (t1reg),
        .opc (opc1reg),
        .B   (o1reg),
        .Z   (r1)
    );

    assign r1data = r1;

endmodule
