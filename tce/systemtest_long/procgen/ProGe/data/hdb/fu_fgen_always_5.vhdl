library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

entity fu_fgen_always_5 is
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
end fu_fgen_always_5;

architecture rtl OF fu_fgen_always_5 is
  
  component fft_coeff_generator
    generic (
      dataw : integer;
      N : integer;
      stage_bits : integer;
      addr_width : integer);
    port(
      stage : in  std_logic_vector(dataw-1 downto 0);  --trigger
      index : in  std_logic_vector(dataw-1 downto 0);  --operand
      --rst_x : in std_logic;
      clk : in std_logic;
      t1load_1 : in std_logic;
      t1load_2 : in std_logic;
      coeff : out std_logic_vector(dataw-1 downto 0));  --result
  end component;

  signal t1reg   : std_logic_vector(dataw-1 downto 0);
  signal o1reg   : std_logic_vector(dataw-1 downto 0);
  signal r1      : std_logic_vector(dataw-1 downto 0);
  signal r1reg   : std_logic_vector(dataw-1 downto 0);
  signal r1reg_1   : std_logic_vector(dataw-1 downto 0);  
  signal control : std_logic_vector(1 downto 0);

  signal t1load_1 : std_logic;
  signal t1load_2 : std_logic;
  signal result_en_reg : std_logic;
  signal result_en_reg_1 : std_logic;  
  

  
begin

  
  -- fft_coeff_generator has 2 pipeline stages
  fu_arch : fft_coeff_generator
    generic map (
      dataw => dataw,
      N => 1024,
      stage_bits => 3,
      addr_width => 9
      )
    port map(
      index => t1reg,
      stage => o1reg,
      t1load_1 => t1load_1,
      clk => clk,
      t1load_2 => t1load_2,
      coeff => r1);

  control <= o1load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg <= (others => '0');
      o1reg <= (others => '0');
      r1reg <= (others => '0');
      t1load_1 <= '0';
      t1load_2 <= '0';
      result_en_reg <= '0';
      r1reg <= (others => '0');
      r1reg_1 <= (others => '0');
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
        -- to obtain latency of five load signal must be delayed
        -- by four stages and ouput data from coeff_generator
        -- have to delay one cycle.
        t1load_1 <= t1load;
        t1load_2 <= t1load_1;
        
        result_en_reg <= t1load_2;
        result_en_reg_1 <= result_en_reg;


        if result_en_reg = '1' then
          r1reg <= r1;
        end if;
        if result_en_reg_1 = '1' then
          r1reg_1 <= r1reg;
        end if;

      end if;
    end if;
  end process regs;

  r1data <= sxt(r1reg_1, busw);
  --r1data <= r1;
  
end rtl;
