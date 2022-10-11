
-- library IEEE;
-- use IEEE.std_logic_1164.all;
-- LIBRARY WORK;
-- 
-- package float_sabrewing_opcodes_with_half_and_msu is
--   constant OPC_ADD   : std_logic_vector(4 downto 0) := "00001";
--   constant OPC_ADDF  : std_logic_vector(4 downto 0) := "00010";
--   constant OPC_ADDH  : std_logic_vector(4 downto 0) := "00011";
--   constant OPC_EQ    : std_logic_vector(4 downto 0) := "00100";
--   constant OPC_EQF   : std_logic_vector(4 downto 0) := "00101";
--   constant OPC_EQH   : std_logic_vector(4 downto 0) := "00110";
--   constant OPC_GT    : std_logic_vector(4 downto 0) := "00111";
--   constant OPC_GTF   : std_logic_vector(4 downto 0) := "01000";
--   constant OPC_GTH   : std_logic_vector(4 downto 0) := "01001";
--   constant OPC_LT    : std_logic_vector(4 downto 0) := "01010";
--   constant OPC_LTF   : std_logic_vector(4 downto 0) := "01011";
--   constant OPC_LTH   : std_logic_vector(4 downto 0) := "01100";
--   constant OPC_MAC   : std_logic_vector(4 downto 0) := "01101";
--   constant OPC_MACF  : std_logic_vector(4 downto 0) := "01110";
--   constant OPC_MACH  : std_logic_vector(4 downto 0) := "01111";
--   constant OPC_MSU   : std_logic_vector(4 downto 0) := "10000";
--   constant OPC_MSUF  : std_logic_vector(4 downto 0) := "10001";
--   constant OPC_MSUH  : std_logic_vector(4 downto 0) := "10010";
--   constant OPC_MUL   : std_logic_vector(4 downto 0) := "10011";
--   constant OPC_MULF  : std_logic_vector(4 downto 0) := "10100";
--   constant OPC_MULH  : std_logic_vector(4 downto 0) := "10101";
--   constant OPC_SHL   : std_logic_vector(4 downto 0) := "10110";
--   constant OPC_SHR   : std_logic_vector(4 downto 0) := "10111";
--   constant OPC_SUB   : std_logic_vector(4 downto 0) := "11000";
--   constant OPC_SUBF  : std_logic_vector(4 downto 0) := "11001";
--   constant OPC_SUBH  : std_logic_vector(4 downto 0) := "11010";
-- end float_sabrewing_opcodes_with_half_and_msu;

library IEEE;
use IEEE.std_logic_1164.all;
LIBRARY WORK;

package float_sabrewing_opcodes is
  constant OPC_ADD   : std_logic_vector(3 downto 0) := "0000";
  constant OPC_ADDF  : std_logic_vector(3 downto 0) := "0001";
  constant OPC_EQ    : std_logic_vector(3 downto 0) := "0010";
  constant OPC_EQF   : std_logic_vector(3 downto 0) := "0011";
  constant OPC_GT    : std_logic_vector(3 downto 0) := "0100";
  constant OPC_GTF   : std_logic_vector(3 downto 0) := "0101";
  constant OPC_LT    : std_logic_vector(3 downto 0) := "0110";
  constant OPC_LTF   : std_logic_vector(3 downto 0) := "0111";
  constant OPC_MAC   : std_logic_vector(3 downto 0) := "1000";
  constant OPC_MACF  : std_logic_vector(3 downto 0) := "1001";
  constant OPC_MUL   : std_logic_vector(3 downto 0) := "1010";
  constant OPC_MULF  : std_logic_vector(3 downto 0) := "1011";
  constant OPC_SHL   : std_logic_vector(3 downto 0) := "1100";
  constant OPC_SHR   : std_logic_vector(3 downto 0) := "1101";
  constant OPC_SUB   : std_logic_vector(3 downto 0) := "1110";
  constant OPC_SUBF  : std_logic_vector(3 downto 0) := "1111";
end float_sabrewing_opcodes;
  

library ieee;
use ieee.numeric_std.all;
use ieee.std_logic_1164.all;
use work.alu_pkg_lvl1.all;
use work.alu_pkg_lvl2.all;

library ieee;
use work.alu_pkg_lvl2.all;
use ieee.std_logic_1164.all;
use work.float_sabrewing_opcodes.all;

ENTITY sabrewing_tce IS
  GENERIC(
    dataw    : integer := 32;
    busw     : integer := 32;
    ew       : integer := 8;
    mw       : integer := 23;
    bypass_2 : boolean := True
    );
  PORT (
    clk      : IN std_logic;
    rstx     : IN std_logic;
    glock    : IN std_logic;
    t1load   : IN std_logic;
    t1opcode : IN std_logic_vector(3 DOWNTO 0);
    o1load   : IN std_logic;
    o2load   : IN std_logic;
    t1data   : IN  std_logic_vector(dataw-1 DOWNTO 0);  
    o1data   : IN  std_logic_vector(dataw-1 DOWNTO 0);
    o2data   : IN  std_logic_vector(dataw-1 DOWNTO 0);
    r1data   : OUT std_logic_vector(busw-1 DOWNTO 0)
    );

END sabrewing_tce;


ARCHITECTURE rtl OF sabrewing_tce IS

  CONSTANT guard_bits : INTEGER := 0; --guard bits for extra
                  
  component alu is
    port(
      clk      : in std_logic;
      rstx     : in std_logic;
      glock    : in std_logic;
      operand_a1 : in std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
      operand_a2 : in std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
      operand_b1 : in std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
      operand_b2 : in std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
      operand_c1 : in std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
      operand_c2 : in std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
      instruction_sel : in std_logic_vector(INSTRUCTIONWIDTH-1 downto 0);
      status : out std_logic_vector(STATUSWIDTH-1 downto 0);
      result_l : out std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
      result_r : out std_logic_vector(SIGNIFICANDWIDTH-1 downto 0)
    );
  end component;

  signal opcodereg :  std_logic_vector(3 DOWNTO 0);
  signal o1reg     :  std_logic_vector(dataw-1 DOWNTO 0);
  signal t1reg     :  std_logic_vector(dataw-1 DOWNTO 0);  
  signal o2reg     :  std_logic_vector(dataw-1 DOWNTO 0);


  alias sign_o1 : std_logic is o1reg(ew+mw);
  alias sign_t1 : std_logic is t1reg(ew+mw);
  alias sign_o2 : std_logic is o2reg(ew+mw);
  alias exponent_o1 : std_logic_vector(ew-1 downto 0) is o1reg(ew+mw-1 downto mw);
  alias exponent_t1 : std_logic_vector(ew-1 downto 0) is t1reg(ew+mw-1 downto mw);
  alias exponent_o2 : std_logic_vector(ew-1 downto 0) is o2reg(ew+mw-1 downto mw);
  alias significand_o1 : std_logic_vector(mw-1 downto 0) is o1reg(mw-1 downto 0);
  alias significand_t1 : std_logic_vector(mw-1 downto 0) is t1reg(mw-1 downto 0);
  alias significand_o2 : std_logic_vector(mw-1 downto 0) is o2reg(mw-1 downto 0);


  signal alu_operand_a1_next : std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
  signal alu_operand_a2_next : std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
  signal alu_operand_b1_next : std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
  signal alu_operand_b2_next : std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
  signal alu_operand_c1_next : std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
  signal alu_operand_c2_next : std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
  signal alu_instruction_sel_next : std_logic_vector(INSTRUCTIONWIDTH-1 downto 0);

  signal alu_operand_a1 : std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
  signal alu_operand_a2 : std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
  signal alu_operand_b1 : std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
  signal alu_operand_b2 : std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
  signal alu_operand_c1 : std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
  signal alu_operand_c2 : std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
  signal alu_instruction_sel : std_logic_vector(INSTRUCTIONWIDTH-1 downto 0);

  signal alu_status : std_logic_vector(STATUSWIDTH-1 downto 0);
  signal alu_result_l : std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
  signal alu_result_r : std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
  


  -- Truncated t1data, o1data, r1data with shorter word lengths 
  signal t1trun : std_logic_vector(ew + mw downto 0);
  signal o1trun : std_logic_vector(ew + mw downto 0);
  signal o2trun : std_logic_vector(ew + mw downto 0);
  signal r1trun : std_logic_vector(ew + mw downto 0);

  signal o1tempdata : std_logic_vector(ew + mw downto 0);
  signal o2tempdata : std_logic_vector(ew + mw downto 0);


BEGIN  
 
  wing : alu
    PORT MAP (
      operand_a1 => alu_operand_a1,
      operand_a2 => alu_operand_a2,
      operand_b1 => alu_operand_b1,
      operand_b2 => alu_operand_b2,
      operand_c1 => alu_operand_c1,
      operand_c2 => alu_operand_c2,
      instruction_sel => alu_instruction_sel,
      status => alu_status,
      result_l => alu_result_l,
      result_r => alu_result_r,

      clk => clk,
      rstx => rstx,
      glock => glock );
      
  -- Must use internally smaller word length
  t1trun <= t1data(ew + mw downto 0);
  o1trun <= o1data(ew + mw downto 0);
  o2trun <= o2data(ew + mw downto 0);
  r1data(ew + mw downto 0) <= r1trun;
  r1data(busw-1 downto ew + mw + 1) <= (others => '0');

  wrapper: process(t1reg, o1reg, o2reg, opcodereg)
  BEGIN
    -- TCE block: o1+t1*o2
    -- ALU block: a*b+c
    -- o1 => c, t1 => a, o2 => b

    if (opcodereg = OPC_ADD) or (opcodereg = OPC_SUB) or 
        (opcodereg = OPC_MAC) or (opcodereg = OPC_MUL) then
      alu_instruction_sel_next <= INTMAC;
    elsif (opcodereg = OPC_ADDF) or (opcodereg = OPC_SUBF) or 
          (opcodereg = OPC_MACF) or (opcodereg = OPC_MULF) then
      alu_instruction_sel_next <= SPFMAZ;
    elsif (opcodereg = OPC_LTF) then
      alu_instruction_sel_next <= SPFLTV;
    elsif (opcodereg = OPC_GTF) then
      alu_instruction_sel_next <= SPFGTV;
    elsif (opcodereg = OPC_EQF) then
      alu_instruction_sel_next <= SPFETV;
    elsif (opcodereg = OPC_LT) then
      alu_instruction_sel_next <= INTLTV;
    elsif (opcodereg = OPC_GT) then
      alu_instruction_sel_next <= INTGTV;
    elsif (opcodereg = OPC_EQ) then
      alu_instruction_sel_next <= INTETV;
    elsif (opcodereg = OPC_SHL) then
      alu_instruction_sel_next <= INTSLV;
    elsif (opcodereg = OPC_SHR) then
      alu_instruction_sel_next <= INTSRV;
    end if;
    
    if (opcodereg = OPC_ADDF) or (opcodereg = OPC_SUBF) or
       (opcodereg = OPC_MACF) or (opcodereg = OPC_MULF) or
       (opcodereg = OPC_EQF)  or (opcodereg = OPC_GTF)  or
       (opcodereg = OPC_LTF) then

      alu_operand_a1_next(SIGNIFICANDWIDTH-1 downto 0) <= (others=>'0');
      alu_operand_b1_next(SIGNIFICANDWIDTH-1 downto 0) <= (others=>'0');
      alu_operand_c1_next(SIGNIFICANDWIDTH-1 downto 0) <= (others=>'0');

      alu_operand_a2_next <= t1reg;

      if (opcodereg = OPC_ADDF) then
        alu_operand_b2_next <= x"3f800000";
      elsif (opcodereg = OPC_SUBF)  then
        alu_operand_b2_next <= x"bf800000";
      else
        alu_operand_b2_next <= o2reg;
      end if;

      if (opcodereg = OPC_MULF) then
        alu_operand_c2_next <= (others=>'0');
      else
        alu_operand_c2_next <= o1reg;
      end if;

    else
      -- int operation
      alu_operand_a1_next <= (others=>'0');
      alu_operand_b1_next <= (others=>'0');
      alu_operand_c1_next <= (others=>'0');

      alu_operand_a2_next <= t1reg;

      if (opcodereg = OPC_ADD) then
        alu_operand_b2_next(SIGNIFICANDWIDTH-1 downto 1) <= (others=>'0');  -- a+1*c
        alu_operand_b2_next(0) <= '1';
      elsif (opcodereg = OPC_SUB) then
        alu_operand_b2_next(SIGNIFICANDWIDTH-1 downto 0) <= (others=>'1');  -- a+(-1)*c
      else
        alu_operand_b2_next <= o2reg;
      end if;

      if (opcodereg = OPC_MUL) then
        alu_operand_c2_next <= (others=>'0');
      else
        alu_operand_c2_next <= o1reg;
      end if;
    end if;


  END PROCESS wrapper;

  fpu: PROCESS (clk, rstx)
  BEGIN  -- PROCESS fpu
    IF(rstx = '0') THEN  
      t1reg <= (OTHERS => '0');
      o1reg <= (OTHERS => '0');
      o2reg <= (OTHERS => '0');
      opcodereg <= (OTHERS => '0');
      o1tempdata <= (OTHERS => '0');
      o2tempdata <= (OTHERS => '0');

      if( bypass_2 = False ) then
        alu_operand_a1 <= (OTHERS => '0');
        alu_operand_a2 <= (OTHERS => '0');
        alu_operand_b1 <= (OTHERS => '0');
        alu_operand_b2 <= (OTHERS => '0');
        alu_operand_c1 <= (OTHERS => '0');
        alu_operand_c2 <= (OTHERS => '0');
        alu_instruction_sel <= (OTHERS => '0');
      end if;
    ELSIF(clk'event AND clk = '1') then
      if(glock = '0') then
        if( t1load = '1' ) then
          t1reg <= t1trun;
          opcodereg <= t1opcode;

          if( o1load = '1' ) then
            o1reg <= o1trun;
          else
            o1reg <= o1tempdata;
          end if;

          if( o2load = '1' ) then
            o2reg <= o2trun;
          else
            o2reg <= o2tempdata;
          end if;
        end if;

        if( o1load = '1' ) then
          o1tempdata <= o1trun;
        end if;

        if( o2load = '1' ) then
          o2tempdata <= o2trun;
        end if;

        if( bypass_2 = False ) then
          alu_operand_a1 <= alu_operand_a1_next;
          alu_operand_a2 <= alu_operand_a2_next;
          alu_operand_b1 <= alu_operand_b1_next;
          alu_operand_b2 <= alu_operand_b2_next;
          alu_operand_c1 <= alu_operand_c1_next;
          alu_operand_c2 <= alu_operand_c2_next;
          alu_instruction_sel <= alu_instruction_sel_next;
        end if;
      END IF;
    END IF;  
    if( bypass_2 = True ) then
      alu_operand_a1 <= alu_operand_a1_next;
      alu_operand_a2 <= alu_operand_a2_next;
      alu_operand_b1 <= alu_operand_b1_next;
      alu_operand_b2 <= alu_operand_b2_next;
      alu_operand_c1 <= alu_operand_c1_next;
      alu_operand_c2 <= alu_operand_c2_next;
      alu_instruction_sel <= alu_instruction_sel_next;
    end if;
  END PROCESS fpu;
  
  results : process ( alu_status, alu_result_l, alu_result_r ) 
  begin
    if alu_status=STATUS_LOGIC_TRUE then
      r1trun(mw+ew downto 1) <= (others=>'0');
      r1trun(0) <= '1';
    elsif alu_status=STATUS_LOGIC_FALSE then
      r1trun(r1trun'range) <= (others=>'0');
    else
      r1trun <= alu_result_r;
    end if;
  end process;
END rtl;
