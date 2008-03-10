-------------------------------------------------------------------------------
-- Complex addition and subtraction
-- All operands must appear at the same clock cycle. Otherwise,
-- the previous result is corrupted.
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;


entity fu_sq_always_1 is
  generic (
    dataw : integer := 32);            
  port(
    o1data : in  std_logic_vector(dataw-1 downto 0);
    o1load : in  std_logic;
    rdata : out std_logic_vector(dataw-1 downto 0);
    opcode : in  std_logic_vector (0 downto 0);
    glock  : in  std_logic;
    rstx   : in  std_logic;
    clk    : in  std_logic);
end;

architecture rtl of fu_sq_always_1 is

  signal m : std_logic_vector(dataw-1 downto 0);
  signal product : std_logic_vector(2*dataw-1 downto 0);
  
begin
  
   regs : process (clk, rstx)
   begin  
     if rstx = '0' then                  
       m  <= (others => '0');
     elsif clk'event and clk = '1' then  -- rising clock edge
       if (glock = '0') then
         -- load operands
         if o1load = '1' then
           m <= o1data;
         end if;                
       end if; -- glock
     end if; -- clock
   end process regs;
   
   product <= signed(m) * signed(m);
   rdata <= product(dataw+9 downto 10);
  
end rtl;


