library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use work.globals.all;

entity decompressor is

  port (
    fetch_en        : out std_logic;
    lock            : in  std_logic;
    fetchblock      : in  std_logic_vector(IMEMWIDTHINMAUS*IMEMMAUWIDTH-1 downto 0);
    instructionword : out std_logic_vector(INSTRUCTIONWIDTH-1 downto 0);
    glock           : out std_logic;
    lock_r          : in  std_logic;
    clk             : in  std_logic;
    rstx            : in  std_logic);
  
end decompressor;

architecture default of decompressor is
  
begin  -- default

  glock <= lock;
  fetch_en <= not lock_r;
  instructionword <= fetchblock(fetchblock'length-1 downto fetchblock'length-INSTRUCTIONWIDTH);
  
end default;
