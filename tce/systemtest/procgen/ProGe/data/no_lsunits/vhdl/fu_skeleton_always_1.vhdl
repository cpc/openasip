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


entity fu_cadd_csub_always_1 is
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

architecture rtl of fu_cadd_csub_always_1 is
  
  signal re1   : std_logic_vector(dataw/2-1 downto 0);
  signal re2   : std_logic_vector(dataw/2-1 downto 0);    
  signal im1   : std_logic_vector(dataw/2-1 downto 0);
  signal im2   : std_logic_vector(dataw/2-1 downto 0);
  signal re    : std_logic_vector(dataw/2-1 downto 0);
  signal im    : std_logic_vector(dataw/2-1 downto 0);
  
begin
  
  regs : process (clk, rstx)
  begin  
    if rstx = '0' then                  
      re1  <= (others => '0');
      re2  <= (others => '0');
      im1  <= (others => '0');
      im2  <= (others => '0');
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then
        
        -- load operands
        if o1load = '1' then
          re1 <= o1data(dataw-1 downto dataw/2);
          im1 <= o1data(dataw/2-1 downto 0);
        end if;
        if o2load = '1' then
          re2 <= o2data(dataw-1 downto dataw/2);
          im2 <= o2data(dataw/2-1 downto 0);
        end if;
                
      end if; -- glock
    end if; -- clock
  end process regs;

  -- compute
  re <= signed(re1) + signed(re2) when opcode = "0" else
        signed(re1) - signed(re2);

  im <= signed(im1) + signed(im2) when opcode = "0" else
        signed(im1) - signed(im2);
        
  rdata <= re & im;
  
end rtl;


