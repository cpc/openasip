-------------------------------------------------------------------------------
-- Title      : Load/Store unit for TTA
-- Project    : FlexDSP
-------------------------------------------------------------------------------
-- File       : ld_st.vhdl
-- Author     : Jaakko Sertamo  <sertamo@jaguar.cs.tut.fi>
-- Company    : 
-- Created    : 2002-06-24
-- Last update: 2008/05/28
-- Platform   : 
-------------------------------------------------------------------------------
-- Description: Load Store functional unit
--
--              opcode 0 load   address:t1data
--                     1 store  address:o1data  data:t1data
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2002-06-24  1.0      sertamo Created
-------------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;

entity fu_ldw_stw_always_3 is
  generic (
    dataw : integer := 32;
    addrw : integer := 11);
  port(
    -- socket interfaces:
    t1data    : in  std_logic_vector(addrw+1 downto 0);
    t1load    : in  std_logic;
    t1opcode  : in  std_logic_vector(0 downto 0);
    -- CHANGE
    o1data    : in  std_logic_vector(dataw-1 downto 0);
    o1load    : in  std_logic;
    r1data    : out std_logic_vector(dataw-1 downto 0);
    -- external memory unit interface:
    data_in   : in  std_logic_vector(dataw-1 downto 0);
    data_out  : out std_logic_vector(dataw-1 downto 0);
    --mem_address : out std_logic_vector(addrw-1 downto 0);
    addr      : out std_logic_vector(addrw-1 downto 0);
    -- memory control signals
    mem_en_x  : out std_logic_vector(0 downto 0);          -- active low
    wr_en_x   : out std_logic_vector(0 downto 0);          -- active low
    wr_mask_x : out std_logic_vector(dataw-1 downto 0);

    -- control signals:
    glock : in std_logic;
    clk   : in std_logic;
    rstx  : in std_logic);
end fu_ldw_stw_always_3;

architecture rtl of fu_ldw_stw_always_3 is

  type action_reg_type is array (natural range <>) of std_logic_vector(1 downto 0);

  signal addr_reg     : std_logic_vector(addrw-1 downto 0);
  signal data_out_reg : std_logic_vector(dataw-1 downto 0);
  signal wr_en_x_reg  : std_logic_vector(0 downto 0);
  signal mem_en_x_reg : std_logic_vector(0 downto 0);

  signal action_reg : action_reg_type(0 to 1);

  signal o1shadow_reg : std_logic_vector(dataw-1 downto 0);
  signal r1_reg       : std_logic_vector(dataw-1 downto 0);
  signal sel          : std_logic_vector(2 downto 0);

  constant LOAD       : std_logic_vector(1 downto 0) := "10";
  constant TRIG_OP_ST : std_logic_vector(2 downto 0) := "111";
  constant TRIG_OP_LD : std_logic_vector(2 downto 0) := "110";
  constant TRIG_LD    : std_logic_vector(2 downto 0) := "100";
  constant TRIG_ST    : std_logic_vector(2 downto 0) := "101";
  constant OP1        : std_logic_vector(2 downto 0) := "010";
  constant OP2        : std_logic_vector(2 downto 0) := "011";
    
begin

  sel <= t1load & o1load & t1opcode;

  seq : process (clk, rstx)

  begin  -- process seq
    if rstx = '0' then                  -- asynchronous reset (active low)
      addr_reg     <= (others => '0');
      data_out_reg <= (others => '0');
      -- use preset instead of reset
      wr_en_x_reg  <= "1";
      mem_en_x_reg <= "1";

      for idx in (action_reg'length-1) downto 0 loop
        action_reg(idx) <= (others => '0');
      end loop;  -- idx

      o1shadow_reg <= (others => '0');
      r1_reg       <= (others => '0');
      
    elsif clk'event and clk = '1' then  -- rising clock edge
      if glock = '0' then
        
        case sel is
          
          when TRIG_OP_ST =>
            addr_reg     <= t1data(addrw+1 downto 2);
            data_out_reg <= o1data;
            o1shadow_reg <= o1data;
            mem_en_x_reg <= "0";
            wr_en_x_reg  <= "0";

          when TRIG_OP_LD =>
            addr_reg     <= t1data(addrw+1 downto 2);
            o1shadow_reg <= o1data;
            mem_en_x_reg <= "0";
            wr_en_x_reg  <= "1";
            
            
          when TRIG_LD =>
            addr_reg     <= t1data(addrw+1 downto 2);
            mem_en_x_reg <= "0";
            wr_en_x_reg  <= "1";
            
          when TRIG_ST =>
            addr_reg     <= t1data(addrw+1 downto 2);
            data_out_reg <= o1shadow_reg;
            mem_en_x_reg <= "0";
            wr_en_x_reg  <= "0";

          when OP1 =>
            o1shadow_reg <= o1data;
            wr_en_x_reg  <= "1";
            mem_en_x_reg <= "1";
            
          when OP2 =>
            o1shadow_reg <= o1data;
            wr_en_x_reg  <= "1";
            mem_en_x_reg <= "1";
            
          when others =>
            wr_en_x_reg  <= "1";
            mem_en_x_reg <= "1";
        end case;

        action_reg(1) <= action_reg(0);
        action_reg(0) <= t1load & t1opcode;

        if action_reg(1) = LOAD then
          r1_reg <= data_in;
        end if;

      end if;
    end if;
  end process seq;

  wr_mask_x <= (others => '0');
  mem_en_x(0)  <= mem_en_x_reg(0) or glock;
  wr_en_x   <= wr_en_x_reg;
  data_out  <= data_out_reg;
  addr      <= addr_reg;
  r1data    <= r1_reg;
  
end rtl;
