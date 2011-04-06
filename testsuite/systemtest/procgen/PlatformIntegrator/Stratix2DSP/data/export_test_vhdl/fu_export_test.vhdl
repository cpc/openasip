-------------------------------------------------------------------------------
-- Imaginary FU for testing unknown imported signals in platform integrator
-- NOT FOR REAL USE

library ieee;
use ieee.std_logic_1164.all;

entity fu_export_test_always_1 is

  generic (
    dataw : integer := 32);
  port (
    t1data         : in  std_logic_vector(dataw-1 downto 0);
    t1load         : in  std_logic;
    r1data         : out std_logic_vector(dataw-1 downto 0);
    -- hoax ports for testing exported signals
    debug_result   : out std_logic_vector(dataw-1 downto 0);
    debug_internal : out std_logic_vector(0 downto 0);
    -- control signals
    clk            : in  std_logic;
    rstx           : in  std_logic;
    glock          : in  std_logic
    );

end fu_export_test_always_1;

architecture rtl of fu_export_test_always_1 is

  signal t1data_r : std_logic_vector(dataw-1 downto 0);
  
begin  -- rtl

  process (clk, rstx)
  begin  -- process
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1data_r <= (others => '0');
    elsif clk'event and clk = '1' then  -- rising clock edge
      if glock = '0' then
        if t1load = '1' then
          t1data_r <= t1data;
        end if;

        debug_result   <= t1data_r;
        debug_internal <= "0";
      else
        -- glock = 1
        debug_internal <= "1";
      end if;
    end if;
  end process;

  r1data <= t1data_r;
  
end rtl;
