-------------------------------------------------------------------------------
-- Title      : 
-- Project    : 
-------------------------------------------------------------------------------
-- File       : cadd.vhd
-- Author     : Risto Mäkinen <rmmakine@cs.tut.fi>
-- Company    : IDCS
-- Created    : 2004-09-22
-- Last update: 2005/11/22
-- Platform   : 
-------------------------------------------------------------------------------
-- Description: VHDL implementation of SFU for R4 DIT FFT's complex number butterfly
-- summations. There are 4 input operands and a trigger operand that
-- tells the format in which the input operands are added together.
-- There are 4 possible summation formats, that can be encoded by 2 bits,
-- i.e. '00','01','10' and '11'. The overflow in every addition is
-- always avoided by dividing the result of every addition by 2.
-------------------------------------------------------------------------------
-- Copyright (c) 2004 
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-- Entity declaration for r4bfidx unit's user-defined architecture 
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;
--use IEEE.numeric_std.all;
--use IEEE.numeric_signed.all;
entity cadd is
  
  generic (
    complexw : integer := 32;           -- bit width of complex number
    reimw    : integer := 16);           -- bit width of real and imaginary parts of a complex number

  port (
    num1  : in std_logic_vector(complexw-1 downto 0);
    num2  : in std_logic_vector(complexw-1 downto 0);
    num3  : in std_logic_vector(complexw-1 downto 0);
    num4  : in std_logic_vector(complexw-1 downto 0);
    format : in std_logic_vector(1 downto 0);
    sum    : out std_logic_vector(complexw-1 downto 0)
  );
  
end cadd;

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;
--use IEEE.numeric_std.all;
--use IEEE.numeric_signed.all;
entity sxt_add_sub_sra1 is
  generic (
    reimw : integer := 16);
  port (
    A : in std_logic_vector(reimw-1 downto 0);
    B : in std_logic_vector(reimw-1 downto 0);
    operation : in std_logic;
    C : out std_logic_vector(reimw-1 downto 0)
    );
end sxt_add_sub_sra1;

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;
--use IEEE.numeric_std.all;
--use IEEE.numeric_signed.all;


-------------------------------------------------------------------------------
-- Architecture declaration for cadd unit's user-defined architecture
-------------------------------------------------------------------------------

architecture rtl of cadd is
  signal sum_reg : std_logic_vector(complexw-1 downto 0);
  signal re_num1 : std_logic_vector(reimw-1 downto 0);
  signal im_num1 : std_logic_vector(reimw-1 downto 0);
  signal re_num2 : std_logic_vector(reimw-1 downto 0);
  signal im_num2 : std_logic_vector(reimw-1 downto 0);
  signal re_num3 : std_logic_vector(reimw-1 downto 0); 
  signal im_num3 : std_logic_vector(reimw-1 downto 0);
  signal re_num4 : std_logic_vector(reimw-1 downto 0); 
  signal im_num4 : std_logic_vector(reimw-1 downto 0);
  signal re_num1_reg : std_logic_vector(reimw-1 downto 0);
  signal im_num1_reg : std_logic_vector(reimw-1 downto 0);
  signal re_num2_reg : std_logic_vector(reimw-1 downto 0);
  signal im_num2_reg : std_logic_vector(reimw-1 downto 0);
  signal re_num3_reg : std_logic_vector(reimw-1 downto 0); 
  signal im_num3_reg : std_logic_vector(reimw-1 downto 0);
  signal re_num4_reg : std_logic_vector(reimw-1 downto 0); 
  signal im_num4_reg : std_logic_vector(reimw-1 downto 0);   

begin  -- rtl
  process (num1,num2,num3,num4)
  begin  -- process
    re_num1_reg <= num1(complexw-1 downto reimw);
    im_num1_reg <= num1(reimw-1 downto 0);
    re_num2_reg <= num2(complexw-1 downto reimw);
    im_num2_reg <= num2(reimw-1 downto 0);
    re_num3_reg <= num3(complexw-1 downto reimw);
    im_num3_reg <= num3(reimw-1 downto 0);
    re_num4_reg <= num4(complexw-1 downto reimw);
    im_num4_reg <= num4(reimw-1 downto 0);
  end process;
  
  re_num1 <= re_num1_reg;
  re_num2 <= re_num2_reg;
  re_num3 <= re_num3_reg;
  re_num4 <= re_num4_reg;
  im_num1 <= im_num1_reg;
  im_num2 <= im_num2_reg;
  im_num3 <= im_num3_reg;
  im_num4 <= im_num4_reg;
  
  process (re_num1,re_num2,re_num3,re_num4,im_num1,im_num2,im_num3,im_num4, format)
    variable re_sum : std_logic_vector(reimw-1 downto 0);
    variable im_sum : std_logic_vector(reimw-1 downto 0);
    variable temp    : std_logic_vector(reimw downto 0);    
  begin  -- process

    case format is
      when "00" =>
        --Calculate real and imaginary parts of the sum
        temp := signed(sxt(re_num1,reimw+1)) + signed(sxt(re_num2,reimw+1));
        re_sum(reimw-1 downto 0) := temp(reimw downto 1);
        temp := signed(sxt(re_sum,reimw+1)) + signed(sxt(re_num3,reimw+1));
        re_sum(reimw-1 downto 0) := temp(reimw downto 1);
        temp := signed(sxt(re_sum,reimw+1)) + signed(sxt(re_num4,reimw+1));
        re_sum(reimw-1 downto 0) := temp(reimw downto 1);

        temp := signed(sxt(im_num1,reimw+1)) + signed(sxt(im_num2,reimw+1));
        im_sum(reimw-1 downto 0) := temp(reimw downto 1);
        temp := signed(sxt(im_sum,reimw+1)) + signed(sxt(im_num3,reimw+1));
        im_sum(reimw-1 downto 0) := temp(reimw downto 1);
        temp := signed(sxt(im_sum,reimw+1)) + signed(sxt(im_num4,reimw+1));
        im_sum(reimw-1 downto 0) := temp(reimw downto 1);

        --Assign imal and imaginary parts to the final sum complex number
        sum_reg(complexw-1 downto reimw) <= re_sum;
        sum_reg(reimw-1 downto 0) <= im_sum;

      when "01" =>
        --Calculate real and imaginary parts of the sum
        temp := signed(sxt(re_num1,reimw+1)) + signed(sxt(im_num2,reimw+1));
        re_sum(reimw-1 downto 0) := temp(reimw downto 1);
        temp := signed(sxt(re_sum,reimw+1)) - signed(sxt(re_num3,reimw+1));
        re_sum(reimw-1 downto 0) := temp(reimw downto 1);
        temp := signed(sxt(re_sum,reimw+1)) + signed(sxt(im_num4,reimw+1));
        re_sum(reimw-1 downto 0) := temp(reimw downto 1);

        temp := signed(sxt(im_num1,reimw+1)) - signed(sxt(re_num2,reimw+1));
        im_sum(reimw-1 downto 0) := temp(reimw downto 1);
        temp := signed(sxt(im_sum,reimw+1)) - signed(sxt(im_num3,reimw+1));
        im_sum(reimw-1 downto 0) := temp(reimw downto 1);
        temp := signed(sxt(im_sum,reimw+1)) - signed(sxt(re_num4,reimw+1));
        im_sum(reimw-1 downto 0) := temp(reimw downto 1);

        --Assign imal and imaginary parts to the final sum complex number
        sum_reg(complexw-1 downto reimw) <= re_sum;
        sum_reg(reimw-1 downto 0) <= im_sum;        

      when "10" =>
        --Calculate real and imaginary parts of the sum
        temp := signed(sxt(re_num1,reimw+1)) - signed(sxt(re_num2,reimw+1));
        re_sum(reimw-1 downto 0) := temp(reimw downto 1);
        temp := signed(sxt(re_sum,reimw+1)) + signed(sxt(re_num3,reimw+1));
        re_sum(reimw-1 downto 0) := temp(reimw downto 1);
        temp := signed(sxt(re_sum,reimw+1)) - signed(sxt(re_num4,reimw+1));
        re_sum(reimw-1 downto 0) := temp(reimw downto 1);

        temp := signed(sxt(im_num1,reimw+1)) - signed(sxt(im_num2,reimw+1));
        im_sum(reimw-1 downto 0) := temp(reimw downto 1);
        temp := signed(sxt(im_sum,reimw+1)) + signed(sxt(im_num3,reimw+1));
        im_sum(reimw-1 downto 0) := temp(reimw downto 1);
        temp := signed(sxt(im_sum,reimw+1)) - signed(sxt(im_num4,reimw+1));
        im_sum(reimw-1 downto 0) := temp(reimw downto 1);

        --Assign imal and imaginary parts to the final sum complex number
        sum_reg(complexw-1 downto reimw) <= re_sum;
        sum_reg(reimw-1 downto 0) <= im_sum;                

      when "11" =>
        --Calculate real and imaginary parts of the sum
        temp := signed(sxt(re_num1,reimw+1)) - signed(sxt(im_num2,reimw+1));
        re_sum(reimw-1 downto 0) := temp(reimw downto 1);
        temp := signed(sxt(re_sum,reimw+1)) - signed(sxt(re_num3,reimw+1));
        re_sum(reimw-1 downto 0) := temp(reimw downto 1);
        temp := signed(sxt(re_sum,reimw+1)) - signed(sxt(im_num4,reimw+1));
        re_sum(reimw-1 downto 0) := temp(reimw downto 1);

        temp := signed(sxt(im_num1,reimw+1)) + signed(sxt(re_num2,reimw+1));
        im_sum(reimw-1 downto 0) := temp(reimw downto 1);
        temp := signed(sxt(im_sum,reimw+1)) - signed(sxt(im_num3,reimw+1));
        im_sum(reimw-1 downto 0) := temp(reimw downto 1);
        temp := signed(sxt(im_sum,reimw+1)) + signed(sxt(re_num4,reimw+1));
        im_sum(reimw-1 downto 0) := temp(reimw downto 1);

        --Assign imal and imaginary parts to the final sum complex number
        sum_reg(complexw-1 downto reimw) <= re_sum;
        sum_reg(reimw-1 downto 0) <= im_sum;                        

      when others => null;
    end case;
   end process; 
   sum <= sum_reg;
end rtl;

architecture rtl2 of cadd is
   signal sum_reg : std_logic_vector(complexw-1 downto 0);     
begin  -- rtl2
  
   -- purpose: calculate correct shifted sum according to summation format
   -- type   : combinational
   -- inputs : num1, num2, num3, num4, format
   -- outputs: sum
  process (num1, num2, num3, num4, format)
    variable re_sum : std_logic_vector(reimw-1 downto 0);
    variable im_sum : std_logic_vector(reimw-1 downto 0);
    variable re_num1 : std_logic_vector(reimw-1 downto 0);
    variable im_num1 : std_logic_vector(reimw-1 downto 0);
    variable re_num2 : std_logic_vector(reimw-1 downto 0);
    variable im_num2 : std_logic_vector(reimw-1 downto 0);
    variable re_num3 : std_logic_vector(reimw-1 downto 0); 
    variable im_num3 : std_logic_vector(reimw-1 downto 0);
    variable re_num4 : std_logic_vector(reimw-1 downto 0); 
    variable im_num4 : std_logic_vector(reimw-1 downto 0);
    variable temp    : std_logic_vector(reimw downto 0);    
  begin  -- process

    re_num1 := num1(complexw-1 downto reimw);
    im_num1 := num1(reimw-1 downto 0);
    re_num2 := num2(complexw-1 downto reimw);
    im_num2 := num2(reimw-1 downto 0);
    re_num3 := num3(complexw-1 downto reimw);
    im_num3 := num3(reimw-1 downto 0);
    re_num4 := num4(complexw-1 downto reimw);
    im_num4 := num4(reimw-1 downto 0);
    
    case format is
      when "00" =>
        --Calculate real and imaginary parts of the sum
        temp := signed(sxt(re_num1,reimw+1)) + signed(sxt(re_num1,reimw+1));
        re_sum(reimw-1 downto 0) := temp(reimw downto 1);
        temp := signed(sxt(re_sum,reimw+1)) + signed(sxt(re_num3,reimw+1));
        re_sum(reimw-1 downto 0) := temp(reimw downto 1);
        temp := signed(sxt(re_sum,reimw+1)) + signed(sxt(re_num4,reimw+1));
        re_sum(reimw-1 downto 0) := temp(reimw downto 1);

        temp := signed(sxt(im_num1,reimw+1)) + signed(sxt(im_num2,reimw+1));
        im_sum(reimw-1 downto 0) := temp(reimw downto 1);
        temp := signed(sxt(im_sum,reimw+1)) + signed(sxt(im_num3,reimw+1));
        im_sum(reimw-1 downto 0) := temp(reimw downto 1);
        temp := signed(sxt(im_sum,reimw+1)) + signed(sxt(im_num4,reimw+1));
        im_sum(reimw-1 downto 0) := temp(reimw downto 1);

        --Assign imal and imaginary parts to the final sum complex number
        sum_reg(complexw-1 downto reimw) <= re_sum;
        sum_reg(reimw-1 downto 0) <= im_sum;
      when "01" =>
        --Calculate real and imaginary parts of the sum
        temp := signed(sxt(re_num1,reimw+1)) + signed(sxt(im_num2,reimw+1));
        re_sum(reimw-1 downto 0) := temp(reimw downto 1);
        temp := signed(sxt(re_sum,reimw+1)) - signed(sxt(re_num3,reimw+1));
        re_sum(reimw-1 downto 0) := temp(reimw downto 1);
        temp := signed(sxt(re_sum,reimw+1)) + signed(sxt(im_num4,reimw+1));
        re_sum(reimw-1 downto 0) := temp(reimw downto 1);

        temp := signed(sxt(im_num1,reimw+1)) - signed(sxt(re_num2,reimw+1));
        im_sum(reimw-1 downto 0) := temp(reimw downto 1);
        temp := signed(sxt(im_sum,reimw+1)) - signed(sxt(im_num3,reimw+1));
        im_sum(reimw-1 downto 0) := temp(reimw downto 1);
        temp := signed(sxt(im_sum,reimw+1)) - signed(sxt(re_num4,reimw+1));
        im_sum(reimw-1 downto 0) := temp(reimw downto 1);

        --Assign imal and imaginary parts to the final sum complex number
        sum_reg(complexw-1 downto reimw) <= re_sum;
        sum_reg(reimw-1 downto 0) <= im_sum;
      when "10" =>
        --Calculate real and imaginary parts of the sum
        temp := signed(sxt(re_num1,reimw+1)) - signed(sxt(re_num2,reimw+1));
        re_sum(reimw-1 downto 0) := temp(reimw downto 1);
        temp := signed(sxt(re_sum,reimw+1)) + signed(sxt(re_num3,reimw+1));
        re_sum(reimw-1 downto 0) := temp(reimw downto 1);
        temp := signed(sxt(re_sum,reimw+1)) - signed(sxt(re_num4,reimw+1));
        re_sum(reimw-1 downto 0) := temp(reimw downto 1);

        temp := signed(sxt(im_num1,reimw+1)) - signed(sxt(im_num2,reimw+1));
        im_sum(reimw-1 downto 0) := temp(reimw downto 1);
        temp := signed(sxt(im_sum,reimw+1)) + signed(sxt(im_num3,reimw+1));
        im_sum(reimw-1 downto 0) := temp(reimw downto 1);
        temp := signed(sxt(im_sum,reimw+1)) - signed(sxt(im_num4,reimw+1));
        im_sum(reimw-1 downto 0) := temp(reimw downto 1);

        --Assign imal and imaginary parts to the final sum complex number
        sum_reg(complexw-1 downto reimw) <= re_sum;
        sum_reg(reimw-1 downto 0) <= im_sum;
      when "11" =>
        --Calculate real and imaginary parts of the sum
        temp := signed(sxt(re_num1,reimw+1)) - signed(sxt(im_num2,reimw+1));
        re_sum(reimw-1 downto 0) := temp(reimw downto 1);
        temp := signed(sxt(re_sum,reimw+1)) - signed(sxt(re_num3,reimw+1));
        re_sum(reimw-1 downto 0) := temp(reimw downto 1);
        temp := signed(sxt(re_sum,reimw+1)) - signed(sxt(im_num4,reimw+1));
        re_sum(reimw-1 downto 0) := temp(reimw downto 1);

        temp := signed(sxt(im_num1,reimw+1)) + signed(sxt(re_num2,reimw+1));
        im_sum(reimw-1 downto 0) := temp(reimw downto 1);
        temp := signed(sxt(im_sum,reimw+1)) - signed(sxt(im_num3,reimw+1));
        im_sum(reimw-1 downto 0) := temp(reimw downto 1);
        temp := signed(sxt(im_sum,reimw+1)) + signed(sxt(re_num4,reimw+1));
        im_sum(reimw-1 downto 0) := temp(reimw downto 1);

        --Assign imal and imaginary parts to the final sum complex number
        sum_reg(complexw-1 downto reimw) <= re_sum;
        sum_reg(reimw-1 downto 0) <= im_sum;
      when others => null;
    end case;
   end process; 
   sum <= sum_reg;
end rtl2;

architecture rtl3 of cadd is
  signal re_num1       : std_logic_vector(reimw-1 downto 0);
  signal im_num1       : std_logic_vector(reimw-1 downto 0);
  signal re_num2       : std_logic_vector(reimw-1 downto 0);
  signal im_num2       : std_logic_vector(reimw-1 downto 0);
  signal re_num3       : std_logic_vector(reimw-1 downto 0); 
  signal im_num3       : std_logic_vector(reimw-1 downto 0);
  signal re_num4       : std_logic_vector(reimw-1 downto 0); 
  signal im_num4       : std_logic_vector(reimw-1 downto 0);
  signal re_first_out  : std_logic_vector(reimw-1 downto 0);
  signal re_second_out : std_logic_vector(reimw-1 downto 0);
  signal re_third_out  : std_logic_vector(reimw-1 downto 0);
  signal im_first_out  : std_logic_vector(reimw-1 downto 0);
  signal im_second_out : std_logic_vector(reimw-1 downto 0);
  signal im_third_out  : std_logic_vector(reimw-1 downto 0);  
  signal re_op1        : std_logic;
  signal re_op2        : std_logic;
  signal re_op3        : std_logic;
  signal im_op1        : std_logic;
  signal im_op2        : std_logic;
  signal im_op3        : std_logic;

  component sxt_add_sub_sra1
    generic (
      reimw : integer);
    port (
      A         : in  std_logic_vector(reimw-1 downto 0);
      B         : in  std_logic_vector(reimw-1 downto 0);
      operation : in  std_logic;
      C         : out std_logic_vector(reimw-1 downto 0));
  end component;

  begin
    re_first : sxt_add_sub_sra1
      generic map (
        reimw => reimw)
      port map (
        A => re_num1,
        B => re_num2,
        operation => re_op1,
        C => re_first_out);
    re_second : sxt_add_sub_sra1
      generic map (
        reimw => reimw)
      port map (
        A => re_num3,
        B => re_num4,
        operation => re_op2,
        C => re_second_out);
    re_third : sxt_add_sub_sra1
      generic map (
        reimw => reimw)
      port map (
        A => re_first_out,
        B => re_second_out,
        operation => re_op3,
        C => re_third_out);
    im_first : sxt_add_sub_sra1
      generic map (
        reimw => reimw)
      port map (
        A => im_num1,
        B => im_num2,
        operation => im_op1,
        C => im_first_out);
    im_second : sxt_add_sub_sra1
      generic map (
        reimw => reimw)
      port map (
        A => im_num3,
        B => im_num4,
        operation => im_op2,
        C => im_second_out);
    im_third : sxt_add_sub_sra1
      generic map (
        reimw => reimw)
      port map (
        A => im_first_out,
        B => im_second_out,
        operation => im_op3,
        C => im_third_out);    
    
    process (num1,num2,num3,num4, format)
      --variable re_sum : std_logic_vector(reimw-1 downto 0);
      --variable im_sum : std_logic_vector(reimw-1 downto 0);
      --variable temp    : std_logic_vector(reimw downto 0);    
    begin  -- process

      case format is
        when "00" =>
          re_num1 <= num1(complexw-1 downto reimw);
          im_num1 <= num1(reimw-1 downto 0);
          re_num2 <= num2(complexw-1 downto reimw);
          im_num2 <= num2(reimw-1 downto 0);
          re_num3 <= num3(complexw-1 downto reimw);
          im_num3 <= num3(reimw-1 downto 0);
          re_num4 <= num4(complexw-1 downto reimw);
          im_num4 <= num4(reimw-1 downto 0);        
          re_op1 <= '0';
          re_op2 <= '0';
          re_op3 <= '0';
          im_op1 <= '0';
          im_op2 <= '0';
          im_op3 <= '0';
        
        when "01" =>
          re_num1 <= num1(complexw-1 downto reimw);
          im_num1 <= num1(reimw-1 downto 0);
          im_num2 <= num2(complexw-1 downto reimw);
          re_num2 <= num2(reimw-1 downto 0);
          re_num3 <= num3(complexw-1 downto reimw);
          im_num3 <= num3(reimw-1 downto 0);
          im_num4 <= num4(complexw-1 downto reimw);
          re_num4 <= num4(reimw-1 downto 0);        
          re_op1 <= '0';
          re_op2 <= '0';
          re_op3 <= '1';
          im_op1 <= '1';
          im_op2 <= '1';
          im_op3 <= '1';
        when "10" =>
          re_num1 <= num1(complexw-1 downto reimw);
          im_num1 <= num1(reimw-1 downto 0);
          re_num2 <= num2(complexw-1 downto reimw);
          im_num2 <= num2(reimw-1 downto 0);
          re_num3 <= num3(complexw-1 downto reimw);
          im_num3 <= num3(reimw-1 downto 0);
          re_num4 <= num4(complexw-1 downto reimw);
          im_num4 <= num4(reimw-1 downto 0);        
          re_op1 <= '1';
          re_op2 <= '1';
          re_op3 <= '0';          
          im_op1 <= '1';
          im_op2 <= '1';
          im_op3 <= '0';          
        when "11" =>
          re_num1 <= num1(complexw-1 downto reimw);
          im_num1 <= num1(reimw-1 downto 0);
          im_num2 <= num2(complexw-1 downto reimw);
          re_num2 <= num2(reimw-1 downto 0);
          re_num3 <= num3(complexw-1 downto reimw);
          im_num3 <= num3(reimw-1 downto 0);
          im_num4 <= num4(complexw-1 downto reimw);
          re_num4 <= num4(reimw-1 downto 0);        
          re_op1 <= '1';
          re_op2 <= '1';
          re_op3 <= '1';
          im_op1 <= '0';
          im_op2 <= '0';
          im_op3 <= '1';
        when others => null;
      end case;
    end process;
    sum <= re_third_out&im_third_out;
 
end rtl3;

architecture rtl of sxt_add_sub_sra1 is
  signal extended_A : std_logic_vector(reimw downto 0);
  signal extended_B : std_logic_vector(reimw downto 0);
  signal result : signed(reimw downto 0);

begin  -- rtl

  extension: process (A,B)
  begin  -- process extension
    extended_A <= sxt(A,reimw+1);
    extended_B <= sxt(B,reimw+1);    
  end process extension;

  process (extended_A,extended_B,operation)
  begin  -- process
    if operation = '0' then
      result <= signed(extended_A) + signed(extended_B);
    else
      result <= signed(extended_A) - signed(extended_B);     
    end if;
  end process;
  
  C(reimw-1 downto 0) <= conv_std_logic_vector(result(reimw downto 1),C'length);
                         
end rtl;

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;
--use IEEE.numeric_std.all;
--use IEEE.numeric_signed.all;

entity fu_cadd_always_1 is
  generic (
    dataw : integer := 32;              
    halfdataw  : integer := 16;
    busw : integer := 32);        

  port(
    t1data   : in  std_logic_vector(1 downto 0);
    t1load   : in  std_logic;
    o1data   : in  std_logic_vector(dataw-1 downto 0);
    o1load   : in  std_logic;
    o2data   : in  std_logic_vector(dataw-1 downto 0);
    o2load   : in  std_logic;
    o3data   : in  std_logic_vector(dataw-1 downto 0);
    o3load   : in  std_logic;
    o4data   : in  std_logic_vector(dataw-1 downto 0);
    o4load   : in  std_logic;    
    r1data   : out std_logic_vector(busw-1 downto 0);
    glock      : in std_logic;
    rstx     : in  std_logic;
    clk      : in  std_logic);
end fu_cadd_always_1;

architecture rtl of fu_cadd_always_1 is

  component cadd
  generic (
    complexw : integer := 32;           -- bit width of complex number
    reimw    : integer := 16);           -- bit width of real and imaginary parts of a complex number

  port (
    num1  : in std_logic_vector(complexw-1 downto 0);
    num2  : in std_logic_vector(complexw-1 downto 0);
    num3  : in std_logic_vector(complexw-1 downto 0);
    num4  : in std_logic_vector(complexw-1 downto 0);
    format : in std_logic_vector(1 downto 0);
    sum    : out std_logic_vector(complexw-1 downto 0));
  end component;
    
  signal t1reg     : std_logic_vector(1 downto 0);
  signal o1reg     : std_logic_vector(dataw-1 downto 0);
  signal o1temp    : std_logic_vector(dataw-1 downto 0);
  signal o2reg     : std_logic_vector(dataw-1 downto 0);
  signal o2temp    : std_logic_vector(dataw-1 downto 0);
  signal o3reg     : std_logic_vector(dataw-1 downto 0);
  signal o3temp    : std_logic_vector(dataw-1 downto 0);
  signal o4reg     : std_logic_vector(dataw-1 downto 0);
  signal o4temp    : std_logic_vector(dataw-1 downto 0);  
  signal r1        : std_logic_vector(dataw-1 downto 0);
  signal control   : std_logic_vector(4 downto 0);

begin

  cadd_arch : cadd
    generic map (
      complexw => dataw,
      reimw    => halfdataw)
    port map (
      num1   => o1reg,
      num2   => o2reg,
      num3   => o3reg,
      num4   => o4reg,
      format => t1reg,
      sum    => r1);
      
  
  control <= o1load&o2load&o3load&o4load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg     <= (others => '0');
      o1reg     <= (others => '0');
      o1temp    <= (others => '0');
      o2reg     <= (others => '0');
      o2temp    <= (others => '0');
      o3reg     <= (others => '0');
      o3temp    <= (others => '0');
      o4reg     <= (others => '0');
      o4temp    <= (others => '0');      
      
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock='0') then

      case control is 

        when "00001" =>
          t1reg     <= t1data;
          o1reg     <= o1temp;
          o2reg     <= o2temp;
          o3reg     <= o3temp;
          o4reg     <= o4temp;
        when "10000" =>
          o1temp    <= o1data;
        when "01000" =>
          o2temp    <= o2data;
        when "00100" =>
          o3temp    <= o3data;
        when "00010" =>
          o4temp    <= o4data;
        when "11000" =>
          o1temp    <= o1data;
          o2temp    <= o2data;
        when "10100" =>
          o1temp    <= o1data;
          o3temp    <= o3data;
        when "10010" =>
          o1temp    <= o1data;
          o4temp    <= o4data;
        when "01100" =>
          o2temp    <= o2data;
          o3temp    <= o3data;
        when "01010" =>
          o2temp    <= o2data;
          o4temp    <= o4data;
        when "00110" =>
          o3temp    <= o3data;
          o4temp    <= o4data;
        when "11100" =>
          o1temp    <= o1data;
          o2temp    <= o2data;
          o3temp    <= o3data;
        when "11010" =>
          o1temp    <= o1data;
          o2temp    <= o2data;
          o4temp    <= o4data;
        when "01110" =>
          o2temp    <= o2data;
          o3temp    <= o3data;
          o4temp    <= o4data;
        when "11110" =>
          o1temp    <= o1data;
          o2temp    <= o2data;
          o3temp    <= o3data;
          o4temp    <= o4data;          
        when "10001" =>
          t1reg     <= t1data;
          o1reg     <= o1data;
          o1temp    <= o1data;
          o2reg     <= o2temp;
          o3reg     <= o3temp;
          o4reg     <= o4temp;
        when "01001" =>
          t1reg     <= t1data;
          o1reg     <= o1temp;
          o2reg     <= o2data;
          o2temp    <= o2data;
          o3reg     <= o3temp;
          o4reg     <= o4temp;
        when "00101" =>
          t1reg     <= t1data;
          o1reg     <= o1temp;
          o2reg     <= o2temp;
          o3reg     <= o3data;
          o3temp    <= o3data;
          o4reg     <= o4temp;
         when "00011" =>
          t1reg     <= t1data;
          o1reg     <= o1temp;
          o2reg     <= o2temp;
          o3reg     <= o3temp;         
          o4reg     <= o4data;
          o4temp    <= o4data;
        when "11001" =>
          t1reg     <= t1data;
          o1reg     <= o1data;
          o1temp    <= o1data;
          o2reg     <= o2data;
          o2temp    <= o2data;
          o3reg     <= o3temp;
          o4reg     <= o4temp;
        when "10101" =>
          t1reg     <= t1data;
          o1reg     <= o1data;
          o1temp    <= o1data;
          o2reg     <= o2temp;
          o3reg     <= o3data;
          o3temp    <= o3data;
          o4reg     <= o4temp;
        when "10011" =>
          t1reg     <= t1data;
          o1reg     <= o1data;
          o1temp    <= o1data;
          o2reg     <= o2temp;
          o3reg     <= o3temp;           
          o4reg     <= o4data;
          o4temp    <= o4data;
         when "01101" =>
          t1reg     <= t1data;
          o1reg     <= o1temp;
          o2reg     <= o2data;
          o2temp    <= o2data;
          o3reg     <= o3data;
          o3temp    <= o3data;
          o4reg     <= o4temp;
         when "01011" =>
          t1reg     <= t1data;
          o1reg     <= o1temp;
          o2reg     <= o2data;
          o2temp    <= o2data;
          o3reg     <= o3temp;          
          o4reg     <= o4data;
          o4temp    <= o4data;
         when "00111" =>
          t1reg     <= t1data;
          o1reg     <= o1temp;
          o2reg     <= o2temp;          
          o3reg     <= o3data;
          o3temp    <= o3data;
          o4reg     <= o4data;
          o4temp    <= o4data;
        when "11101" =>
          t1reg     <= t1data;
          o1reg     <= o1data;
          o1temp    <= o1data;
          o2reg     <= o2data;
          o2temp    <= o2data;
          o3reg     <= o3data;
          o3temp    <= o3data;
          o4reg     <= o4temp;
        when "11011" =>
          t1reg     <= t1data;
          o1reg     <= o1data;
          o1temp    <= o1data;
          o2reg     <= o2data;
          o2temp    <= o2data;
          o3reg     <= o3temp;          
          o4reg     <= o4data;
          o4temp    <= o4data;
        when "01111" =>
          t1reg     <= t1data;
          o1reg     <= o1temp;          
          o2reg     <= o2data;
          o2temp    <= o2data;
          o3reg     <= o3data;
          o3temp    <= o3data;
          o4reg     <= o4data;
          o4temp    <= o4data;
        when "11111" =>
          t1reg     <= t1data;
          o1reg     <= o1data;
          o1temp    <= o1data;
          o2reg     <= o2data;
          o2temp    <= o2data;
          o3reg     <= o3data;
          o3temp    <= o3data;
          o4reg     <= o4data;
          o4temp    <= o4data;                  
        when others => null;
      end case;

       end if;
    end if;
  end process regs;

  r1data <= r1;
  
end rtl;


-------------------------------------------------------------------------------
-- Entity declaration for unit cadd latency 2
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

entity fu_cadd_always_2 is
  generic (
    dataw : integer := 32;              
    halfdataw  : integer := 16;
    busw : integer := 32);        

  port(
    t1data   : in  std_logic_vector(1 downto 0);
    t1load   : in  std_logic;
    o1data   : in  std_logic_vector(dataw-1 downto 0);
    o1load   : in  std_logic;
    o2data   : in  std_logic_vector(dataw-1 downto 0);
    o2load   : in  std_logic;
    o3data   : in  std_logic_vector(dataw-1 downto 0);
    o3load   : in  std_logic;
    o4data   : in  std_logic_vector(dataw-1 downto 0);
    o4load   : in  std_logic;    
    r1data   : out std_logic_vector(busw-1 downto 0);
    glock    : in std_logic;
    rstx     : in  std_logic;
    clk      : in  std_logic);
end fu_cadd_always_2;

architecture rtl of fu_cadd_always_2 is

  component cadd
  generic (
    complexw : integer := 32;           -- bit width of complex number
    reimw    : integer := 16);           -- bit width of real and imaginary parts of a complex number

  port (
    num1  : in std_logic_vector(complexw-1 downto 0);
    num2  : in std_logic_vector(complexw-1 downto 0);
    num3  : in std_logic_vector(complexw-1 downto 0);
    num4  : in std_logic_vector(complexw-1 downto 0);
    format : in std_logic_vector(1 downto 0);
    sum    : out std_logic_vector(complexw-1 downto 0));
  end component;
    
  signal t1reg     : std_logic_vector(dataw-1 downto 0);
  signal o1reg     : std_logic_vector(dataw-1 downto 0);
  signal o2reg     : std_logic_vector(dataw-1 downto 0);
  signal o3reg     : std_logic_vector(dataw-1 downto 0);
  signal o4reg     : std_logic_vector(dataw-1 downto 0);
  signal r1        : std_logic_vector(dataw-1 downto 0);
  signal r1reg     : std_logic_vector(dataw-1 downto 0);
  signal control   : std_logic_vector(4 downto 0);

  signal result_en_reg : std_logic;
  
begin

  cadd_arch : cadd
    generic map (
      complexw => dataw,
      reimw    => halfdataw)
    port map (
      num1   => o1reg,
      num2   => o2reg,
      num3   => o3reg,
      num4   => o4reg,
      format => t1reg,
      sum    => r1);
      
  control <= o1load&o2load&o3load&o4load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg     <= (others => '0');
      o1reg     <= (others => '0');
      o2reg     <= (others => '0');
      o3reg     <= (others => '0');
      o4reg     <= (others => '0');      
      r1reg     <= (others => '0');
      result_en_reg <= '0';
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock='0') then
        case control is
        when "00001" =>
          t1reg     <= t1data;
        when "10000" =>
          o1reg    <= o1data;
        when "01000" =>
          o2reg    <= o2data;
        when "00100" =>
          o3reg    <= o3data;
        when "00010" =>
          o4reg    <= o4data;
        when "11000" =>
          o1reg    <= o1data;
          o2reg    <= o2data;
        when "10100" =>
          o1reg    <= o1data;
          o3reg    <= o3data;
        when "10010" =>
          o1reg    <= o1data;
          o4reg    <= o4data;
        when "01100" =>
          o2reg    <= o2data;
          o3reg    <= o3data;
        when "01010" =>
          o2reg    <= o2data;
          o4reg    <= o4data;
        when "00110" =>
          o3reg    <= o3data;
          o4reg    <= o4data;
        when "11100" =>
          o1reg    <= o1data;
          o2reg    <= o2data;
          o3reg    <= o3data;
        when "11010" =>
          o1reg    <= o1data;
          o2reg    <= o2data;
          o4reg    <= o4data;
        when "01110" =>
          o2reg    <= o2data;
          o3reg    <= o3data;
          o4reg    <= o4data;
        when "11110" =>
          o1reg    <= o1data;
          o2reg    <= o2data;
          o3reg    <= o3data;
          o4reg    <= o4data;          
        when "10001" =>
          t1reg     <= t1data;
          o1reg     <= o1data;
        when "01001" =>
          t1reg     <= t1data;
          o2reg     <= o2data;
        when "00101" =>
          t1reg     <= t1data;
          o3reg     <= o3data;
         when "00011" =>
          t1reg     <= t1data;
          o4reg     <= o4data;
        when "11001" =>
          t1reg     <= t1data;
          o1reg     <= o1data;
          o2reg     <= o2data;
        when "10101" =>
          t1reg     <= t1data;
          o1reg     <= o1data;
          o3reg     <= o3data;
        when "10011" =>
          t1reg     <= t1data;
          o1reg     <= o1data;
          o4reg     <= o4data;
         when "01101" =>
          t1reg     <= t1data;
          o2reg     <= o2data;
          o3reg     <= o3data;
         when "01011" =>
          t1reg     <= t1data;
          o2reg     <= o2data;
          o4reg     <= o4data;
         when "00111" =>
          t1reg     <= t1data;
          o3reg     <= o3data;
          o4reg     <= o4data;
        when "11101" =>
          t1reg     <= t1data;
          o1reg     <= o1data;
          o2reg     <= o2data;
          o3reg     <= o3data;
        when "11011" =>
          t1reg     <= t1data;
          o1reg     <= o1data;
          o2reg     <= o2data;
          o4reg     <= o4data;
        when "01111" =>
          t1reg     <= t1data;
          o2reg     <= o2data;
          o3reg     <= o3data;
          o4reg     <= o4data;
        when "11111" =>
          t1reg     <= t1data;
          o1reg     <= o1data;
          o2reg     <= o2data;
          o3reg     <= o3data;
          o4reg     <= o4data;
        when others => null;
      end case;
        
      -- update result only when a new operation was triggered
      result_en_reg <= t1load;

      if result_en_reg = '1' then
        r1reg <= r1;
      end if;

      end if;
    end if;
  end process regs;

  r1data <= sxt(r1reg, busw);
  --r1data <= r1;
  
end rtl;
