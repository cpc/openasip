--
-- VHDL Architecture FFT_lib.addrgen2.RTL
--
-- Created:
--          by - tero.tero (localhost.localdomain)
--          at - 13:46:28 08/24/05
--
-- using Mentor Graphics HDL Designer(TM) 2004.1 (Build 41)
--

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------

-- Address generator for coefficient memory reduction technique for Radix-4 FFT.
-- Version 0.9
--
-- Inputs: stage = 0,1,...,LOG4(N)-1
--         index = index of needed coefficient = 0,1,...,N-1
--
-- Outputs: addr_out = address to minimized coefficient memory
--          opcode = operation code for the coefficient modification
--
-- If you use larger than 1024-point FFT, you need to add more case statements,
-- check the code.
--
-- This version was modified for synthesis, but not tested after that.

-- Send possible comments, bug info or questions to tero.partanen@tut.fi
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------


LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.std_logic_arith.all;
use IEEE.std_logic_signed.all;


ENTITY addrgen IS
  GENERIC (
    addr_width : integer := 8;
    dataw : integer := 32;
    N     : integer := 1024;   -- N-point FFT
    stage_bits  : INTEGER := 3 --optimization: insert here ceil(LOG2(LOG4(N))),
                               --that is how many bits you need for stages.
                               --with 1024-point FFT you have 5 stages -> 3 bits
    );
  PORT (
    --clk   : in std_logic;
    --rst_x : in std_logic;
    stage : in std_logic_vector(dataw-1 DOWNTO 0);
    index : in std_logic_vector(dataw-1 DOWNTO 0);
    addr_out : out std_logic_vector(addr_width-1 DOWNTO 0);
    opcode : out std_logic_vector(2 DOWNTO 0)
    );
END ENTITY addrgen;


--
ARCHITECTURE RTL OF addrgen IS
    
    
BEGIN

  address_generate : PROCESS (stage,index)
  
  VARIABLE inner_index : INTEGER RANGE 0 TO N;
  VARIABLE coeff_index : INTEGER RANGE 0 TO N;
  VARIABLE scale       : INTEGER RANGE 0 TO N/16;
  VARIABLE addr_temp   : INTEGER RANGE 0 TO N/8;
  
  BEGIN
    --IF(rst_x = '0') THEN
    --  addr_out <= (OTHERS => '0');
    --  opcode <= (others => '0');
    --ELSIF(clk'event AND clk='1') THEN
            
      --at stage zero there is no multiplying
      IF(CONV_INTEGER(stage(stage_bits-1 DOWNTO 0)) = 0 OR index(1 DOWNTO 0) = "00") THEN
        addr_out <= (OTHERS => '0');
        opcode <= (OTHERS => '0');
      ELSE
        --check what is the current stage
        CASE CONV_INTEGER(UNSIGNED(stage(stage_bits-1 DOWNTO 0))) IS
          --transform size 16
          WHEN 1 =>
            inner_index := CONV_INTEGER(UNSIGNED(index(3 DOWNTO 2)));
            scale := N/16;
          --transform size 64
          WHEN 2 =>
            inner_index := CONV_INTEGER(UNSIGNED(index(5 DOWNTO 2)));
            scale := N/64;
          --transform size 256
          WHEN 3 =>
            inner_index := CONV_INTEGER(UNSIGNED(index(7 DOWNTO 2)));
            scale := N/256;
          --transform size 1024
          WHEN 4 =>
            inner_index := CONV_INTEGER(UNSIGNED(index(9 DOWNTO 2)));
            scale := N/1024;

          --And so on. Add more, if you need 
          -- WHEN 5 =>
          --   inner_index := CONV_INTEGER(UNSIGNED(index(11 DOWNTO 2)));
          --   scale := N/4096;

          WHEN OTHERS =>
            inner_index := 0;    
        END CASE;

        
        --calculate coefficient index("upper" index)
        CASE CONV_INTEGER(UNSIGNED(index(1 DOWNTO 0))) IS
          WHEN 1 =>
            coeff_index := inner_index*scale;
          WHEN 2 =>
            coeff_index := inner_index*scale*2;
          WHEN 3 => 
            coeff_index := inner_index*scale*3;
          WHEN OTHERS =>
            coeff_index := 0;
        END CASE;

        
        --first half of the first quarter (coefficients in unit circle)
        IF(coeff_index <= N/8) THEN
          addr_temp:= coeff_index;
          opcode <= "000";
        --second half..
        ELSIF(coeff_index < N/4) THEN
          addr_temp := N/4 - coeff_index;
          opcode <= "001";
        --first half of the second quarter
        ELSIF(coeff_index <= N*3/8) THEN
          addr_temp := coeff_index - N/4;
          opcode <= "010";
        --second half
        ELSIF(coeff_index < N/2) THEN
          addr_temp := N/2 - coeff_index;
          opcode <= "011";
        --first half of the third quarter
        ELSIF(coeff_index <= N*5/8) THEN
          addr_temp := coeff_index - N/2;
          opcode <= "100";
        --second half of the third quarter
        ELSE
          addr_temp := N*3/4 - coeff_index;
          opcode <= "101";
        END IF;    

        addr_out <= CONV_STD_LOGIC_VECTOR(addr_temp, addr_width);          
      END IF;
              
    --END IF;
  END PROCESS address_generate;
            
END ARCHITECTURE RTL;

architecture RTL2 of addrgen is

  signal inner_index : integer range 0 to N;
  signal coeff_index : INTEGER RANGE 0 TO N;
  signal scale       : INTEGER RANGE 0 TO N/16;
  signal addr_temp   : INTEGER RANGE 0 TO N/8;  

  begin  -- RTL2
  gen_inner_index_and_scale: process (stage, index)
  begin  -- process gen_inner_index
    case CONV_INTEGER(UNSIGNED(stage(stage_bits-1 downto 0))) is
      WHEN 1 =>
        inner_index <= CONV_INTEGER(UNSIGNED(index(3 DOWNTO 2)));
        scale <= N/16;
        --transform size 64
      WHEN 2 =>
        inner_index <= CONV_INTEGER(UNSIGNED(index(5 DOWNTO 2)));
        scale <= N/64;
        --transform size 256
      WHEN 3 =>
        inner_index <= CONV_INTEGER(UNSIGNED(index(7 DOWNTO 2)));
        scale <= N/256;
        --transform size 1024
      WHEN 4 =>
        inner_index <= CONV_INTEGER(UNSIGNED(index(9 DOWNTO 2)));
        scale <= N/1024;
        
        --And so on. Add more, if you need 
        -- WHEN 5 =>
        --   inner_index := CONV_INTEGER(UNSIGNED(index(11 DOWNTO 2)));
        --   scale := N/4096;
        
      when others => inner_index <= 0;
    end case;    
  end process gen_inner_index_and_scale;

  generate_coeff_index: process (inner_index, scale, index)
  begin  -- process generate_coeff_index
    --calculate coefficient index("upper" index)
    CASE CONV_INTEGER(UNSIGNED(index(1 DOWNTO 0))) IS
      WHEN 1 =>
        coeff_index <= inner_index*scale;
      WHEN 2 =>
        coeff_index <= inner_index*scale*2;
      WHEN 3 => 
        coeff_index <= inner_index*scale*3;
      WHEN OTHERS =>
        coeff_index <= 0;
    END CASE;    
  end process generate_coeff_index;

  gen_output: process (coeff_index)
  begin  -- process gen_output
    --first half of the first quarter (coefficients in unit circle)
    IF(coeff_index <= N/8) THEN
      addr_temp <= coeff_index;
      opcode <= "000";
      --second half..
    ELSIF(coeff_index < N/4) THEN
      addr_temp <= N/4 - coeff_index;
      opcode <= "001";
      --first half of the second quarter
    ELSIF(coeff_index <= N*3/8) THEN
      addr_temp <= coeff_index - N/4;
      opcode <= "010";
      --second half
    ELSIF(coeff_index < N/2) THEN
      addr_temp <= N/2 - coeff_index;
      opcode <= "011";
      --first half of the third quarter
    ELSIF(coeff_index <= N*5/8) THEN
      addr_temp <= coeff_index - N/2;
      opcode <= "100";
      --second half of the third quarter
    ELSE
      addr_temp <= N*3/4 - coeff_index;
      opcode <= "101";
    END IF;        
  end process gen_output;

  addr_out <= CONV_STD_LOGIC_VECTOR(addr_temp, addr_width);          

end RTL2;
