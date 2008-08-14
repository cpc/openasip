--
-- VHDL Architecture FFT_lib.mem.RTL
--
-- Created:
--          by - tero.tero (localhost.localdomain)
--          at - 15:19:15 08/31/05
--
-- using Mentor Graphics HDL Designer(TM) 2004.1 (Build 41)
--
--This unit is used as a coefficient memory for the "FFT mem reduction technique"
--Uses package file named coefficients_pkg.vhdl.
--Use the existing Matlab functions to generate the package file. 

LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.std_logic_arith.all;
USE work.coefficients.all;

ENTITY mem IS
  GENERIC (
    dataw : INTEGER := 32;
    address_width :integer := 8
  );
  PORT (
    --clk : in STD_LOGIC;
    address : in STD_LOGIC_VECTOR(address_width-1 DOWNTO 0);
    data_out : out STD_LOGIC_VECTOR(dataw-1 DOWNTO 0)
  );
END ENTITY mem;

--
ARCHITECTURE RTL OF mem IS

  CONSTANT coeffs : integer_matrix(0 TO coefficients'length-1) := coefficients;
--  SIGNAL line : INTEGER RANGE 0 TO coefficients'length-1;
  signal line_1 : integer RANGE 0 TO coefficients'length-1;
BEGIN
  
  line_1 <= CONV_INTEGER(UNSIGNED(address));
  
  memory : PROCESS (line_1)
  
  BEGIN
    --IF(clk'event and clk = '1') THEN
      data_out(dataw-1 DOWNTO dataw/2) <= CONV_STD_LOGIC_VECTOR(coeffs(line_1*2), dataw/2);
      data_out(dataw/2-1 DOWNTO 0) <= CONV_STD_LOGIC_VECTOR(coeffs(line_1*2+1), dataw/2);
    --END IF;
  END PROCESS memory;
END ARCHITECTURE RTL;

