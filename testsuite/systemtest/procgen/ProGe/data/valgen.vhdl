
library ieee;
use ieee.std_logic_1164.all;

entity valgen is

  port (
    clk    : in  std_logic;
    rst_n  : in  std_logic;
    --
    glock  : in  std_logic;
    t1data : in  std_logic_vector(32-1 downto 0);
    t1load : in  std_logic;
    o1data : out std_logic_vector(64-1 downto 0);
    o1load : in  std_logic);

end entity valgen;

architecture rtl of valgen is

begin  -- architecture rtl

  o1data <= (others => '1');

end architecture rtl;
