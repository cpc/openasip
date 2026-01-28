library ieee;
use ieee.std_logic_1164.all;
package util is
  
  function flip_bits(in_vec : std_logic_vector)  -- make unconstrained
    return std_logic_vector;

  function int_to_str (InputInt  : Integer           )
    return string;

  FUNCTION bit_width (num : integer)
    RETURN integer;

end package util;

package body util is

  function flip_bits(in_vec : std_logic_vector)  -- make unconstrained
    return std_logic_vector is

    variable flipped_vec : std_logic_vector(in_vec'Reverse_range);
  begin
    for i in in_vec'range loop
      flipped_vec(i) := in_vec(i);
    end loop;
    return flipped_vec;
  end flip_bits;

  -- ------------------------------------------------------------------------
  -- PROCEDURE NAME:  Int_To_Str
  --
  -- PARAMETERS    :  InputInt     - Integer to be converted to String.
  --                  ResultStr    - String buffer for converted Integer
  --                  AppendPos    - Position in buffer to place result
  --
  -- DESCRIPTION   :  This procedure is used to convert an input integer
  --                  into a string representation.  The converted string
  --                  may be placed at a specific position in the result
  --                  buffer.
  --
  -- ------------------------------------------------------------------------

  function int_to_str (InputInt : INTEGER )
    return string is 

    -- Look-up table.  Given an int, we can get the character.
    TYPE integer_table_type IS ARRAY (0 TO 9) OF CHARACTER;
    CONSTANT integer_table : integer_table_type :=
      (
        '0', '1', '2', '3', '4',
        '5', '6', '7', '8', '9'
        ) ;

    --  Local variables used in this function.
    
    VARIABLE inpVal      : INTEGER := inputInt;
    VARIABLE divisor     : INTEGER := 10;
    VARIABLE tmpStrIndex : INTEGER := 1;
    VARIABLE tmpStr      : STRING ( 1 TO 256 );
    variable ResultStr   : STRING ( 1 TO 256);
    variable appendPos : integer := 1;
    
  BEGIN
    
    IF ( inpVal = 0 ) THEN
      tmpStr(tmpStrIndex) := integer_table ( 0 );
      tmpStrIndex         := tmpStrIndex + 1;
    ELSE
      WHILE ( inpVal > 0 ) LOOP
        tmpStr(tmpStrIndex) := integer_table ( inpVal mod divisor );
        tmpStrIndex         := tmpStrIndex + 1;
        inpVal              := inpVal / divisor;
      END LOOP;
    END IF;

    IF ( appendPos /= 1 ) THEN
      resultStr(appendPos) := ',';
      appendPos            := appendPos + 1;
    END IF;

    FOR i IN tmpStrIndex-1 DOWNTO 1 LOOP
      resultStr(appendPos) := tmpStr(i);
      appendPos            := appendPos + 1;
    END LOOP;

    return ResultStr;
    
  END int_to_str;

  FUNCTION bit_width (num : integer) RETURN integer IS
         variable count : integer;
         Begin
           count := 1;
           if (num <= 0) then return 0;
           elsif (num <= 2**10) then
            for i in 1 to 10 loop
             if (2**count >= num)  then
              return i; 
             end if;
             count := count + 1;
            end loop;
           elsif (num <= 2**20) then
            for i in 1 to 20 loop
             if (2**count >= num)  then
              return i; 
             end if;
             count := count + 1;
            end loop;
           elsif (num <= 2**30) then
            for i in 1 to 30 loop
             if (2**count >= num)  then
              return i; 
             end if;
             count := count + 1;
            end loop;
           else
            for i in 1 to num loop
             if (2**i >= num)  then
              return i; 
             end if;
            end loop;
          end if;
         end bit_width;

  
end package body util;





