library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity printchar_always_1 is
  generic (
    dataw  : integer := 8);
  port (
    t1data : in std_logic_vector(dataw-1 downto 0);
    t1load : in std_logic;
    t2data : in std_logic_vector(0 downto 0);
    t2load : in std_logic;
    o1data : out std_logic_vector(0 downto 0);
    glock  : in std_logic;
    clk    : in std_logic;
    rstx   : in std_logic);

end printchar_always_1;

architecture sim of printchar_always_1 is

--pragma translate_off
  function to_char(constant Byte : std_logic_vector(7 downto 0))
    return character is
  begin
    return character'val(to_integer(unsigned(Byte)));
  end function;
--pragma translate_on
begin
--pragma translate_off
  file_output : process (t1data, t1load, clk, glock)
    type ChFile is file of character;
    file OutFile : ChFile open write_mode is "hdl_sim_stdout.txt";

  begin  -- process
    if clk'event and clk='1' and t1load='1' and glock='0' then
      write(OutFile, to_char(t1data(7 downto 0)));
    end if;
  end process file_output;

  o1data <= (others => '0');
--pragma translate_on

end sim;
