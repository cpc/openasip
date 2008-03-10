--
-- VHDL Architecture FFT_lib.coeff_transformer.RTL
--
-- Created:
--          by - tero.tero (localhost.localdomain)
--          at - 12:27:33 08/31/05
--
-- using Mentor Graphics HDL Designer(TM) 2004.1 (Build 41)
--

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------

-- "Mem reduction technique for Radix-4 FFT"
-- This block interchanges and/or complements real and imaginary parts of the
-- input vector "coeff" according to input "opcode".

-- Send possible comments, bug info or questions to tero.partanen@tut.fi

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------


LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.std_logic_arith.all;

ENTITY coeff_regen IS
  GENERIC (
    dataw : INTEGER := 32
  );
  PORT (
    --clk   : IN STD_LOGIC;
    --rst_x : IN STD_LOGIC;
    opcode : IN STD_LOGIC_VECTOR(2 DOWNTO 0);
    coeff : IN STD_LOGIC_VECTOR(dataw-1 DOWNTO 0);
    transf_coeff : OUT STD_LOGIC_VECTOR(dataw-1 DOWNTO 0)
  );
END ENTITY coeff_regen;

--
ARCHITECTURE RTL OF coeff_regen IS
BEGIN
  transf_process : PROCESS (coeff,opcode)
    
    BEGIN
   --   IF(rst_x = '0') THEN
   --     transf_coeff <= (OTHERS => '0');
   --   ELSIF(clk'event AND clk = '1') THEN
      CASE opcode IS
        WHEN "000" =>
          transf_coeff <= coeff;
        WHEN "001" =>
          transf_coeff(dataw-1 DOWNTO dataw/2) <= 0-UNSIGNED(coeff(dataw/2-1 DOWNTO 0));
          transf_coeff(dataw/2-1 DOWNTO 0) <= 0-UNSIGNED(coeff(dataw-1 DOWNTO dataw/2));
        WHEN "010" =>
          transf_coeff(dataw-1 DOWNTO dataw/2) <= coeff(dataw/2-1 DOWNTO 0);
          transf_coeff(dataw/2-1 DOWNTO 0) <= 0-UNSIGNED(coeff(dataw-1 DOWNTO dataw/2));
        WHEN "011" =>
          transf_coeff(dataw-1 DOWNTO dataw/2) <= 0-UNSIGNED(coeff(dataw-1 DOWNTO dataw/2)); 
          transf_coeff(dataw/2-1 DOWNTO 0) <= coeff(dataw/2-1 DOWNTO 0);
        WHEN "100" =>
          transf_coeff(dataw-1 DOWNTO dataw/2) <= 0-UNSIGNED(coeff(dataw-1 DOWNTO dataw/2)); 
          transf_coeff(dataw/2-1 DOWNTO 0) <= 0-UNSIGNED(coeff(dataw/2-1 DOWNTO 0));
        WHEN "101" =>
          transf_coeff(dataw-1 DOWNTO dataw/2) <= coeff(dataw/2-1 DOWNTO 0);
          transf_coeff(dataw/2-1 DOWNTO 0) <= coeff(dataw-1 DOWNTO dataw/2);
        WHEN OTHERS =>
          transf_coeff <= coeff;
      END CASE;
  --    END IF;
    END PROCESS transf_process;
END ARCHITECTURE RTL;

ARCHITECTURE RTL2 OF coeff_regen IS
BEGIN
  --transf_process : PROCESS (coeff,opcode)
    
   -- BEGIN
   --   IF(rst_x = '0') THEN
   --     transf_coeff <= (OTHERS => '0');
   --   ELSIF(clk'event AND clk = '1') THEN
   with opcode select
     transf_coeff(dataw-1 downto dataw/2) <= coeff(dataw-1 downto dataw/2) when "000",
     0-UNSIGNED(coeff(dataw/2-1 DOWNTO 0))     when "001",
     coeff(dataw/2-1 DOWNTO 0)                 when "010",
     0-UNSIGNED(coeff(dataw-1 DOWNTO dataw/2)) when "011",
     0-UNSIGNED(coeff(dataw-1 DOWNTO dataw/2)) when "100",
     coeff(dataw/2-1 DOWNTO 0)                 when "101",
     coeff(dataw-1 downto dataw/2)             when OTHERS;
   with opcode select
     transf_coeff(dataw/2-1 downto 0) <= coeff(dataw/2-1 downto 0) when "000",
      0-UNSIGNED(coeff(dataw-1 DOWNTO dataw/2)) when "001",
      0-UNSIGNED(coeff(dataw-1 DOWNTO dataw/2)) when "010",
      coeff(dataw/2-1 DOWNTO 0)                 when "011",
      0-UNSIGNED(coeff(dataw/2-1 DOWNTO 0))     when "100",
      coeff(dataw-1 DOWNTO dataw/2)             when "101",
      coeff(dataw/2-1 downto 0)                 when OTHERS;
  --    END IF;
  --  END PROCESS transf_process;
END ARCHITECTURE RTL2;



