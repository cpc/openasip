-------------------------------------------------------------------------------
-- Title      : Clock generator
-- Project    : FlexDSP
-------------------------------------------------------------------------------
-- File       : clkgen.vhdl
-- Author     : Jaakko Sertamo  <sertamo@vlad.cs.tut.fi>
-- Company    : 
-- Created    : 2001-07-16
-- Last update: 2003-11-26
-- Platform   : 
-------------------------------------------------------------------------------
-- Description: A 50/50 testbench clock. The clock period is defined by
--           a generic PERIOD          
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2001-07-16  1.0      sertamo	Created
-------------------------------------------------------------------------------


library ieee;
use ieee.std_logic_1164.all;

entity clkgen is
  generic ( PERIOD : time := 100 ns );  -- Clock period
  port    ( clk : out std_logic;         -- Clock signal
            en : in std_logic := '1'   -- Enable signal
            );
end clkgen;

architecture simulation of clkgen is
  signal tmp : std_logic := '1';
begin 
  bistable: process(tmp)
  begin
    if en = '1' then
      tmp <= not tmp after PERIOD/2;
      clk <= tmp ;      
    else
      clk <='0';
    end if;
  end process bistable;

end simulation;

configuration clkgen_cfg of clkgen is
  for simulation
  end for;
end clkgen_cfg;
