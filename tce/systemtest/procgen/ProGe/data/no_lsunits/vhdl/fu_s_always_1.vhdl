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


entity fu_s_always_1 is
  generic (
    dataw : integer := 32);            
  port(
    o1data : in  std_logic_vector(dataw-1 downto 0);
    opcode : in  std_logic_vector (1 downto 0);
    o1load : in  std_logic;
    o2data : in  std_logic_vector(dataw-1 downto 0);
    o2load : in  std_logic;
    o3data : in  std_logic_vector(dataw-1 downto 0);
    o3load : in  std_logic;
    rdata : out std_logic_vector(dataw-1 downto 0);
    glock  : in  std_logic;
    rstx   : in  std_logic;
    clk    : in  std_logic);
end;

architecture rtl of fu_s_always_1 is
  
  signal opr : std_logic_vector(1 downto 0);
  signal a : std_logic_vector(dataw-1 downto 0);
  signal b : std_logic_vector(dataw-1 downto 0);
  signal current : std_logic_vector(dataw-1 downto 0);
  signal current_3 : std_logic_vector(dataw-1 downto 0);
  signal current_6 : std_logic_vector(dataw-1 downto 0);
  signal current_9 : std_logic_vector(dataw-1 downto 0);
  
begin
  
   regs : process (clk, rstx)
   begin  
     if rstx = '0' then                  
       opr <= (others => '0');
       a <= (others => '0');
       b <= (others => '0');
       current <= (others => '0');
     elsif clk'event and clk = '1' then  -- rising clock edge
       if (glock = '0') then
         -- load operands
         if o1load = '1' then
           a(dataw-1 downto 12) <= o1data(dataw-13 downto 0);
           a(11 downto 0) <= (others => '0');
         end if;
         if o2load = '1' then
           b <= o2data(dataw-1 downto 0);
         end if;
         if o3load = '1' then
           current <= o3data(dataw-1 downto 0);
           opr   <= opcode;
         end if;
         
       end if; -- glock
     end if; -- clock
   end process regs;

   current_3(dataw-1 downto 3) <= current(dataw-4 downto 0);
   current_3(2 downto 0) <= (others => '0');

   current_6(dataw-1 downto 6) <= current(dataw-7 downto 0);
   current_6(5 downto 0) <= (others => '0');

   current_9(dataw-1 downto 9) <= current(dataw-10 downto 0);
   current_9(8 downto 0) <= (others => '0');
   
   rdata <= (a and conv_std_logic_vector(16#FFFF000#, dataw)) or
            (conv_std_logic_vector(16#7#, dataw) and current) when opcode = "11" else

            (a and conv_std_logic_vector(16#FFFF000#, dataw)) or
            (b and conv_std_logic_vector(16#E00#, dataw)) or
            (b and conv_std_logic_vector(16#1C0#, dataw)) or
            (current_3 and conv_std_logic_vector(16#38#, dataw)) or
            (b and conv_std_logic_vector(16#7#, dataw)) when opcode = "10" else

            (a and conv_std_logic_vector(16#FFFF000#, dataw)) or
            (b and conv_std_logic_vector(16#E00#, dataw)) or
            (current_6 and conv_std_logic_vector(16#1C0#, dataw)) or
            (b and conv_std_logic_vector(16#38#, dataw)) or
            (b and conv_std_logic_vector(16#7#, dataw)) when opcode = "01" else

            (a and conv_std_logic_vector(16#FFFF000#, dataw)) or
            (current_9 and conv_std_logic_vector(16#E00#, dataw)) or
            (b and conv_std_logic_vector(16#1C0#, dataw)) or
            (b and conv_std_logic_vector(16#38#, dataw)) or
            (b and conv_std_logic_vector(16#7#, dataw)) when opcode = "00" else
            
            (others => '0');
  
end rtl;


