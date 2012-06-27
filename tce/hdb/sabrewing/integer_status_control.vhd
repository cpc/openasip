------------------------------------------------------------------------------------------------
-- Copyright (c) 2011 Tom M. Bruintjes
-- All rights reserved.

-- Redistribution and use in source and binary forms, with or without 
-- modification, are permitted provided that the following conditions are met:
--     * Redistributions of source code must retain the above copyright
--       notice, this list of conditions and the following disclaimer.
--     * Redistributions in binary form must reproduce the above copyright
--       notice, this list of conditions and the following disclaimer in the
--       documentation and/or other materials provided with the distribution.
--     * Neither the name of the copyright holder nor the
--       names of its contributors may be used to endorse or promote products
--       derived from this software without specific prior written permission.

-- THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER ``AS IS'' AND ANY
-- EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
-- IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
-- PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE
-- LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
-- CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
-- SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
-- BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
-- WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
-- OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
-- IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
-- ------------------------------------------------------------------------------------------------
-- ------------------------------------------------------------------------------------------------
--  A  simple  check  to see  if integer  arithmetic  exceeds  32 bits.  Since the  input is  two's
--  complement, it is fairly easy to see if a result needs more than 32 bits.  Whenever the 32 MSBs
--  are either all '0' or all '1', the result has been sign extended to 64 bits and the 32 MSBs are
--  not needed.  The status  bits will  be updated  by this  simple check to indicate if the result
--  occupies more than 32 bits.  The user  may then  decide  not to store the MSBs to save space in
--  memory.
-- ------------------------------------------------------------------------------------------------
library ieee;
use ieee.numeric_std.all;
use ieee.std_logic_1164.all;
use work.alu_pkg_lvl2.all;

entity integer_status_control is
  port(
    int_float_in : in std_logic; -- floating-point or integer operand
    arithmetic_logic_in : in std_logic; -- arithmetic or logic operation
    status_in : in std_logic_vector(STATUSWIDTH-1 downto 0); -- status as determined in the first stage (logical compare)
    left_in : in std_logic_vector(SIGNIFICANDWIDTH-1 downto 0); -- MSBs of the addition result
    status_out : out std_logic_vector(STATUSWIDTH-1 downto 0) -- new status (updated for integer arithmetic)
  );
end integer_status_control;

architecture rtl of integer_status_control is

begin

  combinatorial : process(int_float_in, arithmetic_logic_in, status_in, left_in)
  
  begin
 
  if(int_float_in = '1' and arithmetic_logic_in = '0') then -- integer arithmetic
    if((or_reduce(left_in) = '0') or (and_reduce(left_in) = '1')) then -- fits in a single register (upper bits are merely a sing extension)
      status_out <= STATUS_ARITHMETIC_DEFAULT;
    else -- does not fit in a single register
      status_out <= STATUS_ARITHMETIC_EXCEEDS;
    end if;
  else -- status is determined elsewhere
    status_out <= status_in;
  end if;
    
  end process; 

end rtl;
