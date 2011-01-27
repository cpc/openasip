-----------------------------------------------------------------------------------------------------------
--
--                                      MILK COPROCESSOR BASIC DEFINITIONS
--
-- This file contains basic parameters definitions.	
--
-- Created by Claudio Brunelli, 2004
--
-----------------------------------------------------------------------------------------------------------

--Copyright (c) 2004, Tampere University of Technology.
--All rights reserved.

--Redistribution and use in source and binary forms, with or without modification,
--are permitted provided that the following conditions are met:
--*	Redistributions of source code must retain the above copyright notice,
--	this list of conditions and the following disclaimer.
--*	Redistributions in binary form must reproduce the above copyright notice,
--	this list of conditions and the following disclaimer in the documentation
--	and/or other materials provided with the distribution.
--*	Neither the name of Tampere University of Technology nor the names of its
--	contributors may be used to endorse or promote products derived from this
--	software without specific prior written permission.

--THIS HARDWARE DESCRIPTION OR SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
--CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
--LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND NONINFRINGEMENT AND
--FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
--OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
--EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
--PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
--BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
--CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
--ARISING IN ANY WAY OUT OF THE USE OF THIS HARDWARE DESCRIPTION OR SOFTWARE, EVEN
--IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;


package cop_definitions is 

-----------------------------------------------------------------------------------------------------------
--
-- bus width configuration
--
-----------------------------------------------------------------------------------------------------------

    constant word_width           : positive := 32;  -- data bus width
    constant sreg_width           : positive := 14;  -- status register width
    constant buffer_width         : positive := 32;  -- RF output tristate buffers width
    constant RF_width             : positive := 8;   -- number of general purpose registers                            
    constant instr_word_width     : positive := 24;  -- amount of significative bits loaded in control register
    constant exceptions_amount    : positive := 22;  -- overall number of "internal" exceptions
    constant RF_exceptions_amount : positive := 1;   -- number of "internal" exceptions related to the register file
    constant flags_amount         : positive := 7;   -- number of status flag bits
    constant cop_opcode_width     : positive := 6;   -- amount of bits of the "opcode" field of the instruction word

-----------------------------------------------------------------------------------------------------------
--
-- ctrl_logic configuration
--
-----------------------------------------------------------------------------------------------------------

    constant wrbck_sgls_width     : positive := 8;   -- wrbck signals set in ctrl_logic
    constant m                    : positive := 12;  -- amount of sub-buses in the register delay chain (ctrl_logic)
    constant n                    : positive := 3;   -- width of sub-buses in the register delay chain (ctrl_logic)

    constant conv_clk_cycles      : positive := 2;   -- latency (in clk cycles) required by conv
    constant trunc_clk_cycles     : positive := 2;   -- latency (in clk cycles) required by trunc
    constant mul_clk_cycles       : positive := 3;   -- latency (in clk cycles) required by mul
    constant div_clk_cycles       : positive := 12;  -- latency (in clk cycles) required by div
    constant add_clk_cycles       : positive := 5;   -- latency (in clk cycles) required by add
    constant sqrt_clk_cycles      : positive := 8;   -- latency (in clk cycles) required by sqrt

-----------------------------------------------------------------------------------------------------------
--
-- main constants
--
-----------------------------------------------------------------------------------------------------------

    constant reset_active       : Std_logic := '0';  -- '1' -> positive logic 
    constant we_active          : Std_logic := '1';
    constant oe_active          : Std_logic := '1';
    constant ovfw_active        : Std_logic := '1';
    constant underfw_active     : Std_logic := '1';

-----------------------------------------------------------------------------------------------------------
--
-- FUs insertion generics' boolean values
--
-----------------------------------------------------------------------------------------------------------

    constant conv_flag_value    : integer := 1;
    constant trunc_flag_value   : integer := 1;
    constant mul_flag_value     : integer := 1;
    constant div_flag_value     : integer := 1;
    constant add_flag_value     : integer := 1;
    constant sqrt_flag_value    : integer := 1;
    constant compare_flag_value : integer := 1;

-----------------------------------------------------------------------------------------------------------
--
-- "integer_sqrt" block configuration constants
--
-----------------------------------------------------------------------------------------------------------

    constant radicand_width : positive := 52;  -- operand width
    constant sqrt_width     : positive := 26;  -- result width
    constant rem_width      : positive := 28;  -- remainder width
    constant k_max          : positive := 26;  -- number of "iterations" in the algorithm
    
-----------------------------------------------------------------------------------------------------------
--
-- "integer_divider" block configuration constants
--
-----------------------------------------------------------------------------------------------------------

    constant dividend_width     : positive := 24;  -- operand width
    constant divisor_width      : positive := 24;  -- operand width
    constant quotient_width     : positive := 25;  -- result width
    constant division_rem_width : positive := 25;  -- remainder width
    constant div_k_max          : positive := 25;  -- number of "iterations" in the algorithm

    type bus_8Xd is array (8 downto 0) of std_logic_vector(divisor_width+1 downto 0);

-----------------------------------------------------------------------------------------------------------
--
-- user defined types
--
-----------------------------------------------------------------------------------------------------------

    type bus8X32 is array (RF_width-1 downto 0) of std_logic_vector(word_width-1 downto 0);

    type bus_mXn is array (m downto 0) of std_logic_vector(n-1 downto 0);
      
    subtype rf_addr is std_logic_vector(2 downto 0);  --  RF registers' address bits

    subtype wb_code is std_logic_vector(3 downto 0);  --  result writeback logic's "indexing" bits

    subtype cop_opcode is std_logic_vector(cop_opcode_width-1 downto 0);  --  opcode specification bits

-----------------------------------------------------------------------------------------------------------
--
-- instruction set mnemonics
--
-----------------------------------------------------------------------------------------------------------
-- opcodes are the same as those in the MIPS' coprocessor instruction set
    
    constant cop_add_s   : cop_opcode := "000000";  -- adds two FP single precision numbers                        : ADD.S
    constant cop_sub_s   : cop_opcode := "000001";  -- subtracts two FP single precision numbers                   : SUB.S
    constant cop_mul_s   : cop_opcode := "000010";  -- multiplies two FP single precision numbers                  : MUL.S 
    constant cop_div_s   : cop_opcode := "000011";  -- divides two FP single precision numbers                     : DIV.S
    constant cop_sqrt_s  : cop_opcode := "000100";  -- extracts the square root of a FP single precision number    : SQRT.S
    constant cop_abs_s   : cop_opcode := "000101";  -- extracts the absolute value of a FP single precision number : ABS.S
    constant cop_mov_s   : cop_opcode := "000110";  -- moves a value from a register to another                    : MOV.S
    constant cop_neg_s   : cop_opcode := "000111";  -- inverts the sign of a single precision FP number            : NEG.S
    constant nop         : cop_opcode := "001000";  -- no operation executed                                       : NOP.S
    constant cop_trunc_w : cop_opcode := "001101"; 
    constant cop_cvt_s   : cop_opcode := "100000";  -- converts an integer into a single precision FP              : CVT.S  
    constant cop_cvt_w   : cop_opcode := "100100";  -- converts a single precision FP into an integer              : CVT.W

-- Note: the comparison is obtained through a set of 16 sub-instructions whose opcodes are charaterized by the two
-- most-significant bits set

------------------------------------------------------------------------------------------------------------
--
-- result writeback selection signals
--
------------------------------------------------------------------------------------------------------------
   
    constant wb_cvt_s     : wb_code := "0000";
    constant wb_trunc_w   : wb_code := "0001";
    constant wb_mul_s     : wb_code := "0010";
    constant wb_div_s     : wb_code := "0011";
    constant wb_addsub_s  : wb_code := "0100";  
    constant wb_sqrt_s    : wb_code := "0101";
    constant wb_abs_s     : wb_code := "0110";
    constant wb_mov_s     : wb_code := "0111";
    constant wb_neg_s     : wb_code := "1000";
    constant wb_compare_s : wb_code := "1001";

end cop_definitions ;


package body cop_definitions is
end cop_definitions;










