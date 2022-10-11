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
--  Encodes  the LZA vector  to a binary number  representing  the shift  amount for  normalization
--  (i.e.,  count the number of leading zeros).  A hierarchical tree tructure was  chosen  [1]  for
--  energy efficiency and speed. The LZD is built up from small blocks that count the leading zeros
--  for  a two bit vector.   These small blocks  are connected to  form a  4-bit counter,  an 8-bit
--  counter etc. etc.   Once a 64 bit counter is  achieved, a 32-bit  counter is connected to  form
--  a 96-bit counter. Then a 4-bit and 2-bit counter are added to form a 102-bit counter,  only one
--  bit more  than the absolute minimum needed  for the  32-bit significand  custom  floating-point
--  format (3*(32+1) + 2 = 101).
--
--  !!AS IS: This component does not YET scale along when the significand of the floating-point
--           format. Significands < 32-bit are padded with 0's, significands > 0 are not supported.
--           --> WILL BE FIXED IN THE NEXT VERSION!!
--
--  [1] An algorithmic and Novel Design of Leading Zero Detector Circuit: Comparison with logic
--      synthesis by Oklobdzija
-- ------------------------------------------------------------------------------------------------

------------ BASIC TWO INPUT CELL (2) --------------------

library ieee;
use ieee.numeric_std.all;
use ieee.std_logic_1164.all;

entity lzd2 is
  port(
    pattern2_in : in std_logic_vector(1 downto 0);
    position_out : out std_logic;
    validity_out : out std_logic
    );
end lzd2;
    
architecture rtl of lzd2 is

begin

  -- Position_out indicates the number of leading zeros, hence if MSB = '1' then position = 0.
  position_out <= '0' when pattern2_in(pattern2_in'left) = '1' else
                  '1'; -- don't care if valid bit is '1'
  -- Validity_out indicates if vector was all zeros.
  validity_out <= '0' when pattern2_in = "00" else
                  '1';

end rtl;

------------ EXTEND TO FOUR (4) --------------------

library ieee;
use ieee.numeric_std.all;
use ieee.std_logic_1164.all;

entity lzd4 is
  port(
    pattern4_in : in std_logic_vector(3 downto 0);
    position_out : out std_logic_vector(1 downto 0);
    validity_out : out std_logic
    );
end lzd4;
    
architecture rtl of lzd4 is

  signal pleft, vleft, pright, vright : std_logic;

begin

  -- instantiate a 2-bit LZA
  u_lzd2_left : entity work.lzd2(rtl)
  port map(
    pattern2_in => pattern4_in(3 downto 2),
    position_out => pleft,
    validity_out => vleft
  );
  
  -- instantiate a 2-bit LZA
  u_lzd2_right : entity work.lzd2(rtl)
  port map(
    pattern2_in => pattern4_in(1 downto 0),
    position_out => pright,
    validity_out => vright
  );

  combinatorial : process(pleft, vleft, pright, vright)
  
  begin
  
    -- connect 2-bit LZAs [1]
    validity_out <= vleft or vright;
    if (vleft = '1') then
      position_out <= not(vleft) & pleft;
    else
      position_out <= not(vleft) & pright;
    end if;
    
  end process;

end rtl;

----- ALTERNATIVE (DIRECT) IMPLEMENTATION (not faster than multiplexers)

-- library ieee;
-- use ieee.numeric_std.all;
-- use ieee.std_logic_1164.all;

-- architecture rtl2 of lzd4 is

-- begin
  
  -- combinatorial : process(pattern4_in)
  
    -- variable position : std_logic_vector(1 downto 0);
  
  -- begin
  
    -- position(0) := pattern4_in(pattern4_in'left) nor pattern4_in(pattern4_in'left-1);
    -- position(1) := (not(pattern4_in(pattern4_in'left-1)) nor pattern4_in(pattern4_in'left)) or (pattern4_in(pattern4_in'left) nor pattern4_in(pattern4_in'left-2));
    -- if(position = "00") then
      -- validity_out <= '0';
    -- else
      -- validity_out <= '1';
    -- end if;
    -- position_out <= position;

  -- end process;

-- end rtl2;

------------ EXTEND TO SIX (6 -> 4 + 2) --------------------

library ieee;
use ieee.numeric_std.all;
use ieee.std_logic_1164.all;

entity lzd6 is
  port(
    pattern6_in : std_logic_vector(5 downto 0);
    position_out : out std_logic_vector(2 downto 0);
    validity_out : out std_logic
    );
end lzd6;
    
architecture rtl of lzd6 is

  signal vleft, vright, pright : std_logic;
  signal pleft : std_logic_vector(1 downto 0);
  
begin

  u_lzd4_left : entity work.lzd4(rtl)
  port map(
    pattern4_in => pattern6_in(5 downto 2),
    position_out => pleft,
    validity_out => vleft
  );
  
  u_lzd2_right : entity work.lzd2(rtl)
  port map(
    pattern2_in => pattern6_in(1 downto 0),
    position_out => pright,
    validity_out => vright
  );

  combinatorial : process(pleft, vleft, pright, vright)
  
  begin
  
    validity_out <= vleft or vright;
    if (vleft = '1') then
      position_out <= not(vleft) & pleft;
    else
      position_out <= not(vleft) & '0' & pright; -- resize to 3 bits
    end if;
    
  end process;

end rtl;


------------ EXTEND TO EIGTH (8) --------------------

library ieee;
use ieee.numeric_std.all;
use ieee.std_logic_1164.all;

entity lzd8 is
  port(
    pattern8_in : std_logic_vector(7 downto 0);
    position_out : out std_logic_vector(2 downto 0);
    validity_out : out std_logic
    );
end lzd8;
    
architecture rtl of lzd8 is

  signal vleft, vright : std_logic;
  signal pleft, pright : std_logic_vector(1 downto 0);
  
begin

  u_lzd4_left : entity work.lzd4(rtl)
  port map(
    pattern4_in => pattern8_in(7 downto 4),
    position_out => pleft,
    validity_out => vleft
  );
  
  u_lzd4_right : entity work.lzd4(rtl)
  port map(
    pattern4_in => pattern8_in(3 downto 0),
    position_out => pright,
    validity_out => vright
  );

  combinatorial : process(pleft, vleft, pright, vright)
  
  begin
  
    validity_out <= vleft or vright;
    if (vleft = '1') then
      position_out <= not(vleft) & pleft;
    else
      position_out <= not(vleft) & pright;
    end if;
    
  end process;

end rtl;

------------ EXTEND TO SIXTEEN (16) --------------------

library ieee;
use ieee.numeric_std.all;
use ieee.std_logic_1164.all;

entity lzd16 is
  port(
    pattern16_in : std_logic_vector(15 downto 0);
    position_out : out std_logic_vector(3 downto 0);
    validity_out : out std_logic
    );
end lzd16;
    
architecture rtl of lzd16 is

  signal vleft, vright : std_logic;
  signal pleft, pright : std_logic_vector(2 downto 0);
  
begin

  u_lzd8_left : entity work.lzd8(rtl)
  port map(
    pattern8_in => pattern16_in(15 downto 8),
    position_out => pleft,
    validity_out => vleft
  );
  
  u_lzd8_right : entity work.lzd8(rtl)
  port map(
    pattern8_in => pattern16_in(7 downto 0),
    position_out => pright,
    validity_out => vright
  );

  combinatorial : process(pleft, vleft, pright, vright)

  begin

    validity_out <= vleft or vright;
    if (vleft = '1') then
      position_out <= not(vleft) & pleft;
    else
      position_out <= not(vleft) & pright;
    end if;
    
  end process;

end rtl;

------------ EXTEND TO THIRTYTWO (32) --------------------

library ieee;
use ieee.numeric_std.all;
use ieee.std_logic_1164.all;

entity lzd32 is
  port(
    pattern32_in : std_logic_vector(31 downto 0);
    position_out : out std_logic_vector(4 downto 0);
    validity_out : out std_logic
    );
end lzd32;
    
architecture rtl of lzd32 is

  signal vleft, vright : std_logic;
  signal pleft, pright : std_logic_vector(3 downto 0);
  
begin

  u_lzd16_left : entity work.lzd16(rtl)
  port map(
    pattern16_in => pattern32_in(31 downto 16),
    position_out => pleft,
    validity_out => vleft
  );
  
  u_lzd16_right : entity work.lzd16(rtl)
  port map(
    pattern16_in => pattern32_in(15 downto 0),
    position_out => pright,
    validity_out => vright
  );

  combinatorial : process(pleft, vleft, pright, vright)

  begin

    validity_out <= vleft or vright;
    if (vleft = '1') then
      position_out <= not(vleft) & pleft;
    else
      position_out <= not(vleft) & pright;
    end if;
    
  end process;

end rtl;

------------ EXTEND TO SIXTYFOUR (64) --------------------

library ieee;
use ieee.numeric_std.all;
use ieee.std_logic_1164.all;

entity lzd64 is
  port(
    pattern64_in : std_logic_vector(63 downto 0);
    position_out : out std_logic_vector(5 downto 0);
    validity_out : out std_logic
    );
end lzd64;
    
architecture rtl of lzd64 is

  signal vleft, vright : std_logic;
  signal pleft, pright : std_logic_vector(4 downto 0);
  
begin

  u_lzd32_left : entity work.lzd32(rtl)
  port map(
    pattern32_in => pattern64_in(63 downto 32),
    position_out => pleft,
    validity_out => vleft
  );
  
  u_lzd32_right : entity work.lzd32(rtl)
  port map(
    pattern32_in => pattern64_in(31 downto 0),
    position_out => pright,
    validity_out => vright
  );

  combinatorial : process(pleft, vleft, pright, vright)

  begin

    validity_out <= vleft or vright;
    if (vleft = '1') then
      position_out <= not(vleft) & pleft;
    else
      position_out <= not(vleft) & pright;
    end if;
    
  end process;

end rtl;

------------ EXTEND TO NINETYSIX (96 -> 64 + 32) --------------------

library ieee;
use ieee.numeric_std.all;
use ieee.std_logic_1164.all;

entity lzd96 is
  port(
    pattern96_in : std_logic_vector(95 downto 0);
    position_out : out std_logic_vector(6 downto 0);
    validity_out : out std_logic
    );
end lzd96;
    
architecture rtl of lzd96 is

  signal vleft, vright : std_logic;
  signal pleft : std_logic_vector(5 downto 0);
  signal pright : std_logic_vector(4 downto 0);
  
begin

  u_lzd64_left : entity work.lzd64(rtl)
  port map(
    pattern64_in => pattern96_in(95 downto 32),
    position_out => pleft,
    validity_out => vleft
  );
  
  u_lzd32_right : entity work.lzd32(rtl)
  port map(
    pattern32_in => pattern96_in(31 downto 0),
    position_out => pright,
    validity_out => vright
  );

  combinatorial : process(pleft, vleft, pright, vright)

  begin

    validity_out <= vleft or vright;
    if (vleft = '1') then
      position_out <= not(vleft) & pleft;
    else
      position_out <= not(vleft) & std_logic_vector(resize(unsigned(pright),6));
    end if;
    
  end process;

end rtl;

------------ EXTEND TO ONEHUNDREDTWO (102 -> 96 + 6) --------------------

library ieee;
use ieee.numeric_std.all;
use ieee.std_logic_1164.all;

entity lzd102 is
  port(
    pattern102_in : std_logic_vector(101 downto 0);
    position_out : out std_logic_vector(6 downto 0);
    validity_out : out std_logic
    );
end lzd102;
    
architecture rtl of lzd102 is

  signal vleft, vright : std_logic;
  signal pleft : std_logic_vector(6 downto 0);
  signal pright : std_logic_vector(2 downto 0);
  
begin

  u_lzd96_left : entity work.lzd96(rtl)
  port map(
    pattern96_in => pattern102_in(101 downto 6),
    position_out => pleft,
    validity_out => vleft
  );
  
  u_lzd6_right : entity work.lzd6(rtl)
  port map(
    pattern6_in => pattern102_in(5 downto 0),
    position_out => pright,
    validity_out => vright
  );

  combinatorial : process(pleft, vleft, pright, vright)

  begin

    -- Connected differently because 6 additional  bits will not go out of the range of the 96 bits
    -- from  the other  96-bit LZD.   Simply concatenating  results in incorrect answers.  A  7-bit
    -- adder with increment is used to combine 96 and 6-bit LZDs.
    validity_out <= vleft or vright;
    if (vleft = '1') then
      position_out <= pleft;
    else
      position_out <= std_logic_vector(unsigned(pleft) + resize(unsigned(pright),7) + 1); -- synthesis will detect as carry in
    end if;
    
  end process;
  
end rtl;

------------ ACTUAL LZD --------------------

library ieee;
use ieee.numeric_std.all;
use ieee.std_logic_1164.all;
use work.alu_pkg_lvl1.all;
use work.alu_pkg_lvl2.all;

entity lzd is
  port(
    pattern_in : in std_logic_vector(3*(HIDDENBIT+SIGNIFICANDWIDTH)+GUARDBITS-1 downto 0);
    position_out : out std_logic_vector(log2_ceil(3*(HIDDENBIT+SIGNIFICANDWIDTH)+GUARDBITS)-1 downto 0);
    validity_out : out std_logic
    );
end lzd;
    
architecture rtl of lzd is

  signal input : std_logic_vector(101 downto 0);

begin

  u_lzd102 : entity work.lzd102(rtl)
  port map(
    pattern102_in => input,
    position_out => position_out,
    validity_out => validity_out
    );

  combinatorial : process(pattern_in)

  begin
  
    -- Trail input with '0's (will make smaller significands possible, result does not change).
    input <= (others => '0');
    input(input'left downto input'left-(3*(HIDDENBIT+SIGNIFICANDWIDTH)+GUARDBITS-1)) <= pattern_in;
    
  end process;
    
end rtl;
