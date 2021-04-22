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

-- 64-BIT Little-Endian Load-Store Unit for TTA64 Project.
-- Designer: Latif AKCAY
-- University: Bayburt University, Istanbul Technical University, TURKEY.

-- LSU64 Operations
-- ld16  : 0
-- ld32  : 1
-- ld64  : 2
-- ld8   : 3
-- ldu16 : 4
-- ldu32 : 5
-- ldu8  : 6
-- st16  : 7
-- st32  : 8
-- st64  : 9
-- st8   : 10


package ld8_ld16_ld32_ldu8_ldu16_ldu32_ld64_st8_st16_st32_st64_opcodes is

  constant OPC_LD16   : integer := 0;
  constant OPC_LD32  : integer := 1;
  constant OPC_LD64  : integer := 2;
  constant OPC_LD8  : integer := 3;
  constant OPC_LDU16  : integer := 4;
  constant OPC_LDU32 : integer := 5;
  constant OPC_LDU8 : integer := 6;
  constant OPC_ST16   : integer := 7;
  constant OPC_ST32  : integer := 8;
  constant OPC_ST64  : integer := 9;
  constant OPC_ST8  : integer := 10;

end ld8_ld16_ld32_ldu8_ldu16_ldu32_ld64_st8_st16_st32_st64_opcodes;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use work.ld8_ld16_ld32_ldu8_ldu16_ldu32_ld64_st8_st16_st32_st64_opcodes.all;

entity lsu_ld8_ld16_ld32_ldu8_ldu16_ldu32_ld64_st8_st16_st32_st64_le3 is
  generic (
    dataw : integer := 64;
    addrw : integer := 16);
  port(
    -- socket interfaces:
    t1data        : in  std_logic_vector(addrw-1 downto 0);
    t1load        : in  std_logic;
    t1opcode      : in  std_logic_vector(3 downto 0);
    -- CHANGE
    o1data        : in  std_logic_vector(dataw-1 downto 0);
    o1load        : in  std_logic;
    r1data        : out std_logic_vector(dataw-1 downto 0);
    -- external memory unit interface:
    data_in   : in  std_logic_vector(dataw-1 downto 0);
    data_out  : out std_logic_vector(dataw-1 downto 0);
    addr      : out std_logic_vector(addrw-3-1 downto 0);
    -- control signals
    mem_en_x  : out std_logic_vector(0 downto 0);          -- active low
    wr_en_x   : out std_logic_vector(0 downto 0);          -- active low
    wr_mask_x : out std_logic_vector(dataw-1 downto 0);

    -- control signals:
    glock : in std_logic;
    clk   : in std_logic;
    rstx  : in std_logic
	);
end lsu_ld8_ld16_ld32_ldu8_ldu16_ldu32_ld64_st8_st16_st32_st64_le3;

architecture rtl of lsu_ld8_ld16_ld32_ldu8_ldu16_ldu32_ld64_st8_st16_st32_st64_le3 is

  type reg_array is array (natural range <>) of std_logic_vector(5 downto 0);

  signal addr_reg      : std_logic_vector(addrw-3-1 downto 0);
  signal data_out_reg  : std_logic_vector(dataw-1 downto 0);
  signal wr_en_x_reg   : std_logic_vector(0 downto 0);
  signal mem_en_x_reg  : std_logic_vector(0 downto 0);
  signal wr_mask_x_reg : std_logic_vector(dataw-1 downto 0);

  signal status_addr_reg : reg_array(1 downto 0);

  signal t1data_lower_3 : std_logic_vector(2 downto 0);
  signal t1data_lower_2 : std_logic_vector(1 downto 0);

  -- information on the word (lsw/msw) needed in register
  signal o1shadow_reg : std_logic_vector(dataw-1 downto 0);
  signal r1_reg       : std_logic_vector(dataw-1 downto 0);

  constant NOT_LOAD  : std_logic_vector(2 downto 0) := "000";
  constant LD32      : std_logic_vector(2 downto 0) := "001";
  constant LD16      : std_logic_vector(2 downto 0) := "010";
  constant LD8       : std_logic_vector(2 downto 0) := "011";
  constant LDU16     : std_logic_vector(2 downto 0) := "100";
  constant LDU32     : std_logic_vector(2 downto 0) := "101";
  constant LD64      : std_logic_vector(2 downto 0) := "110";
  constant LDU8      : std_logic_vector(2 downto 0) := "111";
 
  constant MSHW_MASK_LITTLE_ENDIAN : std_logic_vector := "1111111111111111111111111111111100000000000000000000000000000000";
  constant LSHW_MASK_LITTLE_ENDIAN : std_logic_vector := "0000000000000000000000000000000011111111111111111111111111111111";

  constant ONES  : std_logic_vector := "11111111";
  constant ZEROS : std_logic_vector := "00000000";

  constant SIZE_OF_BYTE : integer := 8;
  constant SIZE_OF_HW : integer := 16;
  
  begin

  t1data_lower_3 <= t1data(2 downto 0);
  t1data_lower_2 <= t1data(2 downto 1);

  seq : process (clk, rstx)
    variable opc : integer;
    variable idx : integer;
  begin  -- process seq

    if rstx = '0' then                  -- asynchronous reset (active low)
      addr_reg        <= (others => '0');
      data_out_reg    <= (others => '0');
      -- use preset instead of reset
      wr_en_x_reg(0)  <= '1';
      mem_en_x_reg(0) <= '1';
      wr_mask_x_reg    <= (others => '1');
      idx             := 1; -- status_addr_reg'length-1;            
      for idx in 1 downto 0 loop  
        status_addr_reg(idx) <= (others => '0');
      end loop;  -- idx

      o1shadow_reg <= (others => '0');
      r1_reg       <= (others => '0');

    elsif clk'event and clk = '1' then  -- rising clock edge
      if glock = '0' then
        
        if t1load = '1' then
          opc := conv_integer(unsigned(t1opcode));
          case opc is
            when OPC_LD32 =>
              status_addr_reg(0) <= LD32 & t1data_lower_3;
              addr_reg           <= t1data(addrw-1 downto 3);
              mem_en_x_reg(0)    <= '0';
              wr_en_x_reg(0)     <= '1';
             -- wr_mask_x_reg       <= (others => '1');
            when OPC_LD16 =>
              status_addr_reg(0) <= LD16 & t1data_lower_3;
              addr_reg           <= t1data(addrw-1 downto 3);
              mem_en_x_reg(0)    <= '0';
              wr_en_x_reg(0)     <= '1';
            --  wr_mask_x_reg       <= (others => '1');
            when OPC_LD8 =>
              status_addr_reg(0) <= LD8 & t1data_lower_3;
              addr_reg           <= t1data(addrw-1 downto 3);
              mem_en_x_reg(0)    <= '0';
              wr_en_x_reg(0)     <= '1';
            --  wr_mask_x_reg       <= (others => '1');
            when OPC_LDU16 =>
              status_addr_reg(0) <= LDU16 & t1data_lower_3;
              addr_reg           <= t1data(addrw-1 downto 3);
              mem_en_x_reg(0)    <= '0';
              wr_en_x_reg(0)     <= '1';
            --  wr_mask_x_reg       <= (others => '1');
            when OPC_LDU32 =>
              status_addr_reg(0) <= LDU32 & t1data_lower_3;
              addr_reg           <= t1data(addrw-1 downto 3);
              mem_en_x_reg(0)    <= '0';
              wr_en_x_reg(0)     <= '1';
            --  wr_mask_x_reg       <= (others => '1');
	        when OPC_LD64 =>
              status_addr_reg(0) <= LD64 & t1data_lower_3;
              addr_reg           <= t1data(addrw-1 downto 3);
              mem_en_x_reg(0)    <= '0';
              wr_en_x_reg(0)     <= '1';
            --  wr_mask_x_reg       <= (others => '1');
            when OPC_LDU8 =>
              status_addr_reg(0) <= LDU8 & t1data_lower_3;
              addr_reg           <= t1data(addrw-1 downto 3);
              mem_en_x_reg(0)    <= '0';
              wr_en_x_reg(0)     <= '1';
           --   wr_mask_x_reg       <= (others => '1');

            when OPC_ST64 =>
              status_addr_reg(0)(5 downto 3) <= NOT_LOAD;
              if o1load = '1' then
                data_out_reg <= o1data;
              else
                data_out_reg <= o1shadow_reg;
              end if;
              mem_en_x_reg(0) <= '0';
              wr_en_x_reg(0)  <= '0';
              wr_mask_x_reg    <= (others => '0');
              addr_reg        <= t1data(addrw-1 downto 3);
            when OPC_ST32 =>
              status_addr_reg(0)(5 downto 3) <= NOT_LOAD;
              -- endianes dependent code
              -- DEFAULT ENDIANESS
              -- little endian             
	          addr_reg <= t1data(addrw-1 downto 3);
              if o1load = '1' then
                if t1data(2) = '0' then
                  wr_mask_x_reg <= MSHW_MASK_LITTLE_ENDIAN;
                  data_out_reg <= ZEROS&ZEROS&ZEROS&ZEROS&o1data(dataw/2-1 downto 0);
                else
                  wr_mask_x_reg <= LSHW_MASK_LITTLE_ENDIAN;
                  data_out_reg <= o1data(dataw/2-1 downto 0)&ZEROS&ZEROS&ZEROS&ZEROS;
                end if;
              else
                -- endianes dependent code
                if t1data(2) = '0' then
                  wr_mask_x_reg <= MSHW_MASK_LITTLE_ENDIAN;
                  data_out_reg <= ZEROS&ZEROS&ZEROS&ZEROS&o1shadow_reg(dataw/2-1 downto 0);
                else
                  wr_mask_x_reg <= LSHW_MASK_LITTLE_ENDIAN;
                  data_out_reg <= o1shadow_reg(dataw/2-1 downto 0)&ZEROS&ZEROS&ZEROS&ZEROS;
                end if;
              end if;
              mem_en_x_reg(0) <= '0';
              wr_en_x_reg(0)  <= '0';

            when OPC_ST16 =>
              status_addr_reg(0)(5 downto 3) <= NOT_LOAD;
              -- endianes dependent code
              -- DEFAULT ENDIANESS
              -- little endian
              --        Byte #
              --        |3|2|1|0|
              addr_reg <= t1data(addrw-1 downto 3);
              if o1load = '1' then
                case t1data_lower_2 is
                  -- endianes dependent code                            
                  when "11" =>
                    wr_mask_x_reg <= ZEROS&ZEROS&ONES&ONES&ONES&ONES&ONES&ONES;
                    data_out_reg <= o1data(SIZE_OF_HW-1 downto 0)&ZEROS&ZEROS&ZEROS&ZEROS&ZEROS&ZEROS;
                  when "10" =>
                    wr_mask_x_reg <= ONES&ONES&ZEROS&ZEROS&ONES&ONES&ONES&ONES;
                    data_out_reg <= ZEROS&ZEROS&o1data(SIZE_OF_HW-1 downto 0)&ZEROS&ZEROS&ZEROS&ZEROS;
                  when "01" =>
                    wr_mask_x_reg <= ONES&ONES&ONES&ONES&ZEROS&ZEROS&ONES&ONES;
                    data_out_reg <= ZEROS&ZEROS&ZEROS&ZEROS&o1data(SIZE_OF_HW-1 downto 0)&ZEROS&ZEROS;
                  when others =>
                    wr_mask_x_reg <= ONES&ONES&ONES&ONES&ONES&ONES&ZEROS&ZEROS;
                    data_out_reg <= ZEROS&ZEROS&ZEROS&ZEROS&ZEROS&ZEROS&o1data(SIZE_OF_HW-1 downto 0);
                end case;
              else
                case t1data_lower_2 is
                  -- endianes dependent code                            
                  when "11" =>
                    wr_mask_x_reg <= ZEROS&ZEROS&ONES&ONES&ONES&ONES&ONES&ONES;
                    data_out_reg <= o1shadow_reg(SIZE_OF_HW-1 downto 0)&ZEROS&ZEROS&ZEROS&ZEROS&ZEROS&ZEROS;
                  when "10" =>
                    wr_mask_x_reg <= ONES&ONES&ZEROS&ZEROS&ONES&ONES&ONES&ONES;
                    data_out_reg <= ZEROS&ZEROS&o1shadow_reg(SIZE_OF_HW-1 downto 0)&ZEROS&ZEROS&ZEROS&ZEROS;
                  when "01" =>
                    wr_mask_x_reg <= ONES&ONES&ONES&ONES&ZEROS&ZEROS&ONES&ONES;
                    data_out_reg <= ZEROS&ZEROS&ZEROS&ZEROS&o1shadow_reg(SIZE_OF_HW-1 downto 0)&ZEROS&ZEROS;
                  when others =>
                    wr_mask_x_reg <= ONES&ONES&ONES&ONES&ONES&ONES&ZEROS&ZEROS;
                    data_out_reg <= ZEROS&ZEROS&ZEROS&ZEROS&ZEROS&ZEROS&o1shadow_reg(SIZE_OF_HW-1 downto 0);
                end case;
              end if;
              mem_en_x_reg(0) <= '0';
              wr_en_x_reg(0)  <= '0';

            when OPC_ST8 =>
              status_addr_reg(0)(5 downto 3) <= NOT_LOAD;
              -- endianes dependent code
              -- DEFAULT ENDIANESS
              -- little endian
              addr_reg  <= t1data(addrw-1 downto 3);
              if o1load = '1' then
                case t1data_lower_3 is
                  -- endianes dependent code                            
                  when "111" =>
                    wr_mask_x_reg <= ZEROS&ONES&ONES&ONES&ONES&ONES&ONES&ONES;
                    data_out_reg <= o1data(SIZE_OF_BYTE-1 downto 0)&ZEROS&ZEROS&ZEROS&ZEROS&ZEROS&ZEROS&ZEROS;
                  when "110" =>
                    wr_mask_x_reg <= ONES&ZEROS&ONES&ONES&ONES&ONES&ONES&ONES;
                    data_out_reg <= ZEROS&o1data(SIZE_OF_BYTE-1 downto 0)&ZEROS&ZEROS&ZEROS&ZEROS&ZEROS&ZEROS;
                  when "101" =>
                    wr_mask_x_reg <= ONES&ONES&ZEROS&ONES&ONES&ONES&ONES&ONES;
                    data_out_reg <= ZEROS&ZEROS&o1data(SIZE_OF_BYTE-1 downto 0)&ZEROS&ZEROS&ZEROS&ZEROS&ZEROS;
                  when "100" =>
                    wr_mask_x_reg <= ONES&ONES&ONES&ZEROS&ONES&ONES&ONES&ONES;
                    data_out_reg <= ZEROS&ZEROS&ZEROS&o1data(SIZE_OF_BYTE-1 downto 0)&ZEROS&ZEROS&ZEROS&ZEROS;
		          when "011" =>
                    wr_mask_x_reg <= ONES&ONES&ONES&ONES&ZEROS&ONES&ONES&ONES;
                    data_out_reg <= ZEROS&ZEROS&ZEROS&ZEROS&o1data(SIZE_OF_BYTE-1 downto 0)&ZEROS&ZEROS&ZEROS;
                  when "010" =>
                    wr_mask_x_reg <= ONES&ONES&ONES&ONES&ONES&ZEROS&ONES&ONES;
                    data_out_reg <= ZEROS&ZEROS&ZEROS&ZEROS&ZEROS&o1data(SIZE_OF_BYTE-1 downto 0)&ZEROS&ZEROS;
                  when "001" =>
                    wr_mask_x_reg <= ONES&ONES&ONES&ONES&ONES&ONES&ZEROS&ONES;
                    data_out_reg <= ZEROS&ZEROS&ZEROS&ZEROS&ZEROS&ZEROS&o1data(SIZE_OF_BYTE-1 downto 0)&ZEROS;
                  when others =>
                    wr_mask_x_reg <= ONES&ONES&ONES&ONES&ONES&ONES&ONES&ZEROS;
                    data_out_reg <= ZEROS&ZEROS&ZEROS&ZEROS&ZEROS&ZEROS&ZEROS&o1data(SIZE_OF_BYTE-1 downto 0);
                end case;
              else
                case t1data_lower_3 is
                   -- endianes dependent code                            
                  when "111" =>
                    wr_mask_x_reg <= ZEROS&ONES&ONES&ONES&ONES&ONES&ONES&ONES;
                    data_out_reg <= o1shadow_reg(SIZE_OF_BYTE-1 downto 0)&ZEROS&ZEROS&ZEROS&ZEROS&ZEROS&ZEROS&ZEROS;
                  when "110" =>
                    wr_mask_x_reg <= ONES&ZEROS&ONES&ONES&ONES&ONES&ONES&ONES;
                    data_out_reg <= ZEROS&o1shadow_reg(SIZE_OF_BYTE-1 downto 0)&ZEROS&ZEROS&ZEROS&ZEROS&ZEROS&ZEROS;
                  when "101" =>
                    wr_mask_x_reg <= ONES&ONES&ZEROS&ONES&ONES&ONES&ONES&ONES;
                    data_out_reg <= ZEROS&ZEROS&o1shadow_reg(SIZE_OF_BYTE-1 downto 0)&ZEROS&ZEROS&ZEROS&ZEROS&ZEROS;
                  when "100" =>
                    wr_mask_x_reg <= ONES&ONES&ONES&ZEROS&ONES&ONES&ONES&ONES;
                    data_out_reg <= ZEROS&ZEROS&ZEROS&o1shadow_reg(SIZE_OF_BYTE-1 downto 0)&ZEROS&ZEROS&ZEROS&ZEROS;
		          when "011" =>
                    wr_mask_x_reg <= ONES&ONES&ONES&ONES&ZEROS&ONES&ONES&ONES;
                    data_out_reg <= ZEROS&ZEROS&ZEROS&ZEROS&o1shadow_reg(SIZE_OF_BYTE-1 downto 0)&ZEROS&ZEROS&ZEROS;
                  when "010" =>
                    wr_mask_x_reg <= ONES&ONES&ONES&ONES&ONES&ZEROS&ONES&ONES;
                    data_out_reg <= ZEROS&ZEROS&ZEROS&ZEROS&ZEROS&o1shadow_reg(SIZE_OF_BYTE-1 downto 0)&ZEROS&ZEROS;
                  when "001" =>
                    wr_mask_x_reg <= ONES&ONES&ONES&ONES&ONES&ONES&ZEROS&ONES;
                    data_out_reg <= ZEROS&ZEROS&ZEROS&ZEROS&ZEROS&ZEROS&o1shadow_reg(SIZE_OF_BYTE-1 downto 0)&ZEROS;
                  when others =>
                    wr_mask_x_reg <= ONES&ONES&ONES&ONES&ONES&ONES&ONES&ZEROS;
                    data_out_reg <= ZEROS&ZEROS&ZEROS&ZEROS&ZEROS&ZEROS&ZEROS&o1shadow_reg(SIZE_OF_BYTE-1 downto 0);
                end case;
              end if;
              mem_en_x_reg(0) <= '0';
              wr_en_x_reg(0)  <= '0';
            when others =>
              null;
          end case;
        else
          status_addr_reg(0)(5 downto 3) <= NOT_LOAD;
          wr_en_x_reg(0)                 <= '1';
          mem_en_x_reg(0)                <= '1';
        end if;

        if o1load = '1' then
          o1shadow_reg <= o1data;
        end if;
	
        status_addr_reg(1) <= status_addr_reg(0);

        if status_addr_reg(1)(5 downto 3) = LD64 then
          r1_reg <= data_in;
        elsif status_addr_reg(1)(5 downto 3) = LD32 then
          -- endianes dependent code
          -- select either upper or lower part of the word
          if status_addr_reg(1)(1) = '1' then
            r1_reg <= SXT(data_in(dataw-1 downto dataw/2), r1_reg'length);
          else
            r1_reg <= SXT(data_in(dataw/2-1 downto 0), r1_reg'length);
          end if;

        elsif status_addr_reg(1)(5 downto 3) = LD16 then
          case status_addr_reg(1)(2 downto 1) is
            -- endianes dependent code
            when "11" =>
              r1_reg <= SXT(data_in(dataw-1 downto dataw-SIZE_OF_HW), r1_reg'length);
            when "10" =>
              r1_reg <= SXT(data_in(dataw-SIZE_OF_HW-1 downto dataw-2*SIZE_OF_HW), r1_reg'length);
            when "01" =>
              r1_reg <= SXT(data_in(dataw-2*SIZE_OF_HW-1 downto dataw-3*SIZE_OF_HW), r1_reg'length);
            when others =>
              r1_reg <= SXT(data_in(dataw-3*SIZE_OF_HW-1 downto dataw-4*SIZE_OF_HW), r1_reg'length);
          end case;
        
        elsif status_addr_reg(1)(5 downto 3) = LD8 then
          case status_addr_reg(1)(2 downto 0) is
            -- endianes dependent code
            when "111" =>
              r1_reg <= SXT(data_in(dataw-1 downto dataw-SIZE_OF_BYTE), r1_reg'length);
            when "110" =>
              r1_reg <= SXT(data_in(dataw-SIZE_OF_BYTE-1 downto dataw-2*SIZE_OF_BYTE), r1_reg'length);
            when "101" =>
              r1_reg <= SXT(data_in(dataw-2*SIZE_OF_BYTE-1 downto dataw-3*SIZE_OF_BYTE), r1_reg'length);
            when "100" =>
              r1_reg <= SXT(data_in(dataw-3*SIZE_OF_BYTE-1 downto dataw-4*SIZE_OF_BYTE), r1_reg'length);
            when "011" =>
              r1_reg <= SXT(data_in(dataw-4*SIZE_OF_BYTE-1 downto dataw-5*SIZE_OF_BYTE), r1_reg'length);
            when "010" =>
              r1_reg <= SXT(data_in(dataw-5*SIZE_OF_BYTE-1 downto dataw-6*SIZE_OF_BYTE), r1_reg'length);
            when "001" =>
              r1_reg <= SXT(data_in(dataw-6*SIZE_OF_BYTE-1 downto dataw-7*SIZE_OF_BYTE), r1_reg'length);
            when others =>
              r1_reg <= SXT(data_in(dataw-7*SIZE_OF_BYTE-1 downto dataw-8*SIZE_OF_BYTE), r1_reg'length);
          end case;

        elsif status_addr_reg(1)(5 downto 3) = LDU32 then
          -- endianes dependent code
          -- select either upper or lower part of the word
          if status_addr_reg(1)(1) = '1' then
            r1_reg <= EXT(data_in(dataw-1 downto dataw/2), r1_reg'length);
          else
            r1_reg <= EXT(data_in(dataw/2-1 downto 0), r1_reg'length);
          end if;

        elsif status_addr_reg(1)(5 downto 3) = LDU16 then
          case status_addr_reg(1)(2 downto 1) is
            -- endianes dependent code
            when "11" =>
              r1_reg <= EXT(data_in(dataw-1 downto dataw-SIZE_OF_HW), r1_reg'length);
            when "10" =>
              r1_reg <= EXT(data_in(dataw-SIZE_OF_HW-1 downto dataw-2*SIZE_OF_HW), r1_reg'length);
            when "01" =>
              r1_reg <= EXT(data_in(dataw-2*SIZE_OF_HW-1 downto dataw-3*SIZE_OF_HW), r1_reg'length);
            when others =>
              r1_reg <= EXT(data_in(dataw-3*SIZE_OF_HW-1 downto dataw-4*SIZE_OF_HW), r1_reg'length);
          end case;
       
        elsif status_addr_reg(1)(5 downto 3) = LDU8 then
          case status_addr_reg(1)(2 downto 0) is
            -- endianes dependent code
            when "111" =>
              r1_reg <= EXT(data_in(dataw-1 downto dataw-SIZE_OF_BYTE), r1_reg'length);
            when "110" =>
              r1_reg <= EXT(data_in(dataw-SIZE_OF_BYTE-1 downto dataw-2*SIZE_OF_BYTE), r1_reg'length);
            when "101" =>
              r1_reg <= EXT(data_in(dataw-2*SIZE_OF_BYTE-1 downto dataw-3*SIZE_OF_BYTE), r1_reg'length);
            when "100" =>
              r1_reg <= EXT(data_in(dataw-3*SIZE_OF_BYTE-1 downto dataw-4*SIZE_OF_BYTE), r1_reg'length);
            when "011" =>
              r1_reg <= EXT(data_in(dataw-4*SIZE_OF_BYTE-1 downto dataw-5*SIZE_OF_BYTE), r1_reg'length);
            when "010" =>
              r1_reg <= EXT(data_in(dataw-5*SIZE_OF_BYTE-1 downto dataw-6*SIZE_OF_BYTE), r1_reg'length);
            when "001" =>
              r1_reg <= EXT(data_in(dataw-6*SIZE_OF_BYTE-1 downto dataw-7*SIZE_OF_BYTE), r1_reg'length);
            when others =>
              r1_reg <= EXT(data_in(dataw-7*SIZE_OF_BYTE-1 downto dataw-8*SIZE_OF_BYTE), r1_reg'length);
          end case;

        end if;

      end if;
    end if;

  end process seq;

  mem_en_x(0) <= mem_en_x_reg(0) or glock;
  wr_en_x     <= wr_en_x_reg;
  wr_mask_x    <= wr_mask_x_reg;
  data_out    <= data_out_reg;
  addr        <= addr_reg;
  r1data      <= r1_reg;

end rtl;



