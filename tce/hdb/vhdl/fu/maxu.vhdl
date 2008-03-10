library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;

entity maxu_arith is
  
  generic (
    dataw : integer := 32);

  port (
    a   : in  std_logic_vector(dataw-1 downto 0);
    b   : in  std_logic_vector(dataw-1 downto 0);
    res : out std_logic_vector(dataw-1 downto 0));
end maxu_arith;

architecture rtl of maxu_arith is
begin  -- rtl

  process (a,b)
  begin  -- process
    if (unsigned(a) >= unsigned(b)) then
      res <= a;
    else
      res <= b;
    end if;
  end process;
end rtl;
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;

entity fu_maxu_always_1 is
  generic (
    dataw : integer := 32;              -- Operand Width
    busw  : integer := 32);             -- Bus Width

  port(
    t1data : in  std_logic_vector(dataw-1 downto 0);
    t1load : in  std_logic;
    o1data : in  std_logic_vector(dataw-1 downto 0);
    o1load : in  std_logic;
    r1data : out std_logic_vector(busw-1 downto 0);
    glock  : in  std_logic;
    rstx   : in  std_logic;
    clk    : in  std_logic);
end fu_maxu_always_1;

architecture rtl of fu_maxu_always_1 is

  component maxu_arith 
    generic (
      dataw : integer := 32);
    
    port (
      a   : in  std_logic_vector(dataw-1 downto 0);
      b   : in  std_logic_vector(dataw-1 downto 0);
      res : out std_logic_vector(dataw-1 downto 0));
  end component;
    
  signal t1reg   : std_logic_vector(dataw-1 downto 0);
  signal o1reg   : std_logic_vector(dataw-1 downto 0);
  signal o1temp  : std_logic_vector(dataw-1 downto 0);
  signal r1      : std_logic_vector(dataw-1 downto 0);
  signal control : std_logic_vector(1 downto 0);
  
begin

  fu_arch : maxu_arith
    generic map (
      dataw => dataw)
    port map (
      a   => t1reg,
      b   => o1reg,
      res => r1);
  
  control <= o1load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg  <= (others => '0');
      o1reg  <= (others => '0');
      o1temp <= (others => '0');
      
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then

        case control is
          when "11" =>
            t1reg  <= t1data;
            o1reg  <= o1data;
            o1temp <= o1data;
          when "10" =>
            o1temp <= o1data;
          when "01" =>
            t1reg <= t1data;
            o1reg <= o1temp;
          when others => null;
        end case;

      end if;
    end if;
  end process regs;

  process (r1)
  begin  -- process
    if busw < dataw then
      r1data(dataw-1) <= r1(dataw-1);
      r1data(busw-2 downto 0) <= r1(busw-2 downto 0);
    else
      r1data <= sxt(r1,busw);
    end if;
  end process;

end rtl;

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

entity fu_maxu_always_2 is
  generic (
    dataw : integer := 32;              -- Operand Width
    busw  : integer := 32);             -- Bus Width

  port(
    t1data : in  std_logic_vector(dataw-1 downto 0);
    t1load : in  std_logic;
    o1data : in  std_logic_vector(dataw-1 downto 0);
    o1load : in  std_logic;
    r1data : out std_logic_vector(busw-1 downto 0);
    glock  : in  std_logic;
    rstx   : in  std_logic;
    clk    : in  std_logic);
end fu_maxu_always_2;

architecture rtl of fu_maxu_always_2 is
  component maxu_arith 
    generic (
      dataw : integer := 32);
    
    port (
      a   : in  std_logic_vector(dataw-1 downto 0);
      b   : in  std_logic_vector(dataw-1 downto 0);
      res : out std_logic_vector(dataw-1 downto 0));
  end component;
  
  signal t1reg   : std_logic_vector(dataw-1 downto 0);
  signal o1reg   : std_logic_vector(dataw-1 downto 0);
  signal r1      : std_logic_vector(dataw-1 downto 0);
  signal r1reg   : std_logic_vector(busw-1 downto 0);
  signal control : std_logic_vector(1 downto 0);

  signal result_en_reg : std_logic;
  
begin

  fu_arch : maxu_arith
    generic map (
      dataw => dataw)
    port map (
      a   => t1reg,
      b   => o1reg,
      res => r1);  
  control <= o1load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg <= (others => '0');
      o1reg <= (others => '0');
      r1reg <= (others => '0');

      result_en_reg <= '0';
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then

        case control is
          when "11" =>
            t1reg <= t1data;
            o1reg <= o1data;
          when "10" =>
            o1reg <= o1data;
          when "01" =>
            t1reg <= t1data;
          when others => null;
        end case;

        -- update result only when new operation was triggered
        result_en_reg <= t1load;

        if result_en_reg = '1' then
          -- if busw is bigger keep sign and take bit's from
          -- lsb path.
          if busw < dataw then
            r1reg(dataw-1) <= r1(dataw-1);
            r1reg(busw-2 downto 0) <= r1(busw-2 downto 0);
          else
            r1reg <= sxt(r1,busw);
          end if;
          --r1reg <= r1;          
        end if;

      end if;
    end if;
  end process regs;
  --r1data <= sxt(r1reg, busw);
  r1data <= r1reg;
  
end rtl;
