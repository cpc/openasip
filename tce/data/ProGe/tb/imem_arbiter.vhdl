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
-- Revisions  :
-- Date        Version  Author  Description
-- 2003-08-28  1.0      sertamo Created
-------------------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;

entity imem_arbiter is
  
  generic (
    PORTW     : integer := 32;
    ADDRWIDTH : integer := 7);

  port (

    d_2 : in  std_logic_vector(PORTW-1 downto 0);
    d   : out std_logic_vector(PORTW-1 downto 0);

    addr_1 : in  std_logic_vector(ADDRWIDTH-1 downto 0);
    addr_2 : in  std_logic_vector(ADDRWIDTH-1 downto 0);
    addr   : out std_logic_vector(ADDRWIDTH-1 downto 0);

    en_1_x : in  std_logic;
    en_2_x : in  std_logic;
    en_x   : out std_logic;

    wr_2_x : in  std_logic;
    wr_x   : out std_logic;

    bit_wr_2_x : in  std_logic_vector(PORTW-1 downto 0);
    bit_wr_x   : out std_logic_vector(PORTW-1 downto 0);

    mem_busy : out std_logic
    );
end imem_arbiter;

architecture comb of imem_arbiter is

begin  -- comb

  -- purpose: select which of the two candidates have access
  -- to the control interface of the memory
  --
  -- in case of simultaneous access, _1 has the privilige
  -- type   : combinational
  -- inputs :   d_a_1, d_a_2, addr_a_1. addr_a_2, en_a_1_x,
  --            en_a_2_x, wr_a_1_x. wr_a_2_x
  -- outputs:   d_a, addr_a, en_a_x, wr_a_x
  select_access : process (d_2, addr_1, addr_2, en_1_x, en_2_x, wr_2_x, bit_wr_2_x)
    variable sel : std_logic_vector(1 downto 0);
  begin  -- process select_access
    sel := en_1_x&en_2_x;
    case sel is
      when "00" =>
        -- simultaneous access: input 1 has access, busy set high
        addr     <= addr_1;
        en_x     <= en_1_x;
        mem_busy <= '1';
        -- input 2 has access
      when "10" =>
        addr     <= addr_2;
        en_x     <= en_2_x;
        mem_busy <= '0';
        -- default
      when others =>
        addr     <= addr_1;
        en_x     <= en_1_x;
        mem_busy <= '0';
        
    end case;

    -- instruction memory written only from outside the movecore,
    -- thus, these external ports corresponding to writing the
    -- memory are directly forwarded to memory
    d        <= d_2;
    bit_wr_x <= bit_wr_2_x;
    wr_x     <= wr_2_x;
  end process select_access;
  

end comb;
