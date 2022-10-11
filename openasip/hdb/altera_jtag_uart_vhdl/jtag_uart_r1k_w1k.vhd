--Legal Notice: (C)2011 Altera Corporation. All rights reserved.  Your
--use of Altera Corporation's design tools, logic functions and other
--software and tools, and its AMPP partner logic functions, and any
--output files any of the foregoing (including device programming or
--simulation files), and any associated documentation or information are
--expressly subject to the terms and conditions of the Altera Program
--License Subscription Agreement or other applicable license agreement,
--including, without limitation, that your use is for the sole purpose
--of programming logic devices manufactured by Altera and sold by Altera
--or its authorized distributors.  Please refer to the applicable
--agreement for further details.


-- turn off superfluous VHDL processor warnings 
-- altera message_level Level1 
-- altera message_off 10034 10035 10036 10037 10230 10240 10030 

library altera;
use altera.altera_europa_support_lib.all;

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

library std;
use std.textio.all;

entity jtag_uart_log_module is 
        port (
              -- inputs:
                 signal clk : IN STD_LOGIC;
                 signal data : IN STD_LOGIC_VECTOR (7 DOWNTO 0);
                 signal strobe : IN STD_LOGIC;
                 signal valid : IN STD_LOGIC
              );
end entity jtag_uart_log_module;


architecture europa of jtag_uart_log_module is
  
  file text_handle : TEXT ;
                           
  -- synthesis translate_off
  -- purpose: convert 8 bit signal data to 8 bit string
  FUNCTION bin_to_char(vec_to_convert : STD_LOGIC_VECTOR (7 downto 0))
    RETURN CHARACTER IS
    VARIABLE result: CHARACTER;
  BEGIN
    CASE vec_to_convert IS      -- cover basic ascii printable characters...
      when X"0a" => result := lf; -- \n, linefeed
      when X"0d" => result := nul; -- \r, Ctrl-M
      when X"09" => result := ht; -- \t, Ctrl-I, TAB
      when X"20" => result := ' ' ;
      when X"21" => result := '!' ;
      when X"22" => result := '"' ;
      when X"23" => result := '#' ;
      when X"24" => result := '$' ;
      when X"25" => result := '%' ;
      when X"26" => result := '&' ;
      when X"27" => result := ''' ; -- sync ' char for hilighting txt editors
      when X"28" => result := '(' ;
      when X"29" => result := ')' ;
      when X"2a" => result := '*' ;
      when X"2b" => result := '+' ;
      when X"2c" => result := ',' ;
      when X"2d" => result := '-' ;
      when X"2e" => result := '.' ;
      when X"2f" => result := '/' ;
      when X"30" => result := '0' ;
      when X"31" => result := '1' ;
      when X"32" => result := '2' ;
      when X"33" => result := '3' ;
      when X"34" => result := '4' ;
      when X"35" => result := '5' ;
      when X"36" => result := '6' ;
      when X"37" => result := '7' ;
      when X"38" => result := '8' ;
      when X"39" => result := '9' ;
      when X"3a" => result := ':' ;
      when X"3b" => result := ';' ;
      when X"3c" => result := '<' ;
      when X"3d" => result := '=' ;
      when X"3e" => result := '>' ;
      when X"3f" => result := '?' ;
      when X"40" => result := '@' ;
      when X"41" => result := 'A' ;
      when X"42" => result := 'B' ;
      when X"43" => result := 'C' ;
      when X"44" => result := 'D' ;
      when X"45" => result := 'E' ;
      when X"46" => result := 'F' ;
      when X"47" => result := 'G' ;
      when X"48" => result := 'H' ;
      when X"49" => result := 'I' ;
      when X"4a" => result := 'J' ;
      when X"4b" => result := 'K' ;
      when X"4c" => result := 'L' ;
      when X"4d" => result := 'M' ;
      when X"4e" => result := 'N' ;
      when X"4f" => result := 'O' ;
      when X"50" => result := 'P' ;
      when X"51" => result := 'Q' ;
      when X"52" => result := 'R' ;
      when X"53" => result := 'S' ;
      when X"54" => result := 'T' ;
      when X"55" => result := 'U' ;
      when X"56" => result := 'V' ;
      when X"57" => result := 'W' ;
      when X"58" => result := 'X' ;
      when X"59" => result := 'Y' ;
      when X"5a" => result := 'Z' ;
      when X"5b" => result := '[' ;
      when X"5c" => result := '\' ;
      when X"5d" => result := ']' ;
      when X"5e" => result := '^' ;
      when X"5f" => result := '_' ;
      when X"60" => result := '`' ;
      when X"61" => result := 'a' ;
      when X"62" => result := 'b' ;
      when X"63" => result := 'c' ;
      when X"64" => result := 'd' ;
      when X"65" => result := 'e' ;
      when X"66" => result := 'f' ;
      when X"67" => result := 'g' ;
      when X"68" => result := 'h' ;
      when X"69" => result := 'i' ;
      when X"6a" => result := 'j' ;
      when X"6b" => result := 'k' ;
      when X"6c" => result := 'l' ;
      when X"6d" => result := 'm' ;
      when X"6e" => result := 'n' ;
      when X"6f" => result := 'o' ;
      when X"70" => result := 'p' ;
      when X"71" => result := 'q' ;
      when X"72" => result := 'r' ;
      when X"73" => result := 's' ;
      when X"74" => result := 't' ;
      when X"75" => result := 'u' ;
      when X"76" => result := 'v' ;
      when X"77" => result := 'w' ;
      when X"78" => result := 'x' ;
      when X"79" => result := 'y' ;
      when X"7a" => result := 'z' ;
      when X"7b" => result := '{' ;
      when X"7c" => result := '|' ;
      when X"7d" => result := '}' ;
      when X"7e" => result := '~' ;
      when X"7f" => result := '_' ;
      WHEN others =>
        ASSERT False REPORT "data contains a non-printable character" SEVERITY Warning;
        result := nul;
    END case;
    RETURN result;
  end bin_to_char;
  -- synthesis translate_on
                             

begin

--synthesis translate_off


  -- purpose: simulate verilog initial function to open file in write mode
  -- type   : combinational
  -- inputs : initial
  -- outputs: <none>
  process is
    variable initial : boolean := true; -- not initialized yet
    variable status : file_open_status; -- status for fopen
  begin  -- process
    if initial = true then
      file_open (status, text_handle, "/home/otski/div/unnamed_sim/jtag_uart_output_stream.dat", WRITE_MODE);
      initial := false;                 -- done!
    end if;
    wait;                               -- wait forever
  end process;

  process (clk)
    variable data_string : LINE;        -- for line buffer to file
    variable status : file_open_status; -- status for fopen
                        
    variable echo_string : LINE;        -- for line buffer to screen (stdout)
                          
  begin  -- process clk
    if clk'event and clk = '1' then -- sync ' chars for hilighting txt editors
      if (valid and strobe) = '1' then
                        
        write (data_string,To_bitvector(data)); -- every char flushes line
        writeline (text_handle,data_string);
        file_close (text_handle);     -- flush buffer
        file_open (status, text_handle, "/home/otski/div/unnamed_sim/jtag_uart_output_stream.dat", APPEND_MODE);
                           
        -- save up characters into a line to send to the screen
        write (echo_string,bin_to_char(data));
        if data = X"0a" or data = X"0d" then -- \n or \r will flush line
          writeline (output,echo_string);
        end if;
                          
      end if;
    end if;
  end process;
                       --synthesis translate_on

end europa;



-- turn off superfluous VHDL processor warnings 
-- altera message_level Level1 
-- altera message_off 10034 10035 10036 10037 10230 10240 10030 

library altera;
use altera.altera_europa_support_lib.all;

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity jtag_uart_sim_scfifo_w is 
        port (
              -- inputs:
                 signal clk : IN STD_LOGIC;
                 signal fifo_wdata : IN STD_LOGIC_VECTOR (7 DOWNTO 0);
                 signal fifo_wr : IN STD_LOGIC;

              -- outputs:
                 signal fifo_FF : OUT STD_LOGIC;
                 signal r_dat : OUT STD_LOGIC_VECTOR (7 DOWNTO 0);
                 signal wfifo_empty : OUT STD_LOGIC;
                 signal wfifo_used : OUT STD_LOGIC_VECTOR (9 DOWNTO 0)
              );
end entity jtag_uart_sim_scfifo_w;


architecture europa of jtag_uart_sim_scfifo_w is
--synthesis translate_off
component jtag_uart_log_module is 
           port (
                 -- inputs:
                    signal clk : IN STD_LOGIC;
                    signal data : IN STD_LOGIC_VECTOR (7 DOWNTO 0);
                    signal strobe : IN STD_LOGIC;
                    signal valid : IN STD_LOGIC
                 );
end component jtag_uart_log_module;

--synthesis translate_on

begin

--synthesis translate_off
    --jtag_uart_log, which is an e_log
    jtag_uart_log : jtag_uart_log_module
      port map(
        clk => clk,
        data => fifo_wdata,
        strobe => fifo_wr,
        valid => fifo_wr
      );


    wfifo_used <= A_REP(std_logic'('0'), 10);
    r_dat <= A_REP(std_logic'('0'), 8);
    fifo_FF <= std_logic'('0');
    wfifo_empty <= std_logic'('1');
--synthesis translate_on

end europa;



-- turn off superfluous VHDL processor warnings 
-- altera message_level Level1 
-- altera message_off 10034 10035 10036 10037 10230 10240 10030 

library altera;
use altera.altera_europa_support_lib.all;

library altera_mf;
use altera_mf.all;

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

library lpm;
use lpm.all;

entity jtag_uart_scfifo_w is 
        port (
              -- inputs:
                 signal clk : IN STD_LOGIC;
                 signal fifo_clear : IN STD_LOGIC;
                 signal fifo_wdata : IN STD_LOGIC_VECTOR (7 DOWNTO 0);
                 signal fifo_wr : IN STD_LOGIC;
                 signal rd_wfifo : IN STD_LOGIC;

              -- outputs:
                 signal fifo_FF : OUT STD_LOGIC;
                 signal r_dat : OUT STD_LOGIC_VECTOR (7 DOWNTO 0);
                 signal wfifo_empty : OUT STD_LOGIC;
                 signal wfifo_used : OUT STD_LOGIC_VECTOR (9 DOWNTO 0)
              );
end entity jtag_uart_scfifo_w;


architecture europa of jtag_uart_scfifo_w is
--synthesis translate_off
component jtag_uart_sim_scfifo_w is 
           port (
                 -- inputs:
                    signal clk : IN STD_LOGIC;
                    signal fifo_wdata : IN STD_LOGIC_VECTOR (7 DOWNTO 0);
                    signal fifo_wr : IN STD_LOGIC;

                 -- outputs:
                    signal fifo_FF : OUT STD_LOGIC;
                    signal r_dat : OUT STD_LOGIC_VECTOR (7 DOWNTO 0);
                    signal wfifo_empty : OUT STD_LOGIC;
                    signal wfifo_used : OUT STD_LOGIC_VECTOR (9 DOWNTO 0)
                 );
end component jtag_uart_sim_scfifo_w;

--synthesis translate_on
--synthesis read_comments_as_HDL on
--  component scfifo is
--GENERIC (
--      lpm_hint : STRING;
--        lpm_numwords : NATURAL;
--        lpm_showahead : STRING;
--        lpm_type : STRING;
--        lpm_width : NATURAL;
--        lpm_widthu : NATURAL;
--        overflow_checking : STRING;
--        underflow_checking : STRING;
--        use_eab : STRING
--      );
--    PORT (
--    signal full : OUT STD_LOGIC;
--        signal q : OUT STD_LOGIC_VECTOR (7 DOWNTO 0);
--        signal usedw : OUT STD_LOGIC_VECTOR (9 DOWNTO 0);
--        signal empty : OUT STD_LOGIC;
--        signal rdreq : IN STD_LOGIC;
--        signal aclr : IN STD_LOGIC;
--        signal data : IN STD_LOGIC_VECTOR (7 DOWNTO 0);
--        signal clock : IN STD_LOGIC;
--        signal wrreq : IN STD_LOGIC
--      );
--  end component scfifo;
--synthesis read_comments_as_HDL off
                signal internal_fifo_FF :  STD_LOGIC;
                signal internal_r_dat :  STD_LOGIC_VECTOR (7 DOWNTO 0);
                signal internal_wfifo_empty :  STD_LOGIC;
                signal internal_wfifo_used :  STD_LOGIC_VECTOR (9 DOWNTO 0);

begin

  --vhdl renameroo for output signals
  fifo_FF <= internal_fifo_FF;
  --vhdl renameroo for output signals
  r_dat <= internal_r_dat;
  --vhdl renameroo for output signals
  wfifo_empty <= internal_wfifo_empty;
  --vhdl renameroo for output signals
  wfifo_used <= internal_wfifo_used;
--synthesis translate_off
    --the_jtag_uart_sim_scfifo_w, which is an e_instance
    the_jtag_uart_sim_scfifo_w : jtag_uart_sim_scfifo_w
      port map(
        fifo_FF => internal_fifo_FF,
        r_dat => internal_r_dat,
        wfifo_empty => internal_wfifo_empty,
        wfifo_used => internal_wfifo_used,
        clk => clk,
        fifo_wdata => fifo_wdata,
        fifo_wr => fifo_wr
      );


--synthesis translate_on
--synthesis read_comments_as_HDL on
--    wfifo : scfifo
--      generic map(
--        lpm_hint => "RAM_BLOCK_TYPE=AUTO",
--        lpm_numwords => 1024,
--        lpm_showahead => "OFF",
--        lpm_type => "scfifo",
--        lpm_width => 8,
--        lpm_widthu => 10,
--        overflow_checking => "OFF",
--        underflow_checking => "OFF",
--        use_eab => "ON"
--      )
--      port map(
--                aclr => fifo_clear,
--                clock => clk,
--                data => fifo_wdata,
--                empty => internal_wfifo_empty,
--                full => internal_fifo_FF,
--                q => internal_r_dat,
--                rdreq => rd_wfifo,
--                usedw => internal_wfifo_used,
--                wrreq => fifo_wr
--      );
--
--synthesis read_comments_as_HDL off

end europa;



-- turn off superfluous VHDL processor warnings 
-- altera message_level Level1 
-- altera message_off 10034 10035 10036 10037 10230 10240 10030 

library altera;
use altera.altera_europa_support_lib.all;

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

library std;
use std.textio.all;

entity jtag_uart_drom_module is 
        generic (
                 POLL_RATE : integer := 100
                 );
        port (
              -- inputs:
                 signal clk : IN STD_LOGIC;
                 signal incr_addr : IN STD_LOGIC;
                 signal reset_n : IN STD_LOGIC;

              -- outputs:
                 signal new_rom : OUT STD_LOGIC;
                 signal num_bytes : OUT STD_LOGIC_VECTOR (31 DOWNTO 0);
                 signal q : OUT STD_LOGIC_VECTOR (7 DOWNTO 0);
                 signal safe : OUT STD_LOGIC
              );
end entity jtag_uart_drom_module;


architecture europa of jtag_uart_drom_module is
                signal address :  STD_LOGIC_VECTOR (11 DOWNTO 0);
                signal d1_pre :  STD_LOGIC;
                signal d2_pre :  STD_LOGIC;
                signal d3_pre :  STD_LOGIC;
                signal d4_pre :  STD_LOGIC;
                signal d5_pre :  STD_LOGIC;
                signal d6_pre :  STD_LOGIC;
                signal d7_pre :  STD_LOGIC;
                signal d8_pre :  STD_LOGIC;
                signal d9_pre :  STD_LOGIC;
                TYPE mem_type is ARRAY( 2047 DOWNTO 0) of STD_LOGIC_VECTOR(7 DOWNTO 0);
              signal mem_array : mem_type;
                TYPE mem_type1 is ARRAY( 1 DOWNTO 0) of STD_LOGIC_VECTOR(31 DOWNTO 0);
              signal mutex : mem_type1;
                signal pre :  STD_LOGIC;
  
          signal safe_wire : STD_LOGIC; -- deal with bogus VHDL type casting
          signal safe_delay : STD_LOGIC; 
          FILE mutex_handle : TEXT ;  -- open this for read and write manually.
          -- stream can be opened simply for read...
          FILE stream_handle : TEXT open READ_MODE is "/home/otski/div/unnamed_sim/jtag_uart_input_stream.dat";

-- synthesis translate_off
-- convert functions deadlifted from e_rom.pm

FUNCTION convert_string_to_number(string_to_convert : STRING;
                                  final_char_index : NATURAL := 0)
  RETURN NATURAL IS
  VARIABLE result: NATURAL := 0;
  VARIABLE current_index : NATURAL := 1;
  VARIABLE the_char : CHARACTER;
  
BEGIN
  IF final_char_index = 0 THEN
    result := 0;
  ELSE
    WHILE current_index <= final_char_index LOOP
      the_char := string_to_convert(current_index);
      IF    '0' <= the_char AND the_char <= '9' THEN
        result := result * 16 + character'pos(the_char) - character'pos('0');
      ELSIF 'A' <= the_char AND the_char <= 'F' THEN
        result := result * 16 + character'pos(the_char) - character'pos('A') + 10;
      ELSIF 'a' <= the_char AND the_char <= 'f' THEN
        result := result * 16 + character'pos(the_char) - character'pos('a') + 10;
      ELSE
        report "convert_string_to_number: Ack, a formatting error!";
      END IF;
      current_index := current_index + 1;
    END LOOP;
  END IF; 
  RETURN result;
END convert_string_to_number;


FUNCTION convert_string_to_std_logic(value : STRING; num_chars : INTEGER; mem_width_chars : INTEGER)
  RETURN STD_LOGIC_VECTOR is                       
  VARIABLE num_bits: integer := mem_width_chars * 4;
  VARIABLE result: std_logic_vector(num_bits-1 downto 0);
  VARIABLE curr_char : integer;
  VARIABLE min_width : integer := mem_width_chars;
  VARIABLE num_nibbles : integer := 0;
  
BEGIN
  result := (others => '0');
  num_nibbles := mem_width_chars;
  IF (mem_width_chars > num_chars) THEN
    num_nibbles := num_chars;
  END IF;
  
  FOR I IN 1 TO num_nibbles LOOP
    curr_char := num_nibbles - (I-1);
    
    CASE value(I) IS
      WHEN '0' =>  result((4*curr_char)-1  DOWNTO 4*(curr_char-1)) := "0000";
      WHEN '1' =>  result((4*curr_char)-1  DOWNTO 4*(curr_char-1)) := "0001";
      WHEN '2' =>  result((4*curr_char)-1  DOWNTO 4*(curr_char-1)) := "0010";
      WHEN '3' =>  result((4*curr_char)-1  DOWNTO 4*(curr_char-1)) := "0011";
      WHEN '4' =>  result((4*curr_char)-1  DOWNTO 4*(curr_char-1)) := "0100";
      WHEN '5' =>  result((4*curr_char)-1  DOWNTO 4*(curr_char-1)) := "0101";
      WHEN '6' =>  result((4*curr_char)-1  DOWNTO 4*(curr_char-1)) := "0110";
      WHEN '7' =>  result((4*curr_char)-1  DOWNTO 4*(curr_char-1)) := "0111";
      WHEN '8' =>  result((4*curr_char)-1  DOWNTO 4*(curr_char-1)) := "1000";
      WHEN '9' =>  result((4*curr_char)-1  DOWNTO 4*(curr_char-1)) := "1001";
      WHEN 'A' | 'a' => result((4*curr_char)-1  DOWNTO 4*(curr_char-1)) := "1010";
      WHEN 'B' | 'b' => result((4*curr_char)-1  DOWNTO 4*(curr_char-1)) := "1011";
      WHEN 'C' | 'c' => result((4*curr_char)-1  DOWNTO 4*(curr_char-1)) := "1100";
      WHEN 'D' | 'd' => result((4*curr_char)-1  DOWNTO 4*(curr_char-1)) := "1101";
      WHEN 'E' | 'e' => result((4*curr_char)-1  DOWNTO 4*(curr_char-1)) := "1110";
      WHEN 'F' | 'f' => result((4*curr_char)-1  DOWNTO 4*(curr_char-1)) := "1111";
      WHEN ' ' => EXIT;
      WHEN HT  => exit;
      WHEN others =>
        ASSERT False
          REPORT "function From_Hex: string """ & value & """ contains non-hex character"
          severity Error;
        EXIT;
    END case;
  END loop;
  RETURN result;
END convert_string_to_std_logic;

-- purpose: open mutex/discard @address/convert value to std_logic_vector
function get_mutex_val (file_name : string)
  return STD_LOGIC_VECTOR is
  VARIABLE result : STD_LOGIC_VECTOR (31 downto 0) := X"00000000";
  FILE handle : TEXT ;
  VARIABLE status : file_open_status; -- status for fopen
  VARIABLE data_line : LINE;
  VARIABLE the_character_from_data_line : CHARACTER;
  VARIABLE converted_number : NATURAL := 0;
  VARIABLE found_string_array : STRING(1 TO 128);
  VARIABLE string_index : NATURAL := 0;
  VARIABLE line_length : NATURAL := 0; 
 
begin  -- get_mutex_val

  file_open (status, handle, file_name, READ_MODE);

  WHILE NOT(endfile(handle)) LOOP
    readline(handle, data_line);
    line_length := data_line'LENGTH; -- match ' for emacs font-lock
    
    WHILE line_length > 0 LOOP
      read(data_line, the_character_from_data_line);
      -- check for the @ character indicating a new address wad
      -- if found, ignore the line!  This is just protection
      IF '@' = the_character_from_data_line THEN
        exit;                           -- bail out of this line
      end if;
      -- process the hex address, character by character ...
      IF NOT(' ' = the_character_from_data_line) THEN
        string_index := string_index + 1;
        found_string_array(string_index) := the_character_from_data_line;
      END IF;
      line_length := line_length - 1; 
    end loop;                           -- read characters

  end loop;                             -- read lines

  file_close (handle);
  
  if string_index /= 0 then
    result := convert_string_to_std_logic(found_string_array, string_index, 8);
  end if;

  return result;
  
end get_mutex_val;

-- purpose: emulate verilogs readmemh function (mostly)
-- in verilog you say: $readmemh ("file", array);
-- in VHDL, we say: array <= readmemh("file"); -- which makes more sense.
function readmemh (file_name : string)
  return mem_type is
  VARIABLE result : mem_type;
  FILE handle : TEXT ;
  VARIABLE status : file_open_status; -- status for fopen
  VARIABLE data_line : LINE;
  VARIABLE b_address : BOOLEAN := FALSE; -- distinguish between addrs and data
  VARIABLE the_character_from_data_line : CHARACTER;
  VARIABLE converted_number : NATURAL := 0;
  VARIABLE found_string_array : STRING(1 TO 128);
  VARIABLE string_index : NATURAL := 0;
  VARIABLE line_length : NATURAL := 0; 
  VARIABLE load_address : NATURAL := 0;
  VARIABLE mem_index : NATURAL := 0;
begin  -- readmemh

  file_open (status, handle, file_name, READ_MODE);

  WHILE NOT(endfile(handle)) LOOP
    readline(handle, data_line);
    line_length := data_line'LENGTH; -- match ' for emacs font-lock
    b_address := false;

    WHILE line_length > 0 LOOP
      read(data_line, the_character_from_data_line);
      -- check for the @ character indicating a new address wad
      -- if found, ignore the line!  This is just protection
      IF '@' = the_character_from_data_line and not b_address then -- is addr
        b_address := true;
      end if;
      -- process the hex address, character by character ...
      IF NOT((' ' = the_character_from_data_line) or
	     ('@' = the_character_from_data_line) or
             (lf = the_character_from_data_line) or
	     (cr = the_character_from_data_line)) THEN
        string_index := string_index + 1;
        found_string_array(string_index) := the_character_from_data_line;
      END IF;
      line_length := line_length - 1;
    end loop;                           -- read characters

    if b_address then
      mem_index := convert_string_to_number(found_string_array, string_index);
      b_address := FALSE;
    else
      result(mem_index) := convert_string_to_std_logic(found_string_array, string_index, 2);
    end if;

    string_index := 0;

  end loop;                             -- read lines

  file_close (handle);
  
  return result;
  
end readmemh;
                           

-- purpose: emulate verilogs readmemb function (mostly)
-- in verilog you say: $readmemb ("file", array);
-- in VHDL, we say: array <= readmemb("file"); -- which makes more sense.
function readmemb (file_name : string)
  return mem_type is
  VARIABLE result : mem_type;
  FILE handle : TEXT ;
  VARIABLE status : file_open_status; -- status for fopen
  VARIABLE data_line : LINE;
  VARIABLE the_character_from_data_line : BIT_VECTOR(7 DOWNTO 0); -- '0' & '1's
  VARIABLE line_length : NATURAL := 0; 
  VARIABLE mem_index : NATURAL := 0;
begin  -- readmemb

  file_open (status, handle, file_name, READ_MODE);

  WHILE NOT(endfile(handle)) LOOP
    readline(handle, data_line);
    line_length := data_line'LENGTH; -- match ' for emacs font-lock

    WHILE line_length > 7 LOOP
      read(data_line, the_character_from_data_line);
      -- No @ characters allowed in binary/bit_vector mode
      result(mem_index) := To_stdlogicvector(the_character_from_data_line);
      mem_index := mem_index + 1;
      line_length := line_length - 8;
    end loop;                           -- read characters

  end loop;                             -- read lines

  file_close (handle);
  
  return result;
  
end readmemb;
                          
-- synthesis translate_on
                      

begin

--synthesis translate_off
    q <= mem_array(CONV_INTEGER(UNSIGNED((address))));
    process (clk, reset_n)
    begin
      if reset_n = '0' then
        d1_pre <= std_logic'('0');
        d2_pre <= std_logic'('0');
        d3_pre <= std_logic'('0');
        d4_pre <= std_logic'('0');
        d5_pre <= std_logic'('0');
        d6_pre <= std_logic'('0');
        d7_pre <= std_logic'('0');
        d8_pre <= std_logic'('0');
        d9_pre <= std_logic'('0');
        new_rom <= std_logic'('0');
      elsif clk'event and clk = '1' then
        if (std_logic_vector'("00000000000000000000000000000001")) /= std_logic_vector'("00000000000000000000000000000000") then 
          d1_pre <= pre;
          d2_pre <= d1_pre;
          d3_pre <= d2_pre;
          d4_pre <= d3_pre;
          d5_pre <= d4_pre;
          d6_pre <= d5_pre;
          d7_pre <= d6_pre;
          d8_pre <= d7_pre;
          d9_pre <= d8_pre;
          new_rom <= d9_pre;
        end if;
      end if;

    end process;


    num_bytes <= mutex(1);
                         
    safe <= safe_wire;
    safe_wire <= to_std_logic( address < mutex(1) );

    process (clk, reset_n)
    begin
      if reset_n = '0' then
        safe_delay <= '0';
      elsif clk'event and clk = '1' then -- balance ' for emacs quoting
        safe_delay <= safe_wire;
      end if;
    end process;

    process (clk, reset_n)
      variable poll_count : integer := POLL_RATE; -- STD_LOGIC_VECTOR (31:0);
      variable status : file_open_status; -- status for fopen
      variable mutex_string : LINE;  -- temp space for read/write data
      variable stream_string : LINE;  -- temp space for read data
      variable init_done : BOOLEAN; -- only used if non-interactive
      variable interactive : BOOLEAN := FALSE;
    begin
      if reset_n /= '1' then
        address <= "000000000000";
        mem_array(0) <= X"00";
        mutex(0) <= X"00000000";
        mutex(1) <= X"00000000";
        pre <= '0';
        init_done := FALSE;
      elsif clk'event and clk = '1' then -- balance ' for emacs quoting
        pre <= '0';
        if incr_addr = '1' and safe_wire = '1' then
          address <= address + "000000000001";
        end if;
        -- blast mutex via textio after falling edge of safe
        if mutex(0) /= X"00000000" and safe_wire = '0' and safe_delay = '1' then
	  if interactive then           -- bash mutex
            file_open (status, mutex_handle, "/home/otski/div/unnamed_sim/jtag_uart_input_mutex.dat", WRITE_MODE);
            write (mutex_string, string'("0")); -- balance ' for emacs quoting
            writeline (mutex_handle, mutex_string);
            file_close (mutex_handle);
            mutex(0) <= X"00000000";
	  else -- non-nteractive does not bash mutex: it stops poll counter
	    init_done := TRUE;
          end if;
        end if;
        if poll_count < POLL_RATE then  -- wait
          if not init_done then         -- stop counting if init_done is TRUE
            poll_count := poll_count + 1;
          end if;
        else                            -- do the real work
          poll_count := 0;
          -- get mutex via textio ...
          mutex(0) <= get_mutex_val ("/home/otski/div/unnamed_sim/jtag_uart_input_mutex.dat");
          if mutex(0) /= X"00000000" and safe_wire = '0' then
            -- read stream into array after previous stream is complete
            mutex (1) <= mutex (0); -- save mutex value for address compare
            -- get mem_array via textio ...
            mem_array <= readmemb("/home/otski/div/unnamed_sim/jtag_uart_input_stream.dat");
            -- prep address and pre-pulse to alert world to new contents
            address <= "000000000000";
            pre <= '1';
          end if; -- poll_count
        end if;   -- clock
      end if;     -- reset
    end process;
                         --synthesis translate_on

end europa;



-- turn off superfluous VHDL processor warnings 
-- altera message_level Level1 
-- altera message_off 10034 10035 10036 10037 10230 10240 10030 

library altera;
use altera.altera_europa_support_lib.all;

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity jtag_uart_sim_scfifo_r is 
        port (
              -- inputs:
                 signal clk : IN STD_LOGIC;
                 signal fifo_rd : IN STD_LOGIC;
                 signal rst_n : IN STD_LOGIC;

              -- outputs:
                 signal fifo_EF : OUT STD_LOGIC;
                 signal fifo_rdata : OUT STD_LOGIC_VECTOR (7 DOWNTO 0);
                 signal rfifo_full : OUT STD_LOGIC;
                 signal rfifo_used : OUT STD_LOGIC_VECTOR (9 DOWNTO 0)
              );
end entity jtag_uart_sim_scfifo_r;


architecture europa of jtag_uart_sim_scfifo_r is
--synthesis translate_off
component jtag_uart_drom_module is 
           generic (
                    POLL_RATE : integer := 100
                    );
           port (
                 -- inputs:
                    signal clk : IN STD_LOGIC;
                    signal incr_addr : IN STD_LOGIC;
                    signal reset_n : IN STD_LOGIC;

                 -- outputs:
                    signal new_rom : OUT STD_LOGIC;
                    signal num_bytes : OUT STD_LOGIC_VECTOR (31 DOWNTO 0);
                    signal q : OUT STD_LOGIC_VECTOR (7 DOWNTO 0);
                    signal safe : OUT STD_LOGIC
                 );
end component jtag_uart_drom_module;

--synthesis translate_on
                signal bytes_left :  STD_LOGIC_VECTOR (31 DOWNTO 0);
                signal fifo_rd_d :  STD_LOGIC;
                signal internal_fifo_rdata1 :  STD_LOGIC_VECTOR (7 DOWNTO 0);
                signal internal_rfifo_full1 :  STD_LOGIC;
                signal new_rom :  STD_LOGIC;
                signal num_bytes :  STD_LOGIC_VECTOR (31 DOWNTO 0);
                signal rfifo_entries :  STD_LOGIC_VECTOR (10 DOWNTO 0);
                signal safe :  STD_LOGIC;

begin

  --vhdl renameroo for output signals
  fifo_rdata <= internal_fifo_rdata1;
  --vhdl renameroo for output signals
  rfifo_full <= internal_rfifo_full1;
--synthesis translate_off
    --jtag_uart_drom, which is an e_drom
    jtag_uart_drom : jtag_uart_drom_module
      port map(
        new_rom => new_rom,
        num_bytes => num_bytes,
        q => internal_fifo_rdata1,
        safe => safe,
        clk => clk,
        incr_addr => fifo_rd_d,
        reset_n => rst_n
      );


    -- Generate rfifo_entries for simulation
    process (clk, rst_n)
    begin
      if rst_n = '0' then
        bytes_left <= std_logic_vector'("00000000000000000000000000000000");
        fifo_rd_d <= std_logic'('0');
      elsif clk'event and clk = '1' then
        fifo_rd_d <= fifo_rd;
        -- decrement on read
        if std_logic'(fifo_rd_d) = '1' then 
          bytes_left <= A_EXT (((std_logic_vector'("0") & (bytes_left)) - (std_logic_vector'("00000000000000000000000000000000") & (A_TOSTDLOGICVECTOR(std_logic'('1'))))), 32);
        end if;
        -- catch new contents
        if std_logic'(new_rom) = '1' then 
          bytes_left <= num_bytes;
        end if;
      end if;

    end process;

    fifo_EF <= to_std_logic((bytes_left = std_logic_vector'("00000000000000000000000000000000")));
    internal_rfifo_full1 <= to_std_logic((bytes_left>std_logic_vector'("00000000000000000000010000000000")));
    rfifo_entries <= A_EXT (A_WE_StdLogicVector((std_logic'((internal_rfifo_full1)) = '1'), std_logic_vector'("00000000000000000000010000000000"), bytes_left), 11);
    rfifo_used <= rfifo_entries(9 DOWNTO 0);
--synthesis translate_on

end europa;



-- turn off superfluous VHDL processor warnings 
-- altera message_level Level1 
-- altera message_off 10034 10035 10036 10037 10230 10240 10030 

library altera;
use altera.altera_europa_support_lib.all;

library altera_mf;
use altera_mf.all;

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

library lpm;
use lpm.all;

entity jtag_uart_scfifo_r is 
        port (
              -- inputs:
                 signal clk : IN STD_LOGIC;
                 signal fifo_clear : IN STD_LOGIC;
                 signal fifo_rd : IN STD_LOGIC;
                 signal rst_n : IN STD_LOGIC;
                 signal t_dat : IN STD_LOGIC_VECTOR (7 DOWNTO 0);
                 signal wr_rfifo : IN STD_LOGIC;

              -- outputs:
                 signal fifo_EF : OUT STD_LOGIC;
                 signal fifo_rdata : OUT STD_LOGIC_VECTOR (7 DOWNTO 0);
                 signal rfifo_full : OUT STD_LOGIC;
                 signal rfifo_used : OUT STD_LOGIC_VECTOR (9 DOWNTO 0)
              );
end entity jtag_uart_scfifo_r;


architecture europa of jtag_uart_scfifo_r is
--synthesis translate_off
component jtag_uart_sim_scfifo_r is 
           port (
                 -- inputs:
                    signal clk : IN STD_LOGIC;
                    signal fifo_rd : IN STD_LOGIC;
                    signal rst_n : IN STD_LOGIC;

                 -- outputs:
                    signal fifo_EF : OUT STD_LOGIC;
                    signal fifo_rdata : OUT STD_LOGIC_VECTOR (7 DOWNTO 0);
                    signal rfifo_full : OUT STD_LOGIC;
                    signal rfifo_used : OUT STD_LOGIC_VECTOR (9 DOWNTO 0)
                 );
end component jtag_uart_sim_scfifo_r;

--synthesis translate_on
--synthesis read_comments_as_HDL on
--  component scfifo is
--GENERIC (
--      lpm_hint : STRING;
--        lpm_numwords : NATURAL;
--        lpm_showahead : STRING;
--        lpm_type : STRING;
--        lpm_width : NATURAL;
--        lpm_widthu : NATURAL;
--        overflow_checking : STRING;
--        underflow_checking : STRING;
--        use_eab : STRING
--      );
--    PORT (
--    signal full : OUT STD_LOGIC;
--        signal q : OUT STD_LOGIC_VECTOR (7 DOWNTO 0);
--        signal usedw : OUT STD_LOGIC_VECTOR (9 DOWNTO 0);
--        signal empty : OUT STD_LOGIC;
--        signal rdreq : IN STD_LOGIC;
--        signal aclr : IN STD_LOGIC;
--        signal data : IN STD_LOGIC_VECTOR (7 DOWNTO 0);
--        signal clock : IN STD_LOGIC;
--        signal wrreq : IN STD_LOGIC
--      );
--  end component scfifo;
--synthesis read_comments_as_HDL off
                signal internal_fifo_EF :  STD_LOGIC;
                signal internal_fifo_rdata :  STD_LOGIC_VECTOR (7 DOWNTO 0);
                signal internal_rfifo_full :  STD_LOGIC;
                signal internal_rfifo_used :  STD_LOGIC_VECTOR (9 DOWNTO 0);

begin

  --vhdl renameroo for output signals
  fifo_EF <= internal_fifo_EF;
  --vhdl renameroo for output signals
  fifo_rdata <= internal_fifo_rdata;
  --vhdl renameroo for output signals
  rfifo_full <= internal_rfifo_full;
  --vhdl renameroo for output signals
  rfifo_used <= internal_rfifo_used;
--synthesis translate_off
    --the_jtag_uart_sim_scfifo_r, which is an e_instance
    the_jtag_uart_sim_scfifo_r : jtag_uart_sim_scfifo_r
      port map(
        fifo_EF => internal_fifo_EF,
        fifo_rdata => internal_fifo_rdata,
        rfifo_full => internal_rfifo_full,
        rfifo_used => internal_rfifo_used,
        clk => clk,
        fifo_rd => fifo_rd,
        rst_n => rst_n
      );


--synthesis translate_on
--synthesis read_comments_as_HDL on
--    rfifo : scfifo
--      generic map(
--        lpm_hint => "RAM_BLOCK_TYPE=AUTO",
--        lpm_numwords => 1024,
--        lpm_showahead => "OFF",
--        lpm_type => "scfifo",
--        lpm_width => 8,
--        lpm_widthu => 10,
--        overflow_checking => "OFF",
--        underflow_checking => "OFF",
--        use_eab => "ON"
--      )
--      port map(
--                aclr => fifo_clear,
--                clock => clk,
--                data => t_dat,
--                empty => internal_fifo_EF,
--                full => internal_rfifo_full,
--                q => internal_fifo_rdata,
--                rdreq => fifo_rd,
--                usedw => internal_rfifo_used,
--                wrreq => wr_rfifo
--      );
--
--synthesis read_comments_as_HDL off

end europa;



-- turn off superfluous VHDL processor warnings 
-- altera message_level Level1 
-- altera message_off 10034 10035 10036 10037 10230 10240 10030 

library altera;
use altera.altera_europa_support_lib.all;

library altera_mf;
use altera_mf.all;

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

library lpm;
use lpm.all;

entity jtag_uart is 
        port (
              -- inputs:
                 signal av_address : IN STD_LOGIC;
                 signal av_chipselect : IN STD_LOGIC;
                 signal av_read_n : IN STD_LOGIC;
                 signal av_write_n : IN STD_LOGIC;
                 signal av_writedata : IN STD_LOGIC_VECTOR (31 DOWNTO 0);
                 signal clk : IN STD_LOGIC;
                 signal rst_n : IN STD_LOGIC;

              -- outputs:
                 signal av_irq : OUT STD_LOGIC;
                 signal av_readdata : OUT STD_LOGIC_VECTOR (31 DOWNTO 0);
                 signal av_waitrequest : OUT STD_LOGIC;
                 signal dataavailable : OUT STD_LOGIC;
                 signal readyfordata : OUT STD_LOGIC
              );
attribute ALTERA_ATTRIBUTE : string;
attribute ALTERA_ATTRIBUTE of jtag_uart : entity is "SUPPRESS_DA_RULE_INTERNAL=""R101,C106,D101,D103""";
end entity jtag_uart;


architecture europa of jtag_uart is
component jtag_uart_scfifo_w is 
           port (
                 -- inputs:
                    signal clk : IN STD_LOGIC;
                    signal fifo_clear : IN STD_LOGIC;
                    signal fifo_wdata : IN STD_LOGIC_VECTOR (7 DOWNTO 0);
                    signal fifo_wr : IN STD_LOGIC;
                    signal rd_wfifo : IN STD_LOGIC;

                 -- outputs:
                    signal fifo_FF : OUT STD_LOGIC;
                    signal r_dat : OUT STD_LOGIC_VECTOR (7 DOWNTO 0);
                    signal wfifo_empty : OUT STD_LOGIC;
                    signal wfifo_used : OUT STD_LOGIC_VECTOR (9 DOWNTO 0)
                 );
end component jtag_uart_scfifo_w;

component jtag_uart_scfifo_r is 
           port (
                 -- inputs:
                    signal clk : IN STD_LOGIC;
                    signal fifo_clear : IN STD_LOGIC;
                    signal fifo_rd : IN STD_LOGIC;
                    signal rst_n : IN STD_LOGIC;
                    signal t_dat : IN STD_LOGIC_VECTOR (7 DOWNTO 0);
                    signal wr_rfifo : IN STD_LOGIC;

                 -- outputs:
                    signal fifo_EF : OUT STD_LOGIC;
                    signal fifo_rdata : OUT STD_LOGIC_VECTOR (7 DOWNTO 0);
                    signal rfifo_full : OUT STD_LOGIC;
                    signal rfifo_used : OUT STD_LOGIC_VECTOR (9 DOWNTO 0)
                 );
end component jtag_uart_scfifo_r;

--synthesis read_comments_as_HDL on
--  component alt_jtag_atlantic is
--GENERIC (
--      INSTANCE_ID : NATURAL;
--        LOG2_RXFIFO_DEPTH : NATURAL;
--        LOG2_TXFIFO_DEPTH : NATURAL;
--        SLD_AUTO_INSTANCE_INDEX : STRING
--      );
--    PORT (
--    signal t_pause : OUT STD_LOGIC;
--        signal r_ena : OUT STD_LOGIC;
--        signal t_ena : OUT STD_LOGIC;
--        signal t_dat : OUT STD_LOGIC_VECTOR (7 DOWNTO 0);
--        signal t_dav : IN STD_LOGIC;
--        signal rst_n : IN STD_LOGIC;
--        signal r_dat : IN STD_LOGIC_VECTOR (7 DOWNTO 0);
--        signal r_val : IN STD_LOGIC;
--        signal clk : IN STD_LOGIC
--      );
--  end component alt_jtag_atlantic;
--synthesis read_comments_as_HDL off
                signal ac :  STD_LOGIC;
                signal activity :  STD_LOGIC;
                signal fifo_AE :  STD_LOGIC;
                signal fifo_AF :  STD_LOGIC;
                signal fifo_EF :  STD_LOGIC;
                signal fifo_FF :  STD_LOGIC;
                signal fifo_clear :  STD_LOGIC;
                signal fifo_rd :  STD_LOGIC;
                signal fifo_rdata :  STD_LOGIC_VECTOR (7 DOWNTO 0);
                signal fifo_wdata :  STD_LOGIC_VECTOR (7 DOWNTO 0);
                signal fifo_wr :  STD_LOGIC;
                signal ien_AE :  STD_LOGIC;
                signal ien_AF :  STD_LOGIC;
                signal internal_av_waitrequest :  STD_LOGIC;
                signal ipen_AE :  STD_LOGIC;
                signal ipen_AF :  STD_LOGIC;
                signal pause_irq :  STD_LOGIC;
                signal r_dat :  STD_LOGIC_VECTOR (7 DOWNTO 0);
                signal r_ena :  STD_LOGIC;
                signal r_val :  STD_LOGIC;
                signal rd_wfifo :  STD_LOGIC;
                signal read_0 :  STD_LOGIC;
                signal rfifo_full :  STD_LOGIC;
                signal rfifo_used :  STD_LOGIC_VECTOR (9 DOWNTO 0);
                signal rvalid :  STD_LOGIC;
                signal sim_r_ena :  STD_LOGIC;
                signal sim_t_dat :  STD_LOGIC;
                signal sim_t_ena :  STD_LOGIC;
                signal sim_t_pause :  STD_LOGIC;
                signal t_dat :  STD_LOGIC_VECTOR (7 DOWNTO 0);
                signal t_dav :  STD_LOGIC;
                signal t_ena :  STD_LOGIC;
                signal t_pause :  STD_LOGIC;
                signal wfifo_empty :  STD_LOGIC;
                signal wfifo_used :  STD_LOGIC_VECTOR (9 DOWNTO 0);
                signal woverflow :  STD_LOGIC;
                signal wr_rfifo :  STD_LOGIC;

begin

  --avalon_jtag_slave, which is an e_avalon_slave
  rd_wfifo <= r_ena AND NOT wfifo_empty;
  wr_rfifo <= t_ena AND NOT rfifo_full;
  fifo_clear <= NOT rst_n;
  --the_jtag_uart_scfifo_w, which is an e_instance
  the_jtag_uart_scfifo_w : jtag_uart_scfifo_w
    port map(
      fifo_FF => fifo_FF,
      r_dat => r_dat,
      wfifo_empty => wfifo_empty,
      wfifo_used => wfifo_used,
      clk => clk,
      fifo_clear => fifo_clear,
      fifo_wdata => fifo_wdata,
      fifo_wr => fifo_wr,
      rd_wfifo => rd_wfifo
    );


  --the_jtag_uart_scfifo_r, which is an e_instance
  the_jtag_uart_scfifo_r : jtag_uart_scfifo_r
    port map(
      fifo_EF => fifo_EF,
      fifo_rdata => fifo_rdata,
      rfifo_full => rfifo_full,
      rfifo_used => rfifo_used,
      clk => clk,
      fifo_clear => fifo_clear,
      fifo_rd => fifo_rd,
      rst_n => rst_n,
      t_dat => t_dat,
      wr_rfifo => wr_rfifo
    );


  ipen_AE <= ien_AE AND fifo_AE;
  ipen_AF <= ien_AF AND ((pause_irq OR fifo_AF));
  av_irq <= ipen_AE OR ipen_AF;
  activity <= t_pause OR t_ena;
  process (clk, rst_n)
  begin
    if rst_n = '0' then
      pause_irq <= std_logic'('0');
    elsif clk'event and clk = '1' then
      -- only if fifo is not empty...
      if std_logic'((t_pause AND NOT fifo_EF)) = '1' then 
        pause_irq <= std_logic'('1');
      elsif std_logic'(read_0) = '1' then 
        pause_irq <= std_logic'('0');
      end if;
    end if;

  end process;

  process (clk, rst_n)
  begin
    if rst_n = '0' then
      r_val <= std_logic'('0');
      t_dav <= std_logic'('1');
    elsif clk'event and clk = '1' then
      r_val <= r_ena AND NOT wfifo_empty;
      t_dav <= NOT rfifo_full;
    end if;

  end process;

  process (clk, rst_n)
  begin
    if rst_n = '0' then
      fifo_AE <= std_logic'('0');
      fifo_AF <= std_logic'('0');
      fifo_wr <= std_logic'('0');
      rvalid <= std_logic'('0');
      read_0 <= std_logic'('0');
      ien_AE <= std_logic'('0');
      ien_AF <= std_logic'('0');
      ac <= std_logic'('0');
      woverflow <= std_logic'('0');
      internal_av_waitrequest <= std_logic'('1');
    elsif clk'event and clk = '1' then
      fifo_AE <= to_std_logic(((std_logic_vector'("000000000000000000000") & (Std_Logic_Vector'(A_ToStdLogicVector(fifo_FF) & wfifo_used)))<=std_logic_vector'("00000000000000000000000000001000")));
      fifo_AF <= to_std_logic(((std_logic_vector'("00000000000000000000") & (((std_logic_vector'("010000000000") - (std_logic_vector'("0") & (Std_Logic_Vector'(A_ToStdLogicVector(rfifo_full) & rfifo_used)))))))<=std_logic_vector'("00000000000000000000000000001000")));
      fifo_wr <= std_logic'('0');
      read_0 <= std_logic'('0');
      internal_av_waitrequest <= NOT (((av_chipselect AND ((NOT av_write_n OR NOT av_read_n))) AND internal_av_waitrequest));
      if std_logic'(activity) = '1' then 
        ac <= std_logic'('1');
      end if;
      -- write
      if std_logic'(((av_chipselect AND NOT av_write_n) AND internal_av_waitrequest)) = '1' then 
        -- addr 1 is control; addr 0 is data
        if std_logic'(av_address) = '1' then 
          ien_AF <= av_writedata(0);
          ien_AE <= av_writedata(1);
          if std_logic'((av_writedata(10) AND NOT activity)) = '1' then 
            ac <= std_logic'('0');
          end if;
        else
          fifo_wr <= NOT fifo_FF;
          woverflow <= fifo_FF;
        end if;
      end if;
      -- read
      if std_logic'(((av_chipselect AND NOT av_read_n) AND internal_av_waitrequest)) = '1' then 
        -- addr 1 is interrupt; addr 0 is data
        if std_logic'(NOT av_address) = '1' then 
          rvalid <= NOT fifo_EF;
        end if;
        read_0 <= NOT av_address;
      end if;
    end if;

  end process;

  fifo_wdata <= av_writedata(7 DOWNTO 0);
  fifo_rd <= A_WE_StdLogic((std_logic'(((((av_chipselect AND NOT av_read_n) AND internal_av_waitrequest) AND NOT av_address))) = '1'), NOT fifo_EF, std_logic'('0'));
  av_readdata <= A_EXT (A_WE_StdLogicVector((std_logic'(read_0) = '1'), (std_logic_vector'("0") & ((A_REP(std_logic'('0'), 5) & A_ToStdLogicVector(rfifo_full) & rfifo_used & A_ToStdLogicVector(rvalid) & A_ToStdLogicVector(woverflow) & A_ToStdLogicVector(NOT fifo_FF) & A_ToStdLogicVector(NOT fifo_EF) & A_ToStdLogicVector(std_logic'('0')) & A_ToStdLogicVector(ac) & A_ToStdLogicVector(ipen_AE) & A_ToStdLogicVector(ipen_AF) & fifo_rdata))), (A_REP(std_logic'('0'), 5) & ((std_logic_vector'("010000000000") - (std_logic_vector'("0") & (Std_Logic_Vector'(A_ToStdLogicVector(fifo_FF) & wfifo_used))))) & A_ToStdLogicVector(rvalid) & A_ToStdLogicVector(woverflow) & A_ToStdLogicVector(NOT fifo_FF) & A_ToStdLogicVector(NOT fifo_EF) & A_ToStdLogicVector(std_logic'('0')) & A_ToStdLogicVector(ac) & A_ToStdLogicVector(ipen_AE) & A_ToStdLogicVector(ipen_AF) & A_REP(std_logic'('0'), 6) & A_ToStdLogicVector(ien_AE) & A_ToStdLogicVector(ien_AF))), 32);
  process (clk, rst_n)
  begin
    if rst_n = '0' then
      readyfordata <= std_logic'('0');
    elsif clk'event and clk = '1' then
      if (std_logic_vector'("00000000000000000000000000000001")) /= std_logic_vector'("00000000000000000000000000000000") then 
        readyfordata <= NOT fifo_FF;
      end if;
    end if;

  end process;

  --vhdl renameroo for output signals
  av_waitrequest <= internal_av_waitrequest;
--synthesis translate_off
    -- Tie off Atlantic Interface signals not used for simulation
    process (clk)
    begin
      if clk'event and clk = '1' then
        sim_t_pause <= std_logic'('0');
        sim_t_ena <= std_logic'('0');
        sim_t_dat <= Vector_To_Std_Logic(A_WE_StdLogicVector((std_logic'(t_dav) = '1'), r_dat, A_REP(r_val, 8)));
        sim_r_ena <= std_logic'('0');
      end if;

    end process;

    r_ena <= sim_r_ena;
    t_ena <= sim_t_ena;
    t_dat <= std_logic_vector'("0000000") & (A_TOSTDLOGICVECTOR(sim_t_dat));
    t_pause <= sim_t_pause;
    process (fifo_EF)
    begin
        if (std_logic_vector'("00000000000000000000000000000001")) /= std_logic_vector'("00000000000000000000000000000000") then 
          dataavailable <= NOT fifo_EF;
        end if;

    end process;

--synthesis translate_on
--synthesis read_comments_as_HDL on
--    jtag_uart_alt_jtag_atlantic : alt_jtag_atlantic
--      generic map(
--        INSTANCE_ID => 0,
--        LOG2_RXFIFO_DEPTH => 10,
--        LOG2_TXFIFO_DEPTH => 10,
--        SLD_AUTO_INSTANCE_INDEX => "YES"
--      )
--      port map(
--                clk => clk,
--                r_dat => r_dat,
--                r_ena => r_ena,
--                r_val => r_val,
--                rst_n => rst_n,
--                t_dat => t_dat,
--                t_dav => t_dav,
--                t_ena => t_ena,
--                t_pause => t_pause
--      );
--
--    process (clk, rst_n)
--    begin
--      if rst_n = '0' then
--        dataavailable <= std_logic'('0');
--      elsif clk'event and clk = '1' then
--        if (std_logic_vector'("00000000000000000000000000000001")) /= std_logic_vector'("00000000000000000000000000000000") then 
--          dataavailable <= NOT fifo_EF;
--        end if;
--      end if;
--
--    end process;
--
--synthesis read_comments_as_HDL off

end europa;

