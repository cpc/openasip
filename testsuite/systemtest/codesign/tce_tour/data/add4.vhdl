-------------------------------------------------------------------------------
-- Title      : Four-input adder for the TCE tutorial example.
-- Project    : 
-------------------------------------------------------------------------------
-- File       : add4.vhdl
-- Author     : Pekka J‰‰skel‰inen
-- Company    : 
-- Created    : 2006-06-15
-- Last update: 2006/06/16
-- Platform   : 
-------------------------------------------------------------------------------
-- Description: Performs the additions in a single cycle.
--              Untested.
-------------------------------------------------------------------------------
-- Copyright (c) 2006 
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author   Description
-- 2006-06-16  1.0      pjaaskel first revision
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

entity add4_always_1 is
  generic (
    dataw : integer := 32;              -- Operand Width
    busw  : integer := 32);             -- Bus Width

  port(
    i1data : in  std_logic_vector(dataw-1 downto 0);
    i1load : in  std_logic;        
    i2data : in  std_logic_vector(dataw-1 downto 0);
    i2load : in  std_logic;    
    i3data : in  std_logic_vector(dataw-1 downto 0);
    i3load : in  std_logic;
    i4data : in  std_logic_vector(dataw-1 downto 0);    
    i4load : in  std_logic;
    r1data : out std_logic_vector(busw-1 downto 0);
    glock  : in  std_logic;
    rstx   : in  std_logic;
    clk    : in  std_logic);
end add4_always_1;

architecture rtl of add4_always_1 is
  
  signal i1reg   : std_logic_vector(dataw-1 downto 0);
  signal i2reg   : std_logic_vector(dataw-1 downto 0);
  signal i3reg   : std_logic_vector(dataw-1 downto 0);
  signal i4reg   : std_logic_vector(dataw-1 downto 0);  
  signal r1reg     : std_logic_vector(dataw-1 downto 0);
  
begin
  
  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      i1reg  <= (others => '0');
      i2reg  <= (others => '0');
      i3reg  <= (others => '0');
      i4reg  <= (others => '0');
      r1reg  <= (others => '0');            
      
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then

        case i1load is
          when '1' =>
            i1reg  <= i1data;
          when others => null;
        end case;

        case i2load is
          when '1' =>
            i2reg  <= i2data;
          when others => null;
        end case;

        case i3load is
          when '1' =>
            i3reg  <= i3data;
          when others => null;
        end case;

        -- This is the triggering port. When data is loaded in this,
        -- we'll compute the result.
        case i4load is
          when '1' =>
            i4reg  <= i4data;
            -- TODO: does this work? Are the regs updated already?
            r1reg <= conv_std_logic_vector(
              signed(i1reg) + signed(i2reg) + signed(i3reg) + signed(i4reg),
              dataw);
          when others => null;
        end case;        

      end if;
    end if;
  end process regs;

  r1data <= r1reg;
  
end rtl;

