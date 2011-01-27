-----------------------------------------------------------------------------------------------------------
--
--				           SINGLE PRECISION FP NUMBERS DIVIDER
--
-- Created by Claudio Brunelli, 2004
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

entity sp_fdivider is
    port( clk,reset,enable            : in  std_logic;
          dividend,divisor            : in  std_logic_vector(word_width-1 downto 0);
          div_result                  : out std_logic_vector(word_width-1 downto 0);
          exc_overflow_div            : out std_logic;  
          exc_underflow_div           : out std_logic;  
          exc_inexact_div             : out std_logic;
          exc_invalid_operation_div   : out std_logic;
          exc_division_by_zero        : out std_logic ); 
end sp_fdivider ;

-------------------------------------------------------------------------------
architecture rtl of sp_fdivider is

    component normalizer
        port( denormal_operand    : in  std_logic_vector(word_width-2 downto 0);
              normalized_mantissa : out std_logic_vector(23 downto 0);
              normalized_exponent : out std_logic_vector(8 downto 0)
            );   
    end component;

    component div_packer
    port( internal_exponent      : in std_logic_vector(9 downto 0);
          internal_mantissa      : in std_logic_vector(quotient_width-1 downto 0);
          internal_remainder     : in std_logic_vector(division_rem_width downto 0);
          packed_exponent        : out std_logic_vector(7 downto 0);
          packed_fractional_part : out std_logic_vector(22 downto 0);
          exc_overflow           : out std_logic;
          exc_underflow          : out std_logic;
          exc_inexact            : out std_logic
        ); 
    end component;

    component integer_divider
        port( clk   : in std_logic;
              reset : in std_logic;
              dvd   : in std_logic_vector(dividend_width-1 downto 0);
              dvs   : in std_logic_vector(divisor_width-1 downto 0);
              quot  : out std_logic_vector(quotient_width-1 downto 0);
              rest  : out std_logic_vector(division_rem_width-1 downto 0) );
    end component;
    
    signal pipelined_dividend,
           pipelined_divisor        : std_logic_vector(word_width-1 downto 0);
    signal delayed_dividend,
           delayed_divisor          : std_logic_vector(word_width-1 downto 0);
    signal xa, xb                   : std_logic_vector(8 downto 0);
    signal pipelined_xa,
           pipelined_xb             : std_logic_vector(8 downto 0);
    signal ma, mb                   : std_logic_vector(23 downto 0);
    signal pipelined_ma,
           pipelined_mb,
           delayed_mb               : std_logic_vector(23 downto 0);
    signal sub                      : integer RANGE 233 TO 789;
    signal div                      : std_logic_vector(quotient_width-1 downto 0);
    signal pipelined_div            : std_logic_vector(quotient_width-1 downto 0);
    signal extended_sub,
           delayed_extended_sub     : std_logic_vector(9 downto 0);
    signal normalized_sub           : std_logic_vector(9 downto 0);
    signal pipelined_normalized_sub : std_logic_vector(9 downto 0);
    signal normalized_div           : std_logic_vector(quotient_width-1 downto 0);
    signal pipelined_normalized_div : std_logic_vector(quotient_width-1 downto 0);
    signal normalized_remainder,
           pipelined_normalized_remainder : std_logic_vector(division_rem_width downto 0);
    signal quotient                 : std_logic_vector(word_width-1 downto 0);
    signal remainder                : std_logic_vector(division_rem_width-1 downto 0);
    signal pipelined_remainder      : std_logic_vector(division_rem_width-1 downto 0);
    signal unsigned_zero            : std_logic_vector(word_width-2 downto 0);
    signal infinity                 : std_logic_vector(word_width-2 downto 0); 
    signal s, pipelined_s,
           delayed_s                : std_logic;
    signal sgl_overflow_div,
           sgl_underflow_div,
           sgl_inexact_div          : std_logic;
    signal sgl_packed_exponent      : std_logic_vector(7 downto 0);
    signal sgl_packed_fractional_part : std_logic_vector(22 downto 0);
    signal fixed_enable             : std_logic;

    
    begin

-----------------------------------------------------------------------------------------------------------------          

        unsigned_zero <= conv_std_logic_vector(0,31);
        infinity <= "1111111100000000000000000000000";  -- The sign bit is specified elsewhere
        fixed_enable <= '1';

-----------------
-- INPUT STAGE
-----------------

-- operands unpacking; conversion into "internal format"

        ADD_DIVIDEND_NORMALIZATION: normalizer 
                                    port map ( denormal_operand => dividend(30 downto 0), normalized_mantissa => ma, normalized_exponent => xa);

        ADD_DIVISOR_NORMALIZATION: normalizer 
                                   port map ( denormal_operand => divisor(30 downto 0), normalized_mantissa => mb, normalized_exponent => xb);

-- sign determination

        s <= ( dividend(31) xor divisor(31) );       

        FIRST_FF_SIGN: wedff port map (clk => clk, reset => reset, we => enable, d => s, q => pipelined_s);
        PIPELINE_FF_CHAIN_SIGN: ff_chain generic map (length => div_clk_cycles-2)
                                                           port map (clk => clk, reset => reset, enable => fixed_enable, ff_chain_in => pipelined_s, ff_chain_out => delayed_s);

        ----------------------------------------------------------


        FIRST_PIPELINE_REG_CHAIN_DIVIDEND: we_register generic map (reg_width => word_width)
                                                       port map (clk => clk, reset => reset, we => enable, data_in => dividend, data_out => pipelined_dividend);
        PIPELINE_REG_CHAIN_DIVIDEND: simple_register_chain generic map (length => div_clk_cycles-2, width => word_width)
                                                           port map (clk => clk, reset => reset, enable => fixed_enable, reg_chain_in => pipelined_dividend, reg_chain_out => delayed_dividend);

        FIRST_PIPELINE_REG_CHAIN_DIVISOR: we_register generic map (reg_width => word_width)
                                                       port map (clk => clk, reset => reset, we => enable, data_in => divisor, data_out => pipelined_divisor);
        PIPELINE_REG_CHAIN_DIVISOR: simple_register_chain generic map (length => div_clk_cycles-2, width => word_width)
                                                          port map (clk => clk, reset => reset, enable => fixed_enable, reg_chain_in => pipelined_divisor, reg_chain_out => delayed_divisor);



-- special cases handling

        SPECIAL_OP_DETECTION: process(delayed_dividend,delayed_divisor,delayed_s,quotient,unsigned_zero,infinity,sgl_overflow_div,sgl_underflow_div,sgl_inexact_div)  
        begin
             
    -- (dividend is zero)      
            if ( (delayed_dividend(30 downto 0)=unsigned_zero) and (delayed_divisor(30 downto 23)/="11111111") and (delayed_divisor(30 downto 0)/=unsigned_zero) ) then
                    -- dividend is zero and divisor is not infinity nor a NaN nor zero    
                    div_result <= ( delayed_s & unsigned_zero );
                    exc_overflow_div <= '0'; exc_underflow_div <= '0'; exc_inexact_div <= '0';
                    exc_invalid_operation_div <= '0'; exc_division_by_zero <= '0';   

            elsif ( (delayed_dividend(30 downto 0)=unsigned_zero) and (delayed_divisor(30 downto 0)=infinity) ) then
                    -- dividend is zero and divisor is infinity
                    div_result <= ( delayed_s & unsigned_zero);
                    exc_overflow_div <= '0'; exc_underflow_div <= '0'; exc_inexact_div <= '0';
                    exc_invalid_operation_div <= '0'; exc_division_by_zero <= '0';

            elsif ( (delayed_dividend(30 downto 0)=unsigned_zero) and (delayed_divisor(30 downto 0)=unsigned_zero) ) then
                    -- zero/zero: invalid operation!
                    div_result <= "01111111110000000000000000000001";  -- QNaN, if trap disabled (default)
                    exc_overflow_div <= '0'; exc_underflow_div <= '0'; exc_inexact_div <= '0';
                    exc_invalid_operation_div <= '1'; exc_division_by_zero <= '0';

    -- removed cases where divisor/dividend is inf or nan, or divisor is zero

            else

    -- DEFAULT
                    div_result <= quotient;
                    exc_overflow_div <= sgl_overflow_div; exc_underflow_div <= sgl_underflow_div; exc_inexact_div <= sgl_inexact_div;
                    exc_invalid_operation_div <= '0'; exc_division_by_zero <= '0';
            end if;

        end process;

----------------------------------         
-- EXTENDED QUOTIENT CALCULATION
----------------------------------

        PIPELINE_REG_XA: we_register generic map (reg_width => 9)
                                       port map (clk => clk, reset => reset, we => enable, data_in => xa, data_out => pipelined_xa);
        PIPELINE_REG_XB: we_register generic map (reg_width => 9)
                                       port map (clk => clk, reset => reset, we => enable, data_in => xb, data_out => pipelined_xb);
        PIPELINE_REG_MA: we_register generic map (reg_width => 24)
                                       port map (clk => clk, reset => reset, we => enable, data_in => ma, data_out => pipelined_ma);
        PIPELINE_REG_MB: we_register generic map (reg_width => 24)
                                       port map (clk => clk, reset => reset, we => enable, data_in => mb, data_out => pipelined_mb);


-- fractional part division

        INTEGER_DIV: integer_divider
            port map (clk => clk, reset => reset, dvd  => pipelined_ma, dvs  => pipelined_mb, quot => div, rest => remainder); 

        ----------------------------------------------------------
 
-- exponent subtraction
-- Note: one bias has to be restored!    
   
        sub <= ( conv_integer(unsigned(pipelined_xa)) - conv_integer(unsigned(pipelined_xb)) + 511 );        
        extended_sub <= conv_std_logic_vector( sub, 10 );                             



        PIPELINE_REG_DIV: data_register generic map (reg_width => quotient_width)
                                        port map (clk => clk, reset => reset, data_in => div, data_out => pipelined_div);
        PIPELINE_REG_REMAINDER: data_register generic map (reg_width => division_rem_width)
                                              port map (clk => clk, reset => reset, data_in => remainder, data_out => pipelined_remainder);


        PIPELINE_CHAIN_SUB: simple_register_chain generic map (length => 9, width => 10)
                                                 port map (clk => clk, reset => reset, enable => fixed_enable, reg_chain_in => extended_sub, reg_chain_out => delayed_extended_sub);
        PIPELINE_CHAIN_MB: simple_register_chain generic map (length => 9, width => 24)
                                                 port map (clk => clk, reset => reset, enable => fixed_enable, reg_chain_in => pipelined_mb, reg_chain_out => delayed_mb);

------------------------
-- NORMALIZATION STAGE 
------------------------

-- partial result normalization
-- Note: Exponent adjustment is required following possible "SHL" of div: the exponent should be
--       decremented by one if div(24)='0', and remains untouched whenever div(24) is the 
--       leading bit.

        process(pipelined_div, delayed_extended_sub, pipelined_remainder, delayed_mb) 
        begin
            if pipelined_div(24) = '0' then
                    -- div(23) is the leading bit => SHL(div), exp <- (exp - 1)
                    normalized_sub <= conv_std_logic_vector( (unsigned(delayed_extended_sub)-1),10 );
                    if ( unsigned(pipelined_remainder) < unsigned(delayed_mb) ) then                   -- 2*(r(i-1)) < D => q(i)=0
                            normalized_div <= ( pipelined_div(23 downto 0) & '0' );
                            normalized_remainder <= (pipelined_remainder&'0');
                    else
                            normalized_div <= ( pipelined_div(23 downto 0) & '1' );
                            normalized_remainder <= ( unsigned(pipelined_remainder & '0') - unsigned(delayed_mb & '0') );
                    end if;
            else 
                    -- div(24) is the leading bit
                    normalized_sub <= conv_std_logic_vector( (unsigned(delayed_extended_sub)),10 );
                    normalized_div<= pipelined_div(24 downto 0);
                    normalized_remainder <= ('0' & pipelined_remainder);                     
            end if;
        end process;     
       

        PIPELINE_REG_NORMALIZED_DIV: data_register generic map (reg_width => quotient_width)
                                                   port map (clk => clk, reset => reset, data_in => normalized_div, data_out => pipelined_normalized_div);
        PIPELINE_REG_NORMALIZED_REMAINDER: data_register generic map (reg_width => division_rem_width+1)
                                                         port map (clk => clk, reset => reset, data_in => normalized_remainder, data_out => pipelined_normalized_remainder);
        PIPELINE_REG_NORMALIZED_SUB: data_register generic map (reg_width => 10)
                                                   port map (clk => clk, reset => reset, data_in => normalized_sub, data_out => pipelined_normalized_sub);

-----------------------------
-- RESULT GENERATION STAGE 
-----------------------------

        RESULT_PACKING_LOGIC: div_packer 
                              port map (internal_exponent => pipelined_normalized_sub, internal_mantissa => pipelined_normalized_div, internal_remainder => pipelined_normalized_remainder,
                                        packed_exponent => sgl_packed_exponent, packed_fractional_part => sgl_packed_fractional_part,
                                        exc_overflow => sgl_overflow_div, exc_underflow => sgl_underflow_div, exc_inexact => sgl_inexact_div);

-- result packing:

        quotient <= ( delayed_s & sgl_packed_exponent & sgl_packed_fractional_part);

-----------------------------------------------------------------------------------------------------------------       

end rtl;
