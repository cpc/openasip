-----------------------------------------------------------------------------------------------------------
--
--	   			           NONRESTORING INTEGER OPERANDS DIVIDER 
--
-- Created by Claudio Brunelli, 2004
--
-----------------------------------------------------------------------------------------------------------

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


entity integer_divider is
    port( clk              : in std_logic;
          reset            : in std_logic;
          dvd              : in std_logic_vector(dividend_width-1 downto 0);
          dvs              : in std_logic_vector(divisor_width-1 downto 0);
          quot             : out std_logic_vector(quotient_width-1 downto 0);
          rest             : out std_logic_vector(division_rem_width-1 downto 0) ); 
end integer_divider ;

-------------------------------------------------------------------------------
architecture rtl of integer_divider is
 
    type vector_bus is array (div_k_max downto 0) of std_logic_vector(26 downto 0);

    signal positive_q,
           turned_positive_q,
           temp_q,
           q                      : std_logic_vector(div_k_max downto 0);        
    signal r                      : vector_bus;
    signal pipelined_r1,
           pipelined_r4,
           pipelined_r7,
           pipelined_r10,
           pipelined_r13,
           pipelined_r16,
           pipelined_r19,
           pipelined_r22          : std_logic_vector(26 downto 0);
    signal temp_remainder     : std_logic_vector(26 downto 0);
    signal x, d,
           pipelined_d1,
           pipelined_d2,
           pipelined_d3,
           pipelined_d4,
           pipelined_d5,
           pipelined_d6,
           pipelined_d7,
           pipelined_d8           : std_logic_vector(26 downto 0);
    signal vector_q               : std_logic_vector(quotient_width-1 downto 0);

    signal fixed_enable           : std_logic;
    signal d_is_greater           : std_logic;
    signal delayed_d_is_greater   : std_logic; 
    signal delayed_positive_q1,           
           delayed_positive_q2,
           delayed_positive_q3,
           delayed_positive_q4,          
           delayed_positive_q5,
           delayed_positive_q6,
           delayed_positive_q7,          
           delayed_positive_q8,
           delayed_positive_q9,
           delayed_positive_q10,          
           delayed_positive_q11,
           delayed_positive_q12,
           delayed_positive_q13,         
           delayed_positive_q14,
           delayed_positive_q15,
           delayed_positive_q16,          
           delayed_positive_q17,
           delayed_positive_q18,
           delayed_positive_q19,          
           delayed_positive_q20,
           delayed_positive_q21   : std_logic; 
    signal triple_y               : integer;
    signal r24_case_a,
           r24_case_b,
           r24_case_c,
           r24_case_d             : std_logic_vector(26 downto 0);
    signal r25_case_a,
           r25_case_b, 
           r25_case_c, 
           r25_case_d, 
           r25_case_e, 
           r25_case_f, 
           r25_case_g, 
           r25_case_h             : std_logic_vector(26 downto 0);
    signal simple_x               : std_logic_vector(27 downto 0);
    signal double_x               : std_logic_vector(28 downto 0);
    signal double_y               : std_logic_vector(27 downto 0);
    signal quad_y                 : std_logic_vector(28 downto 0);

    begin
                
-----------------------------------------------------------------------------------------------------------------  

        fixed_enable <= '1';

-- d_is_greater delay chain

            D_IS_GREATER_PIPELINE: ff_chain
                                   generic map (length => 8)
                                   port map ( clk => clk, reset => reset, enable => fixed_enable, ff_chain_in => d_is_greater, ff_chain_out => delayed_d_is_greater);

-- positive_q delay chain

            ---------------------------------------------

            POSITIVE_Q1_PIPELINE: ff_chain
                                  generic map (length => 7)
                                  port map ( clk => clk, reset => reset, enable => fixed_enable, ff_chain_in => positive_q(1), ff_chain_out => delayed_positive_q1);

            POSITIVE_Q2_PIPELINE: ff_chain
                                  generic map (length => 7)
                                  port map ( clk => clk, reset => reset, enable => fixed_enable, ff_chain_in => positive_q(2), ff_chain_out => delayed_positive_q2);

            POSITIVE_Q3_PIPELINE: ff_chain
                                  generic map (length => 7)
                                  port map ( clk => clk, reset => reset, enable => fixed_enable, ff_chain_in => positive_q(3), ff_chain_out => delayed_positive_q3);

            ---------------------------------------------

            POSITIVE_Q4_PIPELINE: ff_chain
                                  generic map (length => 6)
                                  port map ( clk => clk, reset => reset, enable => fixed_enable, ff_chain_in => positive_q(4), ff_chain_out => delayed_positive_q4);

            POSITIVE_Q5_PIPELINE: ff_chain
                                  generic map (length => 6)
                                  port map ( clk => clk, reset => reset, enable => fixed_enable, ff_chain_in => positive_q(5), ff_chain_out => delayed_positive_q5);

            POSITIVE_Q6_PIPELINE: ff_chain
                                  generic map (length => 6)
                                  port map ( clk => clk, reset => reset, enable => fixed_enable, ff_chain_in => positive_q(6), ff_chain_out => delayed_positive_q6);

            ---------------------------------------------

            POSITIVE_Q7_PIPELINE: ff_chain
                                  generic map (length => 5)
                                  port map ( clk => clk, reset => reset, enable => fixed_enable, ff_chain_in => positive_q(7), ff_chain_out => delayed_positive_q7);

            POSITIVE_Q8_PIPELINE: ff_chain
                                  generic map (length => 5)
                                  port map ( clk => clk, reset => reset, enable => fixed_enable, ff_chain_in => positive_q(8), ff_chain_out => delayed_positive_q8);

            POSITIVE_Q9_PIPELINE: ff_chain
                                  generic map (length => 5)
                                  port map ( clk => clk, reset => reset, enable => fixed_enable, ff_chain_in => positive_q(9), ff_chain_out => delayed_positive_q9);

            ---------------------------------------------

            POSITIVE_Q10_PIPELINE: ff_chain
                                  generic map (length => 4)
                                  port map ( clk => clk, reset => reset, enable => fixed_enable, ff_chain_in => positive_q(10), ff_chain_out => delayed_positive_q10);

            POSITIVE_Q11_PIPELINE: ff_chain
                                  generic map (length => 4)
                                  port map ( clk => clk, reset => reset, enable => fixed_enable, ff_chain_in => positive_q(11), ff_chain_out => delayed_positive_q11);

            POSITIVE_Q12_PIPELINE: ff_chain
                                  generic map (length => 4)
                                  port map ( clk => clk, reset => reset, enable => fixed_enable, ff_chain_in => positive_q(12), ff_chain_out => delayed_positive_q12);

            ---------------------------------------------

            POSITIVE_Q13_PIPELINE: ff_chain
                                  generic map (length => 3)
                                  port map ( clk => clk, reset => reset, enable => fixed_enable, ff_chain_in => positive_q(13), ff_chain_out => delayed_positive_q13);

            POSITIVE_Q14_PIPELINE: ff_chain
                                  generic map (length => 3)
                                  port map ( clk => clk, reset => reset, enable => fixed_enable, ff_chain_in => positive_q(14), ff_chain_out => delayed_positive_q14);

            POSITIVE_Q15_PIPELINE: ff_chain
                                  generic map (length => 3)
                                  port map ( clk => clk, reset => reset, enable => fixed_enable, ff_chain_in => positive_q(15), ff_chain_out => delayed_positive_q15);

            ---------------------------------------------

            POSITIVE_Q16_PIPELINE: ff_chain
                                  generic map (length => 2)
                                  port map ( clk => clk, reset => reset, enable => fixed_enable, ff_chain_in => positive_q(16), ff_chain_out => delayed_positive_q16);

            POSITIVE_Q17_PIPELINE: ff_chain
                                  generic map (length => 2)
                                  port map ( clk => clk, reset => reset, enable => fixed_enable, ff_chain_in => positive_q(17), ff_chain_out => delayed_positive_q17);

            POSITIVE_Q18_PIPELINE: ff_chain
                                  generic map (length => 2)
                                  port map ( clk => clk, reset => reset, enable => fixed_enable, ff_chain_in => positive_q(18), ff_chain_out => delayed_positive_q18);

            ---------------------------------------------

            POSITIVE_Q19_PIPELINE: ff_chain
                                  generic map (length => 1)
                                  port map ( clk => clk, reset => reset, enable => fixed_enable, ff_chain_in => positive_q(19), ff_chain_out => delayed_positive_q19);

            POSITIVE_Q20_PIPELINE: ff_chain
                                  generic map (length => 1)
                                  port map ( clk => clk, reset => reset, enable => fixed_enable, ff_chain_in => positive_q(20), ff_chain_out => delayed_positive_q20);

            POSITIVE_Q21_PIPELINE: ff_chain
                                  generic map (length => 1)
                                  port map ( clk => clk, reset => reset, enable => fixed_enable, ff_chain_in => positive_q(21), ff_chain_out => delayed_positive_q21);


            ---------------------------------------------

-- d delay chain

        D_PIPELINE_REG1: data_register generic map (reg_width => divisor_width+3)
                                       port map (clk => clk, reset => reset, data_in => d, data_out => pipelined_d1);

        D_PIPELINE_REG2: data_register generic map (reg_width => divisor_width+3)
                                       port map (clk => clk, reset => reset, data_in => pipelined_d1, data_out => pipelined_d2);

        D_PIPELINE_REG3: data_register generic map (reg_width => divisor_width+3)
                                       port map (clk => clk, reset => reset, data_in => pipelined_d2, data_out => pipelined_d3);

        D_PIPELINE_REG4: data_register generic map (reg_width => divisor_width+3)
                                       port map (clk => clk, reset => reset, data_in => pipelined_d3, data_out => pipelined_d4);

        D_PIPELINE_REG5: data_register generic map (reg_width => divisor_width+3)
                                       port map (clk => clk, reset => reset, data_in => pipelined_d4, data_out => pipelined_d5);

        D_PIPELINE_REG6: data_register generic map (reg_width => divisor_width+3)
                                       port map (clk => clk, reset => reset, data_in => pipelined_d5, data_out => pipelined_d6);

        D_PIPELINE_REG7: data_register generic map (reg_width => divisor_width+3)
                                       port map (clk => clk, reset => reset, data_in => pipelined_d6, data_out => pipelined_d7);

        D_PIPELINE_REG8: data_register generic map (reg_width => divisor_width+3)
                                       port map (clk => clk, reset => reset, data_in => pipelined_d7, data_out => pipelined_d8);


        process(dvd, dvs, r, x, d, d_is_greater)
        begin

            -- normalized operands are added with a "sign bit" and a "buffer bit"; a new
            -- null LSB is added to avoid data loss in case of  dividend pre-scaling: r(0) <-- x/2

            x <= ("00" & dvd & '0');     
            d <= ("00" & dvs & '0');  
            positive_q(0) <= '0';

            if (x < d) then              
                    d_is_greater <= '1';   
            else
                    d_is_greater <= '0';
            end if;

            if (d_is_greater = '1') then              -- dividend pre-scaling: r(0) determination
                    r(0) <= x;   
            else
                    r(0) <= ( '0' & x(26 downto 1) );
            end if;

        end process;

            --------------------------------------------------

            -- since r(0) is always positive, there's no need to select between 2 possibilities

        process(r, d)
        begin

            --positive_q(1) <= '1';
            r(1) <= conv_std_logic_vector( (conv_integer(signed(r(0)&'0')) - conv_integer(signed(d))), 27 );

        end process;

            --------------------------------------------------

------------------------------------
-- INTERNAL PIPELINE:   register 1                       
------------------------------------

            PIPELINE_REG1_R: data_register generic map (reg_width => division_rem_width+2)
                                           port map (clk => clk, reset => reset, data_in => r(1), data_out => pipelined_r1);

        -- k=2
        process(pipelined_r1, pipelined_d1)
        begin

            if ( (pipelined_r1(26)) = '1' ) then         -- r(i-1) < 0   =>   2r(i-1) < 0   (this way only sign bit check is needed)
                    positive_q(1) <= '0';
                    r(2) <= conv_std_logic_vector( (conv_integer(signed(pipelined_r1&'0')) + conv_integer(signed(pipelined_d1))), 27 );
            else
                    positive_q(1) <= '1';
                    r(2) <= conv_std_logic_vector( (conv_integer(signed(pipelined_r1&'0')) - conv_integer(signed(pipelined_d1))), 27 );
            end if;

        end process;

            --------------------------------------------------

        -- k=3
        process(r, pipelined_d1)
        begin

            if ( (r(2)(26)) = '1' ) then         -- r(i-1) < 0   =>   2r(i-1) < 0   (this way only sign bit check is needed)
                    positive_q(2) <= '0';
                    r(3) <= conv_std_logic_vector( (conv_integer(signed(r(2)&'0')) + conv_integer(signed(pipelined_d1))), 27 );
            else
                    positive_q(2) <= '1';
                    r(3) <= conv_std_logic_vector( (conv_integer(signed(r(2)&'0')) - conv_integer(signed(pipelined_d1))), 27 );
            end if;

        end process;

            --------------------------------------------------

        -- k=4
        process(r, pipelined_d1)
        begin

            if ( (r(3)(26)) = '1' ) then         -- r(i-1) < 0   =>   2r(i-1) < 0   (this way only sign bit check is needed)
                    positive_q(3) <= '0';
                    r(4) <= conv_std_logic_vector( (conv_integer(signed(r(3)&'0')) + conv_integer(signed(pipelined_d1))), 27 );
            else
                    positive_q(3) <= '1';
                    r(4) <= conv_std_logic_vector( (conv_integer(signed(r(3)&'0')) - conv_integer(signed(pipelined_d1))), 27 );
            end if;

        end process;

            --------------------------------------------------

------------------------------------
-- INTERNAL PIPELINE:   register 2                       
------------------------------------

            PIPELINE_REG2_R: data_register generic map (reg_width => division_rem_width+2)
                                           port map (clk => clk, reset => reset, data_in => r(4), data_out => pipelined_r4);


        -- k=5
        process(pipelined_r4, pipelined_d2)
        begin

            if ( (pipelined_r4(26)) = '1' ) then         -- r(i-1) < 0   =>   2r(i-1) < 0   (this way only sign bit check is needed)
                    positive_q(4) <= '0';
                    r(5) <= conv_std_logic_vector( (conv_integer(signed(pipelined_r4&'0')) + conv_integer(signed(pipelined_d2))), 27 );
            else
                    positive_q(4) <= '1';
                    r(5) <= conv_std_logic_vector( (conv_integer(signed(pipelined_r4&'0')) - conv_integer(signed(pipelined_d2))), 27 );
            end if;

        end process;

            --------------------------------------------------

        -- k=6
        process(r, pipelined_d2)
        begin

            if ( (r(5)(26)) = '1' ) then         -- r(i-1) < 0   =>   2r(i-1) < 0   (this way only sign bit check is needed)
                    positive_q(5) <= '0';
                    r(6) <= conv_std_logic_vector( (conv_integer(signed(r(5)&'0')) + conv_integer(signed(pipelined_d2))), 27 );
            else
                    positive_q(5) <= '1';
                    r(6) <= conv_std_logic_vector( (conv_integer(signed(r(5)&'0')) - conv_integer(signed(pipelined_d2))), 27 );
            end if;

        end process;

            --------------------------------------------------

        -- k=7
        process(r, pipelined_d2)
        begin

            if ( (r(6)(26)) = '1' ) then         -- r(i-1) < 0   =>   2r(i-1) < 0   (this way only sign bit check is needed)
                    positive_q(6) <= '0';
                    r(7) <= conv_std_logic_vector( (conv_integer(signed(r(6)&'0')) + conv_integer(signed(pipelined_d2))), 27 );
            else
                    positive_q(6) <= '1';
                    r(7) <= conv_std_logic_vector( (conv_integer(signed(r(6)&'0')) - conv_integer(signed(pipelined_d2))), 27 );
            end if;

        end process;

            --------------------------------------------------

------------------------------------
-- INTERNAL PIPELINE:   register 3                       
------------------------------------

            PIPELINE_REG3_R: data_register generic map (reg_width => division_rem_width+2)
                                           port map (clk => clk, reset => reset, data_in => r(7), data_out => pipelined_r7);

        -- k=8
        process(pipelined_r7, pipelined_d3)
        begin

            if ( (pipelined_r7(26)) = '1' ) then         -- r(i-1) < 0   =>   2r(i-1) < 0   (this way only sign bit check is needed)
                    positive_q(7) <= '0';
                    r(8) <= conv_std_logic_vector( (conv_integer(signed(pipelined_r7&'0')) + conv_integer(signed(pipelined_d3))), 27 );
            else
                    positive_q(7) <= '1';
                    r(8) <= conv_std_logic_vector( (conv_integer(signed(pipelined_r7&'0')) - conv_integer(signed(pipelined_d3))), 27 );
            end if;

        end process;

            --------------------------------------------------

        -- k=9
        process(r, pipelined_d3)
        begin

            if ( (r(8)(26)) = '1' ) then         -- r(i-1) < 0   =>   2r(i-1) < 0   (this way only sign bit check is needed)
                    positive_q(8) <= '0';
                    r(9) <= conv_std_logic_vector( (conv_integer(signed(r(8)&'0')) + conv_integer(signed(pipelined_d3))), 27 );
            else
                    positive_q(8) <= '1';
                    r(9) <= conv_std_logic_vector( (conv_integer(signed(r(8)&'0')) - conv_integer(signed(pipelined_d3))), 27 );
            end if;

        end process;

            --------------------------------------------------

        -- k=10
        process(r, pipelined_d3)
        begin

            if ( (r(9)(26)) = '1' ) then         -- r(i-1) < 0   =>   2r(i-1) < 0   (this way only sign bit check is needed)
                    positive_q(9) <= '0';
                    r(10) <= conv_std_logic_vector( (conv_integer(signed(r(9)&'0')) + conv_integer(signed(pipelined_d3))), 27 );
            else
                    positive_q(9) <= '1';
                    r(10) <= conv_std_logic_vector( (conv_integer(signed(r(9)&'0')) - conv_integer(signed(pipelined_d3))), 27 );
            end if;

        end process;

            --------------------------------------------------

------------------------------------
-- INTERNAL PIPELINE:   register 4                       
------------------------------------

            PIPELINE_REG4_R: data_register generic map (reg_width => division_rem_width+2)
                                           port map (clk => clk, reset => reset, data_in => r(10), data_out => pipelined_r10);


        -- k=11
        process(pipelined_r10, pipelined_d4)
        begin

            if ( (pipelined_r10(26)) = '1' ) then         -- r(i-1) < 0   =>   2r(i-1) < 0   (this way only sign bit check is needed)
                    positive_q(10) <= '0';
                    r(11) <= conv_std_logic_vector( (conv_integer(signed(pipelined_r10&'0')) + conv_integer(signed(pipelined_d4))), 27 );
            else
                    positive_q(10) <= '1';
                    r(11) <= conv_std_logic_vector( (conv_integer(signed(pipelined_r10&'0')) - conv_integer(signed(pipelined_d4))), 27 );
            end if;

        end process;

            --------------------------------------------------

        -- k=12
        process(r, pipelined_d4)
        begin

            if ( (r(11)(26)) = '1' ) then         -- r(i-1) < 0   =>   2r(i-1) < 0   (this way only sign bit check is needed)
                    positive_q(11) <= '0';
                    r(12) <= conv_std_logic_vector( (conv_integer(signed(r(11)&'0')) + conv_integer(signed(pipelined_d4))), 27 );
            else
                    positive_q(11) <= '1';
                    r(12) <= conv_std_logic_vector( (conv_integer(signed(r(11)&'0')) - conv_integer(signed(pipelined_d4))), 27 );
            end if;

        end process;

            --------------------------------------------------

        -- k=13
        process(r, pipelined_d4)
        begin

            if ( (r(12)(26)) = '1' ) then         -- r(i-1) < 0   =>   2r(i-1) < 0   (this way only sign bit check is needed)
                    positive_q(12) <= '0';
                    r(13) <= conv_std_logic_vector( (conv_integer(signed(r(12)&'0')) + conv_integer(signed(pipelined_d4))), 27 );
            else
                    positive_q(12) <= '1';
                    r(13) <= conv_std_logic_vector( (conv_integer(signed(r(12)&'0')) - conv_integer(signed(pipelined_d4))), 27 );
            end if;

        end process;

            --------------------------------------------------

------------------------------------
-- INTERNAL PIPELINE:   register 5                       
------------------------------------

            PIPELINE_REG5_R: data_register generic map (reg_width => division_rem_width+2)
                                           port map (clk => clk, reset => reset, data_in => r(13), data_out => pipelined_r13);

        -- k=14
        process(pipelined_r13, pipelined_d5)
        begin
            if ( (pipelined_r13(26)) = '1' ) then         -- r(i-1) < 0   =>   2r(i-1) < 0   (this way only sign bit check is needed)
                    positive_q(13) <= '0';
                    r(14) <= conv_std_logic_vector( (conv_integer(signed(pipelined_r13&'0')) + conv_integer(signed(pipelined_d5))), 27 );
            else
                    positive_q(13) <= '1';
                    r(14) <= conv_std_logic_vector( (conv_integer(signed(pipelined_r13&'0')) - conv_integer(signed(pipelined_d5))), 27 );
            end if;

        end process;

            --------------------------------------------------

        -- k=15
        process(r, pipelined_d5)
        begin
            if ( (r(14)(26)) = '1' ) then         -- r(i-1) < 0   =>   2r(i-1) < 0   (this way only sign bit check is needed)
                    positive_q(14) <= '0';
                    r(15) <= conv_std_logic_vector( (conv_integer(signed(r(14)&'0')) + conv_integer(signed(pipelined_d5))), 27 );
            else
                    positive_q(14) <= '1';
                    r(15) <= conv_std_logic_vector( (conv_integer(signed(r(14)&'0')) - conv_integer(signed(pipelined_d5))), 27 );
            end if;

        end process;

            --------------------------------------------------

        -- k=16
        process(r, pipelined_d5)
        begin

            if ( (r(15)(26)) = '1' ) then         -- r(i-1) < 0   =>   2r(i-1) < 0   (this way only sign bit check is needed)
                    positive_q(15) <= '0';
                    r(16) <= conv_std_logic_vector( (conv_integer(signed(r(15)&'0')) + conv_integer(signed(pipelined_d5))), 27 );
            else
                    positive_q(15) <= '1';
                    r(16) <= conv_std_logic_vector( (conv_integer(signed(r(15)&'0')) - conv_integer(signed(pipelined_d5))), 27 );
            end if;

        end process;

            --------------------------------------------------

------------------------------------
-- INTERNAL PIPELINE:   register 6                       
------------------------------------

            PIPELINE_REG6_R: data_register generic map (reg_width => division_rem_width+2)
                                           port map (clk => clk, reset => reset, data_in => r(16), data_out => pipelined_r16);

        -- k=17
        process(pipelined_r16, pipelined_d6)
        begin
            if ( (pipelined_r16(26)) = '1' ) then         -- r(i-1) < 0   =>   2r(i-1) < 0   (this way only sign bit check is needed)
                    positive_q(16) <= '0';
                    r(17) <= conv_std_logic_vector( (conv_integer(signed(pipelined_r16&'0')) + conv_integer(signed(pipelined_d6))), 27 );
            else
                    positive_q(16) <= '1';
                    r(17) <= conv_std_logic_vector( (conv_integer(signed(pipelined_r16&'0')) - conv_integer(signed(pipelined_d6))), 27 );
            end if;

        end process;

            --------------------------------------------------

        -- k=18
        process(r, pipelined_d6)
        begin
            if ( (r(17)(26)) = '1' ) then         -- r(i-1) < 0   =>   2r(i-1) < 0   (this way only sign bit check is needed)
                    positive_q(17) <= '0';
                    r(18) <= conv_std_logic_vector( (conv_integer(signed(r(17)&'0')) + conv_integer(signed(pipelined_d6))), 27 );
            else
                    positive_q(17) <= '1';
                    r(18) <= conv_std_logic_vector( (conv_integer(signed(r(17)&'0')) - conv_integer(signed(pipelined_d6))), 27 );
            end if;

        end process;

            --------------------------------------------------

        -- k=19
        process(r, pipelined_d6)
        begin
            if ( (r(18)(26)) = '1' ) then         -- r(i-1) < 0   =>   2r(i-1) < 0   (this way only sign bit check is needed)
                    positive_q(18) <= '0';
                    r(19) <= conv_std_logic_vector( (conv_integer(signed(r(18)&'0')) + conv_integer(signed(pipelined_d6))), 27 );
            else
                    positive_q(18) <= '1';
                    r(19) <= conv_std_logic_vector( (conv_integer(signed(r(18)&'0')) - conv_integer(signed(pipelined_d6))), 27 );
            end if;

        end process;

            --------------------------------------------------

------------------------------------
-- INTERNAL PIPELINE:   register 7                       
------------------------------------

            PIPELINE_REG7_R: data_register generic map (reg_width => division_rem_width+2)
                                           port map (clk => clk, reset => reset, data_in => r(19), data_out => pipelined_r19);

        -- k=20
        process(pipelined_r19, pipelined_d7)
        begin
            if ( (pipelined_r19(26)) = '1' ) then         -- r(i-1) < 0   =>   2r(i-1) < 0   (this way only sign bit check is needed)
                    positive_q(19) <= '0';
                    r(20) <= conv_std_logic_vector( (conv_integer(signed(pipelined_r19&'0')) + conv_integer(signed(pipelined_d7))), 27 );
            else
                    positive_q(19) <= '1';
                    r(20) <= conv_std_logic_vector( (conv_integer(signed(pipelined_r19&'0')) - conv_integer(signed(pipelined_d7))), 27 );
            end if;

        end process;

            --------------------------------------------------

        -- k=21
        process(r, pipelined_d7)
        begin

            if ( (r(20)(26)) = '1' ) then         -- r(i-1) < 0   =>   2r(i-1) < 0   (this way only sign bit check is needed)
                    positive_q(20) <= '0';
                    r(21) <= conv_std_logic_vector( (conv_integer(signed(r(20)&'0')) + conv_integer(signed(pipelined_d7))), 27 );
            else
                    positive_q(20) <= '1';
                    r(21) <= conv_std_logic_vector( (conv_integer(signed(r(20)&'0')) - conv_integer(signed(pipelined_d7))), 27 );
            end if;

        end process;

            --------------------------------------------------

        -- k=22
        process(r, pipelined_d7)
        begin

            if ( (r(21)(26)) = '1' ) then         -- r(i-1) < 0   =>   2r(i-1) < 0   (this way only sign bit check is needed)
                    positive_q(21) <= '0';
                    r(22) <= conv_std_logic_vector( (conv_integer(signed(r(21)&'0')) + conv_integer(signed(pipelined_d7))), 27 );
            else
                    positive_q(21) <= '1';
                    r(22) <= conv_std_logic_vector( (conv_integer(signed(r(21)&'0')) - conv_integer(signed(pipelined_d7))), 27 );
            end if;

        end process;

            --------------------------------------------------

------------------------------------
-- INTERNAL PIPELINE:   register 8                       
------------------------------------

            PIPELINE_REG8_R: data_register generic map (reg_width => division_rem_width+2)
                                           port map (clk => clk, reset => reset, data_in => r(22), data_out => pipelined_r22);


                    triple_y <= ( conv_integer(signed(pipelined_d8&'0')) + conv_integer(signed(pipelined_d8)) );
        

        -- k=23
        process(pipelined_r22, pipelined_d8)
        begin

            if ( (pipelined_r22(26)) = '1' ) then         -- r(i-1) < 0   =>   2r(i-1) < 0   (this way only sign bit check is needed)
                    positive_q(22) <= '0';
                    r(23) <= conv_std_logic_vector( (conv_integer(signed(pipelined_r22&'0')) + conv_integer(signed(pipelined_d8))), 27 );
            else
                    positive_q(22) <= '1';
                    r(23) <= conv_std_logic_vector( (conv_integer(signed(pipelined_r22&'0')) - conv_integer(signed(pipelined_d8))), 27 );
            end if;

        end process;

            --------------------------------------------------

        -- r(23), has been calculated; now special quantities are calculated to allow early r(25) determination

        double_x <= (r(23)&"00");
        simple_x <= (r(23)&'0');
        double_y <= (pipelined_d8&'0');
        quad_y   <= (pipelined_d8&"00");

        r24_case_a <= conv_std_logic_vector( (conv_integer(signed(simple_x)) + conv_integer(signed(double_y))), 27 );
        r24_case_b <= conv_std_logic_vector( (conv_integer(signed(simple_x)) + conv_integer(signed(pipelined_d8))), 27 );
        r24_case_c <= conv_std_logic_vector( (conv_integer(signed(simple_x)) ), 27 );
        r24_case_d <= conv_std_logic_vector( (conv_integer(signed(simple_x)) - conv_integer(signed(pipelined_d8))), 27 );

        r25_case_a <= conv_std_logic_vector( (conv_integer(signed(double_x)) + triple_y), 27 );
        r25_case_b <= conv_std_logic_vector( (conv_integer(signed(double_x)) + conv_integer(signed(pipelined_d8))), 27 );
        r25_case_c <= conv_std_logic_vector( (conv_integer(signed(double_x)) - conv_integer(signed(pipelined_d8))), 27 );
        r25_case_d <= conv_std_logic_vector( (conv_integer(signed(double_x)) - triple_y), 27 );
        r25_case_e <= conv_std_logic_vector( (conv_integer(signed(double_x)) + conv_integer(signed(quad_y))), 27 );
        r25_case_f <= conv_std_logic_vector( (conv_integer(signed(double_x)) + conv_integer(signed(double_y))), 27 );
        r25_case_g <= conv_std_logic_vector( (conv_integer(signed(double_x))), 27 );
        r25_case_h <= conv_std_logic_vector( (conv_integer(signed(double_x)) - conv_integer(signed(double_y))), 27 );


        -- k=24
        process(r)
        begin

            if ( (r(23)(26)) = '1' ) then         -- r(i-1) < 0   =>   2r(i-1) < 0   (this way only sign bit check is needed)
                    positive_q(23) <= '0';
            else
                    positive_q(23) <= '1';
            end if;

        end process;

            --------------------------------------------------

            -- k=25
        process(r, r24_case_b, r24_case_d)
        begin

                    if ( ((r(23)(26)) = '1') and (r24_case_b(26) = '1') ) then         -- r(i-1) < 0   =>   2r(i-1) < 0   (this way only sign bit check is needed)
                           positive_q(24) <= '0';
                    elsif ( ((r(23)(26)) = '1') and (r24_case_b(26) = '0') ) then   
                           positive_q(24) <= '1';
                    elsif ( ((r(23)(26)) = '0') and (r24_case_d(26) = '1') ) then 
                           positive_q(24) <= '0';
                    else
                           positive_q(24) <= '1';
                    end if;

        end process;

            --------------------------------------------------

        positive_q(25) <= '1';


        -- Signed Digit (SD) to binary form result conversion

             turned_positive_q(div_k_max)   <= '0';
             turned_positive_q(div_k_max-1) <= delayed_positive_q1;
             turned_positive_q(div_k_max-2) <= delayed_positive_q2;
             turned_positive_q(div_k_max-3) <= delayed_positive_q3;
             turned_positive_q(div_k_max-4) <= delayed_positive_q4;
             turned_positive_q(div_k_max-5) <= delayed_positive_q5;
             turned_positive_q(div_k_max-6) <= delayed_positive_q6;
             turned_positive_q(div_k_max-7) <= delayed_positive_q7;
             turned_positive_q(div_k_max-8) <= delayed_positive_q8;
             turned_positive_q(div_k_max-9) <= delayed_positive_q9;
             turned_positive_q(div_k_max-10) <= delayed_positive_q10;
             turned_positive_q(div_k_max-11) <= delayed_positive_q11;
             turned_positive_q(div_k_max-12) <= delayed_positive_q12;
             turned_positive_q(div_k_max-13) <= delayed_positive_q13;
             turned_positive_q(div_k_max-14) <= delayed_positive_q14;
             turned_positive_q(div_k_max-15) <= delayed_positive_q15;
             turned_positive_q(div_k_max-16) <= delayed_positive_q16;
             turned_positive_q(div_k_max-17) <= delayed_positive_q17;
             turned_positive_q(div_k_max-18) <= delayed_positive_q18;
             turned_positive_q(div_k_max-19) <= delayed_positive_q19;
             turned_positive_q(div_k_max-20) <= delayed_positive_q20;
             turned_positive_q(div_k_max-21) <= delayed_positive_q21;
             turned_positive_q(div_k_max-22) <= positive_q(22);
             turned_positive_q(div_k_max-23) <= positive_q(23);
             turned_positive_q(div_k_max-24) <= positive_q(24);
             turned_positive_q(div_k_max-25) <= positive_q(25);


            q <= (turned_positive_q);


            --------------------------------------------------

        -- remainder and quotient selection
        -- note that bits 26 and 25 in the last remainder can be discarded, since remainder is surely less than divider, which has MSB in position 24

        process(q, delayed_d_is_greater, temp_q, r, r24_case_a, r24_case_b, r24_case_c, r24_case_d,
                r25_case_a, r25_case_b, r25_case_c, r25_case_d, r25_case_e, r25_case_f, r25_case_g, r25_case_h)
        begin


            if (delayed_d_is_greater = '1') then  -- remainder is r(24)

                    if ( ((r(23)(26)) = '1') and (r24_case_b(26) = '1') ) then         -- r(i-1) < 0   =>   2r(i-1) < 0   (this way only sign bit check is needed)
                            temp_q <= (q(div_k_max downto 1) & '0');
                            temp_remainder <= r24_case_a;
                    elsif ( ((r(23)(26)) = '1') and (r24_case_b(26) = '0') ) then   
                            temp_q <= q;   
                            temp_remainder <= r24_case_b;
                    elsif ( ((r(23)(26)) = '0') and (r24_case_d(26) = '1') ) then 
                            temp_q <= (q(div_k_max downto 1) & '0');     
                            temp_remainder <= r24_case_c;
                    else
                            temp_remainder <= r24_case_d;
                            temp_q <= q;
                    end if;

                    vector_q <= temp_q(25 downto 1) ;      -- 0.1xxx

            else                                  -- remainder is r(25)

                    if ( (r(23)(26) = '1') and (r24_case_b(26) = '1') and (r25_case_a(26) = '1') ) then 
                            temp_q <= (q(div_k_max downto 1) & '0');
                            temp_remainder <= r25_case_e;
                    elsif ( (r(23)(26) = '1') and (r24_case_b(26) = '0') and (r25_case_b(26) = '1') ) then 
                            temp_q <= (q(div_k_max downto 1) & '0');
                            temp_remainder <= r25_case_f;
                    elsif ( (r(23)(26) = '0') and (r24_case_d(26) = '1') and (r25_case_c(26) = '1') ) then 
                            temp_q <= (q(div_k_max downto 1) & '0');
                            temp_remainder <= r25_case_g;
                    elsif ( (r(23)(26) = '0') and (r24_case_d(26) = '0') and (r25_case_d(26) = '1') ) then
                            temp_q <= (q(div_k_max downto 1) & '0'); 
                            temp_remainder <= r25_case_h;
                    elsif ( (r(23)(26) = '1') and (r24_case_b(26) = '1') and (r25_case_a(26) = '0') ) then 
                            temp_q <= q;
                            temp_remainder <= r25_case_a;
                    elsif ( (r(23)(26) = '1') and (r24_case_b(26) = '0') and (r25_case_b(26) = '0') ) then 
                            temp_q <= q;
                            temp_remainder <= r25_case_b;
                    elsif ( (r(23)(26) = '0') and (r24_case_d(26) = '1') and (r25_case_c(26) = '0') ) then 
                            temp_q <= q;
                            temp_remainder <= r25_case_c;
                    else
                            temp_q <= q;
                            temp_remainder <= r25_case_d;
                    end if;

                    vector_q <= temp_q(24 downto 0) ;      -- 1.xxxx

           end if;


         end process;

------------------------------------------------------------------------

       quot <= vector_q; 
       rest <= temp_remainder(24 downto 0);
 
-----------------------------------------------------------------------------------------------------------------       

end rtl;

