   --pragma translate_off
   file_output : process
   file rf_trace : text;
   variable line_out : line;
   variable start : boolean := true;
   constant seperator : character := '|';
   constant dash : character := '-';
   variable opc : integer := 0;

   function ceil4 (
    constant val : natural)
    return natural is
  begin  -- function ceil4
    return natural(ceil(real(val)/real(4)))*4;
  end function ceil4;

  function ext_to_multiple_of_4 (
    constant slv : std_logic_vector)
    return std_logic_vector is
  begin
    return std_logic_vector(resize(
    unsigned(slv), ceil4(slv'length)));
  end function ext_to_multiple_of_4;

  function to_unsigned_hex (
    constant slv : std_logic_vector) return string is
    variable resized_slv : std_logic_vector(ceil4(slv'length)-1 downto 0);
    variable result      : string(1 to ceil4(slv'length)/4)
      := (others => ' ');
    subtype digit_t is std_logic_vector(3 downto 0);
    variable digit : digit_t := "0000";
  begin
    resized_slv := ext_to_multiple_of_4(slv);
    for i in result'range loop
      digit := resized_slv(
        resized_slv'length-((i-1)*4)-1 downto resized_slv'length-(i*4));
      case digit is
        when "0000" => result(i) := '0';
        when "0001" => result(i) := '1';
        when "0010" => result(i) := '2';
        when "0011" => result(i) := '3';
        when "0100" => result(i) := '4';
        when "0101" => result(i) := '5';
        when "0110" => result(i) := '6';
        when "0111" => result(i) := '7';
        when "1000" => result(i) := '8';
        when "1001" => result(i) := '9';
        when "1010" => result(i) := 'a';
        when "1011" => result(i) := 'b';
        when "1100" => result(i) := 'c';
        when "1101" => result(i) := 'd';
        when "1110" => result(i) := 'e';
        when "1111" => result(i) := 'f';
        -- For TTAsim bustrace compatibility
        when others => 
          result := (others => '0');
          return result;
      end case;
    end loop;  -- i in result'range
    return result;
  end function to_unsigned_hex;

  function reg_to_alias (
  constant index : integer) return string is
    variable result: string(1 to 3);
    begin
      case index is 
        when 0 => result := "zr ";
        when 1 => result := "ra ";
        when 2 => result := "sp ";
        when 3 => result := "gp ";
        when 4 => result := "tp ";
        when 5 => result := "t0 ";
        when 6 => result := "t1 ";
        when 7 => result := "t2 ";
        when 8 => result := "s0 ";
        when 9 => result := "s1 ";
        when 10 => result := "a0 ";
        when 11 => result := "a1 ";
        when 12 => result := "a2 ";
        when 13 => result := "a3 ";
        when 14 => result := "a4 ";
        when 15 => result := "a5 ";
        when 16 => result := "a6 ";
        when 17 => result := "a7 ";
        when 18 => result := "s2 ";
        when 19 => result := "s3 ";
        when 20 => result := "s4 ";
        when 21 => result := "s5 ";
        when 22 => result := "s6 ";
        when 23 => result := "s7 ";
        when 24 => result := "s8 ";
        when 25 => result := "s9 ";
        when 26 => result := "s10";
        when 27 => result := "s11";
        when 28 => result := "t3 ";
        when 29 => result := "t4 ";
        when 30 => result := "t5 ";
        when 31 => result := "t6 ";
        when others => result := "???";
      end case;
      return result;
    end function reg_to_alias;
