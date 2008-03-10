----------------------------------------------------------------------------
-- Opcodes package
----------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;

package opcodes is
  -- ifetch (jump/call):
  constant IFE_JUMP : std_logic_vector := "0";
  constant IFE_CALL : std_logic_vector := "1";

end opcodes;
