-------------------------------------------------------------------------------
-- Title      : Arbiter for memory interface
-- Project    : FlexDSP
-------------------------------------------------------------------------------
-- File       : mem_arbiter.vhdl
-- Author     : Jaakko Sertamo  <sertamo@vlad.cs.tut.fi>
-- Company    : TUT/IDCS
-- Created    : 2003-08-28
-- Last update: 2006/07/10
-- Platform   : 
-- Standard   : VHDL'87
-------------------------------------------------------------------------------
-- Description: Selects which of the two inputs has the access to the memory
--              Static arbitration, input 1 privililage over input 2
-------------------------------------------------------------------------------
-- Copyright (c) 2003 
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2003-08-28  1.0      sertamo Created
-------------------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;

entity mem_arbiter is
  
  generic (
    PORTW     : integer := 32;
    ADDRWIDTH : integer := 15);

  port (

    d_1 : in  std_logic_vector(PORTW-1 downto 0);
    d_2 : in  std_logic_vector(PORTW-1 downto 0);
    d   : out std_logic_vector(PORTW-1 downto 0);

    addr_1 : in  std_logic_vector(ADDRWIDTH-1 downto 0);
    addr_2 : in  std_logic_vector(ADDRWIDTH-1 downto 0);
    addr   : out std_logic_vector(ADDRWIDTH-1 downto 0);

    en_1_x : in  std_logic;
    en_2_x : in  std_logic;
    en_x   : out std_logic;

    wr_1_x : in  std_logic;
    wr_2_x : in  std_logic;
    wr_x   : out std_logic;

    bit_wr_1_x : in  std_logic_vector(PORTW-1 downto 0);
    bit_wr_2_x : in  std_logic_vector(PORTW-1 downto 0);
    bit_wr_x   : out std_logic_vector(PORTW-1 downto 0);

    mem_busy : out std_logic
    );
end mem_arbiter;

architecture comb of mem_arbiter is

begin  -- comb

  -- purpose: select which of the two candidates have access
  -- to the control interface of the memory
  --
  -- in case of simultaneous access, _1 has the privilige
  -- type   : combinational
  -- inputs :   d_a_1, d_a_2, addr_a_1. addr_a_2, en_a_1_x,
  --            en_a_2_x, wr_a_1_x. wr_a_2_x
  -- outputs:   d_a, addr_a, en_a_x, wr_a_x
  select_access : process (d_1, d_2, addr_1, addr_2, en_1_x, en_2_x, wr_1_x, wr_2_x, bit_wr_1_x, bit_wr_2_x)
    variable sel : std_logic_vector(1 downto 0);
  begin  -- process select_access
    sel := en_1_x&en_2_x;
    case sel is
      when "00" =>
        -- simultaneous access: input 1 has access, busy set high
        d        <= d_1;
        addr     <= addr_1;
        en_x     <= en_1_x;
        wr_x     <= wr_1_x;
        bit_wr_x <= bit_wr_1_x;
        mem_busy <= '1';
        -- input 2 has access
      when "10" =>
        d        <= d_2;
        addr     <= addr_2;
        en_x     <= en_2_x;
        wr_x     <= wr_2_x;
        bit_wr_x <= bit_wr_2_x;
        mem_busy <= '0';
        -- default
      when others =>
        d        <= d_1;
        addr     <= addr_1;
        en_x     <= en_1_x;
        wr_x     <= wr_1_x;
        bit_wr_x <= bit_wr_1_x;
        mem_busy <= '0';
        
    end case;

  end process select_access;
  

end comb;
