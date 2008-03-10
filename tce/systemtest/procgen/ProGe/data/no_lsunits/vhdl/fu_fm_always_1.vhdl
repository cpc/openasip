
library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;


entity fu_fm_always_1 is
  generic (
    dataw : integer := 32);            
  port(
    o1data : in  std_logic_vector(dataw-1 downto 0);
    opcode : in  std_logic_vector (0 downto 0);
    o1load : in  std_logic;
    o2data : in  std_logic_vector(dataw-1 downto 0);
    o2load : in  std_logic;
    rdata : out std_logic_vector(dataw-1 downto 0);
    glock  : in  std_logic;
    rstx   : in  std_logic;
    clk    : in  std_logic);
end;

architecture rtl of fu_fm_always_1 is
  
  signal o1reg     : std_logic_vector(dataw-1 downto 0);
  signal o2reg     : std_logic_vector(dataw-1 downto 0);
  signal o2temp    : std_logic_vector(dataw-1 downto 0);
  signal control   : std_logic_vector(1 downto 0);
  signal product   : std_logic_vector(2*dataw-1 downto 0);
  
begin
  control <= o2load & o1load;

  regs : process (clk, rstx)
  begin  -- process regs
     if rstx = '0' then                  -- asynchronous reset (active low)
       o1reg     <= (others => '0');
       o2reg     <= (others => '0');
       o2temp    <= (others => '0');
      
     elsif clk'event and clk = '1' then  -- rising clock edge
       if (glock='0') then
         case control is
           when "11" =>
             o1reg  <= o1data;
             o2reg  <= o2data;
             o2temp <= o2data;
           when "10" =>
             o2temp <= o2data;
           when "01" =>
             o1reg <= o1data;
             o2reg <= o2temp;
           when others => null;
         end case;
       end if;
     end if;
   end process regs;

   product <= signed(o1reg) * signed(o2reg);
   rdata <= product(dataw+9 downto 10);
     
end rtl;


