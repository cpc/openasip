library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;

entity reflect is
  
  generic (
    dataw : integer := 32);

  port (
    data_in      : in  std_logic_vector(dataw-1 downto 0);
    size         : in  std_logic_vector(5 downto 0);
    reflect_data : out std_logic_vector(dataw-1 downto 0));

end reflect;

architecture reflect of reflect is
  signal reflect_temp : std_logic_vector(dataw-1 downto 0);
begin  -- reflect

  reflect_process: process (data_in,size)
    variable i : integer range 0 to dataw-1 := 0;
  begin  -- process reflect_process
    case size is
      when "100000" =>
        i := 0;
        for i in 0 to dataw-1 loop
          reflect_temp(i) <= data_in(dataw-1-i);
        end loop;  -- i
      when others =>        
        if dataw > 8 then
          i := 0;
          for i in 8 to dataw-1 loop
            reflect_temp(i) <= '0';
          end loop;  -- i          
          i := 0;
          for i in 0 to 7 loop
            reflect_temp(i) <= data_in(7-i);
          end loop;  -- i
        elsif dataw = 8 then
          i := 0;
          for i in 0 to 7 loop
            reflect_temp(i) <= data_in(7-i);
          end loop;  -- i          
        else
          i := 0;
          for i in 0 to dataw-1 loop
            reflect_temp(i) <= data_in(dataw-1-i);
          end loop;  -- i          
        end if;
    end case;
  end process reflect_process;

  reflect_data <= reflect_temp;
end reflect;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;

entity fu_reflect is
  
  generic (
    dataw : integer := 32;
    busw  : integer := 32);

  port (
    t1data : in  std_logic_vector(dataw-1 downto 0);
    t1load : in  std_logic;
    o1data : in  std_logic_vector(dataw-1 downto 0);
    o1load : in  std_logic;
    r1data : out std_logic_vector(busw-1 downto 0);
    clk : in std_logic;
    rstx : in std_logic;
    glock : in std_logic
    );
end fu_reflect;

architecture rtl of fu_reflect is

  component reflect
    generic (
      dataw       : integer := 32);
    port (
      data_in      : in  std_logic_vector(dataw-1 downto 0);
      size         : in  std_logic_vector(5 downto 0);
      reflect_data : out std_logic_vector(dataw-1 downto 0));
  end component;

  signal t1reg   : std_logic_vector(5 downto 0);
  signal o1reg   : std_logic_vector(dataw-1 downto 0);
  signal r1      : std_logic_vector(dataw-1 downto 0);
  signal r1reg   : std_logic_vector(dataw-1 downto 0);
  signal control : std_logic_vector(1 downto 0);

  signal result_en_reg : std_logic;
  
begin
  
  fu_arch : reflect
    generic map (
      dataw       => dataw)
    port map(
      data_in      => o1reg,
      size         => t1reg,
      reflect_data => r1);

  control <= o1load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg   <= (others => '0');
      o1reg   <= (others => '0');
      r1reg   <= (others => '0');

      result_en_reg <= '0';
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then

        case control is
          when "11" =>
            o1reg   <= o1data;
            t1reg   <= t1data(5 downto 0);
          when "10" =>
            o1reg <= o1data;
          when "01" =>
            t1reg   <= t1data(5 downto 0);
          when others => null;
        end case;

        -- update result only when new operation was triggered
        -- This should save power when clock gating is enabled
        result_en_reg <= t1load;

        if result_en_reg = '1' then
          r1reg <= r1;
        end if;

      end if;
    end if;
  end process regs;

  output: process (r1reg)
  begin  -- process output
    if dataw > busw then
      r1data(busw-1) <= r1reg(dataw-1);
      r1data(busw-2 downto 0) <= r1reg(busw-2 downto 0);
    elsif dataw = busw then
      r1data <= r1reg;
    else
      r1data <= sxt(r1reg,r1data'length);
    end if;
  end process output;

end rtl;

