-------------------------------------------------------------------------------
-- Opcode package
-------------------------------------------------------------------------------
library IEEE;
use IEEE.Std_Logic_1164.all;

package opcodes_stream_in_stream_in_status_V3 is

  constant STREAM_IN_PEEK_V3   : std_logic_vector(2-1 downto 0) := "00";
  constant STREAM_IN_STATUS_V3 : std_logic_vector(2-1 downto 0) := "01";
  constant STREAM_IN_V3        : std_logic_vector(2-1 downto 0) := "10";
  
end opcodes_stream_in_stream_in_status_V3;

-------------------------------------------------------------------------------
-- Stream In unit
-------------------------------------------------------------------------------
library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.numeric_std.all;
use work.opcodes_stream_in_stream_in_status_V3.all;

entity stream_in_stream_in_status_V3 is
  
  generic (
    busw : integer := 32);

  port (
    t1data   : in  std_logic_vector(31 downto 0);
    t1load   : in  std_logic;
    t1opcode : in  std_logic_vector(2-1 downto 0);
    r1data   : out std_logic_vector(31 downto 0);
    clk      : in  std_logic;
    rstx     : in  std_logic;
    glock    : in  std_logic;

    -- external port interface
    ext_data   : in  std_logic_vector(31 downto 0);  -- acquired data comes through this
    ext_status : in  std_logic_vector(8 downto 0);   -- status signal provided from outside
    ext_ack    : out std_logic          -- data acknowledge to outside
    );

end stream_in_stream_in_status_V3;


architecture rtl of stream_in_stream_in_status_V3 is
  
  signal r1reg  : std_logic_vector(31 downto 0);
  signal ackreg : std_logic;

begin
  
  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then
      r1reg  <= (others => '0');
      ackreg <= '0';
    elsif clk'event and clk = '1' then
      if glock = '0' then

        -- reset the acknowledge signal after a while
        ackreg <= '0';

        if t1load = '1' then
          case t1opcode is
            when STREAM_IN_V3 =>
              r1reg  <= ext_data;
              ackreg <= '1';
            when STREAM_IN_STATUS_V3 =>
              -- stream_in_status is placed in the least significant bits of r1reg
              r1reg <= (0 => ext_status(0), 1 => ext_status(1), 2 => ext_status(2), 3 => ext_status(3), 4 => ext_status(4), 5 => ext_status(5), 6 => ext_status(6), 7 => ext_status(7), 8 => ext_status(8), others => '0');
            when STREAM_IN_PEEK_V3 =>
              r1reg  <= ext_data;
            when others => null;
          end case;
        end if;

      end if;
    end if;
  end process regs;

  r1data  <= r1reg;
  ext_ack <= ackreg;
  
end rtl;
