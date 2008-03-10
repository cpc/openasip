library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;

entity min_arith is
  
  generic (
    dataw : integer := 32);

  port (
    a   : in  std_logic_vector(dataw-1 downto 0);
    b   : in  std_logic_vector(dataw-1 downto 0);
    res : out std_logic_vector(dataw-1 downto 0));
end min_arith;

architecture rtl of min_arith is
begin  -- rtl

  process (a,b)
  begin  -- process
    if (conv_signed(a) =< conv_signed(b)) then
      res <= a;
    else
      res <= b;
  end process;
end rtl;
