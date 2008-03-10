library ieee;
use ieee.std_logic_1164.all;

package highest is
  type integer_array is array (natural range <>) of integer;
  function return_highest(values : integer_array; numberOfValues : integer)
    return integer;
end package highest;

package body highest is
  function return_highest(values : integer_array; numberOfValues : integer)
    return integer is
    
    variable highest : integer;
    
  begin

    highest := 0;
    for x in 0 to numberOfValues-1 loop
      if values(x) > highest then
        highest := values(x);
      end if;
    end loop;  -- x
    
    return highest;
  end return_highest;
    
end package body highest;      
