library IEEE;
use IEEE.std_logic_1164.all;

package reflect8_reflect32_opcodes is

  constant OPC_REFLECT8  : std_logic_vector(0 downto 0) := "1";
  constant OPC_REFLECT32 : std_logic_vector(0 downto 0) := "0";
  
end reflect8_reflect32_opcodes;



library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use work.reflect8_reflect32_opcodes.all;

entity fu_reflect is
  
  generic (
    busw : integer := 32);

  port (
    t1data   : in  std_logic_vector(busw-1 downto 0);
    t1load   : in  std_logic;
    t1opcode : in  std_logic_vector(0 downto 0);
    r1data   : out std_logic_vector(busw-1 downto 0);
    clk      : in  std_logic;
    rstx     : in  std_logic;
    glock    : in  std_logic
    );
end fu_reflect;


architecture rtl of fu_reflect is

  signal r1reg : std_logic_vector(busw-1 downto 0);
  
begin

  regs : process (clk, rstx)
    variable i   : integer range 0 to busw-1 := 0;
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      r1reg <= (others => '0');
    elsif clk'event and clk = '1' then  -- rising clock edge
      if glock = '0' then
        if t1load = '1' then
          
          case t1opcode is
            when OPC_REFLECT8 =>
              r1reg <= (others => '0');
              i     := 0;
              for i in 0 to 7 loop
                r1reg(i) <= t1data(7-i);
              end loop;
            when OPC_REFLECT32 =>
              r1reg <= (others => '0');
              i     := 0;
              for i in 0 to busw-1 loop
                r1reg(i) <= t1data(busw-1-i);
              end loop;
            when others =>
              null;
          end case;
          
        end if;
      end if;
    end if;
  end process regs;

  r1data <= r1reg;

end rtl;

