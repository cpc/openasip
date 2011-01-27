-----------------------------------------------------------------------------------------------------------
--
--				           SINGLE PRECISION FP NUMBERS SQRT EXTRACTION
--
-- Created by Claudio Brunelli, 2003
--
-----------------------------------------------------------------------------------------------------------
-- The chosen (default) rounding policy is "Round to nearest even" as specified by 
-- IEEE Std 754 (1985)

--Copyright (c) 2004, Tampere University of Technology.
--All rights reserved.

--Redistribution and use in source and binary forms, with or without modification,
--are permitted provided that the following conditions are met:
--*	Redistributions of source code must retain the above copyright notice,
--	this list of conditions and the following disclaimer.
--*	Redistributions in binary form must reproduce the above copyright notice,
--	this list of conditions and the following disclaimer in the documentation
--	and/or other materials provided with the distribution.
--*	Neither the name of Tampere University of Technology nor the names of its
--	contributors may be used to endorse or promote products derived from this
--	software without specific prior written permission.

--THIS HARDWARE DESCRIPTION OR SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
--CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
--LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND NONINFRINGEMENT AND
--FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
--OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
--EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
--PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
--BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
--CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
--ARISING IN ANY WAY OUT OF THE USE OF THIS HARDWARE DESCRIPTION OR SOFTWARE, EVEN
--IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use work.cop_definitions.all;
use work.cop_components.all;

entity sp_fsqrt is
   port( clk,reset,enable           : in  std_logic;
         radicand                   : in std_logic_vector(word_width-1 downto 0);
         sqrt_result                : out std_logic_vector(word_width-1 downto 0);
         exc_inexact_sqrt           : out std_logic;
         exc_invalid_operation_sqrt : out std_logic );
end sp_fsqrt ;

-------------------------------------------------------------------------------
architecture rtl of sp_fsqrt is

    component normalizer
        port( denormal_operand    : in  std_logic_vector(word_width-2 downto 0);
              normalized_mantissa : out std_logic_vector(23 downto 0);
              normalized_exponent : out std_logic_vector(8 downto 0)
            );   
    end component;

    component integer_sqrt
        port( clk        : in std_logic;
              reset      : in std_logic;
              input      : in std_logic_vector(radicand_width-1 downto 0);
              output     : out std_logic_vector(sqrt_width-1 downto 0);
              remainder  : out std_logic_vector(rem_width-1 downto 0) );
    end component;
    

    signal delayed_radicand       : std_logic_vector(word_width-1 downto 0);
    signal xa                     : std_logic_vector(8 downto 0);
    signal pipelined_xa           : std_logic_vector(8 downto 0);
    signal temp_m_radicand        : std_logic_vector(23 downto 0);
    signal m_radicand             : std_logic_vector(radicand_width-2 downto 0);
    signal pipelined_m_radicand   : std_logic_vector(radicand_width-2 downto 0);
    signal extended_m             : std_logic_vector(radicand_width-1 downto 0);
    signal halved_exp             : std_logic_vector(7 downto 0);  
    signal delayed_halved_exp     : std_logic_vector(7 downto 0);
    signal sqrooted_m             : std_logic_vector(sqrt_width-1 downto 0);
    signal pipelined_sqrooted_m   : std_logic_vector(sqrt_width-1 downto 0);
    signal int_sqrt_rem           : std_logic_vector(rem_width-1 downto 0);
    signal pipelined_int_sqrt_rem : std_logic_vector(rem_width-1 downto 0);
    signal sqrt                   : std_logic_vector(word_width-1 downto 0);
    signal rounded_sqrt           : std_logic_vector(24 downto 0);
    signal sgl_inexact_sqrt       : std_logic;
    signal unsigned_zero          : std_logic_vector(word_width-2 downto 0);
    signal infinity               : std_logic_vector(word_width-2 downto 0); 
    signal exp                    : std_logic_vector(7 downto 0);
    signal m                      : std_logic_vector(23 downto 0);
    signal s                      : std_logic;
    signal x                      : std_logic_vector(7 downto 0);
    signal f                      : std_logic_vector(22 downto 0);
    signal rounding_inexact       : std_logic;
    signal result_inexact         : std_logic;
    signal fixed_enable           : std_logic;

    
   
    begin

-----------------------------------------------------------------------------------------------------------------          

        unsigned_zero <= conv_std_logic_vector(0,31);
        infinity <= "1111111100000000000000000000000";  -- The sign bit is specified elsewhere
        fixed_enable <= '1';

-----------------
-- INPUT STAGE
-----------------

-- sign determination

        s <= '0';  -- every valid result has a positive sign, except that for the case in which radicand is "minus zero"

-- operands unpacking; conversion into "internal format"

        RADICAND_NORMALIZATION: normalizer 
                                port map ( denormal_operand => radicand(30 downto 0), normalized_mantissa => temp_m_radicand, normalized_exponent => xa);

        m_radicand <= (temp_m_radicand & conv_std_logic_vector(0,27) );

        ----------------------------------------------------------

        PIPELINE_REG_CHAIN_radicand: simple_register_chain generic map (length => sqrt_clk_cycles-1, width => word_width)
                                                                    port map (clk => clk, reset => reset, enable => fixed_enable, reg_chain_in => radicand, reg_chain_out => delayed_radicand);


-- special cases handling

        SPECIAL_OP_DETECTION: process(delayed_radicand, sqrt, unsigned_zero, infinity, sgl_inexact_sqrt)  
        begin

            if ( delayed_radicand(30 downto 0)=unsigned_zero ) then 
                        -- radicand is null; result is equal to the operand, because sqrt(-0)= -0 and sqrt(+0)= +0
                        sqrt_result <= delayed_radicand;
                        exc_inexact_sqrt <= '0'; exc_invalid_operation_sqrt <= '0';

            elsif ( (delayed_radicand(31)='0') and (delayed_radicand(30 downto 0)=infinity) ) then
                        -- radicand is a positive infinity
                        sqrt_result <= ('0' & infinity);
                        exc_inexact_sqrt <= '0'; exc_invalid_operation_sqrt <= '0';

            elsif ( (delayed_radicand(30 downto 23)="11111111") and (delayed_radicand(22)='0') and (delayed_radicand(21 downto 0)/=conv_std_logic_vector(0,22)) ) then
                        -- radicand is a signaling NaN -> invalid operation exception is raised; output is a QNaN
                        sqrt_result <= "01111111110000000000000000000001";  -- QNaN, if trap disabled (default)
                        exc_inexact_sqrt <= '0'; exc_invalid_operation_sqrt <= '1';

            elsif ( delayed_radicand(30 downto 22)="111111111" ) then
                        -- radicand is a quiet NaN; output is the input NaN
                        sqrt_result <= delayed_radicand;  -- QNaN, if trap disabled (default)
                        exc_inexact_sqrt <= '0'; exc_invalid_operation_sqrt <= '0';

            elsif ( delayed_radicand(31)='1' ) then 
                        -- radicand has negative sign   
                        sqrt_result <= delayed_radicand;  -- my choiche!
                        exc_inexact_sqrt <= '0'; exc_invalid_operation_sqrt <= '1';

            else
                        -- default
                        sqrt_result <= sqrt;
                        exc_inexact_sqrt <= sgl_inexact_sqrt; exc_invalid_operation_sqrt <= '0';
            end if;

        end process;

----------------------------------         
-- EXTENDED SQRT CALCULATION
----------------------------------

        PIPELINE_REG_xa: data_register generic map (reg_width => 9)
                                                 port map (clk => clk, reset => reset, data_in => xa, data_out => pipelined_xa);

        PIPELINE_REG_M_RADICAND: data_register generic map (reg_width => radicand_width-1)
                                               port map (clk => clk, reset => reset, data_in => m_radicand, data_out => pipelined_m_radicand);


-- exponent adjusting 

        process(pipelined_xa,pipelined_m_radicand)
        begin
            if ( pipelined_xa(0) = '0' ) then
                    -- biased exponent is even, but actual exponent is odd, then is made even: [exp <- (exp-1)]  thus mantissa is shifted left by one position 
                    extended_m <= ( pipelined_m_radicand & '0' );
                    halved_exp <= ( unsigned(pipelined_xa(8 downto 1)) - 1 );
            else
                    -- biased exponent is odd, but actual exponent is even: the mantissa is not shifted
                    extended_m <= ( '0' & pipelined_m_radicand );
                    halved_exp <= pipelined_xa(8 downto 1);
            end if;
        end process;

        -- there's no need to explicitly sutract a 1 from odd exponent: both even and odd ("even + 1" exponent)
        -- exponents are lead to the same number once halved (SHR by one position):

        
        ----------------------------------------------------------
 
-- extraction of the square root of the mantissa

        INTEGER_SQUARE_ROOT: integer_sqrt
            port map (clk => clk, reset => reset, input => extended_m, output => sqrooted_m, remainder => int_sqrt_rem);


-- note that due to the particular layout of extended_m, sqrooted_m is always normalized!


-----------------------------
-- RESULT GENERATION STAGE 
-----------------------------

        HALVED_EXP_PIPELINE: simple_register_chain
                               generic map (length => 6, width => 8)
                               port map ( clk => clk, reset => reset, enable => fixed_enable, reg_chain_in => halved_exp, reg_chain_out => delayed_halved_exp);

        PIPELINE_SQROOTED_M: data_register generic map (reg_width => sqrt_width)
                                               port map (clk => clk, reset => reset, data_in => sqrooted_m, data_out => pipelined_sqrooted_m);

        PIPELINE_INT_SQRT_REM: data_register generic map (reg_width => rem_width)
                                               port map (clk => clk, reset => reset, data_in => int_sqrt_rem, data_out => pipelined_int_sqrt_rem);


-- special result detection and result packing

    SQRT_RESULT_GEN: process(delayed_halved_exp,pipelined_sqrooted_m,pipelined_int_sqrt_rem,rounded_sqrt,exp,m)
        variable c : integer;
        begin  
              
----------------------------------------------------------------------------

-- NO special cases
-- result is  always converted into a single precision normalized numbers:

-- rounding

        if ( (conv_integer(unsigned(delayed_halved_exp)) < 128 + 127) and (conv_integer(unsigned(delayed_halved_exp)) > -127 + 127) ) then  
                c := Conv_integer(  unsigned(pipelined_sqrooted_m(25 downto 2))  );
                if ( (pipelined_sqrooted_m(1 downto 0) = "00") and (pipelined_int_sqrt_rem=(conv_std_logic_vector(0,28))) ) then
                        -- only one exact case!
                        rounded_sqrt <= Conv_std_logic_vector(c,25);
                        rounding_inexact <= '0';
                elsif ( (pipelined_sqrooted_m(1) = '0') or (pipelined_sqrooted_m(2) = '0' and pipelined_sqrooted_m(1) = '1' and pipelined_sqrooted_m(0) = '0' and (pipelined_int_sqrt_rem=(conv_std_logic_vector(0,28)))) ) then
	                  rounded_sqrt<= Conv_std_logic_vector(c,25);
                        rounding_inexact <= '1';
                else 									                   
                        rounded_sqrt<= Conv_std_logic_vector(c+1,25);
                        rounding_inexact <= '1';
                end if;
 
                ----------------------------------------------------------

-- If the rounding makes the mantissa to overflow, then it has to be re-adjusted

                if rounded_sqrt(24) = '1' then
                        -- overflow => SHR(rounded_sqrt), exp <- (exp + 1)
                        m <= rounded_sqrt(24 downto 1);
                        exp <= ( unsigned(delayed_halved_exp) + 1 );
                else
                        -- no overflow
                        m <= rounded_sqrt(23 downto 0);
                        exp <= delayed_halved_exp;
                end if;

                ----------------------------------------------------------
                   
-- normalized result packing 

                x <= exp;
                f <= m(22 downto 0);
                result_inexact <= '0';        

                ----------------------------------------------------------

        else

-- invalid operand; NaN is generated

                rounded_sqrt <= (others => '0');
                exp <= (others => '0');
                m <= (others => '0');
                x <= "11111111";
                f <= "00000000000000000000001";
                result_inexact <= '0'; rounding_inexact <= '0';

        end if;

    end process;   
 
-- All "inexact result warning" internal signals are put in logical OR:

        sgl_inexact_sqrt <= ( result_inexact or rounding_inexact );

-- result packing:

        sqrt <= ( s & x & f);

-----------------------------------------------------------------------------------------------------------------       

end rtl;

