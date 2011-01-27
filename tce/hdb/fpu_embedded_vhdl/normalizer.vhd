-----------------------------------------------------------------------------------------------------------
--
--				                   DENORMALS HANDLER
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

entity normalizer is
    port( denormal_operand    : in  std_logic_vector(word_width-2 downto 0);
          normalized_mantissa : out std_logic_vector(23 downto 0);
          normalized_exponent : out std_logic_vector(8 downto 0)
        ); 
end normalizer;

-------------------------------------------------------------------------------
architecture rtl of normalizer is
    
    signal operand              : std_logic_vector(word_width-2 downto 0);
    signal mantissa             : std_logic_vector(23 downto 0);
    signal exponent             : std_logic_vector(8 downto 0);
    signal operand_exp          : std_logic_vector(7 downto 0);

    begin

        operand_exp <= operand(30 downto 23);
        operand     <= denormal_operand;

        normalized_mantissa <= mantissa;
        normalized_exponent <= exponent;
        
-------------------------------------------------------------------------------
-- operand prenormalization
-------------------------------------------------------------------------------

        process(operand, operand_exp) 
                
        begin
            if ( (operand_exp = "00000000") and (operand(22 downto 0) /= Conv_std_logic_vector(0,23)) ) then
                    -- Flush denormal operands to zero
                    mantissa<= conv_std_logic_vector(0,24);      
                    exponent<= conv_std_logic_vector(0,9);  -- zero
            else
                    -- normalized operand_a
                    mantissa <= ('1' & operand(22 downto 0) );
                    -- exponent <= ( unsigned(extended_operand_exp) + unsigned(biasing_value) );  -- newly biased exponent; biasing_value = 128
                    exponent(8) <= operand_exp(7);
                    exponent(7) <= not (operand_exp(7));
                    exponent(6) <= operand_exp(6);
                    exponent(5) <= operand_exp(5);
                    exponent(4) <= operand_exp(4);
                    exponent(3) <= operand_exp(3);
                    exponent(2) <= operand_exp(2);
                    exponent(1) <= operand_exp(1);
                    exponent(0) <= operand_exp(0);
            end if;

        end process;

    end rtl;
