-----------------------------------------------------------------------------------------------------------
--
--				           SINGLE PRECISION FP NUMBERS MULTIPLIER
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

entity sp_fmultiplier is
    port( clk,reset,enable            : in  std_logic;
          multiplicand,multiplicator  : in  std_logic_vector(word_width-1 downto 0);
          mul_result                  : out std_logic_vector(word_width-1 downto 0);
          exc_overflow_mul            : out std_logic;  
          exc_underflow_mul           : out std_logic;  
          exc_inexact_mul             : out std_logic;
          exc_invalid_operation_mul   : out std_logic ); 
end sp_fmultiplier ;

-------------------------------------------------------------------------------
architecture rtl of sp_fmultiplier is

    component normalizer
        port( denormal_operand    : in  std_logic_vector(word_width-2 downto 0);
              normalized_mantissa : out std_logic_vector(23 downto 0);
              normalized_exponent : out std_logic_vector(8 downto 0)
            );   
    end component;

    component mul_packer
        port( internal_exponent      : in std_logic_vector(9 downto 0);
              internal_mantissa      : in std_logic_vector(47 downto 0);
              packed_exponent        : out std_logic_vector(7 downto 0);
              packed_fractional_part : out std_logic_vector(22 downto 0);
              exc_overflow           : out std_logic;
              exc_underflow          : out std_logic;
              exc_inexact            : out std_logic
        );
    end component;
    

    signal pipelined_multiplicand,
           pipelined_multiplicator         : std_logic_vector(word_width-1 downto 0);
    signal delayed_multiplicand,
           delayed_multiplicator           : std_logic_vector(word_width-1 downto 0);
    signal xa, xb                          : std_logic_vector(8 downto 0);
    signal pipelined_xa,
           pipelined_xb                    : std_logic_vector(8 downto 0);
    signal ma, mb                          : std_logic_vector(23 downto 0);
    signal pipelined_ma,
           pipelined_mb                    : std_logic_vector(23 downto 0);
    signal sum                             : integer;
    signal extended_sum,
           pipelined_extended_sum          : std_logic_vector(9 downto 0);
    signal extended_prod,
           pipelined_extended_prod         : std_logic_vector(47 downto 0);
    signal normalized_sum                  : std_logic_vector(9 downto 0);
    signal normalized_prod                 : std_logic_vector(47 downto 0);
    signal prod                            : std_logic_vector(word_width-1 downto 0);
    signal unsigned_zero                   : std_logic_vector(word_width-2 downto 0);
    signal infinity                        : std_logic_vector(word_width-2 downto 0); 
    signal s, pipelined_s, delayed_s       : std_logic;
    signal sgl_overflow_mul, 
           sgl_underflow_mul,
           sgl_inexact_mul                 : std_logic;
    signal sgl_packed_exponent             : std_logic_vector(7 downto 0);
    signal sgl_packed_fractional_part      : std_logic_vector(22 downto 0);

    signal fixed_enable                    : std_logic;
   
    begin

-----------------------------------------------------------------------------------------------------------------          

        unsigned_zero <= conv_std_logic_vector(0,31);
        infinity <= "1111111100000000000000000000000";  -- The sign bit is specified elsewhere
        fixed_enable <= '1';

-----------------
-- INPUT STAGE
-----------------

-- sign determination

        s <= ( multiplicand(31) xor multiplicator(31) );    

        FIRST_FF_S: wedff port map (clk => clk, reset => reset, we => enable, d => s, q => pipelined_s);
        PIPELINE_CHAIN_FF_S: ff_chain
                             generic map (length => mul_clk_cycles-2)
                             port map ( clk => clk, reset => reset, enable => fixed_enable, ff_chain_in => pipelined_s, ff_chain_out => delayed_s);  
        

-- operands unpacking

        ADD_MULTIPLICAND_NORMALIZATION: normalizer 
                                        port map ( denormal_operand => multiplicand(30 downto 0), normalized_mantissa => ma, normalized_exponent => xa);

        ADD_MULTIPLICATOR_NORMALIZATION: normalizer 
                                         port map ( denormal_operand => multiplicator(30 downto 0), normalized_mantissa => mb, normalized_exponent => xb);
 

        ----------------------------------------------------------


        FIRST_PIPELINE_REG_CHAIN_MULTIPLICAND: we_register generic map (reg_width => word_width)
                                                           port map (clk => clk, reset => reset, we => enable, data_in => multiplicand, data_out => pipelined_multiplicand);
        PIPELINE_REG_CHAIN_MULTIPLICAND : simple_register_chain
                                          generic map (length => mul_clk_cycles-2, width => word_width)
                                          port map ( clk => clk, reset => reset, enable => fixed_enable, reg_chain_in => pipelined_multiplicand, reg_chain_out => delayed_multiplicand);

        FIRST_PIPELINE_REG_CHAIN_MULTIPLICATOR: we_register generic map (reg_width => word_width)
                                                            port map (clk => clk, reset => reset, we => enable, data_in => multiplicator, data_out => pipelined_multiplicator);
        PIPELINE_REG_CHAIN_MULTIPLICATOR : simple_register_chain
                                           generic map (length => mul_clk_cycles-2, width => word_width)
                                           port map ( clk => clk, reset => reset, enable => fixed_enable, reg_chain_in => pipelined_multiplicator, reg_chain_out => delayed_multiplicator);


-- special cases handling

        SPECIAL_OP_DETECTION: process(delayed_multiplicand,delayed_multiplicator,delayed_s,prod,unsigned_zero,infinity,sgl_overflow_mul,sgl_underflow_mul,sgl_inexact_mul)  
        begin
                   
            if ( (delayed_multiplicand(30 downto 0)=unsigned_zero and delayed_multiplicator(30 downto 23)/="11111111") 
                or (delayed_multiplicator(30 downto 0)=unsigned_zero and delayed_multiplicand(30 downto 23)/="11111111") ) then 
                        -- at least one null operand and the other one is not infinity nor a NaN    
                        mul_result <= ( delayed_s & unsigned_zero );
                        exc_overflow_mul <= '0'; exc_underflow_mul <= '0'; exc_inexact_mul <= '0'; exc_invalid_operation_mul <= '0';

            elsif ( (delayed_multiplicand(30 downto 0)=infinity and delayed_multiplicator(30 downto 0)/=unsigned_zero and not(delayed_multiplicator(30 downto 23)="11111111" and delayed_multiplicator(30 downto 0)/=infinity)) 
                   or (delayed_multiplicator(30 downto 0)=infinity and delayed_multiplicand(30 downto 0)/=unsigned_zero and not(delayed_multiplicand(30 downto 23)="11111111" and delayed_multiplicand(30 downto 0)/=infinity)) ) then
                        -- at least one infinite operand and the other one is not zero nor a NaN
                        mul_result <= (delayed_s & infinity);
                        exc_overflow_mul <= '1'; exc_underflow_mul <= '0'; exc_inexact_mul <= '1'; exc_invalid_operation_mul <= '0';

            elsif ( (delayed_multiplicand(30 downto 0)=unsigned_zero and delayed_multiplicator(30 downto 0)=infinity)
                   or (delayed_multiplicand(30 downto 0)=infinity and delayed_multiplicator(30 downto 0)=unsigned_zero) ) then
                        -- zero*infinity: invalid operation!
                        mul_result <= "01111111110000000000000000000001";  -- QNaN, if trap disabled (default)
                        exc_overflow_mul <= '0'; exc_underflow_mul <= '0'; exc_inexact_mul <= '0'; exc_invalid_operation_mul <= '1';

            elsif ( ((delayed_multiplicand(30 downto 23)="11111111") and (delayed_multiplicand(22)='0') and (delayed_multiplicand(21 downto 0)/=conv_std_logic_vector(0,22)))
                   or ((delayed_multiplicator(30 downto 23)="11111111") and (delayed_multiplicator(22)='0') and (delayed_multiplicator(21 downto 0)/=conv_std_logic_vector(0,22))) ) then
                        -- at least one operand is a signaling NaN -> invalid operation exception is raised; output is a QNaN
                        mul_result <= "01111111110000000000000000000001";  -- QNaN, if trap disabled (default)
                        exc_overflow_mul <= '0'; exc_underflow_mul <= '0'; exc_inexact_mul <= '0'; exc_invalid_operation_mul <= '1';

            elsif ( (delayed_multiplicand(30 downto 22)="111111111") or (delayed_multiplicator(30 downto 22)="111111111") ) then
                        -- at least one operand is a quiet NaN; output is one of the input NaNs
                        if ( delayed_multiplicand(30 downto 22)="111111111" ) then
                                mul_result <= delayed_multiplicand;  -- QNaN, if trap disabled (default)
                        else
                                mul_result <= delayed_multiplicator;  -- QNaN, if trap disabled (default)
                        end if;
                        exc_overflow_mul <= '0'; exc_underflow_mul <= '0'; exc_inexact_mul <= '0'; exc_invalid_operation_mul <= '0';

            else
                        -- default
                        mul_result <= prod;
                        exc_overflow_mul <= sgl_overflow_mul; exc_underflow_mul <= sgl_underflow_mul; exc_inexact_mul <= sgl_inexact_mul; exc_invalid_operation_mul <= '0';
            end if;

        end process;

----------------------------------         
-- EXTENDED PRODUCT CALCULATION
----------------------------------

        PIPELINE_REG_XA: we_register generic map (reg_width => 9)
                                       port map (clk => clk, reset => reset, we => enable, data_in => xa, data_out => pipelined_xa);
        PIPELINE_REG_XB: we_register generic map (reg_width => 9)
                                       port map (clk => clk, reset => reset, we => enable, data_in => xb, data_out => pipelined_xb);
        PIPELINE_REG_MA: we_register generic map (reg_width => 24)
                                       port map (clk => clk, reset => reset, we => enable, data_in => ma, data_out => pipelined_ma);
        PIPELINE_REG_MB: we_register generic map (reg_width => 24)
                                       port map (clk => clk, reset => reset, we => enable, data_in => mb, data_out => pipelined_mb);


-- fractional part multiplication

        extended_prod <= ( unsigned(pipelined_ma) * unsigned(pipelined_mb) );

        ----------------------------------------------------------

-- exponent sum
-- Note: the sum is "doubly biased", so one bias has to be subtracted!    
   
        sum <= ( conv_integer(unsigned(pipelined_xa)) + conv_integer(unsigned(pipelined_xb)) - 510 + 511 );         
        extended_sum <= conv_std_logic_vector( sum, 10 ); 


        PIPELINE_REG_EXTENDED_PROD: data_register generic map (reg_width => 48)
                                                  port map (clk => clk, reset => reset, data_in => extended_prod, data_out => pipelined_extended_prod);
        PIPELINE_REG_EXTENDED_SUM: data_register generic map (reg_width => 10)
                                                 port map (clk => clk, reset => reset, data_in => extended_sum, data_out => pipelined_extended_sum);


------------------------
-- NORMALIZATION STAGE 
------------------------

-- partial result normalization
-- Note: since the operands has been implicitly multiplied by a 2^-23 factor, their product will be implicitly 
--       multiplied by a 2^46 factor to restore the correct value. Anyway, this moves the implicit fraction
--       dot between bits 46 and 45 of the 48-bit mantissas product (extended_prod) so that bits 45 downto 0 
--       are the "fractional part"; normalization process is intended to move the fraction point just after
--       bit 47 in order to make it to be the leading bit. thus the exponent has to be incremented by one.
--       Anyway, exponent adjustment is also required following possible "SHL" of extended_prod: the exponent 
--       should be decremented by one if extended_prod(47)='0', and remains untouched whenever extended_prod(47)  
--       is the leading bit.

    process(pipelined_extended_prod,pipelined_extended_sum) 
    begin
        if pipelined_extended_prod(47) = '0' then
                -- extended_prod(46) is the leading bit => SHL(extended_prod), exp <- (exp - 1).  
                -- Anyway '1' should be added following fraction dot moving, so that globally
                -- the exponent is not modified
                normalized_sum <= pipelined_extended_sum;
                normalized_prod <= ( pipelined_extended_prod(46 downto 0) & '0' );
        else 
                -- extended_prod(47) is the leading bit; exp <- (exp + 1)
                normalized_sum <= conv_std_logic_vector( (unsigned(pipelined_extended_sum)+ 1 ), 10);
                normalized_prod <= pipelined_extended_prod;                               
        end if;
    end process;            

-----------------------------
-- RESULT GENERATION STAGE 
-----------------------------

        RESULT_PACKING_LOGIC: mul_packer 
                              port map (internal_exponent => normalized_sum, internal_mantissa => normalized_prod,
                                        packed_exponent => sgl_packed_exponent, packed_fractional_part => sgl_packed_fractional_part,
                                        exc_overflow => sgl_overflow_mul, exc_underflow => sgl_underflow_mul, exc_inexact => sgl_inexact_mul);


        prod <= ( delayed_s & sgl_packed_exponent & sgl_packed_fractional_part);
 
-----------------------------------------------------------------------------------------------------------------       

end rtl;
