-- Copyright (c) 2002-2009 Tampere University of Technology.
--
-- This file is part of TTA-Based Codesign Environment (TCE).
-- 
-- Permission is hereby granted, free of charge, to any person obtaining a
-- copy of this software and associated documentation files (the "Software"),
-- to deal in the Software without restriction, including without limitation
-- the rights to use, copy, modify, merge, publish, distribute, sublicense,
-- and/or sell copies of the Software, and to permit persons to whom the
-- Software is furnished to do so, subject to the following conditions:
-- 
-- The above copyright notice and this permission notice shall be included in
-- all copies or substantial portions of the Software.
-- 
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
-- FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
-- DEALINGS IN THE SOFTWARE.
-------------------------------------------------------------------------------

-- 64-BIT LARGE ALU (ALU64) for TTA64 Project.
-- Designer: Latif AKCAY
-- University: Bayburt University, Istanbul Technical University, TURKEY.

library IEEE;
use IEEE.std_Logic_1164.all;
use IEEE.numeric_std.all;

package opcodes_add64_and64_eq64_gt64_gtu64_ior64_ltu64_ne64_shl1add64_shl64_shr64_shru64_sub64_sxh64_sxw64_xor64 is

  constant ADD64_OPC     : std_logic_vector(3 downto 0)  := "0000";  
  constant AND64_OPC     : std_logic_vector(3 downto 0)  := "0001";
  constant EQ64_OPC      : std_logic_vector(3 downto 0)  := "0010";
  constant GT64_OPC      : std_logic_vector(3 downto 0)  := "0011";
  constant GTU64_OPC     : std_logic_vector(3 downto 0)  := "0100";
  constant IOR64_OPC     : std_logic_vector(3 downto 0)  := "0101";
  constant LTU64_OPC     : std_logic_vector(3 downto 0)  := "0110";
  constant NE64_OPC      : std_logic_vector(3 downto 0)  := "0111";
  constant SHL1ADD64_OPC : std_logic_vector(3 downto 0)  := "1000";
  constant SHL64_OPC     : std_logic_vector(3 downto 0)  := "1001";
  constant SHR64_OPC     : std_logic_vector(3 downto 0)  := "1010";
  constant SHRU64_OPC    : std_logic_vector(3 downto 0)  := "1011";
  constant SUB64_OPC     : std_logic_vector(3 downto 0)  := "1100";
  constant SXH64_OPC     : std_logic_vector(3 downto 0)  := "1101";
  constant SXW64_OPC     : std_logic_vector(3 downto 0)  := "1110";
  constant XOR64_OPC     : std_logic_vector(3 downto 0)  := "1111";

end opcodes_add64_and64_eq64_gt64_gtu64_ior64_ltu64_ne64_shl1add64_shl64_shr64_shru64_sub64_sxh64_sxw64_xor64;

library IEEE;
use IEEE.std_Logic_1164.all;
use IEEE.numeric_std.all;
use work.util.all;
use work.opcodes_add64_and64_eq64_gt64_gtu64_ior64_ltu64_ne64_shl1add64_shl64_shr64_shru64_sub64_sxh64_sxw64_xor64.all;


package monolithic_alu_shladd_large_shift_pkg is

  function shift_func (input: std_logic_vector; shft_amount : std_logic_vector;
                       opc : std_logic_vector;dataw : integer; shiftw : integer) 
    return std_logic_vector;
end monolithic_alu_shladd_large_shift_pkg;

package body monolithic_alu_shladd_large_shift_pkg is

  function shift_func (input: std_logic_vector; shft_amount : std_logic_vector;
                       opc: std_logic_vector;dataw : integer; shiftw : integer) 
    return std_logic_vector is
    
    constant max_shift : integer := shiftw;        
    variable shift_in : std_logic;
    type std_logic_vector_array is array (natural range <>) of std_logic_vector(dataw-1 downto 0);
    variable y_temp : std_logic_vector_array (0 to max_shift);
    variable y : std_logic_vector(dataw-1 downto 0);
    variable shift_ammount : std_logic_vector(shiftw-1 downto 0);
  begin
    shift_ammount := shft_amount(shiftw-1 downto 0);
    
    if ((opc = SHR64_OPC) or (opc = SHRU64_OPC)) then
      y_temp(0) := flip_bits(input);      
    else
      y_temp(0) := input;
    end if;

    if (opc = SHR64_OPC) then
      shift_in := y_temp(0)(0);
    else
      shift_in := '0';         
    end if;

    
    for i in 0 to max_shift-1 loop
      if (shift_ammount(i) = '1') then
        y_temp(i+1)                       := (others => shift_in);
        y_temp(i+1) (dataw-1 downto 2**i) := y_temp(i) (dataw-1-2**i downto 0);
      else
        y_temp(i+1) := y_temp(i);
      end if;
    end loop;  -- i

    if ( (opc = SHR64_OPC) or (opc = SHRU64_OPC)) then
      y := flip_bits(y_temp(max_shift));
    else
      y :=  y_temp(max_shift);    
    end if;
    return y;
  end shift_func;
end monolithic_alu_shladd_large_shift_pkg;

library IEEE;
use IEEE.numeric_std.all;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use work.opcodes_add64_and64_eq64_gt64_gtu64_ior64_ltu64_ne64_shl1add64_shl64_shr64_shru64_sub64_sxh64_sxw64_xor64.all;
use work.monolithic_alu_shladd_large_shift_pkg.all;

entity add64_and64_eq64_gt64_gtu64_ior64_ltu64_ne64_shl1add64_shl64_shr64_shru64_sub64_sxh64_sxw64_xor64 is
  generic (
    dataw  : integer := 64;
    busw   : integer := 64;
    shiftw : integer := 5);
  port(
    A   : in  std_logic_vector(dataw-1 downto 0);
    B   : in  std_logic_vector(dataw-1 downto 0);
    OPC : in std_logic_vector(3 downto 0);
    R   : out std_logic_vector(dataw-1 downto 0)
    );
end add64_and64_eq64_gt64_gtu64_ior64_ltu64_ne64_shl1add64_shl64_shr64_shru64_sub64_sxh64_sxw64_xor64;


architecture comb of add64_and64_eq64_gt64_gtu64_ior64_ltu64_ne64_shl1add64_shl64_shr64_shru64_sub64_sxh64_sxw64_xor64 is
  signal add_op1       : std_logic_vector(dataw-1 downto 0);
  signal add_result    : std_logic_vector(dataw-1 downto 0);
  signal shift_result  : std_logic_vector(dataw-1 downto 0);
  signal gt            : std_logic_vector(0 downto 0);
  signal gtu           : std_logic_vector(0 downto 0);
  signal eq            : std_logic_vector(0 downto 0);
  signal neq           : std_logic_vector(0 downto 0);
  signal ltu           : std_logic_vector(0 downto 0);
  signal cmp           : std_logic_vector(0 downto 0);
  signal cmp_ext       : std_logic_vector(dataw-1 downto 0);
begin
  
  gt  <= "1" when (ieee.numeric_std.signed(B) > ieee.numeric_std.signed(A)) else "0";
  gtu <= "1" when (ieee.numeric_std.unsigned(B) > ieee.numeric_std.unsigned(A)) else "0";
  eq  <= "1" when (A=B) else "0";
  neq <= "1" when (A/=B) else "0";  
  ltu <= "1" when (ieee.numeric_std.unsigned(B) < ieee.numeric_std.unsigned(A)) else "0";
  
  process (A,OPC)
  begin
    case OPC is
      when ADD64_OPC =>
        add_op1 <= A;
      when SHL1ADD64_OPC =>
        add_op1 <= A(dataw-2 downto 0)&'0';
      when others => -- SHL2ADD_OPC or others
        add_op1 <= A(dataw-3 downto 0)&"00";
    end case;
  end process;

  process (A,B,OPC, eq, gt, gtu, neq, ltu)
  begin
    case OPC is
      when EQ64_OPC => 
        cmp <= eq;
      when GT64_OPC => 
        cmp <= gt;
      when GTU64_OPC => 
        cmp <= gtu;
      when NE64_OPC =>
        cmp <= neq;
      when LTU64_OPC =>
        cmp <= ltu;
      when others => -- min max or others
        cmp <= not gtu;
    end case;
  end process;

  add_result    <= std_logic_vector(ieee.numeric_std.signed(add_op1) + ieee.numeric_std.signed(B));
  shift_result  <= shift_func(B,A(shiftw-1 downto 0),OPC,dataw,shiftw);
  cmp_ext       <= ext(cmp, R'length);

  process (A,B,OPC, add_result, shift_result, cmp_ext)
  begin  -- process
    case OPC is
      when ADD64_OPC =>
         R  <= add_result;
      when SHL1ADD64_OPC =>
         R  <= add_result;
      when SUB64_OPC => 
         R  <= std_logic_vector(ieee.numeric_std.signed(A) - ieee.numeric_std.signed(B));
      when EQ64_OPC  =>
        R <= cmp_ext;       
      when GT64_OPC =>
        R <= cmp_ext;
      when GTU64_OPC =>
        R <= cmp_ext;
      when SHL64_OPC =>
        R <= shift_result;
      when SHR64_OPC =>
        R <= shift_result;
      when SHRU64_OPC =>
        R <= shift_result;
      when AND64_OPC =>
        R <= A and B;
      when IOR64_OPC =>
        R <= A or B;
      when XOR64_OPC =>
        R <= A xor B;        
      when SXH64_OPC =>
        R <= SXT(A(15 downto 0), R'length);
      when SXW64_OPC =>
        R <= SXT(A(dataw/2-1 downto 0), R'length);      
      when others  =>
        R <= cmp_ext;
    end case;
  end process;
end comb;

-------------------------------------------------------------------------------
-- Entity declaration for unit add64_and64_eq64_gt64_gtu64_ior64_ltu64_ne64_shl1add64_shl64_shr64_shru64_sub64_sxh64_sxw64_xor64 latency 1
-------------------------------------------------------------------------------

library IEEE;
use IEEE.std_Logic_1164.all;
use IEEE.std_Logic_arith.all;


entity fu_add64_and64_eq64_gt64_gtu64_ior64_ltu64_ne64_shl1add64_shl64_shr64_shru64_sub64_sxh64_sxw64_xor64_always_1 is
  generic (
    dataw : integer := 64;              -- Operand Width
    busw  : integer := 64;              -- Bus Width
    shiftw : integer := 5
    );             

  port (
    clk : in std_logic;
    rstx : in std_logic;
    glock : in std_logic;
    operation_in : in std_logic_vector(3 downto 0);
    data_in1t_in : in std_logic_vector(dataw-1 downto 0);
    load_in1t_in : in std_logic;
    data_in2_in :  in std_logic_vector(dataw-1 downto 0);
    load_in2_in :  in std_logic;
    data_out1_out : out std_logic_vector(dataw-1 downto 0)
     );

end fu_add64_and64_eq64_gt64_gtu64_ior64_ltu64_ne64_shl1add64_shl64_shr64_shru64_sub64_sxh64_sxw64_xor64_always_1;

architecture rtl of fu_add64_and64_eq64_gt64_gtu64_ior64_ltu64_ne64_shl1add64_shl64_shr64_shru64_sub64_sxh64_sxw64_xor64_always_1 is
  
  component add64_and64_eq64_gt64_gtu64_ior64_ltu64_ne64_shl1add64_shl64_shr64_shru64_sub64_sxh64_sxw64_xor64
    generic (
      dataw : integer := 64;
      shiftw : integer := 5);
    port(
      A   : in  std_logic_vector(dataw-1 downto 0);
      B   : in  std_logic_vector(dataw-1 downto 0);
      OPC : in std_logic_vector(3 downto 0);
      R   : out std_logic_vector(dataw-1 downto 0)
      );
  end component;

  signal data_in1t_in_reg       : std_logic_vector(dataw-1 downto 0);
  signal data_in2_in_reg        : std_logic_vector(dataw-1 downto 0);
  signal data_in2_in_tempreg    : std_logic_vector(dataw-1 downto 0);
  signal data_out1_out_reg      : std_logic_vector(dataw-1 downto 0);
  signal opc_reg : std_logic_vector(3 downto 0);
  signal control : std_logic_vector(1 downto 0);
  
begin
  
  fu_arch : add64_and64_eq64_gt64_gtu64_ior64_ltu64_ne64_shl1add64_shl64_shr64_shru64_sub64_sxh64_sxw64_xor64
    generic map (
      dataw => dataw,
      shiftw => shiftw)
    port map(
      A   => data_in1t_in_reg,
      B   => data_in2_in_reg,
      OPC => opc_reg,
      R   => data_out1_out_reg
      );

  control <= load_in2_in & load_in1t_in;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous Ret (active low)
      data_in1t_in_reg     <= (others => '0');
      data_in2_in_reg      <= (others => '0');
      data_in2_in_tempreg  <= (others => '0');
      opc_reg              <= (others => '0');
      
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then

        case control is
          when "11" =>
            data_in1t_in_reg     <= data_in1t_in;
            data_in2_in_reg      <= data_in2_in;
            data_in2_in_tempreg  <= data_in2_in;
            opc_reg <= operation_in;
          when "10" =>
            data_in2_in_tempreg <= data_in2_in;
          when "01" =>
            opc_reg <= operation_in;            
            data_in1t_in_reg   <= data_in1t_in;
            data_in2_in_reg    <= data_in2_in_tempreg;
          when others => null;
        end case;

      end if;
    end if;
  end process regs;

  process (data_out1_out_reg)
  begin  -- process
    if busw < dataw then
      if busw > 1 then
        data_out1_out(busw-1) <= data_out1_out_reg(dataw-1);
        data_out1_out(busw-2 downto 0) <= data_out1_out_reg(busw-2 downto 0);
      else
        data_out1_out(0) <= data_out1_out_reg(0);
      end if;
    else
      data_out1_out <= sxt(data_out1_out_reg,data_out1_out_reg'length);
    end if;
  end process;

end rtl;

