

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;


entity fu_add_always_1 is
  generic (
    dataw : integer := 32;              -- Operand Width
    busw  : integer := 32);             -- Bus Width

  port(
    t1data   : in  std_logic_vector(dataw-1 downto 0);
    t1opcode : in  std_logic_vector(0 downto 0);
    t1load   : in  std_logic;
    o1data   : in  std_logic_vector(dataw-1 downto 0);
    o1load   : in  std_logic;
    r1data   : out std_logic_vector(dataw-1 downto 0);
    glock      : in std_logic;
    rstx     : in  std_logic;
    clk      : in  std_logic);
end fu_add_always_1;

architecture rtl of fu_add_always_1 is
  
   signal t1reg     : std_logic_vector(dataw-1 downto 0);
   signal o1reg     : std_logic_vector(dataw-1 downto 0);
   signal o1temp    : std_logic_vector(dataw-1 downto 0);
   signal control   : std_logic_vector(1 downto 0);
  
begin

 control <= o1load&t1load;

   regs : process (clk, rstx)
   begin  -- process regs
     if rstx = '0' then                  -- asynchronous reset (active low)
       t1reg     <= (others => '0');
       o1reg     <= (others => '0');
       o1temp    <= (others => '0');
      
     elsif clk'event and clk = '1' then  -- rising clock edge
       if (glock='0') then
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

  r1data <= unsigned(o1reg) + unsigned(t1reg);


 
  
end rtl;


