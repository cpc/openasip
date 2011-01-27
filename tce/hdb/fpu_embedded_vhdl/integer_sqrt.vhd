-----------------------------------------------------------------------------------------------------------
--
--				           INTEGER OPERAND SQUARE ROOT EXTRACTION
--
-- Created by Claudio Brunelli, 2003
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


entity integer_sqrt is
    port( clk       : in std_logic;
          reset     : in std_logic;
          input     : in std_logic_vector(radicand_width-1 downto 0);
          output    : out std_logic_vector(sqrt_width-1 downto 0);
          remainder : out std_logic_vector(rem_width-1 downto 0) ); 
end integer_sqrt ;

-------------------------------------------------------------------------------
architecture rtl of integer_sqrt is
 
    type int_bus is array (k_max downto 0) of integer;
    type vector_bus is array (k_max-1 downto 0) of std_logic_vector(rem_width+1 downto 0);
    type unsigned_bus is array (k_max-1 downto 0) of unsigned(sqrt_width+1 downto 0);

    signal q, r            : int_bus;
    signal vector_r        : vector_bus;
    signal unsigned_q      : unsigned_bus;
    signal q01             : integer; 
    signal vector_q0       : std_logic_vector(sqrt_width-1 downto 0);

    signal converted_q22,
           converted_q17,
           converted_q12,
           converted_q7,
           converted_q2              : std_logic_vector(sqrt_width+2 downto 0);
    signal converted_r22,
           converted_r17,
           converted_r12,
           converted_r7,
           converted_r2              : std_logic_vector(rem_width+2 downto 0);
    signal pipelined_q22,
           pipelined_q17,
           pipelined_q12,
           pipelined_q7,
           pipelined_q2    : std_logic_vector(sqrt_width+2 downto 0);
    signal pipelined_r22,
           pipelined_r17,
           pipelined_r12,
           pipelined_r7,
           pipelined_r2    : std_logic_vector(rem_width+2 downto 0);
    signal integer_q22,
           integer_q17,
           integer_q12,
           integer_q7,
           integer_q2              : integer;
    signal integer_r22,
           integer_r17,
           integer_r12,
           integer_r7,
           integer_r2              : integer;
    signal input43_34,
           input33_24,
           input23_14,
           input13_4               : std_logic_vector(9 downto 0);
    signal input3_0                : std_logic_vector(3 downto 0);
    signal delayed_input43_34,
           delayed_input33_24,
           delayed_input23_14,
           delayed_input13_4       : std_logic_vector(9 downto 0);
    signal delayed_input3_0        : std_logic_vector(3 downto 0);
    signal fixed_enable            : std_logic;

    begin

-----------------------------------------------------------------------------------------------------------------  

        fixed_enable <= '1';
        q(k_max) <= 0;   -- q(26) value
        r(k_max) <= 0;   -- r(26) value

        input43_34 <= input(43 downto 34);
        input33_24 <= input(33 downto 24);
        input23_14 <= input(23 downto 14);
        input13_4  <= input(13 downto 4);
        input3_0   <= input(3  downto 0);


        INPUT43_34_PIPELINE: simple_register_chain
                             generic map (length => 1, width => 10)
                             port map ( clk => clk, reset => reset, enable => fixed_enable, reg_chain_in => input43_34 , reg_chain_out => delayed_input43_34);

        INPUT33_24_PIPELINE: simple_register_chain
                             generic map (length => 2, width => 10)
                             port map ( clk => clk, reset => reset, enable => fixed_enable, reg_chain_in => input33_24 , reg_chain_out => delayed_input33_24);

        INPUT23_14_PIPELINE: simple_register_chain
                             generic map (length => 3, width => 10)
                             port map ( clk => clk, reset => reset, enable => fixed_enable, reg_chain_in => input23_14 , reg_chain_out => delayed_input23_14);

        INPUT13_4_PIPELINE: simple_register_chain
                             generic map (length => 4, width => 10)
                             port map ( clk => clk, reset => reset, enable => fixed_enable, reg_chain_in => input13_4 , reg_chain_out => delayed_input13_4);

        INPUT3_0_PIPELINE: simple_register_chain
                             generic map (length => 5, width => 4)
                             port map ( clk => clk, reset => reset, enable => fixed_enable, reg_chain_in => input3_0 , reg_chain_out => delayed_input3_0);


-- k = k_max-1 = 25

        process(r, input, vector_r, q)
        begin
                vector_r(25) <= ( (conv_std_logic_vector(r(26), rem_width)) & input(51) & input(50) );   -- va da 3 a 1  
 
                        unsigned_q(25) <= ( conv_unsigned( q(26), sqrt_width) & "01" );       -- vale 1 !!!
                        r(25) <= ( conv_integer(signed(vector_r(25))) - 1 );  -- sicuramente > o = a 0 ( va da 2 a 0)


                        q(25) <= 1;

        end process; 

                -------------------------------------------------------

-- k = k_max-1 = 24

        process(r, input, q, vector_r, unsigned_q)
        begin
                vector_r(24) <= ( (conv_std_logic_vector(r(25), rem_width)) & input(49) & input(48) );
                if ( r(25) >= 0 )  then 
                        unsigned_q(24) <= ( conv_unsigned(q(25), sqrt_width) & "01" );
                        r(24) <= ( conv_integer(signed(vector_r(24))) - conv_integer(unsigned_q(24)) );
                else
                        unsigned_q(24) <= ( conv_unsigned(q(25), sqrt_width) & "11" );
                        r(24) <= ( conv_integer(signed(vector_r(24))) + conv_integer(unsigned_q(24)) );
                end if;
 
 

                if ( r(24) >= 0 ) then 
                        q(24) <= conv_integer( unsigned(unsigned_q(24)(k_max downto 2) & '1') );
                else
                        q(24) <= conv_integer( unsigned(unsigned_q(24)(k_max downto 2) & '0') );
                end if;
        end process;

                -------------------------------------------------------

-- k = k_max-1 = 23

        process(r, input, q, vector_r, unsigned_q)
        begin                
                vector_r(23) <= ( (conv_std_logic_vector(r(24), rem_width)) & input(47) & input(46) );
                if ( r(24) >= 0 )  then 
                        unsigned_q(23) <= ( conv_unsigned(q(24), sqrt_width) & "01" );
                        r(23) <= ( conv_integer(signed(vector_r(23))) - conv_integer(unsigned_q(23)) );
                else
                        unsigned_q(23) <= ( conv_unsigned(q(24), sqrt_width) & "11" );
                        r(23) <= ( conv_integer(signed(vector_r(23))) + conv_integer(unsigned_q(23)) );
                end if;
 
 

                if ( r(23) >= 0 ) then 
                        q(23) <= conv_integer( unsigned(unsigned_q(23)(k_max downto 2) & '1') );
                else
                        q(23) <= conv_integer( unsigned(unsigned_q(23)(k_max downto 2) & '0') );
                end if;
        end process;

                -------------------------------------------------------

-- k = k_max-1 = 22

        process(r, input, q, vector_r, unsigned_q)
        begin 
                vector_r(22) <= ( (conv_std_logic_vector(r(23), rem_width)) & input(45) & input(44) );  -- 30 bit
                if ( r(23) >= 0 )  then 
                        unsigned_q(22) <= ( conv_unsigned(q(23), sqrt_width) & "01" );  -- 28 bit
                        r(22) <= ( conv_integer(signed(vector_r(22))) - conv_integer(unsigned_q(22)) );
                else
                        unsigned_q(22) <= ( conv_unsigned(q(23), sqrt_width) & "11" );
                        r(22) <= ( conv_integer(signed(vector_r(22))) + conv_integer(unsigned_q(22)) );
                end if;
 
 

                if ( r(22) >= 0 ) then 
                        q(22) <= conv_integer( unsigned(unsigned_q(22)(k_max downto 2) & '1') );
                else
                        q(22) <= conv_integer( unsigned(unsigned_q(22)(k_max downto 2) & '0') );
                end if;
        end process;


        converted_q22 <= conv_std_logic_vector(q(22), sqrt_width+3);
        converted_r22 <= conv_std_logic_vector(r(22), rem_width+3);

        Q22_PIPELINE_REG: data_register generic map (reg_width => sqrt_width+3)
                                        port map (clk => clk, reset => reset, data_in => converted_q22, data_out => pipelined_q22);
        R22_PIPELINE_REG: data_register generic map (reg_width => rem_width+3)
                                        port map (clk => clk, reset => reset, data_in => converted_r22, data_out => pipelined_r22);

        integer_q22 <= conv_integer( signed(pipelined_q22) );
        integer_r22 <= conv_integer( signed(pipelined_r22) );


                -------------------------------------------------------

-- k = k_max-1 = 21

        process(r, integer_r22, delayed_input43_34, integer_q22, vector_r, unsigned_q)
        begin 
                vector_r(21) <= ( (conv_std_logic_vector(integer_r22, rem_width)) & delayed_input43_34(9) & delayed_input43_34(8) );
                if ( integer_r22 >= 0 )  then 
                        unsigned_q(21) <= ( conv_unsigned(integer_q22, sqrt_width) & "01" );
                        r(21) <= ( conv_integer(signed(vector_r(21))) - conv_integer(unsigned_q(21)) );
                else
                        unsigned_q(21) <= ( conv_unsigned(integer_q22, sqrt_width) & "11" );
                        r(21) <= ( conv_integer(signed(vector_r(21))) + conv_integer(unsigned_q(21)) );
                end if;
 
 

                if ( r(21) >= 0 ) then 
                        q(21) <= conv_integer( unsigned(unsigned_q(21)(k_max downto 2) & '1') );
                else
                        q(21) <= conv_integer( unsigned(unsigned_q(21)(k_max downto 2) & '0') );
                end if;
        end process;

                -------------------------------------------------------

-- k = k_max-1 = 20

        process(r, delayed_input43_34, q, vector_r, unsigned_q)
        begin
                vector_r(20) <= ( (conv_std_logic_vector(r(21), rem_width)) & delayed_input43_34(7) & delayed_input43_34(6) );
                if ( r(21) >= 0 )  then 
                        unsigned_q(20) <= ( conv_unsigned(q(21), sqrt_width) & "01" );
                        r(20) <= ( conv_integer(signed(vector_r(20))) - conv_integer(unsigned_q(20)) );
                else
                        unsigned_q(20) <= ( conv_unsigned(q(21), sqrt_width) & "11" );
                        r(20) <= ( conv_integer(signed(vector_r(20))) + conv_integer(unsigned_q(20)) );
                end if;
 
 

                if ( r(20) >= 0 ) then 
                        q(20) <= conv_integer( unsigned(unsigned_q(20)(k_max downto 2) & '1') );
                else
                        q(20) <= conv_integer( unsigned(unsigned_q(20)(k_max downto 2) & '0') );
                end if;
        end process;


                -------------------------------------------------------

-- k = k_max-1 = 19

        process(r, delayed_input43_34, q, vector_r, unsigned_q)
        begin
                vector_r(19) <= ( (conv_std_logic_vector(r(20), rem_width)) & delayed_input43_34(5) & delayed_input43_34(4) );
                if ( r(20) >= 0 )  then 
                        unsigned_q(19) <= ( conv_unsigned(q(20), sqrt_width) & "01" );
                        r(19) <= ( conv_integer(signed(vector_r(19))) - conv_integer(unsigned_q(19)) );
                else
                        unsigned_q(19) <= ( conv_unsigned(q(20), sqrt_width) & "11" );
                        r(19) <= ( conv_integer(signed(vector_r(19))) + conv_integer(unsigned_q(19)) );
                end if;
 
 

                if ( r(19) >= 0 ) then 
                        q(19) <= conv_integer( unsigned(unsigned_q(19)(k_max downto 2) & '1') );
                else
                        q(19) <= conv_integer( unsigned(unsigned_q(19)(k_max downto 2) & '0') );
                end if;
        end process;

                -------------------------------------------------------

-- k = k_max-1 = 18

        process(r, delayed_input43_34, q, vector_r, unsigned_q)
        begin
                vector_r(18) <= ( (conv_std_logic_vector(r(19), rem_width)) & delayed_input43_34(3) & delayed_input43_34(2) );
                if ( r(19) >= 0 )  then 
                        unsigned_q(18) <= ( conv_unsigned(q(19), sqrt_width) & "01" );
                        r(18) <= ( conv_integer(signed(vector_r(18))) - conv_integer(unsigned_q(18)) );
                else
                        unsigned_q(18) <= ( conv_unsigned(q(19), sqrt_width) & "11" );
                        r(18) <= ( conv_integer(signed(vector_r(18))) + conv_integer(unsigned_q(18)) );
                end if;
 
 

                if ( r(18) >= 0 ) then 
                        q(18) <= conv_integer( unsigned(unsigned_q(18)(k_max downto 2) & '1') );
                else
                        q(18) <= conv_integer( unsigned(unsigned_q(18)(k_max downto 2) & '0') );
                end if;
        end process;

                -------------------------------------------------------

-- k = k_max-1 = 17

        process(r, delayed_input43_34, q, vector_r, unsigned_q)
        begin
                vector_r(17) <= ( (conv_std_logic_vector(r(18), rem_width)) & delayed_input43_34(1) & delayed_input43_34(0) );
                if ( r(18) >= 0 )  then 
                        unsigned_q(17) <= ( conv_unsigned(q(18), sqrt_width) & "01" );
                        r(17) <= ( conv_integer(signed(vector_r(17))) - conv_integer(unsigned_q(17)) );
                else
                        unsigned_q(17) <= ( conv_unsigned(q(18), sqrt_width) & "11" );
                        r(17) <= ( conv_integer(signed(vector_r(17))) + conv_integer(unsigned_q(17)) );
                end if;
 
 

                if ( r(17) >= 0 ) then 
                        q(17) <= conv_integer( unsigned(unsigned_q(17)(k_max downto 2) & '1') );
                else
                        q(17) <= conv_integer( unsigned(unsigned_q(17)(k_max downto 2) & '0') );
                end if;
        end process;


        converted_q17 <= conv_std_logic_vector(q(17), sqrt_width+3);
        converted_r17 <= conv_std_logic_vector(r(17), rem_width+3);

        Q17_PIPELINE_REG: data_register generic map (reg_width => sqrt_width+3)
                                        port map (clk => clk, reset => reset, data_in => converted_q17, data_out => pipelined_q17);
        R17_PIPELINE_REG: data_register generic map (reg_width => rem_width+3)
                                        port map (clk => clk, reset => reset, data_in => converted_r17, data_out => pipelined_r17);

        integer_q17 <= conv_integer( signed(pipelined_q17) );
        integer_r17 <= conv_integer( signed(pipelined_r17) );

                -------------------------------------------------------

-- k = k_max-1 = 16

        process(r, integer_r17, delayed_input33_24, integer_q17, vector_r, unsigned_q)
        begin
                vector_r(16) <= ( (conv_std_logic_vector(integer_r17, rem_width)) & delayed_input33_24(9) & delayed_input33_24(8) );
                if ( integer_r17 >= 0 )  then 
                        unsigned_q(16) <= ( conv_unsigned(integer_q17, sqrt_width) & "01" );
                        r(16) <= ( conv_integer(signed(vector_r(16))) - conv_integer(unsigned_q(16)) );
                else
                        unsigned_q(16) <= ( conv_unsigned(integer_q17, sqrt_width) & "11" );
                        r(16) <= ( conv_integer(signed(vector_r(16))) + conv_integer(unsigned_q(16)) );
                end if;
 
 

                if ( r(16) >= 0 ) then 
                        q(16) <= conv_integer( unsigned(unsigned_q(16)(k_max downto 2) & '1') );
                else
                        q(16) <= conv_integer( unsigned(unsigned_q(16)(k_max downto 2) & '0') );
                end if;
        end process;

                -------------------------------------------------------

-- k = k_max-1 = 15

        process(r, delayed_input33_24, q, vector_r, unsigned_q)
        begin

                vector_r(15) <= ( (conv_std_logic_vector(r(16), rem_width)) & delayed_input33_24(7) & delayed_input33_24(6) );
                if ( r(16) >= 0 )  then 
                        unsigned_q(15) <= ( conv_unsigned(q(16), sqrt_width) & "01" );
                        r(15) <= ( conv_integer(signed(vector_r(15))) - conv_integer(unsigned_q(15)) );
                else
                        unsigned_q(15) <= ( conv_unsigned(q(16), sqrt_width) & "11" );
                        r(15) <= ( conv_integer(signed(vector_r(15))) + conv_integer(unsigned_q(15)) );
                end if;
 
 

                if ( r(15) >= 0 ) then 
                        q(15) <= conv_integer( unsigned(unsigned_q(15)(k_max downto 2) & '1') );
                else
                        q(15) <= conv_integer( unsigned(unsigned_q(15)(k_max downto 2) & '0') );
                end if;
        end process;

                -------------------------------------------------------

-- k = k_max-1 = 14

        process(r, delayed_input33_24, q, vector_r, unsigned_q)
        begin
                vector_r(14) <= ( (conv_std_logic_vector(r(15), rem_width)) & delayed_input33_24(5) & delayed_input33_24(4) );
                if ( r(15) >= 0 )  then 
                        unsigned_q(14) <= ( conv_unsigned(q(15), sqrt_width) & "01" );
                        r(14) <= ( conv_integer(signed(vector_r(14))) - conv_integer(unsigned_q(14)) );
                else
                        unsigned_q(14) <= ( conv_unsigned(q(15), sqrt_width) & "11" );
                        r(14) <= ( conv_integer(signed(vector_r(14))) + conv_integer(unsigned_q(14)) );
                end if;
 
 

                if ( r(14) >= 0 ) then 
                        q(14) <= conv_integer( unsigned(unsigned_q(14)(k_max downto 2) & '1') );
                else
                        q(14) <= conv_integer( unsigned(unsigned_q(14)(k_max downto 2) & '0') );
                end if;
       end process;

                -------------------------------------------------------

-- k = k_max-1 = 13

        process(r, delayed_input33_24, q, vector_r, unsigned_q)
        begin
                vector_r(13) <= ( (conv_std_logic_vector(r(14), rem_width)) & delayed_input33_24(3) & delayed_input33_24(2) );
                if ( r(14) >= 0 )  then 
                        unsigned_q(13) <= ( conv_unsigned(q(14), sqrt_width) & "01" );
                        r(13) <= ( conv_integer(signed(vector_r(13))) - conv_integer(unsigned_q(13)) );
                else
                        unsigned_q(13) <= ( conv_unsigned(q(14), sqrt_width) & "11" );
                        r(13) <= ( conv_integer(signed(vector_r(13))) + conv_integer(unsigned_q(13)) );
                end if;
 
 

                if ( r(13) >= 0 ) then 
                        q(13) <= conv_integer( unsigned(unsigned_q(13)(k_max downto 2) & '1') );
                else
                        q(13) <= conv_integer( unsigned(unsigned_q(13)(k_max downto 2) & '0') );
                end if;
        end process;

                -------------------------------------------------------

-- k = k_max-1 = 12

        process(r, delayed_input33_24, q, vector_r, unsigned_q)
        begin
                vector_r(12) <= ( (conv_std_logic_vector(r(13), rem_width)) & delayed_input33_24(1) & delayed_input33_24(0) );
                if ( r(13) >= 0 )  then 
                        unsigned_q(12) <= ( conv_unsigned(q(13), sqrt_width) & "01" );
                        r(12) <= ( conv_integer(signed(vector_r(12))) - conv_integer(unsigned_q(12)) );
                else
                        unsigned_q(12) <= ( conv_unsigned(q(13), sqrt_width) & "11" );
                        r(12) <= ( conv_integer(signed(vector_r(12))) + conv_integer(unsigned_q(12)) );
                end if;
 
 

                if ( r(12) >= 0 ) then 
                        q(12) <= conv_integer( unsigned(unsigned_q(12)(k_max downto 2) & '1') );
                else
                        q(12) <= conv_integer( unsigned(unsigned_q(12)(k_max downto 2) & '0') );
                end if;
        end process;


        converted_q12 <= conv_std_logic_vector(q(12), sqrt_width+3);
        converted_r12 <= conv_std_logic_vector(r(12), rem_width+3);

        Q12_PIPELINE_REG: data_register generic map (reg_width => sqrt_width+3)
                                        port map (clk => clk, reset => reset, data_in => converted_q12, data_out => pipelined_q12);
        R12_PIPELINE_REG: data_register generic map (reg_width => rem_width+3)
                                        port map (clk => clk, reset => reset, data_in => converted_r12, data_out => pipelined_r12);

        integer_q12 <= conv_integer( signed(pipelined_q12) );
        integer_r12 <= conv_integer( signed(pipelined_r12) );

                -------------------------------------------------------

-- k = k_max-1 = 11

        process(r, integer_r12, delayed_input23_14, integer_q12, vector_r, unsigned_q)
        begin
                vector_r(11) <= ( (conv_std_logic_vector(integer_r12, rem_width)) & delayed_input23_14(9) & delayed_input23_14(8) );
                if ( integer_r12 >= 0 )  then 
                        unsigned_q(11) <= ( conv_unsigned(integer_q12, sqrt_width) & "01" );
                        r(11) <= ( conv_integer(signed(vector_r(11))) - conv_integer(unsigned_q(11)) );
                else
                        unsigned_q(11) <= ( conv_unsigned(integer_q12, sqrt_width) & "11" );
                        r(11) <= ( conv_integer(signed(vector_r(11))) + conv_integer(unsigned_q(11)) );
                end if;
 
 

                if ( r(11) >= 0 ) then 
                        q(11) <= conv_integer( unsigned(unsigned_q(11)(k_max downto 2) & '1') );
                else
                        q(11) <= conv_integer( unsigned(unsigned_q(11)(k_max downto 2) & '0') );
                end if;
        end process;

                -------------------------------------------------------

-- k = k_max-1 = 10

        process(r, delayed_input23_14, q, vector_r, unsigned_q)
        begin
                vector_r(10) <= ( (conv_std_logic_vector(r(11), rem_width)) & delayed_input23_14(7) & delayed_input23_14(6) );
                if ( r(11) >= 0 )  then 
                        unsigned_q(10) <= ( conv_unsigned(q(11), sqrt_width) & "01" );
                        r(10) <= ( conv_integer(signed(vector_r(10))) - conv_integer(unsigned_q(10)) );
                else
                        unsigned_q(10) <= ( conv_unsigned(q(11), sqrt_width) & "11" );
                        r(10) <= ( conv_integer(signed(vector_r(10))) + conv_integer(unsigned_q(10)) );
                end if;
 
 

                if ( r(10) >= 0 ) then 
                        q(10) <= conv_integer( unsigned(unsigned_q(10)(k_max downto 2) & '1') );
                else
                        q(10) <= conv_integer( unsigned(unsigned_q(10)(k_max downto 2) & '0') );
                end if;
        end process;

                -------------------------------------------------------

-- k = k_max-1 = 9

        process(r, delayed_input23_14, q, vector_r, unsigned_q)
        begin
                vector_r(9) <= ( (conv_std_logic_vector(r(10), rem_width)) & delayed_input23_14(5) & delayed_input23_14(4) );
                if ( r(10) >= 0 )  then 
                        unsigned_q(9) <= ( conv_unsigned(q(10), sqrt_width) & "01" );
                        r(9) <= ( conv_integer(signed(vector_r(9))) - conv_integer(unsigned_q(9)) );
                else
                        unsigned_q(9) <= ( conv_unsigned(q(10), sqrt_width) & "11" );
                        r(9) <= ( conv_integer(signed(vector_r(9))) + conv_integer(unsigned_q(9)) );
                end if;
 
 

                if ( r(9) >= 0 ) then 
                        q(9) <= conv_integer( unsigned(unsigned_q(9)(k_max downto 2) & '1') );
                else
                        q(9) <= conv_integer( unsigned(unsigned_q(9)(k_max downto 2) & '0') );
                end if;
        end process;

                -------------------------------------------------------

-- k = k_max-1 = 8

        process(r, delayed_input23_14, q, vector_r, unsigned_q)
        begin
                vector_r(8) <= ( (conv_std_logic_vector(r(9), rem_width)) & delayed_input23_14(3) & delayed_input23_14(2) );
                if ( r(9) >= 0 )  then 
                        unsigned_q(8) <= ( conv_unsigned(q(9), sqrt_width) & "01" );
                        r(8) <= ( conv_integer(signed(vector_r(8))) - conv_integer(unsigned_q(8)) );
                else
                        unsigned_q(8) <= ( conv_unsigned(q(9), sqrt_width) & "11" );
                        r(8) <= ( conv_integer(signed(vector_r(8))) + conv_integer(unsigned_q(8)) );
                end if;
 
 

                if ( r(8) >= 0 ) then 
                        q(8) <= conv_integer( unsigned(unsigned_q(8)(k_max downto 2) & '1') );
                else
                        q(8) <= conv_integer( unsigned(unsigned_q(8)(k_max downto 2) & '0') );
                end if;
        end process;

                -------------------------------------------------------

-- k = k_max-1 = 7

        process(r, delayed_input23_14, q, vector_r, unsigned_q)
        begin
                vector_r(7) <= ( (conv_std_logic_vector(r(8), rem_width)) & delayed_input23_14(1) & delayed_input23_14(0) );
                if ( r(8) >= 0 )  then 
                        unsigned_q(7) <= ( conv_unsigned(q(8), sqrt_width) & "01" );
                        r(7) <= ( conv_integer(signed(vector_r(7))) - conv_integer(unsigned_q(7)) );
                else
                        unsigned_q(7) <= ( conv_unsigned(q(8), sqrt_width) & "11" );
                        r(7) <= ( conv_integer(signed(vector_r(7))) + conv_integer(unsigned_q(7)) );
                end if;
 
 

                if ( r(7) >= 0 ) then 
                        q(7) <= conv_integer( unsigned(unsigned_q(7)(k_max downto 2) & '1') );
                else
                        q(7) <= conv_integer( unsigned(unsigned_q(7)(k_max downto 2) & '0') );
                end if;
        end process;

        converted_q7 <= conv_std_logic_vector(q(7), sqrt_width+3);
        converted_r7 <= conv_std_logic_vector(r(7), rem_width+3);

        Q7_PIPELINE_REG: data_register generic map (reg_width => sqrt_width+3)
                                        port map (clk => clk, reset => reset, data_in => converted_q7, data_out => pipelined_q7);
        R7_PIPELINE_REG: data_register generic map (reg_width => rem_width+3)
                                        port map (clk => clk, reset => reset, data_in => converted_r7, data_out => pipelined_r7);

        integer_q7 <= conv_integer( signed(pipelined_q7) );
        integer_r7 <= conv_integer( signed(pipelined_r7) );

                -------------------------------------------------------

-- k = k_max-1 = 6

        process(r, integer_r7, delayed_input13_4, integer_q7, vector_r, unsigned_q)
        begin
                vector_r(6) <= ( (conv_std_logic_vector(integer_r7, rem_width)) & delayed_input13_4(9) & delayed_input13_4(8) );
                if ( integer_r7 >= 0 )  then 
                        unsigned_q(6) <= ( conv_unsigned(integer_q7, sqrt_width) & "01" );
                        r(6) <= ( conv_integer(signed(vector_r(6))) - conv_integer(unsigned_q(6)) );
                else
                        unsigned_q(6) <= ( conv_unsigned(integer_q7, sqrt_width) & "11" );
                        r(6) <= ( conv_integer(signed(vector_r(6))) + conv_integer(unsigned_q(6)) );
                end if;
 
 

                if ( r(6) >= 0 ) then 
                        q(6) <= conv_integer( unsigned(unsigned_q(6)(k_max downto 2) & '1') );
                else
                        q(6) <= conv_integer( unsigned(unsigned_q(6)(k_max downto 2) & '0') );
                end if;
        end process;

                -------------------------------------------------------

-- k = k_max-1 = 5

        process(r, delayed_input13_4, q, vector_r, unsigned_q)
        begin
                vector_r(5) <= ( (conv_std_logic_vector(r(6), rem_width)) & delayed_input13_4(7) & delayed_input13_4(6) );
                if ( r(6) >= 0 )  then 
                        unsigned_q(5) <= ( conv_unsigned(q(6), sqrt_width) & "01" );
                        r(5) <= ( conv_integer(signed(vector_r(5))) - conv_integer(unsigned_q(5)) );
                else
                        unsigned_q(5) <= ( conv_unsigned(q(6), sqrt_width) & "11" );
                        r(5) <= ( conv_integer(signed(vector_r(5))) + conv_integer(unsigned_q(5)) );
                end if;
 
 

                if ( r(5) >= 0 ) then 
                        q(5) <= conv_integer( unsigned(unsigned_q(5)(k_max downto 2) & '1') );
                else
                        q(5) <= conv_integer( unsigned(unsigned_q(5)(k_max downto 2) & '0') );
                end if;
        end process;

                -------------------------------------------------------

-- k = k_max-1 = 4

        process(r, delayed_input13_4, q, vector_r, unsigned_q)
        begin
                vector_r(4) <= ( (conv_std_logic_vector(r(5), rem_width)) & delayed_input13_4(5) & delayed_input13_4(4) );
                if ( r(5) >= 0 )  then 
                        unsigned_q(4) <= ( conv_unsigned(q(5), sqrt_width) & "01" );
                        r(4) <= ( conv_integer(signed(vector_r(4))) - conv_integer(unsigned_q(4)) );
                else
                        unsigned_q(4) <= ( conv_unsigned(q(5), sqrt_width) & "11" );
                        r(4) <= ( conv_integer(signed(vector_r(4))) + conv_integer(unsigned_q(4)) );
                end if;
 
 

                if ( r(4) >= 0 ) then 
                        q(4) <= conv_integer( unsigned(unsigned_q(4)(k_max downto 2) & '1') );
                else
                        q(4) <= conv_integer( unsigned(unsigned_q(4)(k_max downto 2) & '0') );
                end if;
        end process;

                -------------------------------------------------------

-- k = k_max-1 = 3

        process(r, delayed_input13_4, q, vector_r, unsigned_q)
        begin
                vector_r(3) <= ( (conv_std_logic_vector(r(4), rem_width)) & delayed_input13_4(3) & delayed_input13_4(2) );
                if ( r(4) >= 0 )  then 
                        unsigned_q(3) <= ( conv_unsigned(q(4), sqrt_width) & "01" );
                        r(3) <= ( conv_integer(signed(vector_r(3))) - conv_integer(unsigned_q(3)) );
                else
                        unsigned_q(3) <= ( conv_unsigned(q(4), sqrt_width) & "11" );
                        r(3) <= ( conv_integer(signed(vector_r(3))) + conv_integer(unsigned_q(3)) );
                end if;
 
 

                if ( r(3) >= 0 ) then 
                        q(3) <= conv_integer( unsigned(unsigned_q(3)(k_max downto 2) & '1') );
                else
                        q(3) <= conv_integer( unsigned(unsigned_q(3)(k_max downto 2) & '0') );
                end if;
       end process;

                -------------------------------------------------------

-- k = k_max-1 = 2

        process(r, delayed_input13_4, q, vector_r, unsigned_q)
        begin
                vector_r(2) <= ( (conv_std_logic_vector(r(3), rem_width)) & delayed_input13_4(1) & delayed_input13_4(0) );
                if ( r(3) >= 0 )  then 
                        unsigned_q(2) <= ( conv_unsigned(q(3), sqrt_width) & "01" );
                        r(2) <= ( conv_integer(signed(vector_r(2))) - conv_integer(unsigned_q(2)) );
                else
                        unsigned_q(2) <= ( conv_unsigned(q(3), sqrt_width) & "11" );
                        r(2) <= ( conv_integer(signed(vector_r(2))) + conv_integer(unsigned_q(2)) );
                end if;
 
 

                if ( r(2) >= 0 ) then 
                        q(2) <= conv_integer( unsigned(unsigned_q(2)(k_max downto 2) & '1') );
                else
                        q(2) <= conv_integer( unsigned(unsigned_q(2)(k_max downto 2) & '0') );
                end if;
        end process;

        converted_q2 <= conv_std_logic_vector(q(2), sqrt_width+3);
        converted_r2 <= conv_std_logic_vector(r(2), rem_width+3);

        Q2_PIPELINE_REG: data_register generic map (reg_width => sqrt_width+3)
                                        port map (clk => clk, reset => reset, data_in => converted_q2, data_out => pipelined_q2);
        R2_PIPELINE_REG: data_register generic map (reg_width => rem_width+3)
                                        port map (clk => clk, reset => reset, data_in => converted_r2, data_out => pipelined_r2);

        integer_q2 <= conv_integer( signed(pipelined_q2) );
        integer_r2 <= conv_integer( signed(pipelined_r2) );

                -------------------------------------------------------

-- k = k_max-1 = 1

        process(r, integer_r2, delayed_input3_0, integer_q2, vector_r, unsigned_q)
        begin
                vector_r(1) <= ( (conv_std_logic_vector(integer_r2, rem_width)) & delayed_input3_0(3) & delayed_input3_0(2) );
                if ( integer_r2 >= 0 )  then 
                        unsigned_q(1) <= ( conv_unsigned(integer_q2, sqrt_width) & "01" );
                        r(1) <= ( conv_integer(signed(vector_r(1))) - conv_integer(unsigned_q(1)) );
                else
                        unsigned_q(1) <= ( conv_unsigned(integer_q2, sqrt_width) & "11" );
                        r(1) <= ( conv_integer(signed(vector_r(1))) + conv_integer(unsigned_q(1)) );
                end if;
 
 

                if ( r(1) >= 0 ) then 
                        q(1) <= conv_integer( unsigned(unsigned_q(1)(k_max downto 2) & '1') );
                else
                        q(1) <= conv_integer( unsigned(unsigned_q(1)(k_max downto 2) & '0') );
                end if;
        end process;

                -------------------------------------------------------

-- k = k_max-1 = 0

        process(r, delayed_input3_0, q, vector_r, unsigned_q)
        begin                
                vector_r(0) <= ( (conv_std_logic_vector(r(1), rem_width)) & delayed_input3_0(1) & delayed_input3_0(0) );
                if ( r(1) >= 0 )  then 
                        unsigned_q(0) <= ( conv_unsigned(q(1), sqrt_width) & "01" );
                        r(0) <= ( conv_integer(signed(vector_r(0))) - conv_integer(unsigned_q(0)) );
                else
                        unsigned_q(0) <= ( conv_unsigned(q(1), sqrt_width) & "11" );
                        r(0) <= ( conv_integer(signed(vector_r(0))) + conv_integer(unsigned_q(0)) );
                end if;
 
 

                if ( r(0) >= 0 ) then 
                        q(0) <= conv_integer( unsigned(unsigned_q(0)(k_max downto 2) & '1') );
                else
                        q(0) <= conv_integer( unsigned(unsigned_q(0)(k_max downto 2) & '0') );
                end if;
       end process;
  
            -------------------------------------------------------



        process(q, vector_q0, r, q01)
        begin

            vector_q0 <= conv_std_logic_vector(q(0), sqrt_width);    
            q01 <= conv_integer( unsigned(vector_q0 & '1') );      

            if (r(0) < 0) then
                    remainder <= conv_std_logic_vector(r(0) + q01, sqrt_width+2);
            else
                    remainder <= conv_std_logic_vector(r(0), sqrt_width+2);
            end if;

       end process;

------------------------------------------------------------------------

        output <= conv_std_logic_vector(q(0), sqrt_width); 
 
-----------------------------------------------------------------------------------------------------------------       

end rtl;

