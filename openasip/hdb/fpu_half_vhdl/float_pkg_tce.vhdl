-- Title      : Floating point unit for TTA
-- Project    :
-------------------------------------------------------------------------------
-- File       : fpadd_fpsub_fpmul_fpneg_fpabs_all.vhdl
-- Author     : Tero Partanen  <tero.partanen@tut.fi>
-- Company    :
-- Created    : 2009/05/25
-- Last update: 2010-01-12
--
--              Arithmetic code is highly based on IEEE floating point VHDL
--              standard libraries by David Bishop (floatfixlib in Modelsim),
--              just modified and simplified for TTA.
--
--              Unit supports four types of roundings: round to nearest,
--              round to positive and negative infinities and round to zero.
--              Guard bits for increased accuracy are also supported, like in
--              original code.
--              Unit does not support denormalized numbers. And does not
--              currently check for NAN or infinity numbers. You can easily add
--              NAN and infinity check on top level.
--
--              Floating point operations: add, subtract, multiply, negate,
--              absolut value.
--
--              This file includes all the needed separate architectures and
--              packages:
--
--              
--              fpmul_block 
--              float_pkg_tce
--
--
-------------------------------------------------------------------------------
-- Description: Floating point multiplier functional unit
--              
--      -opcode 0 : multiply
--              1 : square
-- --------------------------------------------------------------------
-- "float_pkg_tce" package contains functions for floating point math.
-- Code is highly based on IEEE floating point VHDL standard
--              libraries (floatfixlib in Modelsim). Only minor
--              modifications made on some functions.

-- Author     : Tero Partanen  <tero.partanen@tut.fi>
-- Company    :
-- Created    : 2009/05/11
-----------------------------------------------------------------------
-- Modified to support only addition and substraction 
--
-- Modifier   : Janne Janhunen <janne.janhunen@ee.oulu.fi
-- Company    : CWC
-- Modified   : 28/10/2010
--            : Output Registers removed  
-----------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;
 

package float_pkg_tce is


  type round_type is (round_nearest,    -- Default, nearest LSB '0'
                      round_inf,        -- Round toward positive infinity
                      round_neginf,     -- Round toward negative infinity
                      round_zero);      -- Round toward zero (truncate)
-- generic (
  -- Defaults for sizing routines, when you do a "to_float" this will be
  -- the default size.  Example float32 would be 8 and 23 (8 downto -23)

  --constant float_exponent_width : NATURAL    := 8;
  --constant float_fraction_width : NATURAL    := 23;
  constant float_exponent_width : NATURAL    := 5;
  constant float_fraction_width : NATURAL    := 10;

  -- Rounding algorithm, "round_nearest" is default, other valid values
  -- are "round_zero" (truncation), "round_inf" (round up), and
  -- "round_neginf" (round down)
  constant float_round_style    : round_type := round_zero;
  -- Denormal numbers (very small numbers near zero) true or false
  constant float_denormalize    : BOOLEAN    := false;  --denormals not
                                                        --supported, keep false

  -- Turns on NAN processing (invalid numbers and overflow) true of false
  --constant float_check_error    : BOOLEAN    := true;  --not used
  -- Guard bits are added to the bottom of every operation for rounding.
  -- any natural number (including 0) are valid.
  constant float_guard_bits     : NATURAL    := 1;

  -- Note that the size of the vector is not defined here, but in
  -- the package which calls this one.
  type UNRESOLVED_float is array (INTEGER range <>) of STD_ULOGIC;  -- main type
  subtype U_float is UNRESOLVED_float;

  subtype float is UNRESOLVED_float;


  -- base Unsigned fixed point type, downto direction assumed
  type UNRESOLVED_ufixed is array (INTEGER range <>) of STD_ULOGIC;
  -- base Signed fixed point type, downto direction assumed
  type UNRESOLVED_sfixed is array (INTEGER range <>) of STD_ULOGIC;

  subtype U_ufixed is UNRESOLVED_ufixed;
  subtype U_sfixed is UNRESOLVED_sfixed;

  subtype ufixed is UNRESOLVED_ufixed;
  subtype sfixed is UNRESOLVED_sfixed;
  -----------------------------------------------------------------------------
  -- Use the float type to define your own floating point numbers.
  -- There must be a negative index or the packages will error out.
  -- Minimum supported is "subtype float7 is float (3 downto -3);"
  -- "subtype float16 is float (6 downto -9);" is probably the smallest
  -- practical one to use.
  -----------------------------------------------------------------------------

  -- IEEE 754 single precision
  subtype UNRESOLVED_float32 is UNRESOLVED_float (8 downto -23);
  alias U_float32 is UNRESOLVED_float32;
  subtype float32 is float (8 downto -23);
  -----------------------------------------------------------------------------
  -- IEEE-754 single precision floating point.  This is a "float"
  -- in C, and a FLOAT in Fortran.  The exponent is 8 bits wide, and
  -- the fraction is 23 bits wide.  This format can hold roughly 7 decimal
  -- digits.  Infinity is 2**127 = 1.7E38 in this number system.
  -- The bit representation is as follows:
  -- 1 09876543 21098765432109876543210
  -- 8 76543210 12345678901234567890123
  -- 0 00000000 00000000000000000000000
  -- 8 7      0 -1                  -23
  -- +/-   exp.  fraction
  -----------------------------------------------------------------------------

  -- IEEE 754 double precision
  subtype UNRESOLVED_float64 is UNRESOLVED_float (11 downto -52);
  alias U_float64 is UNRESOLVED_float64;
  subtype float64 is float (11 downto -52);
  -----------------------------------------------------------------------------
  -- IEEE-754 double precision floating point.  This is a "double float"
  -- in C, and a FLOAT*8 in Fortran.  The exponent is 11 bits wide, and
  -- the fraction is 52 bits wide.  This format can hold roughly 15 decimal
  -- digits.  Infinity is 2**2047 in this number system.
  -- The bit representation is as follows:
  --  3 21098765432 1098765432109876543210987654321098765432109876543210
  --  1 09876543210 1234567890123456789012345678901234567890123456789012
  --  S EEEEEEEEEEE FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
  -- 11 10        0 -1                                               -52
  -- +/-  exponent    fraction
  -----------------------------------------------------------------------------

  -- IEEE 854 & C extended precision
  subtype UNRESOLVED_float128 is UNRESOLVED_float (15 downto -112);
  alias U_float128 is UNRESOLVED_float128;
  subtype float128 is float (15 downto -112);
  -----------------------------------------------------------------------------
  -- The 128 bit floating point number is "long double" in C (on
  -- some systems this is a 70 bit floating point number) and FLOAT*32
  -- in Fortran.  The exponent is 15 bits wide and the fraction is 112
  -- bits wide. This number can handle approximately 33 decimal digits.
  -- Infinity is 2**32,767 in this number system.
  -----------------------------------------------------------------------------


  function or_reduce (arg : std_logic_vector)
    return STD_ULOGIC;

  function or_reduce (arg : UNSIGNED)
    return STD_ULOGIC;

  -- Converts an fp into an SLV (needed for synthesis)
  function to_slv (arg : UNRESOLVED_float) return STD_LOGIC_VECTOR;


  function to_float (
    arg                     : STD_LOGIC_VECTOR;
    constant exponent_width : NATURAL := float_exponent_width;  -- length of FP output exponent
    constant fraction_width : NATURAL := float_fraction_width)  -- length of FP output fraction
    return UNRESOLVED_float;

  function to_hstring (value : STD_LOGIC_VECTOR) return STRING;

  -- purpose: returns a part of a vector, this function is here because
  -- or (fractr (to_integer(shiftx) downto 0));
  -- can't be synthesized in some synthesis tools.
  function smallfract (
    arg   : UNSIGNED;
    shift : NATURAL)
    return STD_ULOGIC;

  --functions for normalization

  function normalize (
    fract                   : UNSIGNED;  -- fraction, unnormalized
    expon                   : SIGNED;   -- exponent - 1, normalized
    sign                    : STD_ULOGIC;         -- sign bit
    sticky                  : STD_ULOGIC := '0';  -- Sticky bit (rounding)
    constant exponent_width : NATURAL    := float_exponent_width;  -- size of output exponent
    constant fraction_width : NATURAL    := float_fraction_width;  -- size of output fraction
    constant round_style    : round_type := float_round_style;  -- rounding option
    constant denormalize    : BOOLEAN    := float_denormalize;  -- Use IEEE extended FP
    constant nguard         : NATURAL    := float_guard_bits)   -- guard bits
    return UNRESOLVED_float;

end package float_pkg_tce;

package body float_pkg_tce is

  -- purpose: converts the negative index to a positive one
  -- negative indices are illegal in 1164 and 1076.3
  function to_suv (
    arg : UNRESOLVED_float)             -- fp vector
    return STD_ULOGIC_VECTOR is
    subtype result_subtype is STD_ULOGIC_VECTOR (arg'length-1 downto 0);
    variable result : result_subtype;
  begin  -- function to_std_ulogic_vector
    result := result_subtype (arg);
    return result;
  end function to_suv;

  -- Converts an fp into an SULV
  function to_slv (arg : UNRESOLVED_float) return STD_LOGIC_VECTOR is
  begin
    return to_stdlogicvector (to_suv (arg));
  end function to_slv;

  -- purpose: returns a part of a vector, this function is here because
  -- or (fractr (to_integer(shiftx) downto 0));
  -- can't be synthesized in some synthesis tools.
  function smallfract (
    arg   : UNSIGNED;
    shift : NATURAL)
    return STD_ULOGIC is
    variable orx : STD_ULOGIC;
  begin
    orx := arg(shift);
    for i in arg'range loop
      if i < shift then
        orx := arg(i) or orx;
      end if;
    end loop;
    return orx;
  end function smallfract;  
  -- std_logic_vector to float
  function to_float (
    arg                     : STD_ULOGIC_VECTOR;
    constant exponent_width : NATURAL := float_exponent_width;  -- length of FP output exponent
    constant fraction_width : NATURAL := float_fraction_width)  -- length of FP output fraction
    return UNRESOLVED_float is
    variable fpvar : UNRESOLVED_float (exponent_width downto -fraction_width);
  begin
    fpvar := UNRESOLVED_float(arg);
    return fpvar;
  end function to_float;

  function to_float (
    arg                     : STD_LOGIC_VECTOR;
    constant exponent_width : NATURAL := float_exponent_width;  -- length of FP output exponent
    constant fraction_width : NATURAL := float_fraction_width)  -- length of FP output fraction
    return UNRESOLVED_float is
  begin
    return to_float (
      arg => to_stdulogicvector (arg),
      exponent_width => exponent_width,
      fraction_width => fraction_width);
  end function to_float;

  
  function or_reduce (arg : STD_ULOGIC_VECTOR)
    return STD_LOGIC is
    variable Upper, Lower : STD_ULOGIC;
    variable Half         : INTEGER;
    variable BUS_int      : STD_ULOGIC_VECTOR (arg'length - 1 downto 0);
    variable Result       : STD_ULOGIC;
  begin
    if (arg'length < 1) then            -- In the case of a NULL range
      Result := '0';
    else
      BUS_int := to_ux01 (arg);
      if (BUS_int'length = 1) then
        Result := BUS_int (BUS_int'left);
      elsif (BUS_int'length = 2) then
        Result := BUS_int (BUS_int'right) or BUS_int (BUS_int'left);
      else
        Half   := (BUS_int'length + 1) / 2 + BUS_int'right;
        Upper  := or_reduce (BUS_int (BUS_int'left downto Half));
        Lower  := or_reduce (BUS_int (Half - 1 downto BUS_int'right));
        Result := Upper or Lower;
      end if;
    end if;
    return Result;
  end function or_reduce;

  function or_reduce (arg : std_logic_vector)
    return STD_ULOGIC is
  begin
    return or_reduce (std_ulogic_vector (arg));
  end function or_reduce;

  function or_reduce (arg : UNSIGNED)
    return STD_ULOGIC is
  begin
    return or_reduce (std_ulogic_vector (arg));
  end function or_reduce;

  
  --functions for normalization

  -- Generates the base number for the exponent normalization offset.
  function gen_expon_base (
    constant exponent_width : NATURAL)
    return SIGNED is
    variable result : SIGNED (exponent_width-1 downto 0);
  begin
    result                    := (others => '1');
    result (exponent_width-1) := '0';
    return result;
  end function gen_expon_base;

  -- Match table, copied form new std_logic_1164
  type stdlogic_table is array(STD_ULOGIC, STD_ULOGIC) of STD_ULOGIC;
  constant match_logic_table : stdlogic_table := (
    -----------------------------------------------------
    -- U    X    0    1    Z    W    L    H    -         |   |  
    -----------------------------------------------------
    ('U', 'U', 'U', 'U', 'U', 'U', 'U', 'U', '1'),  -- | U |
    ('U', 'X', 'X', 'X', 'X', 'X', 'X', 'X', '1'),  -- | X |
    ('U', 'X', '1', '0', 'X', 'X', '1', '0', '1'),  -- | 0 |
    ('U', 'X', '0', '1', 'X', 'X', '0', '1', '1'),  -- | 1 |
    ('U', 'X', 'X', 'X', 'X', 'X', 'X', 'X', '1'),  -- | Z |
    ('U', 'X', 'X', 'X', 'X', 'X', 'X', 'X', '1'),  -- | W |
    ('U', 'X', '1', '0', 'X', 'X', '1', '0', '1'),  -- | L |
    ('U', 'X', '0', '1', 'X', 'X', '0', '1', '1'),  -- | H |
    ('1', '1', '1', '1', '1', '1', '1', '1', '1')   -- | - |
    );

  function find_leftmost (ARG : UNSIGNED; Y : STD_ULOGIC)
    return INTEGER is
  begin
    for INDEX in ARG'range loop
      if ARG(INDEX) = Y then
        return INDEX;
      end if;
    end loop;
    return -1;
  end function find_leftmost;

  -- Function to return constants.
  function zerofp (
    constant exponent_width : NATURAL := float_exponent_width;  -- exponent
    constant fraction_width : NATURAL := float_fraction_width)  -- fraction
    return UNRESOLVED_float is
    constant result : UNRESOLVED_float (exponent_width downto -fraction_width) :=
      (others => '0');                                          -- zero
  begin
    return result;
  end function zerofp;

  function pos_inffp (
    constant exponent_width : NATURAL := float_exponent_width;  -- exponent
    constant fraction_width : NATURAL := float_fraction_width)  -- fraction
    return UNRESOLVED_float is
    variable result : UNRESOLVED_float (exponent_width downto -fraction_width) :=
      (others => '0');                  -- zero
  begin
    result (exponent_width-1 downto 0) := (others => '1');  -- Exponent all "1"
    return result;
  end function pos_inffp;

  constant NUS : STRING(2 to 1) := (others => ' ');

  function to_hstring (value : STD_LOGIC_VECTOR) return STRING is
    constant ne     : INTEGER := (value'length+3)/4;
    variable pad    : STD_LOGIC_VECTOR(0 to (ne*4 - value'length) - 1);
    variable ivalue : STD_LOGIC_VECTOR(0 to ne*4 - 1);
    variable result : STRING(1 to ne);
    variable quad   : STD_LOGIC_VECTOR(0 to 3);
  begin
    if value'length < 1 then
      return NUS;
    else
      if value (value'left) = 'Z' then
        pad := (others => 'Z');
      else
        pad := (others => '0');
      end if;
      ivalue := pad & value;
      for i in 0 to ne-1 loop
        quad := To_X01Z(ivalue(4*i to 4*i+3));
        case quad is
          when x"0"   => result(i+1) := '0';
          when x"1"   => result(i+1) := '1';
          when x"2"   => result(i+1) := '2';
          when x"3"   => result(i+1) := '3';
          when x"4"   => result(i+1) := '4';
          when x"5"   => result(i+1) := '5';
          when x"6"   => result(i+1) := '6';
          when x"7"   => result(i+1) := '7';
          when x"8"   => result(i+1) := '8';
          when x"9"   => result(i+1) := '9';
          when x"A"   => result(i+1) := 'A';
          when x"B"   => result(i+1) := 'B';
          when x"C"   => result(i+1) := 'C';
          when x"D"   => result(i+1) := 'D';
          when x"E"   => result(i+1) := 'E';
          when x"F"   => result(i+1) := 'F';
          when "ZZZZ" => result(i+1) := 'Z';
          when others => result(i+1) := 'X';
        end case;
      end loop;
      return result;
    end if;
  end function to_hstring;

  -- purpose: AND all of the bits in a vector together
  -- This is a copy of the proposed "and_reduce" from 1076.3
  function and_reduce (arg : STD_ULOGIC_VECTOR)
    return STD_LOGIC is
    variable Upper, Lower : STD_ULOGIC;
    variable Half         : INTEGER;
    variable BUS_int      : STD_ULOGIC_VECTOR (arg'length - 1 downto 0);
    variable Result       : STD_ULOGIC;
  begin
    if (arg'length < 1) then            -- In the case of a NULL range
      Result := '1';
    else
      BUS_int := to_ux01 (arg);
      if (BUS_int'length = 1) then
        Result := BUS_int (BUS_int'left);
      elsif (BUS_int'length = 2) then
        Result := BUS_int (BUS_int'right) and BUS_int (BUS_int'left);
      else
        Half   := (BUS_int'length + 1) / 2 + BUS_int'right;
        Upper  := and_reduce (BUS_int (BUS_int'left downto Half));
        Lower  := and_reduce (BUS_int (Half - 1 downto BUS_int'right));
        Result := Upper and Lower;
      end if;
    end if;
    return Result;
  end function and_reduce;

  function and_reduce (arg : UNSIGNED)
    return STD_ULOGIC is
  begin
    return and_reduce (std_ulogic_vector (arg));
  end function and_reduce;

  -- purpose: Rounds depending on the state of the "round_style"
  -- Logic taken from
  -- "What Every Computer Scientist Should Know About Floating Point Arithmetic"
  -- by David Goldberg (1991)
  function check_round (
    fract_in             : STD_ULOGIC;  -- input fraction
    sign                 : STD_ULOGIC;  -- sign bit
    remainder            : UNSIGNED;    -- remainder to round from
    sticky               : STD_ULOGIC := '0';      -- Sticky bit
    constant round_style : round_type)  -- rounding type
    return BOOLEAN is
    variable result     : BOOLEAN;
    variable or_reduced : STD_ULOGIC;
  begin  -- function check_round
    result := false;
    if (remainder'length > 0) then      -- if remainder in a null array
      or_reduced := or_reduce (remainder & sticky);
      rounding_case : case round_style is
        when round_nearest =>           -- Round Nearest, default mode
          if remainder(remainder'high) = '1' then  -- round
            if (remainder'length > 1) then
              if ((or_reduce (remainder(remainder'high-1
                                        downto remainder'low)) = '1'
                   or sticky = '1')
                  or fract_in = '1') then
                -- Make the bottom bit zero if possible if we are at 1/2
                result := true;
              end if;
            else
              result := (fract_in = '1' or sticky = '1');
            end if;
          end if;
        when round_inf =>               -- round up if positive, else truncate.
          if or_reduced = '1' and sign = '0' then
            result := true;
          end if;
        when round_neginf =>        -- round down if negative, else truncate.
          if or_reduced = '1' and sign = '1' then
            result := true;
          end if;
        when round_zero =>              -- round toward 0   Truncate
          null;
      end case rounding_case;
    end if;
    return result;
  end function check_round;

  -- purpose: Rounds depending on the state of the "round_style"
  -- unsigned version
  procedure fp_round (
    fract_in  : in  UNSIGNED;           -- input fraction
    expon_in  : in  SIGNED;             -- input exponent
    fract_out : out UNSIGNED;           -- output fraction
    expon_out : out SIGNED) is          -- output exponent
  begin  -- procedure fp_round
    if and_reduce (fract_in) = '1' then        -- Fraction is all "1"
      expon_out := expon_in + 1;
      fract_out := to_unsigned(0, fract_out'high+1);
    else
      expon_out := expon_in;
      fract_out := fract_in + 1;
    end if;
  end procedure fp_round;

-- purpose: normalizes a floating point number
  -- This version assumes an "unsigned" input with
  function normalize (
    fract                   : UNSIGNED;  -- fraction, unnormalized
    expon                   : SIGNED;  -- exponent, normalized by -1
    sign                    : STD_ULOGIC;         -- sign BIT
    sticky                  : STD_ULOGIC := '0';  -- Sticky bit (rounding)
    constant exponent_width : NATURAL    := float_exponent_width;  -- size of output exponent
    constant fraction_width : NATURAL    := float_fraction_width;  -- size of output fraction
    constant round_style    : round_type := float_round_style;  -- rounding option
    constant denormalize    : BOOLEAN    := float_denormalize;  -- Use IEEE extended FP
    constant nguard         : NATURAL    := float_guard_bits)  -- guard bits
    return UNRESOLVED_float is
    variable sfract     : UNSIGNED (fract'high downto 0);  -- shifted fraction
    variable rfract     : UNSIGNED (fraction_width-1 downto 0);   -- fraction
    variable exp        : SIGNED (exponent_width+1 downto 0);  -- exponent
    variable rexp       : SIGNED (exponent_width+1 downto 0);  -- result exponent
    variable rexpon     : UNSIGNED (exponent_width-1 downto 0);   -- exponent
    variable result     : UNRESOLVED_float (exponent_width downto -fraction_width);  -- result
    variable shiftr     : INTEGER;      -- shift amount
    constant expon_base : SIGNED (exponent_width-1 downto 0) :=
      gen_expon_base(exponent_width);   -- exponent offset
    variable round : BOOLEAN;
  begin  -- function normalize
    result (exponent_width) := sign;    -- sign bit
    round                   := false;
    shiftr := find_leftmost (to_01(fract), '1')  -- Find the first "1"
              - fraction_width - nguard;  -- subtract the length we want
    exp := resize (expon, exp'length) + shiftr;
    if (or_reduce (fract) = '0') then          -- Zero
      result := zerofp (fraction_width => fraction_width,
                        exponent_width => exponent_width);
    elsif ((exp <= -resize(expon_base, exp'length)-1) and denormalize)
      or ((exp < -resize(expon_base, exp'length)-1) and not denormalize) then
--      if (exp >= -resize(expon_base, exp'length)-fraction_width-1)
--        and denormalize then
--        exp    := -resize(expon_base, exp'length);
--        shiftr := to_integer (expon + expon_base);  -- new shift
--        sfract := fract sll shiftr;     -- shift
--        if nguard > 0 then
--          round := check_round (
--            fract_in    => sfract (nguard),
--            sign        => sign,
--            remainder   => sfract(nguard-1 downto 0),
--            round_style => round_style);
--        end if;
--        if round then
--          fp_round(fract_in  => sfract (fraction_width-1+nguard downto nguard),
--                   expon_in  => exp,
--                   fract_out => rfract,
--                   expon_out => rexp);
--        else
--          rfract := sfract (fraction_width-1+nguard downto nguard);
--          rexp   := exp;
--        end if;
--        rexpon := UNSIGNED ((rexp(exponent_width-1 downto 0))-1);
--        rexpon(exponent_width-1)           := not rexpon(exponent_width-1);
--        result (rexpon'range)              := UNRESOLVED_float(rexpon);
--        result (-1 downto -fraction_width) := UNRESOLVED_float(rfract);
--      else                              -- return zero
      result := zerofp (fraction_width => fraction_width,
                        exponent_width => exponent_width);
--      end if;
    elsif (exp > expon_base-1) then     -- infinity
      result := pos_inffp (fraction_width => fraction_width,
                           exponent_width => exponent_width);
      result (exponent_width) := sign;  -- redo sign bit for neg inf.
    else                                -- normal number
      sfract := fract srl shiftr;       -- shift
      if nguard > 0 then
        round := check_round (
          fract_in    => sfract (nguard),
          sign        => sign,
          remainder   => sfract(nguard-1 downto 0),
          sticky      => sticky,
          round_style => round_style);
      end if;
      if round then
        fp_round(fract_in  => sfract (fraction_width-1+nguard downto nguard),
                 expon_in  => exp(rexp'range),
                 fract_out => rfract,
                 expon_out => rexp);
      else
        rfract := sfract (fraction_width-1+nguard downto nguard);
        rexp   := exp(rexp'range);
      end if;
      -- result
      rexpon := UNSIGNED (rexp(exponent_width-1 downto 0));
      rexpon(exponent_width-1)           := not rexpon(exponent_width-1);
      result (rexpon'range)              := UNRESOLVED_float(rexpon);
      result (-1 downto -fraction_width) := UNRESOLVED_float(rfract);
    end if;
    return result;
  end function normalize;

end package body float_pkg_tce;




-----------------------------------------------------------------------
-- File       : fpmultiplier_block.vhdl
-- Author     : Tero Partanen  <tero.partanen@tut.fi>
-- Company    :
-- Created    : 2009/05/11

-- Description: Multiplier block for floating point unit
--      Arithmetic code is based on IEEE floating point VHDL standard
--              libraries (floatfixlib in Modelsim)
-----------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;
LIBRARY WORK;
USE WORK.float_pkg_tce.ALL;

ENTITY fpmultiplier_block IS
  GENERIC (
    exp_w       : integer := float_exponent_width;
    frac_w      : integer := float_fraction_width;
    round_style : round_type := float_round_style;
    guard_bits  : integer := float_guard_bits);

  PORT (
    a               : IN  float(exp_w DOWNTO -frac_w);
    b               : IN  float(exp_w DOWNTO -frac_w);
    sign                      : OUT std_ulogic;
    round_guard         : OUT std_ulogic;
    exp_out         : OUT signed(exp_w DOWNTO 0);
    frac_out       : OUT unsigned(frac_w+1+guard_bits DOWNTO 0)
    );
END fpmultiplier_block;

ARCHITECTURE rtl OF fpmultiplier_block IS
BEGIN  

  fp_mul_proc : PROCESS (a,b)

    variable fractional_a : unsigned(frac_w DOWNTO 0);
    variable exponent_a : signed(exp_w-1 DOWNTO 0);
    variable fractional_b : unsigned(frac_w DOWNTO 0);
    variable exponent_b : signed(exp_w-1 DOWNTO 0);
    variable mul_res_temp : unsigned(2*(frac_w)+1 DOWNTO 0);
    variable exponent_res : signed(exp_w DOWNTO 0);
    variable subresult : unsigned(frac_w+1+guard_bits DOWNTO 0);

  BEGIN  -- PROCESS

    --check for zero
    --exponent is all "0"
    IF((or_reduce (to_slv (a (exp_w-1 downto 0)))= '0') OR
       (or_reduce (to_slv (b (exp_w-1 downto 0)))= '0') ) then
      exponent_res := (OTHERS => '0');
      subresult := (OTHERS => '0');
      round_guard <= '0';
      sign <= '0';
    ELSE
      --check the sign
      sign <= a(exp_w) xor b(exp_w);

      --fractional parts
      fractional_a(frac_w-1 DOWNTO 0)  := UNSIGNED (to_slv(
        a(-1 downto -frac_w)));
      fractional_a(frac_w) := '1';
      fractional_b(frac_w-1 DOWNTO 0)  := UNSIGNED (to_slv(
        b(-1 downto -frac_w)));
      fractional_b(frac_w) := '1';
      --exponents
      exponent_a := SIGNED(a(exp_w-1 DOWNTO 0));
      exponent_a(exp_w-1) := NOT exponent_a(exp_w-1);
      exponent_b := SIGNED(b(exp_w-1 DOWNTO 0));
      exponent_b(exp_w-1) := NOT exponent_b(exp_w-1);

      --multiply
      mul_res_temp := fractional_a * fractional_b;
      subresult := mul_res_temp(2*(frac_w)+1 DOWNTO 2*(frac_w)+1 - (frac_w+1+guard_bits));
      
      if(round_style = round_zero) then
        round_guard <= '0';
      else
        round_guard <=
          or_reduce(mul_res_temp
                    (2*frac_w+1 - (frac_w+1+guard_bits) DOWNTO 0));       
      END if;

      --add the exponents
      exponent_res := (exponent_a(exponent_a'high)&exponent_a) +
                      (exponent_b(exponent_b'high)&exponent_b) + 1;
    END IF;

    exp_out <= exponent_res;
    frac_out <= subresult;

  END PROCESS;
END rtl;


-----------------------------------------------------
--                                                 --         
--            Fixed point substraction             --
--                                                 --
-----------------------------------------------------
library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

entity sub_arith is
  generic (
    dataw : integer := 16);
  port(
    A   : in  std_logic_vector(dataw-1 downto 0);
    B   : in  std_logic_vector(dataw-1 downto 0);
    S   : out std_logic_vector(dataw-1 downto 0));
end sub_arith;

architecture comb_if of sub_arith is

begin
  process(A, B)
  begin
          S <= conv_std_logic_vector(signed(B) - signed(A), S'length);
  end process;
end comb_if;


-- File       : normalization.vhdl
-- Author     : Tero Partanen  <tero.partanen@tut.fi>
-- Company    :
-- Created    : 2009/05/11

--  Description: Normalization block for floating point unit

library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;

LIBRARY WORK;
USE WORK.float_pkg_tce.ALL;

ENTITY normalization IS
  GENERIC (
    exp_w      : integer := float_exponent_width;
    frac_w     : integer := float_fraction_width;
    in_exp_w     : integer := float_exponent_width;
    guard_bits : integer := float_guard_bits
    );
  PORT (

    sign : in std_ulogic;
    round_guard : in std_ulogic;
    exp_in : in signed(in_exp_w DOWNTO 0);
    frac_in : in unsigned(frac_w+1+guard_bits DOWNTO 0);
    res_out : OUT float(exp_w DOWNTO -frac_w)
    );
END normalization;

ARCHITECTURE rtl OF normalization IS
BEGIN  -- rtl

  process( sign, round_guard, exp_in, frac_in )
  is
  begin
    --if (in_exp_w > exp_w) and ( exp_in( in_exp_w downto in_exp_w-1 ) = "11" )
    --then
    --  res_out <= to_float( x"0000" );
    ---elsif (in_exp_w > exp_w) and ( exp_in( in_exp_w-1 downto in_exp_w-2 ) = "11" )
    --then
    --  res_out <= to_float( x"0000" ); -- TODO
    --else
      res_out <= normalize (fract  => frac_in,
                            expon  => exp_in,
                            sign   => sign,
                            sticky => round_guard);
    --end if;
  end process;
END rtl;


-- File       : fpadd_block.vhdl
-- Author     : Tero Partanen  <tero.partanen@tut.fi>
-- Company    :
-- Created    : 2009/05/11
-- Description: Addition block for floating point unit
--      Arithmetic code is based on IEEE floating point VHDL standard
--              libraries (floatfixlib in Modelsim)

library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;
LIBRARY WORK;
USE WORK.float_pkg_tce.ALL;

ENTITY fpadd_block IS
  GENERIC (
    exp_w      : integer := float_exponent_width;
    frac_w     : integer := float_fraction_width;
    guard_bits : integer := float_guard_bits);
  PORT (

    a       : IN  float(exp_w DOWNTO -frac_w);
    b       : IN  float(exp_w DOWNTO -frac_w);
    sign : OUT std_ulogic;
    round_guard : OUT std_ulogic;
    exp_out : OUT signed(exp_w DOWNTO 0);
    frac_out : OUT unsigned(frac_w+1+guard_bits DOWNTO 0)
    );
END fpadd_block;

ARCHITECTURE rtl OF fpadd_block IS
BEGIN  -- rtl

  add : PROCESS (a,b)

    VARIABLE a_v : float(exp_w DOWNTO -frac_w);
    VARIABLE b_v : float(exp_w DOWNTO -frac_w);
    VARIABLE sign_v : std_ulogic;
    VARIABLE round_guard_v : std_ulogic;
    VARIABLE exp_out_v : signed(exp_w DOWNTO 0);
    VARIABLE frac_out_v : unsigned(frac_w+1+guard_bits DOWNTO 0);
    PROCEDURE fp_add (
      a           : IN  float(exp_w DOWNTO -frac_w);
      b           : IN  float(exp_w DOWNTO -frac_w);
      sign        : OUT std_ulogic;
      round_guard : OUT std_ulogic;
      exp_out     : OUT signed(exp_w DOWNTO 0);
      frac_out    : OUT unsigned(frac_w+1+guard_bits DOWNTO 0)) IS

      variable exponent_res : signed(exp_w DOWNTO 0);
      variable subresult : unsigned(frac_w+1+guard_bits DOWNTO 0);
      VARIABLE round_guard_v : STD_ULOGIC;
      VARIABLE fractional_a : unsigned(frac_w DOWNTO 0);
      VARIABLE fractional_b : unsigned(frac_w DOWNTO 0);
      VARIABLE exponent_a : signed(exp_w-1 DOWNTO 0);
      VARIABLE exponent_b : signed(exp_w-1 DOWNTO 0);
      VARIABLE addfrac_a : UNSIGNED(frac_w+1+guard_bits DOWNTO 0);
      VARIABLE addfrac_b : UNSIGNED(frac_w+1+guard_bits DOWNTO 0);
      VARIABLE shiftedaddfrac_1 : UNSIGNED(frac_w+1+guard_bits DOWNTO 0);
      VARIABLE shiftedaddfrac_2 : UNSIGNED(frac_w+1+guard_bits DOWNTO 0);
      VARIABLE shifts : SIGNED(exp_w DOWNTO 0);
      VARIABLE a_sign : BOOLEAN;

    begin

      addfrac_a := (OTHERS => '0');
      addfrac_b := (OTHERS => '0');

      --NO SUPPORT FOR DENORMALIZED NUMBERS HERE
      --check input's a exponent for all zeroes, if not
      if(or_reduce(to_slv(a(exp_w-1 downto 0))) /= '0') then      
        --fractional part
        fractional_a(frac_w-1 DOWNTO 0)  := UNSIGNED (to_slv(
          a(-1 downto -frac_w)));
        fractional_a(frac_w) := '1';
        --exponent
        exponent_a := SIGNED(a(exp_w-1 DOWNTO 0));
        exponent_a(exp_w-1) := NOT exponent_a(exp_w-1);
        --exponent is all zero, this is zero number
      else

        fractional_a := (others => '0');
        exponent_a := (others => '1');
        exponent_a(exp_w-1) := '0';
        exponent_a := -exponent_a;
      end if;
      --check if input b is zero
      if(or_reduce(to_slv(b(exp_w-1 downto 0))) /= '0') then
        --fractional part
        fractional_b(frac_w-1 DOWNTO 0)  := UNSIGNED (to_slv(
          b(-1 downto -frac_w)));
        fractional_b(frac_w) := '1';
        --exponent
        exponent_b := SIGNED(b(exp_w-1 DOWNTO 0));
        exponent_b(exp_w-1) := NOT exponent_b(exp_w-1);
      else
        fractional_b := (others => '0');
        exponent_b := (others => '1');
        exponent_b(exp_w-1) := '0';
        exponent_b := -exponent_b;
      end if;

      addfrac_a(frac_w+guard_bits DOWNTO guard_bits) := fractional_a;
      addfrac_b(frac_w+guard_bits DOWNTO guard_bits) := fractional_b;

      --PRE-NORMALIZATION

      --calculate how many bits are shifted
      shifts := (exponent_a(exp_w-1)&exponent_a) - exponent_b;

      if(shifts < -addfrac_a'high) then
        exponent_res := exponent_b(exp_w-1)&exponent_b;
        shiftedaddfrac_2 := addfrac_b;
        shiftedaddfrac_1 := (OTHERS => '0');  --add zero
        a_sign := false;   --use input b sign
        round_guard_v := or_reduce(addfrac_a);
      elsif(shifts < 0) then
        shifts := - shifts;
        shiftedaddfrac_1 := shift_right(addfrac_a,
                                        to_integer(shifts));
        shiftedaddfrac_2 := addfrac_b;
        exponent_res := exponent_b(exp_w-1)&exponent_b;
        a_sign := false;
        round_guard_v := smallfract(addfrac_a, to_integer(shifts));
      elsif(shifts = 0) then
        exponent_res := exponent_a(exp_w-1)&exponent_a;
        round_guard_v := '0';
        if(addfrac_b > addfrac_a) then
          shiftedaddfrac_2 := addfrac_b;
          shiftedaddfrac_1 := addfrac_a;
          a_sign := false;
        else
          shiftedaddfrac_2 := addfrac_a;
          shiftedaddfrac_1 := addfrac_b;
          a_sign := true;
        END if;
      elsif(shifts > addfrac_b'high) then
        exponent_res := exponent_a(exp_w-1)&exponent_a;
        shiftedaddfrac_1 := (OTHERS => '0');
        shiftedaddfrac_2 := addfrac_a;
        a_sign := true;
        round_guard_v := or_reduce(addfrac_b);
      elsif(shifts > 0) then
        exponent_res := exponent_a(exp_w-1)&exponent_a;
        shiftedaddfrac_1 := shift_right(addfrac_b,
                                        to_integer(shifts));
        shiftedaddfrac_2 := addfrac_a;
        a_sign := true;
        round_guard_v := smallfract(addfrac_b, to_integer(shifts));
      END if;

      --Or the rounding guard
      shiftedaddfrac_1(0) := shiftedaddfrac_1(0) OR round_guard_v;
      --same sign
      if(a(a'high) = b(b'high)) then
        subresult := shiftedaddfrac_1 + shiftedaddfrac_2;
        sign := a(a'high);
      else
        subresult := shiftedaddfrac_2 - shiftedaddfrac_1;
        if(a_sign) then
          sign := a(a'high);
        else
          sign := b(b'high);
        END if;
      END if;

      round_guard := round_guard_v;
      exp_out := exponent_res;
      frac_out := subresult;

    END fp_add;

  begin

    fp_add(
      a      =>      a,
      b      =>      b,
      sign   =>      sign_v,
      round_guard => round_guard_v,
      exp_out =>     exp_out_v,
      frac_out =>    frac_out_v);

    sign <= sign_v;
    round_guard <= round_guard_v;
    exp_out <= exp_out_v;
    frac_out <= frac_out_v;
  END PROCESS;
END rtl;



-- File       : fpmac_stage1_block.vhdl
-- Author     : Timo Viitanen  <timo.2.viitanen@tut.fi>
-- Company    :
-- Created    : 2013/03/06

--  Description: Floating point multiply-adder block

library IEEE;
use IEEE.std_logic_1164.all;
use ieee.numeric_std.all;
use work.float_pkg_tce.all;
  
ENTITY fpmac_unpack_block IS
  GENERIC (
    exp_w       : integer := float_exponent_width;
    frac_w      : integer := float_fraction_width;
    guard       : integer := float_guard_bits);
  PORT (
      l                      : IN  float(exp_w DOWNTO -frac_w);
      fractl_out             : OUT UNSIGNED (frac_w downto 0);
      exponl_out             : OUT SIGNED  (exp_w-1 downto 0)
  );
END fpmac_unpack_block;

ARCHITECTURE rtl OF fpmac_unpack_block IS
BEGIN
  stage : process( l )
    variable fractional_a  : UNSIGNED (frac_w downto 0);
    variable exponl        : SIGNED (exp_w-1 downto 0);

  begin  -- multiply
      --NO SUPPORT FOR DENORMALIZED NUMBERS HERE
      --unpack input a
      if ( or_reduce(unsigned(to_slv(l(exp_w-1 downto 0)))) /= '0') then      
        fractional_a(frac_w-1 DOWNTO 0)  := UNSIGNED (to_slv(
          l(-1 downto -frac_w)));
        fractional_a(frac_w) := '1';
        exponl := SIGNED(l(exp_w-1 DOWNTO 0));
        exponl(exp_w-1) := NOT exponl(exp_w-1);
      else
        fractional_a := (others => '0');
        exponl := (others => '1');
        exponl(exp_w-1) := '0';
        exponl := -exponl;
      end if;

      fractl_out <= (OTHERS => '0');
      fractl_out(frac_w DOWNTO 0) <= fractional_a;
      exponl_out <= exponl;
  end process;
END rtl;


-- File       : fpmac_stage1_block.vhdl
-- Author     : Timo Viitanen  <timo.2.viitanen@tut.fi>
-- Company    :
-- Created    : 2013/03/06

--  Description: Floating point multiply-adder block

library IEEE;
use IEEE.std_logic_1164.all;
use ieee.numeric_std.all;
use work.float_pkg_tce.all;
  
ENTITY fpmac_stage1_block IS
  GENERIC (
    exp_w       : integer := float_exponent_width;
    frac_w      : integer := float_fraction_width;
    guard       : integer := float_guard_bits);
  PORT (
      l                      : IN  float(exp_w DOWNTO -frac_w);
      r                      : IN  float(exp_w DOWNTO -frac_w);
      fractl_out             : OUT UNSIGNED (frac_w downto 0);
      fractr_out             : OUT UNSIGNED (frac_w downto 0);  -- fractions
      exponl_out             : OUT SIGNED  (exp_w-1 downto 0);
      exponr_out             : OUT SIGNED  (exp_w-1 downto 0) -- exponents
  );
END fpmac_stage1_block;

ARCHITECTURE rtl OF fpmac_stage1_block IS
BEGIN
  stage : process( l, r )
    variable fractional_a,
             fractional_b              : UNSIGNED (frac_w downto 0);

    variable exponl, exponr            : SIGNED (exp_w-1 downto 0);

    variable fractc, fracts            : UNSIGNED (frac_w+1+guard downto 0);
  begin  -- multiply
      --NO SUPPORT FOR DENORMALIZED NUMBERS HERE
      --unpack input a
      if ( or_reduce(unsigned(to_slv(l(exp_w-1 downto 0)))) /= '0') then      
        fractional_a(frac_w-1 DOWNTO 0)  := UNSIGNED (to_slv(
          l(-1 downto -frac_w)));
        fractional_a(frac_w) := '1';
        exponl := SIGNED(l(exp_w-1 DOWNTO 0));
        exponl(exp_w-1) := NOT exponl(exp_w-1);
      else

        fractional_a := (others => '0');
        exponl := (others => '1');
        exponl(exp_w-1) := '0';
        exponl := -exponl;
      end if;
      --unpack input b
      if(or_reduce(unsigned(to_slv(r(exp_w-1 downto 0)))) /= '0') then
        fractional_b(frac_w-1 DOWNTO 0)  := UNSIGNED (to_slv(
          r(-1 downto -frac_w)));
        fractional_b(frac_w) := '1';
        exponr := SIGNED(r(exp_w-1 DOWNTO 0));
        exponr(exp_w-1) := NOT exponr(exp_w-1);
      else
        fractional_b := (others => '0');
        exponr := (others => '1');
        exponr(exp_w-1) := '0';
        exponr := -exponr;
      end if;

      fractl_out <= (OTHERS => '0');
      fractr_out <= (OTHERS => '0');
      fractl_out(frac_w DOWNTO 0) <= fractional_a;
      fractr_out(frac_w DOWNTO 0) <= fractional_b;
      exponl_out <= exponl;
      exponr_out <= exponr;
  end process;
END rtl;

-- File       : fpmac_stage2_block_v2.vhdl
-- Author     : Timo Viitanen  <timo.2.viitanen@tut.fi>
-- Company    :
-- Created    : 2013/03/06

--  Description: Floating point multiply-adder block

library IEEE;
use IEEE.std_logic_1164.all;
use ieee.numeric_std.all;
use work.float_pkg_tce.all;
  
ENTITY fpmac_stage2_block_v0 IS
  GENERIC (
    exp_w       : integer := float_exponent_width;
    frac_w      : integer := float_fraction_width;
    guard       : integer := float_guard_bits);
  PORT (
      c               : IN  float(exp_w DOWNTO -frac_w);
      fractl          : IN UNSIGNED (frac_w downto 0);
      fractr          : IN UNSIGNED (frac_w downto 0);  -- fractions
      exponl          : IN SIGNED (exp_w-1 downto 0);
      exponr          : IN SIGNED (exp_w-1 downto 0); -- exponents
      rfract_out          : OUT UNSIGNED ((2*(frac_w))+1 downto 0);
      rexpon_out          : OUT SIGNED (exp_w+1 downto 0);
      fractx_out          : OUT UNSIGNED (frac_w+guard downto 0);
      exponc_out          : OUT SIGNED (exp_w-1 downto 0);
      shiftx_out          : OUT SIGNED (exp_w+1 downto 0)  -- shift fractions
  );
END fpmac_stage2_block_v0;

ARCHITECTURE rtl OF fpmac_stage2_block_v0 IS
BEGIN
  stage : process( c, fractl, fractr, exponl, exponr )
    variable fractional_c              : UNSIGNED (frac_w downto 0);
    variable fractx                    : UNSIGNED (frac_w+guard downto 0);
    variable exponc                    : SIGNED (exp_w-1 downto 0); 
    variable shiftx                    : SIGNED (exp_w+1 downto 0);  -- shift fractions
    variable rexpon                    : SIGNED (exp_w+1 downto 0);
  begin
    --unpack input c
    if(or_reduce(unsigned(to_slv(c(exp_w-1 downto 0)))) /= '0') then
      fractional_c(frac_w-1 DOWNTO 0)  := UNSIGNED (to_slv(
        c(-1 downto -frac_w)));
      fractional_c(frac_w) := '1';
      exponc := SIGNED(c(exp_w-1 DOWNTO 0));
      exponc(exp_w-1) := NOT exponc(exp_w-1);
    else
      fractional_c := (others => '0');
      exponc := (others => '1');
      exponc(exp_w-1) := '0';
      exponc := -exponc;
    end if;
    fractx := (OTHERS => '0');
    fractx(frac_w+guard DOWNTO guard) := fractional_c;
    
    fractx_out <= fractx;
    exponc_out <= exponc;
    
    rexpon := resize (exponl, rexpon_out'length) + exponr + 1;
    
    shiftx := rexpon - exponc;
    
    shiftx_out <= shiftx;

    -- multiply
    rfract_out <= fractl * fractr;        -- Multiply the fraction
    -- add the exponents
    rexpon_out <= rexpon;
  end process;
END rtl;

-- File       : fpmac_stage2_block.vhdl
-- Author     : Timo Viitanen  <timo.2.viitanen@tut.fi>
-- Company    :
-- Created    : 2013/03/06

--  Description: Floating point multiply-adder block

library IEEE;
use IEEE.std_logic_1164.all;
use ieee.numeric_std.all;
use work.float_pkg_tce.all;
  
ENTITY fpmac_stage2_block IS
  GENERIC (
    exp_w       : integer := float_exponent_width;
    frac_w      : integer := float_fraction_width;
    guard       : integer := float_guard_bits);
  PORT (
      fractl          : IN UNSIGNED (frac_w downto 0);
      fractr          : IN UNSIGNED (frac_w downto 0);  -- fractions
      exponl          : IN SIGNED (exp_w-1 downto 0);
      exponr          : IN SIGNED (exp_w-1 downto 0); -- exponents
      rfract_out          : OUT UNSIGNED ((2*(frac_w))+1 downto 0);
      rexpon_out          : OUT SIGNED (exp_w+1 downto 0)
  );
END fpmac_stage2_block;

ARCHITECTURE rtl OF fpmac_stage2_block IS
BEGIN
  stage : process( fractl, fractr, exponl, exponr )
  begin
    -- multiply
    rfract_out <= fractl * fractr;        -- Multiply the fraction
    -- add the exponents
    rexpon_out <= resize (exponl, rexpon_out'length) + exponr + 1;
  end process;
END rtl;

-- File       : fpmac_stage2_asymmetric_block.vhdl
-- Author     : Timo Viitanen  <timo.2.viitanen@tut.fi>
-- Company    :
-- Created    : 2013/03/06

--  Description: Floating point multiply-adder block

library IEEE;
use IEEE.std_logic_1164.all;
use ieee.numeric_std.all;
use work.float_pkg_tce.all;
  
ENTITY fpmac_stage2_asymmetric_block IS
  GENERIC (
    exp_w       : integer := float_exponent_width;
    lfrac_w      : integer := float_fraction_width;
    rfrac_w      : integer := float_fraction_width;
    guard       : integer := float_guard_bits);
  PORT (
      fractl          : IN UNSIGNED (lfrac_w-1 downto 0);
      fractr          : IN UNSIGNED (rfrac_w-1 downto 0);  -- fractions
      exponl          : IN SIGNED (exp_w-1 downto 0);
      exponr          : IN SIGNED (exp_w-1 downto 0); -- exponents
      rfract_out          : OUT UNSIGNED (lfrac_w+rfrac_w-1 downto 0);
      rexpon_out          : OUT SIGNED (exp_w+1 downto 0)
  );
END fpmac_stage2_asymmetric_block;

ARCHITECTURE rtl OF fpmac_stage2_asymmetric_block IS
BEGIN
  stage : process( fractl, fractr, exponl, exponr )
  begin
    -- multiply
    rfract_out <= fractl * fractr;
    -- add the exponents
    rexpon_out <= resize (exponl, rexpon_out'length) + exponr + 1;
  end process;
END rtl;

-- File       : fpmac_stage3_block.vhdl
-- Author     : Timo Viitanen  <timo.2.viitanen@tut.fi>
-- Company    :
-- Created    : 2013/03/06

--  Description: Floating point multiply-adder block

library IEEE;
use IEEE.std_logic_1164.all;
use ieee.numeric_std.all;
use work.float_pkg_tce.all;

ENTITY fpmac_stage3_block IS
  GENERIC (
    exp_w       : integer := float_exponent_width;
    frac_w      : integer := float_fraction_width;
    guard       : integer := float_guard_bits);
  PORT (
      rfract          : IN UNSIGNED ((2*(frac_w))+1 downto 0);
      rexpon          : IN SIGNED (exp_w+1 downto 0);
      fractx          : IN UNSIGNED (frac_w+guard downto 0);
      exponc          : IN SIGNED (exp_w-1 downto 0);  -- fractions

      fractc_out      : OUT UNSIGNED (frac_w+1+guard downto 0);
      fracts_out      : OUT UNSIGNED (frac_w+1+guard downto 0);
      leftright_out   : OUT boolean;
      round_guard_out : OUT std_ulogic;
      exp_out         : OUT signed(exp_w+1 DOWNTO 0)
  );
END fpmac_stage3_block;

ARCHITECTURE rtl OF fpmac_stage3_block IS
BEGIN
  stage : process( rfract, rexpon, fractx, exponc )
    variable fractc, fracts            : UNSIGNED (frac_w+1+guard downto 0);
    variable fractstmp                 : UNSIGNED (frac_w+1+guard downto 0);
    variable fractlt                   : BOOLEAN;     -- 
    variable exponeq                   : BOOLEAN;     -- 
    variable exponlt                   : BOOLEAN;     -- 
    variable overflow                  : BOOLEAN;     -- 
    variable leftright                 : BOOLEAN;     -- left or right used
    variable sticky                    : STD_ULOGIC;  -- Holds precision for rounding
    variable rexpon2                   : SIGNED (exp_w+1 downto 0);  -- result exponent
    variable shiftx_var                : SIGNED (rexpon'range);  -- shift fractions
  begin
    shiftx_var := rexpon-exponc;
    overflow := shiftx_var < -frac_w or shiftx_var > fractx'high;
    exponeq  := shiftx_var = 0;
    exponlt  := shiftx_var < 0;
    fractlt  := rfract (rfract'high downto rfract'high - fractc'length+1) < fractx;
    leftright := not( exponlt or (exponeq and fractlt) );

    -- A more IEEE-compliant fpu would need sticky bit computation here.
    sticky    := '0';
    
    if leftright then
      rexpon2   := rexpon;
      fractc    := rfract (rfract'high downto rfract'high - fractc'length+1);
      fractstmp := "0" & fractx;
    else
      rexpon2   := resize (exponc, rexpon2'length);
      fractc    := "0" & fractx;
      fractstmp := rfract (rfract'high downto rfract'high - fractc'length+1);
    end if;
    
    if exponlt then
      shiftx_var := - shiftx_var;
    end if;
    
    if overflow then
      fracts  := (others => '0');
    else  
      fracts := shift_right (fractstmp, to_integer(shiftx_var));
    end if;

    fractc_out <= fractc;
    fracts_out <= fracts;
    round_guard_out <= sticky;
    exp_out <= rexpon2;
    leftright_out <= leftright;
  end process;
END rtl;


-- File       : fpmac_stage3_block_v0.vhdl
-- Author     : Timo Viitanen  <timo.2.viitanen@tut.fi>
-- Company    :
-- Created    : 2013/03/06

--  Description: Floating point multiply-adder block

library IEEE;
use IEEE.std_logic_1164.all;
use ieee.numeric_std.all;
use work.float_pkg_tce.all;

ENTITY fpmac_stage3_block_v0 IS
  GENERIC (
    exp_w       : integer := float_exponent_width;
    frac_w      : integer := float_fraction_width;
    guard       : integer := float_guard_bits);
  PORT (
      rfract          : IN UNSIGNED ((2*(frac_w))+1 downto 0);
      rexpon          : IN SIGNED (exp_w+1 downto 0);
      fractx          : IN UNSIGNED (frac_w+guard downto 0);
      exponc          : IN SIGNED (exp_w-1 downto 0);  -- fractions
      shiftx          : IN SIGNED (exp_w+1 downto 0);  -- shift fractions

      fractc_out      : OUT UNSIGNED (frac_w+1+guard downto 0);
      fracts_out      : OUT UNSIGNED (frac_w+1+guard downto 0);
      leftright_out   : OUT boolean;
      round_guard_out : OUT std_ulogic;
      exp_out         : OUT signed(exp_w+1 DOWNTO 0)
  );
END fpmac_stage3_block_v0;

ARCHITECTURE rtl OF fpmac_stage3_block_v0 IS
BEGIN
  stage : process( rfract, rexpon, fractx, exponc, shiftx )
    variable fractc, fracts            : UNSIGNED (frac_w+1+guard downto 0);
    variable fractstmp                 : UNSIGNED (frac_w+1+guard downto 0);
    variable fractlt                   : BOOLEAN;     -- 
    variable exponeq                   : BOOLEAN;     -- 
    variable exponlt                   : BOOLEAN;     -- 
    variable overflow                  : BOOLEAN;     -- 
    variable leftright                 : BOOLEAN;     -- left or right used
    variable sticky                    : STD_ULOGIC;  -- Holds precision for rounding
    variable rexpon2                   : SIGNED (exp_w+1 downto 0);  -- result exponent
    variable shiftx_var                : SIGNED (rexpon'range);  -- shift fractions
  begin
    shiftx_var := shiftx;
    overflow := shiftx_var < -frac_w or shiftx_var > fractx'high;
    exponeq  := shiftx_var = 0;
    exponlt  := shiftx_var < 0;
    fractlt  := rfract (rfract'high downto rfract'high - fractc'length+1) < fractx;
    leftright := not( exponlt or (exponeq and fractlt) );

    -- A more IEEE-compliant fpu would need sticky bit computation here.
    sticky    := '0';
    
    if leftright then
      rexpon2   := rexpon;
      fractc    := rfract (rfract'high downto rfract'high - fractc'length+1);
      fractstmp := "0" & fractx;
    else
      rexpon2   := resize (exponc, rexpon2'length);
      fractc    := "0" & fractx;
      fractstmp := rfract (rfract'high downto rfract'high - fractc'length+1);
    end if;
    
    if exponlt then
      shiftx_var := - shiftx_var;
    end if;
    
    if overflow then
      fracts  := (others => '0');
    else  
      fracts := shift_right (fractstmp, to_integer(shiftx_var));
    end if;

    fractc_out <= fractc;
    fracts_out <= fracts;
    round_guard_out <= sticky;
    exp_out <= rexpon2;
    leftright_out <= leftright;
  end process;
END rtl;

-- File       : fpmac_stage4_block.vhdl
-- Author     : Timo Viitanen  <timo.2.viitanen@tut.fi>
-- Company    :
-- Created    : 2013/03/06

--  Description: Floating point multiply-adder block

library IEEE;
use IEEE.std_logic_1164.all;
use ieee.numeric_std.all;
use work.float_pkg_tce.all;

ENTITY fpmac_stage4_block IS
  GENERIC (
    exp_w       : integer := float_exponent_width;
    frac_w      : integer := float_fraction_width;
    guard       : integer := float_guard_bits);
  PORT (
    fractc          : IN UNSIGNED (frac_w+1+guard downto 0);
    fracts          : IN UNSIGNED (frac_w+1+guard downto 0);
    lsign           : IN std_ulogic;
    rsign           : IN std_ulogic;
    csign           : IN std_ulogic;
    leftright       : in boolean;

    sign_out        : OUT std_ulogic;
    frac_out        : OUT unsigned(frac_w+1+guard DOWNTO 0)
  );
END fpmac_stage4_block;

ARCHITECTURE rtl OF fpmac_stage4_block IS
BEGIN
  stage : process( fractc, fracts, lsign, rsign, csign, leftright )
    variable fp_sign                   : STD_ULOGIC;  -- sign of result
    variable ufract                    : UNSIGNED (frac_w+1+guard downto 0);  -- result fraction
  begin
    fp_sign := lsign xor rsign;
    if fp_sign = to_X01(csign) then
      ufract := fractc + fracts;
      fp_sign := fp_sign;
    else                              -- signs are different
      ufract := fractc - fracts;      -- always positive result
      if leftright then               -- Figure out which sign to use
        fp_sign := fp_sign;
      else
        fp_sign := csign;
      end if;
    end if;

    frac_out <= ufract;
    sign_out <= fp_sign;
  end process;
END rtl;

-- File       : fpmac_block.vhdl
-- Author     : Timo Viitanen  <timo.2.viitanen@tut.fi>
-- Company    :
-- Created    : 2013/03/06

--  Description: Floating point multiply-adder block

library IEEE;
use IEEE.std_logic_1164.all;
use ieee.numeric_std.all;
use work.float_pkg_tce.all;

-- a + m1*m2
ENTITY fpmac_block IS
  GENERIC (
    exp_w       : integer := float_exponent_width;
    frac_w      : integer := float_fraction_width;
    guard       : integer := float_guard_bits;
    bypass_2    : boolean := False;
    bypass_3    : boolean := False;
    bypass_4    : boolean := False;
    bypass_5    : boolean := False);
  PORT (

    a_in        : IN  float(exp_w DOWNTO -frac_w);
    m1_in       : IN  float(exp_w DOWNTO -frac_w);
    m2_in       : IN  float(exp_w DOWNTO -frac_w);
    res_out     : OUT float(exp_w DOWNTO -frac_w);
    clk         : IN std_logic;
    rstx        : IN std_logic;
    glock       : IN std_logic
    );
END fpmac_block;

ARCHITECTURE rtl OF fpmac_block IS
  signal stage1_fractl, stage2_fractl : UNSIGNED (frac_w downto 0);
  signal stage1_fractr, stage2_fractr : UNSIGNED (frac_w downto 0);
  signal stage2_exponl, stage1_exponl : SIGNED (exp_w-1 downto 0);
  signal stage2_exponr, stage1_exponr : SIGNED (exp_w-1 downto 0);
  signal stage3_fractx, stage2_fractx : UNSIGNED (frac_w+guard downto 0);
  signal stage3_exponc, stage2_exponc : SIGNED (exp_w-1 downto 0);
  signal stage3_rfract, stage2_rfract : UNSIGNED ((2*(frac_w))+1 downto 0);
  signal stage3_rexpon, stage2_rexpon : SIGNED (exp_w+1 downto 0);
  signal stage4_fractc, stage3_fractc : UNSIGNED (frac_w+1+guard downto 0);
  signal stage4_fracts, stage3_fracts : UNSIGNED (frac_w+1+guard downto 0);
  
  signal stage5_sign,  stage4_sign   : std_ulogic;
  signal stage5_guard, stage4_guard, stage3_guard  : std_ulogic;
  signal stage5_exp,   stage4_exp,   stage3_exp    : signed(exp_w+1 DOWNTO 0);
  signal stage5_frac,  stage4_frac   : unsigned(frac_w+1+guard DOWNTO 0);

  signal stage2_lsign, stage3_lsign, stage4_lsign : std_ulogic;
  signal stage2_rsign, stage3_rsign, stage4_rsign : std_ulogic;
  signal stage2_csign, stage3_csign, stage4_csign : std_ulogic;

  signal stage3_leftright, stage4_leftright : boolean;

  signal stage2_c : float(exp_w DOWNTO -frac_w);
  
BEGIN  -- rtl
  
  stage1_unpackl: entity work.fpmac_unpack_block(rtl)
    GENERIC MAP(exp_w => exp_w, frac_w => frac_w, guard => guard)
    PORT MAP (
      l          => m1_in,
      fractl_out => stage1_fractl,
      exponl_out => stage1_exponl);
      
  stage1_unpackr: entity work.fpmac_unpack_block(rtl)
    GENERIC MAP(exp_w => exp_w, frac_w => frac_w, guard => guard)
    PORT MAP (
      l          => m2_in,
      fractl_out => stage1_fractr,
      exponl_out => stage1_exponr );
  
  stage2: entity work.fpmac_stage2_block(rtl)
    GENERIC MAP(exp_w => exp_w, frac_w => frac_w, guard => guard)
    PORT MAP (
      fractl     => stage2_fractl,
      fractr     => stage2_fractr,
      exponl     => stage2_exponl,
      exponr     => stage2_exponr,
      rfract_out => stage2_rfract,
      rexpon_out => stage2_rexpon );
      
  stage2_unpackc: entity work.fpmac_unpack_block(rtl)
    GENERIC MAP(exp_w => exp_w, frac_w => frac_w, guard => guard)
    PORT MAP (
      l          => stage2_c,
      fractl_out => stage2_fractx,
      exponl_out => stage2_exponc );
  
  stage3: entity work.fpmac_stage3_block(rtl)
    GENERIC MAP(exp_w => exp_w, frac_w => frac_w, guard => guard)
    PORT MAP (
    rfract          => stage3_rfract,
    rexpon          => stage3_rexpon,
    fractx          => stage3_fractx,
    exponc          => stage3_exponc,
    fractc_out      => stage3_fractc,
    fracts_out      => stage3_fracts,
    leftright_out   => stage3_leftright,
    round_guard_out => stage3_guard,
    exp_out         => stage3_exp );
  
  stage4: entity work.fpmac_stage4_block(rtl)
    GENERIC MAP(exp_w => exp_w, frac_w => frac_w, guard => guard)
    PORT MAP (
    fractc    => stage4_fractc,
    fracts    => stage4_fracts,
    lsign     => stage4_lsign,
    rsign     => stage4_rsign,
    csign     => stage4_csign,
    leftright => stage4_leftright,
    sign_out  => stage4_sign,
    frac_out  => stage4_frac );
    
  res_out <=       normalize (fract       => stage5_frac,
                              expon       => stage5_exp,
                              sign        => stage5_sign,
                              sticky      => stage5_guard,
                              round_style => round_zero,
                              denormalize => false,
                              nguard      => guard);

  regs: PROCESS (clk, rstx,
                 stage1_fractl,stage1_fractr,stage1_exponl,stage1_exponr,
                 m1_in, m2_in, a_in, a_in)
  BEGIN
    IF(rstx = '0') THEN  
      
      IF bypass_2 = False THEN
        stage2_fractl <= (others=>'0');
        stage2_fractr <= (others=>'0');
        stage2_exponl <= (others=>'0');
        stage2_exponr <= (others=>'0');
          
        stage2_lsign  <= '0';
        stage2_rsign  <= '0';
        stage2_csign  <= '0';
          
        stage2_c      <= (others=>'0');
      END IF;
      
      IF bypass_3 = False THEN
        stage3_rfract <= (others=>'0');
        stage3_rexpon <= (others=>'0');
  
        stage3_lsign  <= '0';
        stage3_rsign  <= '0';
        stage3_csign  <= '0';
      END IF;
        
        
      IF bypass_4 = False THEN
        stage4_fractc <= (others=>'0');
        stage4_fracts <= (others=>'0');
        stage4_leftright <= False;
        
        stage4_lsign  <= '0';
        stage4_rsign  <= '0';
        stage4_csign  <= '0';
      END IF;
      
      IF bypass_5 = False THEN
        stage5_sign   <= '0';
        stage5_guard  <= '0';
        stage5_exp    <= (others=>'0');
        stage5_frac   <= (others=>'0');
      END IF;

    ELSIF(clk'event AND clk = '1') then
      IF(glock = '0') then
        IF bypass_2 = False THEN
          stage2_fractl <= stage1_fractl;
          stage2_fractr <= stage1_fractr;
          stage2_exponl <= stage1_exponl;
          stage2_exponr <= stage1_exponr;
            
          stage2_lsign  <= m1_in(m1_in'high);
          stage2_rsign  <= m2_in(m2_in'high);
          stage2_csign  <= a_in(a_in'high);
            
          stage2_c      <= a_in;
        END IF;
        IF bypass_3 = False THEN
          stage3_rfract <= stage2_rfract;
          stage3_rexpon <= stage2_rexpon;
          stage3_fractx <= stage2_fractx;
          stage3_exponc <= stage2_exponc;
          
          stage3_lsign  <= stage2_lsign;
          stage3_rsign  <= stage2_rsign;
          stage3_csign  <= stage2_csign;
        END IF;
        IF bypass_4 = False THEN
          stage4_fractc    <= stage3_fractc;
          stage4_fracts    <= stage3_fracts;
          stage4_leftright <= stage3_leftright;
          
          stage4_lsign  <= stage3_lsign;
          stage4_rsign  <= stage3_rsign;
          stage4_csign  <= stage3_csign;
          
          stage4_guard  <= stage3_guard;
          stage4_exp    <= stage3_exp;
        END IF;
        IF bypass_5 = False THEN
          stage5_sign   <= stage4_sign;
          stage5_guard  <= stage4_guard;
          stage5_exp    <= stage4_exp;
          stage5_frac   <= stage4_frac;
        END IF;
      END IF;
    END IF;
    IF bypass_2 = True THEN
      stage2_fractl <= stage1_fractl;
      stage2_fractr <= stage1_fractr;
      stage2_exponl <= stage1_exponl;
      stage2_exponr <= stage1_exponr;
        
      stage2_lsign  <= m1_in(m1_in'high);
      stage2_rsign  <= m2_in(m2_in'high);
      stage2_csign  <= a_in(a_in'high);
        
      stage2_c      <= a_in;
    END IF;
    IF bypass_3 = True THEN
      stage3_rfract <= stage2_rfract;
      stage3_rexpon <= stage2_rexpon;
      stage3_fractx <= stage2_fractx;
      stage3_exponc <= stage2_exponc;
      
      stage3_lsign  <= stage2_lsign;
      stage3_rsign  <= stage2_rsign;
      stage3_csign  <= stage2_csign;
    END IF;
    IF bypass_4 = True THEN
      stage4_fractc <= stage3_fractc;
      stage4_fracts <= stage3_fracts;
      stage4_leftright <= stage3_leftright;
      
      stage4_lsign  <= stage3_lsign;
      stage4_rsign  <= stage3_rsign;
      stage4_csign  <= stage3_csign;

      stage4_guard  <= stage3_guard;
      stage4_exp    <= stage3_exp;
    END IF;
    IF bypass_5 = True THEN
      stage5_sign   <= stage4_sign;
      stage5_guard  <= stage4_guard;
      stage5_exp    <= stage4_exp;
      stage5_frac   <= stage4_frac;
    END IF;
  END PROCESS regs;

END rtl;




