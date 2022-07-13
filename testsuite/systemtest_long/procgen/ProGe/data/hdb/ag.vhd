-------------------------------------------------------------------------------
-- Title      : A VHDL-description of the SFU generating memory addresses
--              for the Index Generation of 1024-point in-place radix-4 DIT FFT
-- Project    : FlexDSP
-------------------------------------------------------------------------------
-- File       : ag.vhd
-- Author     : Risto Makinen <rmmakine@cs.tut.fi>
-- Company    :
-- Created    : 2005-03-21
-- Last update: 2019-04-26
-- Platform   :
-------------------------------------------------------------------------------
-- Description: SFU for Index Generation generates always two memory adresses
-- as its outputs. The other adress is an address of the input buffer and the
-- other, respectively, an address of the output buffer. The method with the
-- aid of which the addresses are generated depends on the current stage of
-- FFT computation. 
--
-- If the stage equals zero, the address of the input buffer
-- is generated according to the input permutation principle, i.e., by 
-- manipulating the lowermost ten bits of the linidx. The manipulation is made
-- by swapping two bits' bit fields from the LSB- and MSB-parts of this bit 
-- field of width ten in pairs, i.e. the lowermost two bits are swapped with 
-- the 9th and 8th bits, and the second lowermost two bits with the 7th and 6th
-- bits etc. After this manipulation, the address to be returned is computed by
-- adding the base address of the input buffer together with the manipulated
-- index, that is still shifted two bits to the left before this addition.
-- In the stage zero, the address of the output buffer is always the linear
-- address that corresponds the value of the linear index (linidx).
--
-- With other positive values of the stage-input, the address of the input
-- buffer is negligible from the FFT computation's point of view, so that it
-- is driven to zero. Meanwhile, the address of the output buffer is now
-- generated according to the index generation principle for the operand
-- access of the butterflies, i.e., by first rotating the lowermost n bits
-- of the linidx two bits to the right, and by then adding the rotated index,
-- that is also shifted two bits to the left before the addition, together 
-- with the base address of the output buffer. The number of the lowermost 
-- bits of the linidx, n, to be rotated two bits to the right is determined 
-- always as n = 2s + 2 where s is the current stage of FFT computation, 
-- 1 <= s <= log4(N) - 1.       
-------------------------------------------------------------------------------
-- base_address_in => operand[0]
-- base_address_out  => operand[1]
-- stage => operand[2]
-- linidx => trigger
-- in_addr => result[0]
-- out_addr => result[1]
-------------------------------------------------------------------------------
-- Copyright (c) 2005
-------------------------------------------------------------------------------


-------------------------------------------------------------------------------
-- Entity declaration for ag unit's user-defined architecture
-------------------------------------------------------------------------------
library IEEE;
use IEEE.Std_Logic_1164.all;
--use IEEE.Std_Logic_arith.all;
use ieee.numeric_std.all;

entity ag is
  generic (
    dataw : integer := 32;
    points: integer := 1024);
  port (
    base_address_in  : in std_logic_vector(dataw-1 downto 0);
    base_address_out : in std_logic_vector(dataw-1 downto 0);   
    stage            : in std_logic_vector(dataw-1 downto 0);
    linidx           : in std_logic_vector(dataw-1 downto 0);
    in_addr          : out std_logic_vector(dataw-1 downto 0);
    out_addr         : out std_logic_vector(dataw-1 downto 0));
end ag;


-------------------------------------------------------------------------------
-- Architecture declaration for ag unit's user-defined
-- architecture.
-------------------------------------------------------------------------------
architecture rtl of ag is

begin  -- rtl

  process (base_address_in, base_address_out, stage, linidx)
    variable stage_temp            : integer;
    variable tmp                   : unsigned(dataw-1 downto 0);
    -- Variables for the input permutation.
    variable permuted_idx          : std_logic_vector(dataw-1 downto 0);
    variable shifted_permuted_idx  : std_logic_vector(dataw-1 downto 0);
    variable shifted_linidx        : std_logic_vector(dataw-1 downto 0);
    -- Variables for the operand access.
    variable upper_part            : std_logic_vector(dataw-1 downto 0);
    variable rotation_part         : std_logic_vector(dataw-1 downto 0);
    variable temp                  : unsigned(dataw-1 downto 0);
    variable width_of_stage        : integer;
    variable rotated_index         : std_logic_vector(dataw-1 downto 0);
    variable shifted_stage         : std_logic_vector(dataw-1 downto 0);
    
  begin  -- process

    tmp := unsigned(stage);	
    --stage_temp := conv_integer(tmp);
    stage_temp := to_integer(tmp);
    
    case stage_temp is
      when  0 =>
-- Generate an address for the input permutation.
        permuted_idx(1 downto 0) := linidx(9 downto 8);
        permuted_idx(3 downto 2) := linidx(7 downto 6);
        permuted_idx(5 downto 4) := linidx(5 downto 4);
        permuted_idx(7 downto 6) := linidx(3 downto 2);
        permuted_idx(9 downto 8) := linidx(1 downto 0);	
        permuted_idx(dataw-1 downto 10) := linidx(dataw-1 downto 10);

        shifted_permuted_idx(dataw-1 downto 2) :=
          permuted_idx(dataw-3 downto 0);
        shifted_permuted_idx(1 downto 0) := "00";

        shifted_linidx(dataw-1 downto 2) := linidx(dataw-3 downto 0);
        shifted_linidx(1 downto 0) := "00";

        in_addr <= std_logic_vector(unsigned(shifted_permuted_idx)+
                                    unsigned(base_address_in));
        out_addr <= std_logic_vector(unsigned(shifted_linidx)+
                                     unsigned(base_address_out));

      when others =>
-- Generate an address for the operand access.
        shifted_stage(dataw-1 downto 1) := stage(dataw-2 downto 0);
        shifted_stage(0 downto 0) := "0";
        temp := unsigned(shifted_stage)+2;
        width_of_stage := to_integer(temp);

        case width_of_stage is
          when 2 =>
            upper_part(dataw-1 downto 0) :=linidx(dataw-1 downto 0);
            rotated_index(dataw-1 downto 2) := upper_part(dataw-3 downto 0);
          when 4 =>
            upper_part(dataw-1 downto 4) :=linidx(dataw-1 downto 4);
            rotation_part(4-3 downto 0) := linidx(4-1 downto 2);
            rotation_part(4-1 downto 4-2) := linidx(1 downto 0);
            rotated_index(dataw-1 downto 4+2) := upper_part(dataw-3 downto 4);
            rotated_index(4+1 downto 2) := rotation_part(4-1 downto 0);
          when 6 =>
            upper_part(dataw-1 downto 6) :=linidx(dataw-1 downto 6);
            rotation_part(6-3 downto 0) := linidx(6-1 downto 2);
            rotation_part(6-1 downto 6-2) := linidx(1 downto 0);
            rotated_index(dataw-1 downto 6+2) := upper_part(dataw-3 downto 6);
            rotated_index(6+1 downto 2) := rotation_part(6-1 downto 0);
          when 8 =>
            upper_part(dataw-1 downto 8) :=linidx(dataw-1 downto 8);
            rotation_part(8-3 downto 0) := linidx(8-1 downto 2);
            rotation_part(8-1 downto 8-2) := linidx(1 downto 0);
            rotated_index(dataw-1 downto 8+2) := upper_part(dataw-3 downto 8);
            rotated_index(8+1 downto 2) := rotation_part(8-1 downto 0);
          when 10 =>
            upper_part(dataw-1 downto 10) :=linidx(dataw-1 downto 10);
            rotation_part(10-3 downto 0) := linidx(10-1 downto 2);
            rotation_part(10-1 downto 10-2) := linidx(1 downto 0);
            rotated_index(dataw-1 downto 10+2) := upper_part(dataw-3 downto 10);
            rotated_index(10+1 downto 2) := rotation_part(10-1 downto 0);
          when others => null;
        end case;

        in_addr <= (others => '0');
        rotated_index(1 downto 0) := "00";
        out_addr <= std_logic_vector(unsigned(rotated_index)+
                                          unsigned(base_address_out));
    end case;
  end process;
end rtl;

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

entity fu_ag_always_1 is
  generic (
    dataw : integer := 32;              -- Operand Width
    busw  : integer := 32);             -- Bus Width

  port(
    t1data   : in  std_logic_vector(dataw-1 downto 0);
    t1load   : in  std_logic;
    o1data   : in  std_logic_vector(dataw-1 downto 0);
    o1load   : in  std_logic;
    o2data   : in  std_logic_vector(dataw-1 downto 0);
    o2load   : in  std_logic;
    o3data   : in  std_logic_vector(dataw-1 downto 0);
    o3load   : in  std_logic;
    r1data   : out std_logic_vector(busw-1 downto 0);
    r2data   : out std_logic_vector(busw-1 downto 0);
    glock    : in  std_logic;
    rstx     : in  std_logic;
    clk      : in  std_logic);
end fu_ag_always_1;

architecture rtl of fu_ag_always_1 is

  component ag
    generic (
      dataw            : integer := 32);
    port (
      base_address_in  : in std_logic_vector(dataw-1 downto 0);
      base_address_out : in std_logic_vector(dataw-1 downto 0);   
      stage            : in std_logic_vector(dataw-1 downto 0);
      linidx           : in std_logic_vector(dataw-1 downto 0);
      in_addr          : out std_logic_vector(dataw-1 downto 0);
      out_addr         : out std_logic_vector(dataw-1 downto 0));
  end component;

  signal t1reg     : std_logic_vector(dataw-1 downto 0);
  signal o1reg     : std_logic_vector(dataw-1 downto 0);
  signal o1temp    : std_logic_vector(dataw-1 downto 0);
  signal o2reg     : std_logic_vector(dataw-1 downto 0);
  signal o2temp    : std_logic_vector(dataw-1 downto 0);
  signal o3reg     : std_logic_vector(dataw-1 downto 0);
  signal o3temp    : std_logic_vector(dataw-1 downto 0);
  signal r1        : std_logic_vector(dataw-1 downto 0);
  signal r2        : std_logic_vector(dataw-1 downto 0);
  signal control   : std_logic_vector(3 downto 0);

begin

  ag_arch : ag
    generic map (
      dataw => dataw)
    port map (
      base_address_in  => o1reg,
      base_address_out => o2reg,
      stage            => o3reg,
      linidx           => t1reg,
      in_addr          => r1,
      out_addr         => r2);

  control <= o1load&o2load&o3load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg     <= (others => '0');
      o1reg     <= (others => '0');
      o1temp    <= (others => '0');
      o2reg     <= (others => '0');
      o2temp    <= (others => '0');
      o3reg     <= (others => '0');
      o3temp    <= (others => '0');
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock='0') then

        case control is

          when "0001" =>
            t1reg     <= t1data;
            o1reg     <= o1temp;
            o2reg     <= o2temp;
            o3reg     <= o3temp;
          when "0010" =>
            o3temp    <= o3data;
          when "0011" =>
            t1reg     <= t1data;
            o1reg     <= o1temp;
            o2reg     <= o2temp;
            o3reg     <= o3data;
            o3temp    <= o3data;
          when "0100" =>
            o2temp    <= o2data;
          when "0101" =>
            t1reg     <= t1data;
            o1reg     <= o1temp;
            o2reg     <= o2data;
            o2temp    <= o2data;
            o3reg     <= o3temp;
          when "0110" =>
            o2temp    <= o2data;
            o3temp    <= o3data;
          when "0111" =>
            t1reg     <= t1data;
            o1reg     <= o1temp;
            o2reg     <= o2data;
            o2temp    <= o2data;
            o3reg     <= o3data;
            o3temp    <= o3data;
          when "1000" =>
            o1temp    <= o1data;   
          when "1001" =>
            t1reg     <= t1data;
            o1reg     <= o1data;
            o1temp    <= o1data;
            o2reg     <= o2temp;
            o3reg     <= o3temp;
          when "1010" =>
            o1temp    <= o1data;
            o3temp    <= o3data;
          when "1011" =>
            t1reg     <= t1data;
            o1reg     <= o1data;
            o1temp    <= o1data;
            o2reg     <= o2temp;
            o3reg     <= o3data;
            o3temp    <= o3data;
          when "1100" =>
            o1temp    <= o1data;
            o2temp    <= o2data;
          when "1101" =>
            t1reg     <= t1data;
            o1reg     <= o1data;
            o1temp    <= o1data;
            o2reg     <= o2data;
            o2temp    <= o2data;
            o3reg     <= o3temp;
          when "1110" =>
            o1temp    <= o1data;
            o2temp    <= o2data;
            o3temp    <= o3data;
          when "1111" =>
            t1reg     <= t1data;
            o1reg     <= o1data;
            o1temp    <= o1data;
            o2reg     <= o2data;
            o2temp    <= o2data;
            o3reg     <= o3data;
            o3temp    <= o3data; 
          when others => null;
        end case;

      end if;
    end if;
  end process regs;

  r1data <= r1;
  r2data <= r2; 

end rtl;


-------------------------------------------------------------------------------
-- Entity declaration for unit ag, latency 2
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

entity fu_ag_always_2 is
  generic (
    dataw : integer := 32;              -- Operand Width
    busw  : integer := 32);             -- Bus Width

  port(
    t1data   : in  std_logic_vector(dataw-1 downto 0);
    t1load   : in  std_logic;
    o1data   : in  std_logic_vector(dataw-1 downto 0);
    o1load   : in  std_logic;
    o2data   : in  std_logic_vector(dataw-1 downto 0);
    o2load   : in  std_logic;
    o3data   : in  std_logic_vector(dataw-1 downto 0);
    o3load   : in  std_logic;
    r1data   : out std_logic_vector(busw-1 downto 0);
    r2data   : out std_logic_vector(busw-1 downto 0);
    glock    : in  std_logic;
    rstx     : in  std_logic;
    clk      : in  std_logic);
end fu_ag_always_2;

architecture rtl of fu_ag_always_2 is

  component ag
    generic (
      dataw : integer := 32);
    port (
      base_address_in  : in std_logic_vector(dataw-1 downto 0);
      base_address_out : in std_logic_vector(dataw-1 downto 0);   
      stage            : in std_logic_vector(dataw-1 downto 0);
      linidx           : in std_logic_vector(dataw-1 downto 0);
      in_addr          : out std_logic_vector(dataw-1 downto 0);
      out_addr         : out std_logic_vector(dataw-1 downto 0));
  end component;

  signal t1reg     : std_logic_vector(dataw-1 downto 0);
  signal o1reg     : std_logic_vector(dataw-1 downto 0);
  signal o2reg     : std_logic_vector(dataw-1 downto 0);
  signal o3reg     : std_logic_vector(dataw-1 downto 0);
  signal r1        : std_logic_vector(dataw-1 downto 0);
  signal r1reg     : std_logic_vector(dataw-1 downto 0);
  signal r2        : std_logic_vector(dataw-1 downto 0);
  signal r2reg     : std_logic_vector(dataw-1 downto 0);
  signal control   : std_logic_vector(3 downto 0);

  signal result_en_reg : std_logic;

begin

   ag_arch : ag 
     generic map (
       dataw => dataw)
     port map (
       base_address_in  => o1reg,
       base_address_out => o2reg,
       stage            => o3reg,
       linidx           => t1reg,
       in_addr          => r1,
       out_addr         => r2);

   control <= o1load&o2load&o3load&t1load;

   regs : process (clk, rstx)
   begin  -- process regs
     if rstx = '0' then                  -- asynchronous reset (active low)
       t1reg     <= (others => '0');
       o1reg     <= (others => '0');
       o2reg     <= (others => '0');
       o3reg     <= (others => '0');
       r1reg     <= (others => '0');
       r2reg     <= (others => '0');

       result_en_reg <= '0';
     elsif clk'event and clk = '1' then  -- rising clock edge
       if (glock='0') then

         case control is
           when "0001" =>
             t1reg    <= t1data;
           when "0010" =>
             o3reg    <= o3data;
           when "0011" =>
             t1reg    <= t1data;
             o3reg    <= o3data;
           when "0100" =>
             o2reg    <= o2data;
           when "0101" =>
             t1reg    <= t1data;
             o2reg    <= o2data;
           when "0110" =>
             o2reg    <= o2data;
             o3reg    <= o3data;
           when "0111" =>
             t1reg    <= t1data;
             o2reg    <= o2data;
             o3reg    <= o3data;
           when "1000" =>
             o1reg    <= o1data;
           when "1001" =>
             t1reg    <= t1data;
             o1reg    <= o1data;
           when "1010" =>
             o1reg    <= o1data;
             o3reg    <= o3data;
           when "1011" =>
             t1reg    <= t1data;    
             o1reg    <= o1data;
             o3reg    <= o3data;
           when "1100" =>
             o1reg    <= o1data;
             o2reg    <= o2data;
           when "1101" =>  
             t1reg    <= t1data;    
             o1reg    <= o1data;
             o2reg    <= o2data;
           when "1110" =>
             o1reg    <= o1data;
             o2reg    <= o2data;
             o3reg    <= o3data;
           when "1111" =>
             t1reg    <= t1data;   
             o1reg    <= o1data;
             o2reg    <= o2data;
             o3reg    <= o3data; 
           when others => null;
         end case;
         -- update the result only when a new operation was triggered
         result_en_reg <= t1load;

         if result_en_reg = '1' then
           r1reg <= r1;
           r2reg <= r2; 
         end if;

       end if;
     end if;
   end process regs;

   --r1data <= sxt(r1reg, busw);
   r1data <= r1reg;   

   --r2data <= sxt(r2reg, busw);
   r2data <= r2reg;

end rtl;

