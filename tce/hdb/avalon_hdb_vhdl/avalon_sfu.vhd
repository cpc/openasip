-- Copyright (c) 2002-2010 Tampere University of Technology.
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
-- This TTA to Avalon bus component assumes that all Avalon components either
-- have static latency of one cycles or they use wait_request_n for any other
-- latency
-------------------------------------------------------------------------------
-- t1data = address
-- o1data = data to avalon
-- r1data = data from avalon
-------------------------------------------------------------------------------
-- THIS IS NOT PIPELINED

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use work.avalon_tta_util_32bit.all;
use work.opcodes_avalon_sfu.all;


entity avalon_sfu is
  
  generic (
    busw_g         : integer := 32;
    a_addrw_g      : integer := 11;
    a_byteenable_g : integer := 4;
    a_dataw_g      : integer := 32;
    a_irqw_g       : integer := 32
    );
  port (
    -- data signals
    t1data               : in  std_logic_vector(busw_g-1 downto 0);
    t1load               : in  std_logic;
    t1opcode             : in  std_logic_vector(4-1 downto 0);
    o1data               : in  std_logic_vector(busw_g-1 downto 0);
    o1load               : in  std_logic;
    r1data               : out std_logic_vector(busw_g-1 downto 0);
    -- control signals
    clk                  : in  std_logic;
    rstx                 : in  std_logic;
    glock                : in  std_logic;
    lock_req             : out std_logic;
    -- external signals
    avalon_d_address     : out std_logic_vector(a_addrw_g-1 downto 0);
    avalon_d_byteenable  : out std_logic_vector(a_byteenable_g-1 downto 0);
    avalon_d_read        : out std_logic_vector(1-1 downto 0);
    avalon_d_write       : out std_logic_vector(1-1 downto 0);
    avalon_d_writedata   : out std_logic_vector(a_dataw_g-1 downto 0);
    avalon_d_irq         : in  std_logic_vector(a_irqw_g-1 downto 0);
    avalon_d_readdata    : in  std_logic_vector(a_dataw_g-1 downto 0);
    avalon_d_waitrequest : in  std_logic_vector(1-1 downto 0)
    );

end avalon_sfu;


architecture rtl of avalon_sfu is

  type state_vector is (
    idle,
    init_read,
    wait_read,
    init_readu,
    wait_readu,
    write_data,
    wait_write
    );

  signal t1reg         : std_logic_vector(busw_g-1 downto 0);
  signal o1reg         : std_logic_vector(busw_g-1 downto 0);
  signal o1reg_shadow  : std_logic_vector(busw_g-1 downto 0);
  signal r1reg         : std_logic_vector(busw_g-1 downto 0);
  signal current_state : state_vector;

  signal d_address_r       : std_logic_vector(a_addrw_g-1 downto 0);
  signal d_read_r          : std_logic;
  signal d_write_r         : std_logic;
  signal d_writedata_r     : std_logic_vector(a_dataw_g-1 downto 0);
  signal d_byteenable_r    : std_logic_vector(a_byteenable_g-1 downto 0);
  signal pre_byteenable_r  : std_logic_vector(a_byteenable_g-1 downto 0);
  signal lock_req_r        : std_logic;
  signal wait_request_wire : std_logic;
  signal irq_r             : std_logic_vector(a_irqw_g-1 downto 0);


begin  -- rtl

  main : process (clk, rstx)
  begin  -- process main
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg            <= (others => '0');
      o1reg            <= (others => '0');
      o1reg_shadow     <= (others => '0');
      r1reg            <= (others => '0');
      current_state    <= idle;
      d_address_r      <= (others => '0');
      d_read_r         <= '0';
      d_write_r        <= '0';
      d_writedata_r    <= (others => '0');
      d_byteenable_r   <= (others => '0');
      pre_byteenable_r <= (others => '0');
      lock_req_r       <= '0';
      irq_r            <= (others => '0');
    elsif clk'event and clk = '1' then  -- rising clock edge
      -- stall if someone else has issued glock
      if glock = '0' or lock_req_r = '1' then
        -- read interrupts
        irq_r <= avalon_d_irq;

        if t1load = '1' then
          
          case t1opcode is
            when OPC_STW =>
              t1reg <= t1data;
              case o1load is
                when '1' =>
                  o1reg <= o1data;
                when others =>
                  o1reg <= o1reg_shadow;
              end case;
              current_state    <= write_data;
              pre_byteenable_r <= "1111";

            when OPC_STH =>
              t1reg            <= t1data;
              pre_byteenable_r <= bytemask_16bit_big_endian(t1data(1 downto 0));
              case o1load is
                when '1' =>
                  o1reg <= o1data;
                when others =>
                  o1reg <= o1reg_shadow;
              end case;
              current_state <= write_data;

            when OPC_STQ =>
              t1reg            <= t1data;
              pre_byteenable_r <= bytemask_8bit_big_endian(t1data(1 downto 0));
              case o1load is
                when '1' =>
                  o1reg <= o1data;
                when others =>
                  o1reg <= o1reg_shadow;
              end case;
              current_state <= write_data;

            when OPC_LDW =>
              current_state    <= init_read;
              t1reg            <= t1data;
              pre_byteenable_r <= "1111";

            when OPC_LDH =>
              current_state    <= init_read;
              t1reg            <= t1data;
              pre_byteenable_r <= bytemask_16bit_big_endian(t1data(1 downto 0));
              
            when OPC_LDHU =>
              current_state    <= init_readU;
              t1reg            <= t1data;
              pre_byteenable_r <= bytemask_16bit_big_endian(t1data(1 downto 0));

            when OPC_LDQ =>
              current_state    <= init_read;
              t1reg            <= t1data;
              pre_byteenable_r <= bytemask_8bit_big_endian(t1data(1 downto 0));

            when OPC_LDQU =>
              current_state    <= init_readu;
              t1reg            <= t1data;
              pre_byteenable_r <= bytemask_8bit_big_endian(t1data(1 downto 0));

            when OPC_IRQ =>
              r1reg <= irq_r;
              
            when others =>
              null;
              
          end case;  -- t1opcode
        end if;  -- t1load


        if o1load = '1' then
          o1reg_shadow <= o1data;
        end if;

        case current_state is
          when idle =>
            null;
            
          when write_data =>
            d_address_r    <= t1reg(a_addrw_g-1 downto 0);
            d_write_r      <= '1';
            d_writedata_r  <= align_data_to_mem(o1reg, pre_byteenable_r);
            d_byteenable_r <= pre_byteenable_r;
            current_state  <= wait_write;
            
          when wait_write =>
            if wait_request_wire = '1' then
              lock_req_r <= '1';
            else
              lock_req_r    <= '0';
              d_write_r     <= '0';
              current_state <= idle;
            end if;
            
          when init_read =>
            d_address_r    <= t1reg(a_addrw_g-1 downto 0);
            d_read_r       <= '1';
            d_byteenable_r <= pre_byteenable_r;
            current_state  <= wait_read;

          when wait_read =>
            if wait_request_wire = '1' then
              lock_req_r <= '1';
            else
              lock_req_r    <= '0';
              d_read_r      <= '0';
              r1reg         <= align_data_from_mem_s(avalon_d_readdata, d_byteenable_r);
              current_state <= idle;
            end if;
            
          when init_readu =>
            d_address_r    <= t1reg(a_addrw_g-1 downto 0);
            d_read_r       <= '1';
            d_byteenable_r <= pre_byteenable_r;
            current_state  <= wait_readu;

          when wait_readu =>
            if wait_request_wire = '1' then
              lock_req_r <= '1';
            else
              lock_req_r    <= '0';
              d_read_r      <= '0';
              r1reg         <= align_data_from_mem_u(avalon_d_readdata, d_byteenable_r);
              current_state <= idle;
            end if;
            
          when others => null;
        end case;
        
      end if;  -- if glock etc
    end if;  -- if reset or clk edge
  end process main;


  r1data              <= r1reg;
  avalon_d_address    <= d_address_r;
  avalon_d_read(0)    <= d_read_r;
  avalon_d_write(0)   <= d_write_r;
  avalon_d_byteenable <= d_byteenable_r;
  avalon_d_writedata  <= d_writedata_r;
  wait_request_wire   <= avalon_d_waitrequest(0);
  lock_req            <= lock_req_r;

end rtl;
