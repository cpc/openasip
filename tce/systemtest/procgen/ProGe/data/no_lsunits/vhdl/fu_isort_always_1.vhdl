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


entity fu_isort_always_1 is
  generic (
    dataw : integer := 32);            
  port(
    o1data : in  std_logic_vector(dataw-1 downto 0);
    opcode : in  std_logic_vector (0 downto 0);
    o1load : in  std_logic;
    r1data : out std_logic_vector(dataw-1 downto 0);
    r2data : out std_logic_vector(dataw-1 downto 0);
    r3data : out std_logic_vector(dataw-1 downto 0);
    r4data : out std_logic_vector(dataw-1 downto 0);
    r5data : out std_logic_vector(dataw-1 downto 0);
    r6data : out std_logic_vector(dataw-1 downto 0);
    r7data : out std_logic_vector(dataw-1 downto 0);
    r8data : out std_logic_vector(dataw-1 downto 0);
    r9data : out std_logic_vector(dataw-1 downto 0);
    r10data : out std_logic_vector(dataw-1 downto 0);
    r11data : out std_logic_vector(dataw-1 downto 0);
    r12data : out std_logic_vector(dataw-1 downto 0);
    r13data : out std_logic_vector(dataw-1 downto 0);
    r14data : out std_logic_vector(dataw-1 downto 0);
    r15data : out std_logic_vector(dataw-1 downto 0);
    r16data : out std_logic_vector(dataw-1 downto 0);
    glock  : in  std_logic;
    rstx   : in  std_logic;
    clk    : in  std_logic);
end;

architecture rtl of fu_isort_always_1 is
  
  signal r1,r2,r3,r4,r5,r6,r7,r8        : std_logic_vector(dataw-1 downto 0);
  signal r9,r10,r11,r12,r13,r14,r15,r16 : std_logic_vector(dataw-1 downto 0);
  signal newsymb : std_logic_vector(dataw-1 downto 0);
  signal v1,v2,v3,v4,v5,v6,v7,v8        : unsigned(15 downto 0);
  signal v9,v10,v11,v12,v13,v14,v15,v16 : unsigned(15 downto 0);
  signal newval : unsigned(15 downto 0);
  signal ena : std_logic;
begin
  
   regs : process (clk, rstx)
   begin  
     if rstx = '0' then                  
       r1 <= conv_std_logic_vector(16#FFFFEEE#, dataw);
       r2 <= conv_std_logic_vector(16#FFFFEEE#, dataw);
       r3 <= conv_std_logic_vector(16#FFFFEEE#, dataw);
       r4 <= conv_std_logic_vector(16#FFFFEEE#, dataw);
       r5 <= conv_std_logic_vector(16#FFFFEEE#, dataw);
       r6 <= conv_std_logic_vector(16#FFFFEEE#, dataw);
       r7 <= conv_std_logic_vector(16#FFFFEEE#, dataw);
       r8 <= conv_std_logic_vector(16#FFFFEEE#, dataw);
       r9 <= conv_std_logic_vector(16#FFFFEEE#, dataw);
       r10 <= conv_std_logic_vector(16#FFFFEEE#, dataw);
       r11 <= conv_std_logic_vector(16#FFFFEEE#, dataw);
       r12 <= conv_std_logic_vector(16#FFFFEEE#, dataw);
       r13 <= conv_std_logic_vector(16#FFFFEEE#, dataw);
       r14 <= conv_std_logic_vector(16#FFFFEEE#, dataw);
       r15 <= conv_std_logic_vector(16#FFFFEEE#, dataw);
       r16 <= conv_std_logic_vector(16#FFFFEEE#, dataw);
       newsymb <= (others => '0');
       ena <= '0';
       
     elsif clk'event and clk = '1' then  -- rising clock edge
       if (glock = '0') then        
         if o1load = '1' then
           newsymb <= o1data;
           ena <= '1';
         else
           ena <= '0';
         end if;
         
         if newsymb = conv_std_logic_vector(16#1fffffff#, dataw) and ena = '1' then
           r1 <= conv_std_logic_vector(16#FFFFEEE#, dataw);
           r2 <= conv_std_logic_vector(16#FFFFEEE#, dataw);
           r3 <= conv_std_logic_vector(16#FFFFEEE#, dataw);
           r4 <= conv_std_logic_vector(16#FFFFEEE#, dataw);
           r5 <= conv_std_logic_vector(16#FFFFEEE#, dataw);
           r6 <= conv_std_logic_vector(16#FFFFEEE#, dataw);
           r7 <= conv_std_logic_vector(16#FFFFEEE#, dataw);
           r8 <= conv_std_logic_vector(16#FFFFEEE#, dataw);
           r9 <= conv_std_logic_vector(16#FFFFEEE#, dataw);
           r10 <= conv_std_logic_vector(16#FFFFEEE#, dataw);
           r11 <= conv_std_logic_vector(16#FFFFEEE#, dataw);
           r12 <= conv_std_logic_vector(16#FFFFEEE#, dataw);
           r13 <= conv_std_logic_vector(16#FFFFEEE#, dataw);
           r14 <= conv_std_logic_vector(16#FFFFEEE#, dataw);
           r15 <= conv_std_logic_vector(16#FFFFEEE#, dataw);
           r16 <= conv_std_logic_vector(16#FFFFEEE#, dataw);
         else
         
         if ena = '1' and newval < v16 then
           if newval > v15 then
             r16 <= newsymb;
           elsif newval > v14 then
             r16 <= r15;
             r15 <= newsymb;
           elsif newval > v13 then
             r16 <= r15;
             r15 <= r14;
             r14 <= newsymb;
           elsif newval > v12 then
             r16 <= r15;
             r15 <= r14;
             r14 <= r13;
             r13 <= newsymb;
           elsif newval > v11 then
             r16 <= r15;
             r15 <= r14;
             r14 <= r13;
             r13 <= r12;
             r12 <= newsymb;
           elsif  newval > v10 then
             r16 <= r15;
             r15 <= r14;
             r14 <= r13;
             r13 <= r12;
             r12 <= r11;
             r11 <= newsymb;
           elsif newval > v9 then
             r16 <= r15;
             r15 <= r14;
             r14 <= r13;
             r13 <= r12;
             r12 <= r11;
             r11 <= r10;
             r10 <= newsymb;
           elsif newval > v8 then
             r16 <= r15;
             r15 <= r14;
             r14 <= r13;
             r13 <= r12;
             r12 <= r11;
             r11 <= r10;
             r10 <= r9;
             r9 <= newsymb;
           elsif newval > v7 then
             r16 <= r15;
             r15 <= r14;
             r14 <= r13;
             r13 <= r12;
             r12 <= r11;
             r11 <= r10;
             r10 <= r9;
             r9 <= r8;
             r8 <= newsymb;
           elsif newval > v6 then
             r16 <= r15;
             r15 <= r14;
             r14 <= r13;
             r13 <= r12;
             r12 <= r11;
             r11 <= r10;
             r10 <= r9;
             r9 <= r8;
             r8 <= r7;
             r7 <= newsymb;
           elsif newval > v5 then
             r16 <= r15;
             r15 <= r14;
             r14 <= r13;
             r13 <= r12;
             r12 <= r11;
             r11 <= r10;
             r10 <= r9;
             r9 <= r8;
             r8 <= r7;
             r7 <= r6;
             r6 <= newsymb;
           elsif newval > v4 then
             r16 <= r15;
             r15 <= r14;
             r14 <= r13;
             r13 <= r12;
             r12 <= r11;
             r11 <= r10;
             r10 <= r9;
             r9 <= r8;
             r8 <= r7;
             r7 <= r6;
             r6 <= r5;
             r5 <= newsymb;
           elsif newval > v3 then
             r16 <= r15;
             r15 <= r14;
             r14 <= r13;
             r13 <= r12;
             r12 <= r11;
             r11 <= r10;
             r10 <= r9;
             r9 <= r8;
             r8 <= r7;
             r7 <= r6;
             r6 <= r5;
             r5 <= r4;
             r4 <= newsymb;
           elsif newval > v2 then
             r16 <= r15;
             r15 <= r14;
             r14 <= r13;
             r13 <= r12;
             r12 <= r11;
             r11 <= r10;
             r10 <= r9;
             r9 <= r8;
             r8 <= r7;
             r7 <= r6;
             r6 <= r5;
             r5 <= r4;
             r4 <= r3;
             r3 <= newsymb;
           elsif newval > v1 then
             r16 <= r15;
             r15 <= r14;
             r14 <= r13;
             r13 <= r12;
             r12 <= r11;
             r11 <= r10;
             r10 <= r9;
             r9 <= r8;
             r8 <= r7;
             r7 <= r6;
             r6 <= r5;
             r5 <= r4;
             r4 <= r3;
             r3 <= r2;
             r2 <= newsymb;
           else
             r16 <= r15;
             r15 <= r14;
             r14 <= r13;
             r13 <= r12;
             r12 <= r11;
             r11 <= r10;
             r10 <= r9;
             r9 <= r8;
             r8 <= r7;
             r7 <= r6;
             r6 <= r5;
             r5 <= r4;
             r4 <= r3;
             r3 <= r2;
             r2 <= r1;
             r1 <= newsymb;
           end if;
         end if; -- newval < 16
         end if; -- reset with 0x1fff...
       end if; -- glock
     end if; -- clock
   end process regs;


   -- extract values
   newval <= unsigned(newsymb(27 downto 12));
   
   v1 <= unsigned(r1(27 downto 12));
   v2 <= unsigned(r2(27 downto 12));
   v3 <= unsigned(r3(27 downto 12));
   v4 <= unsigned(r4(27 downto 12));
   v5 <= unsigned(r5(27 downto 12));
   v6 <= unsigned(r6(27 downto 12));
   v7 <= unsigned(r7(27 downto 12));
   v8 <= unsigned(r8(27 downto 12));
   v9 <= unsigned(r9(27 downto 12));
   v10 <= unsigned(r10(27 downto 12));
   v11 <= unsigned(r11(27 downto 12));
   v12 <= unsigned(r12(27 downto 12));
   v13 <= unsigned(r13(27 downto 12));
   v14 <= unsigned(r14(27 downto 12));
   v15 <= unsigned(r15(27 downto 12));
   v16 <= unsigned(r16(27 downto 12));
  
   -- run output
   r1data <= r1;        
   r2data <= r2;
   r3data <= r3;        
   r4data <= r4;
   r5data <= r5;        
   r6data <= r6;
   r7data <= r7;        
   r8data <= r8;
   r9data <= r9;        
   r10data <= r10;
   r11data <= r11;        
   r12data <= r12;
   r13data <= r13;        
   r14data <= r14;
   r15data <= r15;        
   r16data <= r16;
  
end rtl;


