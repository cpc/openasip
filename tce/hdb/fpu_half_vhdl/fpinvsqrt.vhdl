


-----------------------------------------------------
--                                                 --         
--     Floating point fast inverse square root     --
--                                                 --
-----------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;
LIBRARY WORK;
USE WORK.float_pkg_tce.ALL;


---------------------------------------
--         entity INVSQRT            --
---------------------------------------
entity invsqrt is 

generic(
    dataw       : integer := 16;
    exp_w       : integer := 5;
    frac_w      : integer := 10;
    guard_bits 	: integer := float_guard_bits
);

port (
   sum      :in  float(exp_w downto -frac_w); 
   inv_sqrt :out float(exp_w downto -frac_w)  
);  
end entity invsqrt;

---------------------------------------
--       architecture INVSQRT        --
---------------------------------------

architecture beh of invsqrt is
   
component fpmultiplier_block is
  
  port(
    a           : in  float(exp_w downto -frac_w);
    b           : in  float(exp_w downto -frac_w);
    sign        : out std_ulogic;
    round_guard : out std_ulogic;
    exp_out     : out signed(exp_w DOWNTO 0);
    frac_out    : out unsigned(frac_w+1+guard_bits DOWNTO 0) 
    );
end component;
   

component normalization is
  port(
    sign        : in std_ulogic;
    round_guard : in std_ulogic;
    exp_in      : in signed(exp_w DOWNTO 0);
    frac_in     : in unsigned(frac_w+1+guard_bits DOWNTO 0); 
    res_out     : out float(exp_w DOWNTO -frac_w)
    );
end component;
            
component sub_arith is
   port(
    A   : in  std_logic_vector(dataw-1 downto 0);
    B   : in  std_logic_vector(dataw-1 downto 0);
    S   : out std_logic_vector(dataw-1 downto 0));
end component;

component fpadd_block is
  port(
    a           : in  float(exp_w DOWNTO -frac_w);
    b           : in  float(exp_w DOWNTO -frac_w);
    sign        : out std_ulogic;
    round_guard : out std_ulogic;
    exp_out     : out signed(exp_w DOWNTO 0);
    frac_out    : out unsigned(frac_w+1+guard_bits DOWNTO 0) 
    );
end component; 
 
   -- constant magic    : std_logic_vector(dataw-1 downto 0) := "0101100111101111";  -- 0x598f 
   -- constant magic    : std_logic_vector(dataw-1 downto 0) := "0101100110010000";  -- 0x5990 
   constant magic       : std_logic_vector(dataw-1 downto 0) := "0101100110010001";  -- 0x5991 good
   constant half        : float(exp_w downto -frac_w) := "0011100000000000";  --real( 0.5)
   constant threehalfs  : float(exp_w downto -frac_w) := "0011111000000000";  --real( 1.5)
         
   signal sign_mul        : std_ulogic;
   signal roundguard_mul  : std_ulogic;
   signal expout_mul      : signed(exp_w DOWNTO 0);
   signal fracout_mul     : unsigned(frac_w+1+guard_bits DOWNTO 0);
   signal res_out_norm    : float(exp_w DOWNTO -frac_w);
   signal a_int           : std_logic_vector(dataw-1 downto 0);
   signal result_sub      : std_logic_vector(dataw-1 DOWNTO 0);
   signal res_float       : float(exp_w DOWNTO -frac_w);

   signal sign_mulN        : std_ulogic;
   signal roundguard_mulN  : std_ulogic;
   signal expout_mulN      : signed(exp_w DOWNTO 0);
   signal fracout_mulN     : unsigned(frac_w+1+guard_bits DOWNTO 0);
   signal res_newton       : float(exp_w DOWNTO -frac_w);

   signal sign_mulN1       : std_ulogic;
   signal roundguard_mulN1 : std_ulogic;
   signal expout_mulN1     : signed(exp_w DOWNTO 0);
   signal fracout_mulN1    : unsigned(frac_w+1+guard_bits DOWNTO 0);
   signal res_newton1      : float(exp_w DOWNTO -frac_w);
   
   signal sign_subN        : std_ulogic;
   signal roundguard_subN  : std_ulogic;
   signal expout_subN      : signed(exp_w DOWNTO 0);
   signal fracout_subN     : unsigned(frac_w+1+guard_bits DOWNTO 0);
   signal res_newton2      : float(exp_w DOWNTO -frac_w);
   signal negatio          : float(exp_w DOWNTO -frac_w);
  
   signal type_conv        : std_logic_vector(dataw-1 downto 0); 

   signal sign_mulN2       : std_ulogic;
   signal roundguard_mulN2 : std_ulogic;
   signal expout_mulN2     : signed(exp_w DOWNTO 0);
   signal fracout_mulN2    : unsigned(frac_w+1+guard_bits DOWNTO 0);
   signal res_final        : float(exp_w DOWNTO -frac_w);
   
   -- Second Newton's iteration
   signal sign_mulN3       : std_ulogic;
   signal roundguard_mulN3 : std_ulogic;
   signal expout_mulN3     : signed(exp_w DOWNTO 0);
   signal fracout_mulN3    : unsigned(frac_w+1+guard_bits DOWNTO 0);
   signal res_mul_Newton2  : float(exp_w DOWNTO -frac_w);
   
   signal sign_mulN4       : std_ulogic;
   signal roundguard_mulN4 : std_ulogic;
   signal expout_mulN4     : signed(exp_w DOWNTO 0);
   signal fracout_mulN4    : unsigned(frac_w+1+guard_bits DOWNTO 0);
   signal res_newton4      : float(exp_w DOWNTO -frac_w);

   signal type_convN2      : std_logic_vector(dataw-1 downto 0);
   signal negatioN2        : float(exp_w DOWNTO -frac_w);
   
   signal sign_subN2       : std_ulogic;
   signal roundguard_subN2 : std_ulogic;
   signal expout_subN2     : signed(exp_w DOWNTO 0);
   signal fracout_subN2    : unsigned(frac_w+1+guard_bits DOWNTO 0);

   signal sign_mulN5       : std_ulogic;
   signal roundguard_mulN5 : std_ulogic;
   signal expout_mulN5     : signed(exp_w DOWNTO 0);
   signal fracout_mulN5    : unsigned(frac_w+1+guard_bits DOWNTO 0);
   signal res_newton2_sub  : float(exp_w DOWNTO -frac_w);
   
   signal res_final2       : float(exp_w DOWNTO -frac_w);

begin
  
   ----------------------------
   --     Initial guess      --
   ----------------------------
   --initial_guess: process(sum, half, sign_mul, roundguard_mul, expout_mul, fracout_mul) is
   
   
   
   --begin
   
   -- Computes x2
   G1: fpmultiplier_block port map (sum, half, sign_mul, roundguard_mul, expout_mul, fracout_mul);       
   G2: normalization port map (sign_mul, roundguard_mul, expout_mul, fracout_mul, res_out_norm);       
	  
   -- shift original vector element sum
   a_int <= to_slv('0' & sum(exp_w downto -frac_w+1));
	
   -- subtract magic number and shifted sum
   G3: sub_arith port map (a_int, magic, result_sub);
	
   -- convert result to float
   res_float <= to_float(result_sub);
	 
	------------------------------
   -- First Newton's iteration --
   ------------------------------
   -- y*y
   G4: fpmultiplier_block port map (res_float, res_float, sign_mulN, roundguard_mulN, expout_mulN, fracout_mulN);       
   G5: normalization port map (sign_mulN, roundguard_mulN, expout_mulN, fracout_mulN, res_newton);
   -- x2*(y*y)
   G6: fpmultiplier_block port map (res_newton, res_out_norm, sign_mulN1, roundguard_mulN1, expout_mulN1, fracout_mulN1);       
   G7: normalization port map (sign_mulN1, roundguard_mulN1, expout_mulN1, fracout_mulN1, res_newton1);
	
   type_conv <= to_slv(res_newton1);
   negatio <= to_float((NOT type_conv(type_conv'high))&type_conv(type_conv'high-1 DOWNTO 0));
   -- threehalfs - (x2 * y * y)
   G8: fpadd_block port map (negatio, threehalfs, sign_subN, roundguard_subN, expout_subN, fracout_subN);       
   G9: normalization port map (sign_subN, roundguard_subN, expout_subN, fracout_subN, res_newton2);
   -- y = y * threehalfs - (x2 * y * y)
   G10: fpmultiplier_block port map (res_newton2, res_float, sign_mulN2, roundguard_mulN2, expout_mulN2, fracout_mulN2);       
   G11: normalization port map (sign_mulN2, roundguard_mulN2, expout_mulN2, fracout_mulN2, res_final);
	
   inv_sqrt <= res_final; --uncomment if the second Newton's iteration not used
   
   -- Second Newton's iteration
   -- y*y kommentti
   --G12: fpmultiplier_block port map (res_final, res_final, sign_mulN3, roundguard_mulN3, expout_mulN3, fracout_mulN3);    
   --G13: normalization port map (sign_mulN3, roundguard_mulN3, expout_mulN3, fracout_mulN3, res_mul_Newton2);
   -- x2*(y*y) kommentti
   --G14: fpmultiplier_block port map (res_mul_Newton2, res_out_norm, sign_mulN4, roundguard_mulN4, expout_mulN4, fracout_mulN4);       
   --G15: normalization port map (sign_mulN4, roundguard_mulN4, expout_mulN4, fracout_mulN4, res_newton4);
   
   --type_convN2 <= to_slv(res_newton4);
   --negatioN2 <= to_float((NOT type_convN2(type_convN2'high))&type_convN2(type_convN2'high-1 DOWNTO 0));
   --threehalfs - (x2 * y * y) kommentti 
   
   --G16: fpadd_block port map (negatioN2, threehalfs, sign_subN2, roundguard_subN2, expout_subN2, fracout_subN2);       
   --G17: normalization port map (sign_subN2, roundguard_subN2, expout_subN2, fracout_subN2, res_newton2_sub);
   -- y = y * threehalfs - (x2 * y * y) kommentti
   --G18: fpmultiplier_block port map (res_newton2_sub, res_final, sign_mulN5, roundguard_mulN5, expout_mulN5, fracout_mulN5);       
   --G19: normalization port map (sign_mulN5, roundguard_mulN5, expout_mulN5, fracout_mulN5, res_final2);
	
   
   --inv_sqrt <= res_final2;
   
end beh;



---------------------------------------
--             FU_INVSQRT            --
---------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;
LIBRARY WORK;
USE WORK.float_pkg_tce.ALL;


entity fp_invsqrt is

generic(
    busw     : integer := 16;
    dataw    : integer := 16;
    exp_w    : integer := 5;
    frac_w   : integer := 10 
);

port(
    t1data  : in std_logic_vector(dataw-1 downto 0);
    t1load  : in std_logic;
    r0data  : out std_logic_vector(busw-1 downto 0);  
    clk     : in std_logic;
    rstx    : in std_logic;
    glock   : in std_logic
    ); 
end fp_invsqrt;

architecture rtl_invsqrt of fp_invsqrt is

   component invsqrt 
      port(
         sum       :in  float(exp_w downto -frac_w); 
         inv_sqrt  :out float(exp_w downto -frac_w) 
      );  
   end component;
 
   signal t1reg           : float(exp_w downto -frac_w);
   signal r0reg           : float(exp_w downto -frac_w);
   signal control         : std_logic;
   signal result_en_reg   : std_logic; 
   
   -- Truncated t1data, o1data, r1data with shorter word lengths 
   signal t1trun : std_logic_vector(exp_w + frac_w downto 0);
   --signal o1trun : std_logic_vector(exp_w + frac_w downto 0);
   signal r1trun : std_logic_vector(exp_w + frac_w downto 0);

begin 
  
  fu_arch : invsqrt
     port map(
       sum => t1reg,
       inv_sqrt => r0reg
     );
    
  control <= t1load;
  
  -- Must use internally smaller word length
  t1trun <= t1data(exp_w + frac_w downto 0);
  --o1trun <= o1data(exp_w + frac_w downto 0);
  r0data(exp_w + frac_w downto 0) <= r1trun;
  r0data(busw-1 downto exp_w + frac_w + 1) <= (others => '0');
  
  regs: process(clk, rstx)
  begin 
     if(rstx = '0') then  -- asynchronous reset (active low)
        t1reg <= (others => '0');
     elsif(clk'event and clk = '1') then -- rising clock edge
        if(glock = '0') then 
           if(control = '1') then  
              t1reg <= to_float(t1data(exp_w+frac_w downto 0));
           end if;
        
        
         -- Update the output results only when updated
         r1trun <= to_slv(r0reg);
          
        end if; --glock 
     end if;
  end process regs; 
  
end rtl_invsqrt;   


