-----------------------------------------------------------------------------------------------------------
--
--				           SINGLE PRECISION FP NUMBERS COMPARATOR
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

entity sp_fcomparator is
    port( clk,reset,enable              : in  std_logic;
          unordered_query,equal_query,
          less_query,invalid_enable     : in std_logic;
          opa,opb                       : in  std_logic_vector(word_width-1 downto 0);
          comparison_result             : out std_logic_vector(word_width-1 downto 0);
          exc_invalid_operation_compare : out std_logic ); 
end sp_fcomparator ;



architecture rtl of sp_fcomparator is
    
    signal reg_out_opa, 
           reg_out_opb       : std_logic_vector(word_width-1 downto 0);   
    signal selected_opa, 
           selected_opb      : std_logic_vector(word_width-1 downto 0);
    signal current_opcode, 
           selected_opcode,
           reg_out_opcode    : std_logic_vector(3 downto 0);
    signal less,equal,
           unordered,
           condition         : std_logic;
    signal selected_opcode0,
           selected_opcode1,
           selected_opcode2,
           selected_opcode3  : std_logic;
    signal unsigned_zero     : std_logic_vector(word_width-2 downto 0);
    signal c_result          : std_logic_vector(word_width-1 downto 0);
    
    begin

-----------------------------------------------------------------------------------------------------------------          

        unsigned_zero     <= conv_std_logic_vector(0,31);

        current_opcode(0) <= unordered_query;
        current_opcode(1) <= equal_query;
        current_opcode(2) <= less_query;
        current_opcode(3) <= invalid_enable;

        selected_opcode0  <= selected_opcode(0);
        selected_opcode1  <= selected_opcode(1);
        selected_opcode2  <= selected_opcode(2);
        selected_opcode3  <= selected_opcode(3);

-----------------
-- INPUT STAGE
-----------------

-- This logic is used to latch the last loaded operands until new ones are sent to this functional unit
-- (allows low power consumption by avoiding unwanted commutations into this unit when it's not used)

        opa_LATCHING_REG: we_register port map (clk => clk, reset => reset, we => enable,
                                                data_in => selected_opa, data_out => reg_out_opa);

        opa_INPUT_MUX: MY_MUX generic map (width => word_width) 
                              port map (in_a => reg_out_opa, in_b => opa, sel => enable, output => selected_opa);

        opb_LATCHING_REG: we_register port map (clk => clk, reset => reset, we => enable,
                                                data_in => selected_opb, data_out => reg_out_opb);

        opb_INPUT_MUX: MY_MUX generic map (width => word_width) 
                              port map (in_a => reg_out_opb, in_b => opb, sel => enable, output => selected_opb);


        opcode_LATCHING_REG: we_register generic map (reg_width => (cop_opcode_width-2))
                                         port map (clk => clk, reset => reset, we => enable,
                                                   data_in => selected_opcode, data_out => reg_out_opcode);

        opcode_INPUT_MUX_0: simple_mux_2 port map (in_a => reg_out_opcode(0), in_b => current_opcode(0), sel => enable,
                                                   output => selected_opcode(0));

        opcode_INPUT_MUX_1: simple_mux_2 port map (in_a => reg_out_opcode(1), in_b => current_opcode(1), sel => enable,
                                                   output => selected_opcode(1));

        opcode_INPUT_MUX_2: simple_mux_2 port map (in_a => reg_out_opcode(2), in_b => current_opcode(2), sel => enable,
                                                   output => selected_opcode(2));

        opcode_INPUT_MUX_3: simple_mux_2 port map (in_a => reg_out_opcode(3), in_b => current_opcode(3), sel => enable,
                                                   output => selected_opcode(3));

-----------------------------
-- RESULT GENERATION STAGE 
-----------------------------

        SPECIAL_OP_DETECTION: process(selected_opa,selected_opb,selected_opcode3,unsigned_zero)  
        begin

            if ( ((selected_opa(30 downto 23)="11111111") and (selected_opa(22 downto 0)/=conv_std_logic_vector(0,23)))
                   or ((selected_opb(30 downto 23)="11111111") and (selected_opb(22 downto 0)/=conv_std_logic_vector(0,23))) ) then
                        -- at least one operand is a NaN -> invalid operation exception is raised in certain conditions; output is unordered
                        less      <= '0';
                        equal     <= '0';
                        unordered <= '1';
                        if (selected_opcode3='1') then
                                exc_invalid_operation_compare <= '1';
                        else
                                exc_invalid_operation_compare <= '0';
                        end if;

            elsif ( (selected_opa(30 downto 0)=unsigned_zero) and (selected_opb(30 downto 0)=unsigned_zero) ) then
                        -- both of the operands are null, so they are "equal" regardless of their sign
                        less      <= '0';
                        equal     <= '1';  
                        unordered <= '0';
                        exc_invalid_operation_compare <= '0';

            else
                        -- default
                        if ( (selected_opa(31)='0') and (selected_opb(31)='0') ) then
                                -- both of the operands are positive
                                if ( selected_opa(30 downto 0) < selected_opb(30 downto 0) ) then
                                        less <= '1';
                                else
                                        less <= '0';
                                end if;
                                if ( selected_opa(30 downto 0) = selected_opb(30 downto 0) ) then
                                        equal <= '1';
                                else
                                        equal <= '0';
                                end if;
                        elsif ( (selected_opa(31)='0') and (selected_opb(31)='1') ) then
                                -- opa is positive and opb is negative
                                less      <= '0';
                                equal     <= '0';
                        elsif ( (selected_opa(31)='1') and (selected_opb(31)='0') ) then
                                -- opa is negative and opb is positive
                                less      <= '1';
                                equal     <= '0';
                        else 
                                -- both of the operands are negative
                                if ( selected_opa(30 downto 0) > selected_opb(30 downto 0) ) then
                                        less <= '1';
                                else
                                        less <= '0';
                                end if;
                                if ( selected_opa(30 downto 0) = selected_opb(30 downto 0) ) then
                                        equal <= '1';
                                else
                                        equal <= '0';
                                end if;
                        end if;
                        unordered <= '0';
                        exc_invalid_operation_compare <= '0';
            end if;

        end process;


        COMPARISON_RESULT_GEN: process(less,equal,unordered,selected_opcode2,selected_opcode1,selected_opcode0)
            begin

                if ( ((selected_opcode2='1')and(less='1')) or ((selected_opcode1='1')and(equal='1')) or ((selected_opcode0='1')and(unordered='1')) ) then
                        condition <= '1';
                else
                        condition <= '0';
                end if;

            end process;   


        c_result <= ( condition & equal & less & unordered & conv_std_logic_vector(0,28) );
        comparison_result <= c_result;

 
-----------------------------------------------------------------------------------------------------------------       

end rtl;
