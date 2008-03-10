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


entity fu_muxlut_always_1 is
  generic (
    dataw : integer := 32);            
  port(
    o1data : in  std_logic_vector(dataw-1 downto 0);
    opcode : in  std_logic_vector (1 downto 0);
    o1load : in  std_logic;
    rdata  : out std_logic_vector(dataw-1 downto 0);
    glock  : in  std_logic;
    rstx   : in  std_logic;
    clk    : in  std_logic);
end;

architecture rtl of fu_muxlut_always_1 is
  
  signal cb1  : std_logic_vector(2 downto 0);
  signal cb2  : std_logic_vector(2 downto 0);
  signal cb3  : std_logic_vector(2 downto 0);
  signal opr  : std_logic_vector(1 downto 0);
begin
  
  regs : process (clk, rstx)
  begin
    if rstx = '0' then                  
      cb1 <= (others => '0');
      cb2 <= (others => '0');
      cb3 <= (others => '0');
      opr <= (others => '0');
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then
        if o1load = '1' then
          cb1 <= o1data(8 downto 6);
          cb2 <= o1data(5 downto 3);
          cb3 <= o1data(2 downto 0);
          opr <= opcode;
        end if; 
      end if; -- glock
    end if; -- clock
  end process regs;

  rdata <= conv_std_logic_vector(16#FBAD#, dataw) when opr = "00" and cb1 = "000" else
           conv_std_logic_vector(16#FCE9#, dataw) when opr = "00" and cb1 = "001" else
           conv_std_logic_vector(16#FE25#, dataw) when opr = "00" and cb1 = "010" else
           conv_std_logic_vector(16#FF61#, dataw) when opr = "00" and cb1 = "011" else
           conv_std_logic_vector(16#9E#,   dataw) when opr = "00" and cb1 = "100" else
           conv_std_logic_vector(16#1DA#,  dataw) when opr = "00" and cb1 = "101" else
           conv_std_logic_vector(16#316#,  dataw) when opr = "00" and cb1 = "110" else
           conv_std_logic_vector(16#452#,  dataw) when opr = "00" and cb1 = "111" else

           conv_std_logic_vector(16#FBAD#, dataw) when opr = "01" and cb2 = "000" else
           conv_std_logic_vector(16#FCE9#, dataw) when opr = "01" and cb2 = "001" else
           conv_std_logic_vector(16#FE25#, dataw) when opr = "01" and cb2 = "010" else
           conv_std_logic_vector(16#FF61#, dataw) when opr = "01" and cb2 = "011" else
           conv_std_logic_vector(16#9E#,   dataw) when opr = "01" and cb2 = "100" else
           conv_std_logic_vector(16#1DA#,  dataw) when opr = "01" and cb2 = "101" else
           conv_std_logic_vector(16#316#,  dataw) when opr = "01" and cb2 = "110" else
           conv_std_logic_vector(16#452#,  dataw) when opr = "01" and cb2 = "111" else

           conv_std_logic_vector(16#FBAD#, dataw) when opr = "10" and cb3 = "000" else
           conv_std_logic_vector(16#FCE9#, dataw) when opr = "10" and cb3 = "001" else
           conv_std_logic_vector(16#FE25#, dataw) when opr = "10" and cb3 = "010" else
           conv_std_logic_vector(16#FF61#, dataw) when opr = "10" and cb3 = "011" else
           conv_std_logic_vector(16#9E#,   dataw) when opr = "10" and cb3 = "100" else
           conv_std_logic_vector(16#1DA#,  dataw) when opr = "10" and cb3 = "101" else
           conv_std_logic_vector(16#316#,  dataw) when opr = "10" and cb3 = "110" else
           conv_std_logic_vector(16#452#,  dataw) when opr = "10" and cb3 = "111" else

           (others => '0');
  
  
  
end rtl;


