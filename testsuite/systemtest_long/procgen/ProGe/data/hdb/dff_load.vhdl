--
-- VHDL Entity WORK.dff_load.arch_name
--
-- Created:
--          by - tpitkane.tpitkane (elros)
--          at - 12:54:39 11/17/05
--
-- using Mentor Graphics HDL Designer(TM) 2004.1 (Build 41)
--
LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.std_logic_arith.all;

ENTITY dff_load IS
  generic(
    dataw : integer := 32
    );
  port (
    data_in  : in std_logic_vector(dataw-1 downto 0);
    clk      : in std_logic;
    load     : in std_logic;
    data_out : out std_logic_vector(dataw-1 downto 0));   
END ENTITY dff_load;

ARCHITECTURE rtl OF dff_load IS
BEGIN
  process (clk)
    begin   
      if (clk'event and clk = '1') then
        if load = '1' then
          data_out <= data_in;
        end if;
    end if;
  end process;
END ARCHITECTURE rtl;
