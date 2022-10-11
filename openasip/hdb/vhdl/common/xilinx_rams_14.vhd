--
-- Based on code example of Xilinx XST Dual-Port RAM with Enable on Each Port
-- from XST User Guide.
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

entity xilinx_rams_14 is
  generic (
    addrw : integer := 6;
    dataw : integer := 32);
  port (
    clk   : in  std_logic;
    ena   : in  std_logic;
    enb   : in  std_logic;
    wea   : in  std_logic;
    addra : in  std_logic_vector(addrw-1 downto 0);
    addrb : in  std_logic_vector(addrw-1 downto 0);
    dia   : in  std_logic_vector(dataw-1 downto 0);
    doa   : out std_logic_vector(dataw-1 downto 0);
    dob   : out std_logic_vector(dataw-1 downto 0));
end xilinx_rams_14;

architecture syn of xilinx_rams_14 is
  type ram_type is array (2**addrw-1 downto 0) of
    std_logic_vector (dataw-1 downto 0);
  signal RAM        : ram_type;
  signal read_addra : std_logic_vector(addrw-1 downto 0);
  signal read_addrb : std_logic_vector(addrw-1 downto 0);
begin  -- syn

  process (clk)
  begin
    if (clk'event and clk = '1') then
      if (ena = '1') then
        if (wea = '1') then
          RAM (conv_integer(addra)) <= dia;
        end if;
        read_addra <= addra;
      end if;
      if (enb = '1') then
        read_addrb <= addrb;
      end if;
    end if;
  end process;

  doa <= RAM(conv_integer(read_addra));
  dob <= RAM(conv_integer(read_addrb));

end syn;
