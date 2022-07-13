library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity adder_resource is

  port (
    a : in std_logic_vector(31 downto 0);
    b : in std_logic_vector(31 downto 0);
    z : out std_logic_vector(31 downto 0));

end entity adder_resource;

architecture rtl of adder_resource is

begin  -- architecture rtl

  z <= std_logic_vector(signed(a) + signed(b));

end architecture rtl;
