-- Copyright 2002-2008 Tampere University of Technology.  All Rights Reserved.
--
-- This file is part of TTA-Based Codesign Environment (TCE).
--
-- TCE is free software; you can redistribute it and/or modify it under the
-- terms of the GNU General Public License version 2 as published by the Free
-- Software Foundation.
--
-- TCE is distributed in the hope that it will be useful, but WITHOUT ANY
-- WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
-- FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
-- details.
--
-- You should have received a copy of the GNU General Public License along
-- with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
-- St, Fifth Floor, Boston, MA  02110-1301  USA
--
-- As a special exception, you may use this file as part of a free software
-- library without restriction.  Specifically, if other files instantiate
-- templates or use macros or inline functions from this file, or you compile
-- this file and link it with other files to produce an executable, this file
-- does not by itself cause the resulting executable to be covered by the GNU
-- General Public License.  This exception does not however invalidate any
-- other reasons why the executable file might be covered by the GNU General
-- Public License.
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;

entity max_maxu_min_minu_arith is
  
  generic (
    dataw : integer := 32);

  port (
    a   : in  std_logic_vector(dataw-1 downto 0);
    b   : in  std_logic_vector(dataw-1 downto 0);
    opc : in  std_logic_vector(1 downto 0);
    res : out std_logic_vector(dataw-1 downto 0));
end max_maxu_min_minu_arith;

architecture rtl of max_maxu_min_minu_arith is
begin  -- rtl

  process (a,b,opc)
  begin  -- process
    case opc is
      when "00" =>
        if (signed(a) >= signed(b)) then
          res <= a;
        else
          res <= b;
        end if;
      when "01" =>
        if (unsigned(a) >= unsigned(b)) then
          res <= a;
        else
          res <= b;
        end if;
      when "10" =>
        if (signed(a) <= signed(b)) then
          res <= a;
        else
          res <= b;
        end if;
      when others =>
        if (unsigned(a) <= unsigned(b)) then
          res <= a;
        else
          res <= b;
        end if;
    end case;
  end process;
end rtl;
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;

entity fu_max_maxu_min_minu_always_1 is
  generic (
    dataw : integer := 32;              -- Operand Width
    busw  : integer := 32);             -- Bus Width

  port(
    t1data : in  std_logic_vector(dataw-1 downto 0);
    t1opcode : in std_logic_vector(1 downto 0);
    t1load : in  std_logic;
    o1data : in  std_logic_vector(dataw-1 downto 0);
    o1load : in  std_logic;
    r1data : out std_logic_vector(busw-1 downto 0);
    glock  : in  std_logic;
    rstx   : in  std_logic;
    clk    : in  std_logic);
end fu_max_maxu_min_minu_always_1;

architecture rtl of fu_max_maxu_min_minu_always_1 is

  component max_maxu_min_minu_arith 
    generic (
      dataw : integer := 32);
    
    port (
      a   : in  std_logic_vector(dataw-1 downto 0);
      b   : in  std_logic_vector(dataw-1 downto 0);
      opc : in  std_logic_vector(1 downto 0);
      res : out std_logic_vector(dataw-1 downto 0));
  end component;
    
  signal t1reg   : std_logic_vector(dataw-1 downto 0);
  signal o1reg   : std_logic_vector(dataw-1 downto 0);
  signal o1temp  : std_logic_vector(dataw-1 downto 0);
  signal r1      : std_logic_vector(dataw-1 downto 0);
  signal t1opcodereg : std_logic_vector(1 downto 0);
  signal control : std_logic_vector(1 downto 0);
  
begin

  fu_arch : max_maxu_min_minu_arith
    generic map (
      dataw => dataw)
    port map (
      a   => t1reg,
      b   => o1reg,
      opc => t1opcodereg,
      res => r1);
  
  control <= o1load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg  <= (others => '0');
      o1reg  <= (others => '0');
      o1temp <= (others => '0');
      t1opcodereg <= (others => '0');
      
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then

        case control is
          when "11" =>
            t1reg  <= t1data;
            o1reg  <= o1data;
            o1temp <= o1data;
            t1opcodereg <= t1opcode;
          when "10" =>
            o1temp <= o1data;
          when "01" =>
            t1reg <= t1data;
            o1reg <= o1temp;
            t1opcodereg <= t1opcode;
          when others => null;
        end case;

      end if;
    end if;
  end process regs;

  process (r1)
  begin  -- process
    if busw < dataw then
      r1data(dataw-1) <= r1(dataw-1);
      r1data(busw-2 downto 0) <= r1(busw-2 downto 0);
    else
      r1data <= sxt(r1,busw);
    end if;
  end process;

end rtl;

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

entity fu_max_maxu_min_minu_always_2 is
  generic (
    dataw : integer := 32;              -- Operand Width
    busw  : integer := 32);             -- Bus Width

  port(
    t1data : in  std_logic_vector(dataw-1 downto 0);
    t1opcode : in std_logic_vector(1 downto 0);
    t1load : in  std_logic;
    o1data : in  std_logic_vector(dataw-1 downto 0);
    o1load : in  std_logic;
    r1data : out std_logic_vector(busw-1 downto 0);
    glock  : in  std_logic;
    rstx   : in  std_logic;
    clk    : in  std_logic);
end fu_max_maxu_min_minu_always_2;

architecture rtl of fu_max_maxu_min_minu_always_2 is
  component max_maxu_min_minu_arith 
    generic (
      dataw : integer := 32);
    
    port (
      a   : in  std_logic_vector(dataw-1 downto 0);
      b   : in  std_logic_vector(dataw-1 downto 0);
      opc : in  std_logic_vector(1 downto 0);
      res : out std_logic_vector(dataw-1 downto 0));
  end component;
  
  signal t1reg   : std_logic_vector(dataw-1 downto 0);
  signal o1reg   : std_logic_vector(dataw-1 downto 0);
  signal r1      : std_logic_vector(dataw-1 downto 0);
  signal r1reg   : std_logic_vector(busw-1 downto 0);
  signal t1opcodereg : std_logic_vector(1 downto 0);
  signal control : std_logic_vector(1 downto 0);

  signal result_en_reg : std_logic;
  
begin

  fu_arch : max_maxu_min_minu_arith
    generic map (
      dataw => dataw)
    port map (
      a   => t1reg,
      b   => o1reg,
      opc => t1opcodereg,
      res => r1);
  
  control <= o1load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg <= (others => '0');
      o1reg <= (others => '0');
      r1reg <= (others => '0');
      t1opcodereg <= (others => '0');
      result_en_reg <= '0';
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then

        case control is
          when "11" =>
            t1reg <= t1data;
            o1reg <= o1data;
            t1opcodereg <= t1opcode;
          when "10" =>
            o1reg <= o1data;
          when "01" =>
            t1reg <= t1data;
            t1opcodereg <= t1opcode;
          when others => null;
        end case;

        -- update result only when new operation was triggered
        result_en_reg <= t1load;

        if result_en_reg = '1' then
          -- if busw is bigger keep sign and take bit's from
          -- lsb path.
          if busw < dataw then
            r1reg(dataw-1) <= r1(dataw-1);
            r1reg(busw-2 downto 0) <= r1(busw-2 downto 0);
          else
            r1reg <= sxt(r1,busw);
          end if;
          --r1reg <= r1;          
        end if;

      end if;
    end if;
  end process regs;
  --r1data <= sxt(r1reg, busw);
  r1data <= r1reg;
  
end rtl;
