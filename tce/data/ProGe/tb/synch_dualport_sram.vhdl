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
-- Title      : synchronous dual-port static RAM
-- Project    : 
-------------------------------------------------------------------------------
-- File       : synch_dualport_sram.vhdl
-- Author     : Jaakko Sertamo  <sertamo@vlad.cs.tut.fi>
-- Company    : 
-- Created    : 2003-11-14
-- Last update: 2006/07/10
-- Platform   : 
-- Standard   : VHDL'87
-------------------------------------------------------------------------------
-- Description:
-- synchronous static dual-port random-access memory with bit write capability
-- - all the control signals are active low
-- 
-- architecture rtl is synthesizable
-- architecture simulation is for simulation purposes
-- - simulation model can be initialized with a file which containing the
--   the contents of the memory in textual bit-vectors which are mapped to ram
--   starting from the position 0
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2003-11-14  1.0      sertamo Created
-------------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;

entity synch_dualport_sram is
  generic (
    -- pragma translate_off
    init          : boolean := true;
    INITFILENAME  : string  := "data.img";
    trace         : boolean := true;
    TRACEFILENAME : string  := "dpram_trace";
    -- trace_mode 0: hex, trace_mode 1: integer, trace_mode 2: unsigned
    trace_mode    : natural := 0;
    -- pragma translate_on

    DATAW : integer := 32;
    ADDRW : integer := 15);
  port (
    clk : in std_logic;

    d_a : in std_logic_vector(DATAW-1 downto 0);
    d_b : in std_logic_vector(DATAW-1 downto 0);

    addr_a : in std_logic_vector(ADDRW-1 downto 0);
    addr_b : in std_logic_vector(ADDRW-1 downto 0);
    en_a_x : in std_logic;
    en_b_x : in std_logic;
    wr_a_x : in std_logic;
    wr_b_x : in std_logic;

    bit_wr_a_x : in std_logic_vector(DATAW-1 downto 0);
    bit_wr_b_x : in std_logic_vector(DATAW-1 downto 0);

    q_a : out std_logic_vector(DATAW-1 downto 0);
    q_b : out std_logic_vector(DATAW-1 downto 0)
    );
end synch_dualport_sram;

architecture rtl of synch_dualport_sram is

  type std_logic_matrix is array (natural range <>) of
    std_logic_vector (DATAW-1 downto 0);
  subtype mem_line_index is integer range 0 to 2**ADDRW-1;
  signal  mem_r       : std_logic_matrix (2**ADDRW-1 downto 0);
  signal  word_line_a : mem_line_index;
  signal  word_line_b : mem_line_index;

  signal q_a_r : std_logic_vector(DATAW-1 downto 0);
  signal q_b_r : std_logic_vector(DATAW-1 downto 0);
  
begin  -- rtl
  word_line_a <= conv_integer (unsigned (addr_a));
  word_line_b <= conv_integer (unsigned (addr_b));

  -- purpose: read & write memory
  -- type   : sequential
  -- inputs : clk
  regs : process (clk)
  begin  -- process regs
    if clk'event and clk = '1' then     -- rising clock edge
      -- Memory read
      if (en_a_x = '0' and wr_a_x = '0') then
        -- bypass data to output register
        q_a_r
          <= (d_a and (not bit_wr_a_x))
          or (mem_r(word_line_a) and bit_wr_a_x);
      elsif (en_a_x = '0') then
        q_a_r <= mem_r(word_line_a);
      end if;

      if (en_b_x = '0' and wr_b_x = '0') then
        -- bypass data to output register
        q_b_r
          <= (d_b and (not bit_wr_b_x))
          or (mem_r(word_line_b) and bit_wr_b_x);
      elsif (en_b_x = '0') then
        q_b_r <= mem_r(word_line_b);
      end if;

      -- Memory write
      if (en_a_x = '0') and (wr_a_x = '0') then
        mem_r(word_line_a)
          <= (d_a and (not bit_wr_a_x))
          or (mem_r(word_line_a) and bit_wr_a_x);
      end if;
      if (en_b_x = '0') and (wr_b_x = '0') then
        mem_r(word_line_b)
          <= (d_b and (not bit_wr_b_x))
          or (mem_r(word_line_b) and bit_wr_b_x);
      end if;
      
    end if;
  end process regs;

  q_a <= q_a_r;
  q_b <= q_b_r;

end rtl;

-- pragma translate_off
library IEEE, STD;
use std.textio.all;
use IEEE.std_logic_textio.all;

architecture simulation of synch_dualport_sram is

  type std_logic_matrix is array (natural range <>) of
    std_logic_vector (DATAW-1 downto 0);
  subtype mem_line_index is integer range 0 to 2**ADDRW-1;
  signal  word_line_a : mem_line_index;
  signal  word_line_b : mem_line_index;

  signal mem_r : std_logic_matrix (2**ADDRW-1 downto 0);
  signal q_a_r : std_logic_vector(DATAW-1 downto 0);
  signal q_b_r : std_logic_vector(DATAW-1 downto 0);

  signal initialized : boolean := false;
  
begin  -- simulate
  word_line_a <= conv_integer (unsigned (addr_a));
  word_line_b <= conv_integer (unsigned (addr_b));

  -- purpose: read & write + intialize memory from file
  -- type   : sequential
  -- inputs : ck
  regs_init : process (clk)
    -- input file
    file mem_init           : text;
    -- output file
    file mem_trace          : text;
    variable line_in        : line;
    variable line_out       : line;
    variable word_from_file : std_logic_vector(DATAW-1 downto 0);
    variable word_to_mem    : std_logic_vector(DATAW-1 downto 0);
    variable i              : natural;
    variable good           : boolean := false;
    
  begin  -- process regs

    if (init = true) then
      if (initialized = false) then
        i := 0;
        if INITFILENAME/="" then
	        file_open(mem_init, INITFILENAME, read_mode);
	        while (not endfile(mem_init) and i < mem_r'length) loop
	          readline(mem_init, line_in);
	          read(line_in, word_from_file, good);
	          assert good
	            report "Read error in memory initialization file"
	            severity failure;
	          mem_r(i) <= word_from_file;
	          i:= i+1;
	        end loop;
	        assert (not good)
	          report "Memory initialization succesful"
	          severity note;
	    else
	        while (i < mem_r'length) loop
	          mem_r(i) <= (others=>'0');
	          i:= i+1;
	        end loop;
	        assert (false)
	          report "Memory initialized to zeroes!"
	          severity note;	        	    	
	    end if;
        initialized <= true;
      end if;
    end if;

    if clk'event and clk = '1' then     -- rising clock edge
      -- Memory read
      if (en_a_x = '0' and wr_a_x = '0') then
        -- bypass data to output register
        q_a_r
          <= (d_a and (not bit_wr_a_x))
          or (mem_r(word_line_a) and bit_wr_a_x);
      elsif (en_a_x = '0') then
        q_a_r <= mem_r(word_line_a);
      end if;

      if (en_b_x = '0' and wr_b_x = '0') then
        -- bypass data to output register
        q_b_r
          <= (d_b and (not bit_wr_b_x))
          or (mem_r(word_line_b) and bit_wr_b_x);
      elsif (en_b_x = '0') then
        q_b_r <= mem_r(word_line_b);
      end if;

      -- Memory write
      if (en_a_x = '0') and (wr_a_x = '0') then
        word_to_mem := (d_a and (not bit_wr_a_x))
                       or (mem_r(word_line_a) and bit_wr_a_x);
        mem_r(word_line_a) <= word_to_mem;

        -- trace memory to file
        if (trace = true) then
          file_open(mem_trace, TRACEFILENAME, write_mode);
          for i in mem_r'reverse_range loop
            if (i = word_line_a) then
              if (trace_mode = 0) then
                hwrite(line_out, word_to_mem);
              elsif (trace_mode = 1) then
                write(line_out, conv_integer(signed(word_to_mem)));
              else
                write(line_out, conv_integer(unsigned(word_to_mem)));
              end if;
            else
              if (trace_mode = 0) then
                hwrite(line_out, mem_r(i));
              elsif (trace_mode = 1) then
                write(line_out, conv_integer(signed(mem_r(i))));
              else
                write(line_out, conv_integer(unsigned(mem_r(i))));
              end if;
            end if;
            writeline(mem_trace, line_out);
          end loop;  -- i
          file_close(mem_trace);
        end if;

      end if;
      if (en_b_x = '0') and (wr_b_x = '0') then
        word_to_mem := (d_b and (not bit_wr_b_x))
                       or (mem_r(word_line_b) and bit_wr_b_x);
        mem_r(word_line_b) <= word_to_mem;

        -- trace memory to file
        if (trace = true) then
          file_open(mem_trace, TRACEFILENAME, write_mode);
          for i in mem_r'reverse_range loop
            if (i = word_line_b) then
              if (trace_mode = 0) then
                hwrite(line_out, word_to_mem);
              elsif (trace_mode = 1) then
                write(line_out, conv_integer(signed(word_to_mem)));
              else
                write(line_out, conv_integer(unsigned(word_to_mem)));
              end if;
            else
              if (trace_mode = 0) then
                hwrite(line_out, mem_r(i));
              elsif (trace_mode = 1) then
                write(line_out, conv_integer(signed(mem_r(i))));
              else
                write(line_out, conv_integer(unsigned(mem_r(i))));
              end if;
            end if;
            writeline(mem_trace, line_out);
          end loop;  -- i
          file_close(mem_trace);
        end if;

      end if;
    end if;
  end process regs_init;

  q_a <= q_a_r;
  q_b <= q_b_r;

end simulation;
-- pragma translate_on
