-----------------------------------------------------------------------------------------------------------
--
--				         INTERNAL RESULTS PACKING (AND ROUNDING) LOGIC
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


-- Removed special case handling according to OpenCL Embedded. 
-- If the result should be denormal, +INF, -INF or NAN, it is 0.
-- Also changed rounding to "round to zero".
--  -Timo


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use work.cop_definitions.all;
use work.cop_components.all;

entity add_packer is
    port( internal_exponent      : in std_logic_vector(8 downto 0);
          internal_mantissa      : in std_logic_vector(47 downto 0);
          packed_exponent        : out std_logic_vector(7 downto 0);
          packed_fractional_part : out std_logic_vector(22 downto 0);
          exc_overflow           : out std_logic;
          exc_underflow          : out std_logic;
          exc_inexact            : out std_logic
        ); 
end add_packer;

-------------------------------------------------------------------------------
architecture rtl of add_packer is
    
    signal internal_e            : std_logic_vector(8 downto 0);
    signal internal_m            : std_logic_vector(47 downto 0);
    signal exp                   : std_logic_vector(8 downto 0);
    signal m                     : std_logic_vector(23 downto 0);
    signal rounded_add           : std_logic_vector(24 downto 0);
    signal x                     : std_logic_vector(7 downto 0);
    signal f                     : std_logic_vector(22 downto 0);
    signal sgl_exc_overflow     : std_logic;
    signal sgl_exc_underflow     : std_logic;
    signal result_inexact        : std_logic;
    signal rounding_inexact      : std_logic;
    signal sgl_inexact_addiction : std_logic; 

    begin

            internal_e <= internal_exponent;
            internal_m <= internal_mantissa;
            packed_exponent        <= x;
            packed_fractional_part <= f;
            exc_overflow  <= sgl_exc_overflow;
            exc_underflow <= sgl_exc_underflow;
            exc_inexact   <= sgl_inexact_addiction;
            

-- special result detection and result packing into single precision floating-point format:

    SUM_RESULT_GEN: process(internal_e, internal_m, rounded_add, exp, m)
    variable c : integer RANGE 0 to 16777215;
    variable d : integer RANGE 0 to 8388607;
    begin


----------------------------------------------------------------------------

-- result is  always converted into a single precision normalized numbers:

          if ( (Conv_integer(unsigned(internal_e)) < -126 + 255) ) then
  -- Denormal numbers are flushed to zero -Timo
                x <= (others => '0');
                f <= (others => '0');  -- ZERO
                sgl_exc_overflow <= '0'; sgl_exc_underflow <= '1'; result_inexact   <= '1'; rounding_inexact <= '0';
                rounded_add <= (others => '0'); exp <= (others => '0'); m <= (others => '0');

        elsif ( (conv_integer(unsigned(internal_e)) = 127 + 255) and ( internal_m(47 downto 23) = "1111111111111111111111111") ) then
                -- normalized --> overflow limit zone
-- invalid operand or overflow; NaN is generated
                    rounded_add <= (others => '0');
                    exp <= (others => '0');
                    m <= (others => '0');
                    x <= "11111111";
                    f <= "00000000000000000000001";
                    sgl_exc_overflow <= '0'; sgl_exc_underflow <= '0'; result_inexact <= '0'; rounding_inexact <= '0';

-- rounding 

            elsif ( (conv_integer(unsigned(internal_e)) < 128 + 255) and (conv_integer(unsigned(internal_e)) > -127 + 255) ) then
                    c := Conv_integer(  unsigned(internal_m(47 downto 24))  );                                           
                    if internal_m(23 downto 0) = Conv_std_logic_vector(0,24) then
                            -- only one exact case!
                            rounded_add <= Conv_std_logic_vector(c,25);
                            rounding_inexact <= '0';
                    else -- ROUND TO ZERO -Timo                                              
                            rounded_add <= Conv_std_logic_vector(c,25);
                            rounding_inexact <= '1';
                    --elsif ( (internal_m(23) = '0') or ( internal_m(24) = '0' and internal_m(23) = '1' and internal_m(22 downto 0) = Conv_std_logic_vector(0,23) ) ) then
		              --  rounded_add <= Conv_std_logic_vector(c,25);
                    --        rounding_inexact <= '1';
                    --else 									                   
                    --        rounded_add <= Conv_std_logic_vector(c+1,25);
                    --        rounding_inexact <= '1';
                       
                    end if;
                            
                    ----------------------------------------------------------

-- If the rounding makes the mantissa to overflow, then result has to be re-adjusted

                    if rounded_add(24) = '1' then
                            -- overflow => SHR(rounded_add), exp <- (exp + 1)
                            m <= rounded_add(24 downto 1);   -- note: LSB is lost, but there's no information loss in doing this, since the lost bit is zero                                 
                            exp <= ( unsigned(internal_e) + 1 );
                    else
                            -- no overflow
                            m <= rounded_add(23 downto 0);
                            exp <= internal_e;
                    end if;

                    ----------------------------------------------------------

-- normalized result packing

                    x <= conv_std_logic_vector( unsigned(exp) -255 + 127, 8 );
                    f <= m(22 downto 0);                                             
                    sgl_exc_overflow <= '0'; sgl_exc_underflow <= '0'; result_inexact   <= '0';
 
                    ----------------------------------------------------------

            else
-- invalid operand or overflow; NaN is generated
                    rounded_add <= (others => '0');
                    exp <= (others => '0');
                    m <= (others => '0');
                    x <= "11111111";
                    f <= "00000000000000000000001";
                    sgl_exc_overflow <= '0'; sgl_exc_underflow <= '0'; result_inexact <= '0'; rounding_inexact <= '0';

            end if;

        end process;   


-- All "inexact result warning" internal signals are put in logical OR:

        sgl_inexact_addiction <= ( result_inexact or rounding_inexact);


    end rtl;
