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

-- 64-BIT Multiplication and MAC Operation FU for TTA64 Project.
-- Designer: Latif AKCAY
-- University: Bayburt University, Istanbul Technical University, TURKEY.

library IEEE;
use IEEE.Std_Logic_1164.all;

package opcodes_mac64_mul64 is

  constant MUL64_OPC  : std_logic_vector(0 downto 0)  := "1";  
  constant MAC64_OPC  : std_logic_vector(0 downto 0)  := "0";

end opcodes_mac64_mul64;

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all; 
use ieee.std_logic_misc.all;
use work.util.all;
use work.opcodes_mac64_mul64.all;

entity fu_mac64_mul64_always_2 is

  generic (
    	busw : integer := 64;
	dataw : integer := 64
	);

  port (
    clk : in std_logic;
    rstx : in std_logic;
    glock : in std_logic;
    operation_in : in std_logic_vector(0 downto 0);
    data_in1t_in : in std_logic_vector(63 downto 0);
    load_in1t_in : in std_logic;
    data_in2_in :  in std_logic_vector(63 downto 0);
    load_in2_in :  in std_logic;
    data_in3_in :  in std_logic_vector(63 downto 0);
    load_in3_in :  in std_logic;
    data_out1_out : out std_logic_vector(63 downto 0)
    );
end entity fu_mac64_mul64_always_2;

architecture rtl of fu_mac64_mul64_always_2 is

  signal data_in1t : std_logic_vector(63 downto 0);
  signal data_in2 : std_logic_vector(63 downto 0);
  signal data_in3 : std_logic_vector(63 downto 0);

  signal shadow_in2_r : std_logic_vector(63 downto 0);
  signal shadow_in3_r : std_logic_vector(63 downto 0);
  signal operation_1_r : std_logic_vector(0 downto 0);
  signal optrig_1_r : std_logic;
  signal data_out1_r : std_logic_vector(127 downto 0);
  signal data_out1_reg : std_logic_vector(127 downto 0);
  signal data_out1_1_valid_r : std_logic;

begin

  data_in1t <= data_in1t_in;

  shadow_in2_in3_sp : process(clk, rstx)
  begin
    if rstx = '0' then
      shadow_in2_r <= (others => '0');
	  shadow_in3_r <= (others => '0');
    elsif clk = '1' and clk'event then
      if ((glock = '0') and (load_in2_in = '1')) then
        shadow_in2_r <= data_in2_in;
      end if;
	  if ((glock = '0') and (load_in3_in = '1')) then
		shadow_in3_r <= data_in2_in;
	  end if;
    end if;
  end process shadow_in2_in3_sp;

  shadow_in2_in3_cp : process(shadow_in2_r, shadow_in3_r, data_in2_in, data_in3_in, load_in1t_in, load_in2_in, load_in3_in)
  begin
    if ((load_in1t_in = '1') and (load_in2_in = '1')) then
      data_in2 <= data_in2_in;
    else
      data_in2 <= shadow_in2_r;
    end if;
    if ((load_in1t_in = '1') and (load_in3_in = '1')) then
      data_in3 <= data_in3_in;
    else
      data_in3 <= shadow_in3_r;
    end if;
  end process shadow_in2_in3_cp;

  input_pipeline_sp : process(clk, rstx)
  begin
    if rstx = '0' then
      operation_1_r <= (others => '0');
      optrig_1_r <= '0';
    elsif clk = '1' and clk'event then
      if (glock = '0') then
        optrig_1_r <= load_in1t_in;
        if (load_in1t_in = '1') then
          operation_1_r <= operation_in;
        end if;
      end if;
    end if;
  end process input_pipeline_sp;

  output_pipeline_sp : process(clk, rstx)
  begin
    if rstx = '0' then
      data_out1_1_valid_r <= '0';
      data_out1_r <= (others => '0');
      data_out1_reg <= (others => '0');
    elsif clk = '1' and clk'event then
      if (glock = '0') then
        data_out1_1_valid_r <= load_in1t_in;
        if load_in1t_in = '1' then
          case operation_in is
            when MUL64_OPC =>
              data_out1_r <= conv_std_logic_vector(signed(data_in1t) * signed(data_in2), data_out1_r'length );
            when MAC64_OPC =>
              data_out1_r <= conv_std_logic_vector((unsigned(data_in1t) * unsigned(data_in2) + unsigned(data_in3)), data_out1_r'length );
            when others =>
          end case;
        end if;
        if (data_out1_1_valid_r = '1') then
           data_out1_reg <=  data_out1_r;   
        end if;
      end if;
    end if;
  end process output_pipeline_sp;

  data_out1_out <= "00000000000000000000000000000000" & data_out1_reg(31 downto 0); -- result has to be compatible with the currrent version of the operation!

end architecture rtl;










