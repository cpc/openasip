-------------------------------------------------------------------------------
-- Title      : Register File for TTA
-- Project    : FlexDSP
-------------------------------------------------------------------------------
-- File       : rf_2wr_2rd.vhdl
-- Author     : 
-- Company    : 
-- Created    : 2003-18-11
-- Last update: 2007/09/21
-------------------------------------------------------------------------------
-- Description: 2 Write port(s)
--              2 Read port(s)
--              
-------------------------------------------------------------------------------
-- Copyright (c) 2002 
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author   Description
-- 2003-18-11  1.0      sertamo   Created
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.std_logic_arith.all;
use work.util.all;

entity rf_16wr_1rd is
  generic (
    dataw : integer := 32;
    rf_size : integer := 8);
  port(
    
    t1data   : in std_logic_vector (dataw-1 downto 0);
    t1opcode : in std_logic_vector (bit_width(rf_size)-1 downto 0);
    t1load   : in std_logic;
    
    t2data   : in std_logic_vector (dataw-1 downto 0);
    t2opcode : in std_logic_vector (bit_width(rf_size)-1 downto 0);
    t2load   : in std_logic;

    t3data   : in std_logic_vector (dataw-1 downto 0);
    t3opcode : in std_logic_vector (bit_width(rf_size)-1 downto 0);
    t3load   : in std_logic;

    t4data   : in std_logic_vector (dataw-1 downto 0);
    t4opcode : in std_logic_vector (bit_width(rf_size)-1 downto 0);
    t4load   : in std_logic;

    t5data   : in std_logic_vector (dataw-1 downto 0);
    t5opcode : in std_logic_vector (bit_width(rf_size)-1 downto 0);
    t5load   : in std_logic;
    
    t6data   : in std_logic_vector (dataw-1 downto 0);
    t6opcode : in std_logic_vector (bit_width(rf_size)-1 downto 0);
    t6load   : in std_logic;

    t7data   : in std_logic_vector (dataw-1 downto 0);
    t7opcode : in std_logic_vector (bit_width(rf_size)-1 downto 0);
    t7load   : in std_logic;

    t8data   : in std_logic_vector (dataw-1 downto 0);
    t8opcode : in std_logic_vector (bit_width(rf_size)-1 downto 0);
    t8load   : in std_logic;

    t9data   : in std_logic_vector (dataw-1 downto 0);
    t9opcode : in std_logic_vector (bit_width(rf_size)-1 downto 0);
    t9load   : in std_logic;
    
    t10data   : in std_logic_vector (dataw-1 downto 0);
    t10opcode : in std_logic_vector (bit_width(rf_size)-1 downto 0);
    t10load   : in std_logic;

    t11data   : in std_logic_vector (dataw-1 downto 0);
    t11opcode : in std_logic_vector (bit_width(rf_size)-1 downto 0);
    t11load   : in std_logic;

    t12data   : in std_logic_vector (dataw-1 downto 0);
    t12opcode : in std_logic_vector (bit_width(rf_size)-1 downto 0);
    t12load   : in std_logic;

    t13data   : in std_logic_vector (dataw-1 downto 0);
    t13opcode : in std_logic_vector (bit_width(rf_size)-1 downto 0);
    t13load   : in std_logic;
    
    t14data   : in std_logic_vector (dataw-1 downto 0);
    t14opcode : in std_logic_vector (bit_width(rf_size)-1 downto 0);
    t14load   : in std_logic;

    t15data   : in std_logic_vector (dataw-1 downto 0);
    t15opcode : in std_logic_vector (bit_width(rf_size)-1 downto 0);
    t15load   : in std_logic;

    t16data   : in std_logic_vector (dataw-1 downto 0);
    t16opcode : in std_logic_vector (bit_width(rf_size)-1 downto 0);
    t16load   : in std_logic;
    
    r1data   : out std_logic_vector (dataw-1 downto 0);
    r1opcode : in std_logic_vector (bit_width(rf_size)-1 downto 0);
    r1load   : in std_logic;
    
    guard    : out std_logic_vector(rf_size-1 downto 0);
        
    glock: in std_logic;
    rstx : in std_logic;
    clk    : in std_logic);
end rf_16wr_1rd;

architecture rtl of rf_16wr_1rd is
  type   reg_type is array (natural range <>) of std_logic_vector(dataw-1 downto 0 );
  subtype rf_index is integer range 0 to rf_size-1;
  signal reg    : reg_type (rf_size-1 downto 0);

begin

   -- no guard
   guard <= (others => '0');

   output1 : PROCESS (glock, r1load, r1opcode, reg)
   variable opc : integer;
    BEGIN
      IF glock = '0' THEN
        IF r1load = '1' THEN
          opc := conv_integer(unsigned(r1opcode));
          r1data <= reg(opc);
        END IF;
      END IF;
    END PROCESS;
   
 
   regfile_write : process(clk, rstx)
     variable opc : rf_index;
   begin
     if rstx = '0' then
       for idx in (reg'length-1) downto 0 loop
         reg(idx) <= (others => '0');
       end loop;  -- idx

     elsif clk = '1' and clk'event then
       if glock = '0' then
        
         if (t1load = '1') then
           opc      := conv_integer(unsigned(t1opcode));
           reg(opc) <= t1data;
         end if;
        
         if (t2load = '1') then
           opc      := conv_integer(unsigned(t2opcode));
           reg(opc) <= t2data;
         end if;

         if (t3load = '1') then
           opc      := conv_integer(unsigned(t3opcode));
           reg(opc) <= t3data;
         end if;

         if (t4load = '1') then
           opc      := conv_integer(unsigned(t4opcode));
           reg(opc) <= t4data;
         end if;

         if (t5load = '1') then
           opc      := conv_integer(unsigned(t5opcode));
           reg(opc) <= t5data;
         end if;

         if (t6load = '1') then
           opc      := conv_integer(unsigned(t6opcode));
           reg(opc) <= t6data;
         end if;

         if (t7load = '1') then
           opc      := conv_integer(unsigned(t7opcode));
           reg(opc) <= t7data;
         end if;

         if (t8load = '1') then
           opc      := conv_integer(unsigned(t8opcode));
           reg(opc) <= t8data;
         end if;

         if (t9load = '1') then
           opc      := conv_integer(unsigned(t9opcode));
           reg(opc) <= t9data;
         end if;
        
         if (t10load = '1') then
           opc      := conv_integer(unsigned(t10opcode));
           reg(opc) <= t10data;
         end if;

         if (t11load = '1') then
           opc      := conv_integer(unsigned(t11opcode));
           reg(opc) <= t11data;
         end if;

         if (t12load = '1') then
           opc      := conv_integer(unsigned(t12opcode));
           reg(opc) <= t12data;
         end if;

         if (t13load = '1') then
           opc      := conv_integer(unsigned(t13opcode));
           reg(opc) <= t13data;
         end if;

         if (t14load = '1') then
           opc      := conv_integer(unsigned(t14opcode));
           reg(opc) <= t14data;
         end if;

         if (t15load = '1') then
           opc      := conv_integer(unsigned(t15opcode));
           reg(opc) <= t15data;
         end if;

         if (t16load = '1') then
           opc      := conv_integer(unsigned(t16opcode));
           reg(opc) <= t16data;
         end if;

       end if;
     end if;
   end process regfile_write;

end rtl;

