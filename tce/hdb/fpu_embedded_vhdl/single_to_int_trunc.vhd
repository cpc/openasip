-----------------------------------------------------------------------------------------------------------
--
-- 		                 SINGLE PRECISION FP NUMBER TO INTEGER CONVERSION LOGIC
--
-- Created by Claudio Brunelli, 2004
--
-----------------------------------------------------------------------------------------------------------
-- Default rounding mode (round to nearest even) is applied, as specified in IEEE 745 standard 

--Copyright (c) 2004, Tampere University of Technology.
--All rights reserved.

--Redistribution and use in source and binary forms, with or without modification,
--are permitted provided that the following conditions are met:
--*	Redistributions of source code must retain the above copyright notice,
--	this list of conditions and the following disclaimer.
--*	Redistributions in binary form must reproduce the above copyright notice,
--	this list of conditions and the following disclaimer in the documentation
--	and/or other materials provided with the distribution.
--*	Neither the name of Tampere University of Technology nor the names of its
--	contributors may be used to endorse or promote products derived from this
--	software without specific prior written permission.

--THIS HARDWARE DESCRIPTION OR SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
--CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
--LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND NONINFRINGEMENT AND
--FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
--OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
--EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
--PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
--BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
--CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
--ARISING IN ANY WAY OUT OF THE USE OF THIS HARDWARE DESCRIPTION OR SOFTWARE, EVEN
--IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_signed.all;
use work.cop_definitions.all;
use work.cop_components.all;

entity single_to_int_trunc is
    port( clk,reset,enable            : in std_logic;
          cvt_w_in                    : in std_logic_vector (word_width-1 downto 0);
          signselect_in                 : in std_logic; -- True if output should be unsigned; false if signed
	  cvt_w_out                   : out std_logic_vector (word_width-1 downto 0);
          exc_inexact_trunc           : out std_logic;
	  exc_overflow_trunc          : out std_logic;
          exc_invalid_operation_trunc : out std_logic );
end single_to_int_trunc ;


architecture rtl of single_to_int_trunc is


    signal pipelined_cvt_w_in                     : std_logic_vector(word_width-1 downto 0);
    signal pipelined_signselect_in                : std_logic;
    signal s, pipelined_s                         : std_logic;
    signal x, pipelined_x                         : std_logic_vector(7 downto 0);
    signal f                                      : std_logic_vector(22 downto 0);
    signal int,
           pipelined_int,
           normal                                 : std_logic_vector(word_width-1 downto 0);
    signal sgl_inexact_trunc,
           sgl_overflow_trunc                     : std_logic;
    signal pipelined_sgl_inexact_trunc, 
           pipelined_sgl_overflow_trunc           : std_logic;

    begin

-----------------------------------------------------------------------------------------------------------------

-----------------
-- INPUT STAGE
-----------------

        PIPELINE_REG_CVT_W_IN: we_register generic map (reg_width => word_width)
                                           port map (clk => clk, reset => reset, we => enable, data_in => cvt_w_in, data_out => pipelined_cvt_w_in);


-- splitting the FP number into its fields

        s <= cvt_w_in(31);
	x <= cvt_w_in(30 downto 23);
	f <= cvt_w_in(22 downto 0);

        PIPELINE_FF_S: wedff port map (clk => clk, reset => reset, we => enable, d => s, q => pipelined_s);

        PIPELINE_FF_SIGNSELECT: wedff port map (clk => clk, reset => reset, we => enable, d => signselect_in, q => pipelined_signselect_in);
        PIPELINE_REG_X: we_register generic map (reg_width => 8)
                                    port map (clk => clk, reset => reset, we => enable, data_in => x, data_out => pipelined_x);

        ----------------------------------------------------------

        PIPELINE_FF_INEXACT:  wedff port map (clk => clk, reset => reset, we => enable, d => sgl_inexact_trunc, q => pipelined_sgl_inexact_trunc);
        PIPELINE_FF_OVERFLOW: wedff port map (clk => clk, reset => reset, we => enable, d => sgl_overflow_trunc, q => pipelined_sgl_overflow_trunc);

        INVALID_OP_DETECTION: process(pipelined_x, pipelined_cvt_w_in, normal, pipelined_sgl_inexact_trunc, pipelined_sgl_overflow_trunc)
            begin
                if ( pipelined_x = "11111111" ) then
                        -- operand is either an infinity or a NaN: the operand is put outside as
                        -- a result (my choice) and invalid operation exception is raised
                        cvt_w_out <= pipelined_cvt_w_in;
                        exc_inexact_trunc   <= '0';
		        exc_overflow_trunc  <= '0';
                        exc_invalid_operation_trunc <= '1';
                else
                        cvt_w_out <= normal;
                        exc_inexact_trunc   <= pipelined_sgl_inexact_trunc;
  		        exc_overflow_trunc  <= pipelined_sgl_overflow_trunc;
                        exc_invalid_operation_trunc <= '0';
                end if;
        end process;


------------------------------------------------------------------

----------------------------------         
-- ROUND TO NEAREST INTEGER STAGE
----------------------------------

-- This logic builds up a integer starting from the x and f fields of the
-- FP number, which is "unsigned" this way, and is assumed to be positive.  
-- True sign determination is made further in the cose

        INTEGER_DETERMINATION: process(s, x, f)
        variable a : std_logic_vector(7 downto 0);
        variable b : std_logic_vector(7 downto 0);
        variable c : integer;
        
          
	begin

	    a:=Conv_std_logic_vector(31+127,8);    
            b:=Conv_std_logic_vector(-1+127,8);

       if ( signselect_in='1' and (unsigned(x)>unsigned(a)) ) then
          int <= "11111111111111111111111111111111"; -- unsigned MAXINT
                    sgl_inexact_trunc   <= '1';                 --  overflow (positive), inexact result
               sgl_overflow_trunc  <= '1';           
       elsif ( signselect_in='1' and x=a ) then
          int <= '1' & f(22 downto 0) & "00000000";
          sgl_inexact_trunc <= '0';
          sgl_overflow_trunc <= '0';

            
	    elsif ( signselect_in='0' and (unsigned(x) >= unsigned(a)) and s='0') then
                    -- FP number is too big to be faithfully represented in a 32-bit integer format (overflow).
                    -- Maximum representable integer value is anyway delivered as a result:
		    int <= "01111111111111111111111111111111";  --  approx to  MAXINT 
                    sgl_inexact_trunc   <= '1';                 --  overflow (positive), inexact result
	            sgl_overflow_trunc  <= '1';           

            elsif ( signselect_in='0' and (unsigned(x) >= unsigned(a)) and s='1') then
                    if ( (x = Conv_std_logic_vector(31+127,8)) and (f = "00000000000000000000000") ) then
                            -- FP number is exactly the minimum representable 32-bit integer (negative) value:
		            int <= "10000000000000000000000000000000";  --  MININT =  -(MAXINT + 1)
                            sgl_inexact_trunc   <= '0';
		            sgl_overflow_trunc  <= '0';
                    else
                            -- FP number is negative and its ABS value is too big to be represented in a 32-bit integer format (negative overflow).  
                            -- Maximum negative representable integer value is anyway delivered as a result:
                            int <= "10000000000000000000000000000000";  --  approx to  MININT =  -(MAXINT + 1)
                            sgl_inexact_trunc   <= '1';                 --  overflow (negative), inexact result
                            sgl_overflow_trunc  <= '1';
                    end if;

            elsif x = Conv_std_logic_vector(30+127,8) then
                    int <= EXT( ('1' & f(22 downto 0) & "0000000"),32);
                    sgl_inexact_trunc  <= '0';
		    sgl_overflow_trunc <= '0'; 
 				 
            elsif x = Conv_std_logic_vector(29+127,8) then
                    int <= EXT( ('1' & f(22 downto 0) & "000000"),32);
                    sgl_inexact_trunc  <= '0';
		    sgl_overflow_trunc <= '0'; 
               
            elsif x = Conv_std_logic_vector(28+127,8) then
                    int <= EXT( ('1' & f(22 downto 0) & "00000"),32);
                    sgl_inexact_trunc  <= '0';
		    sgl_overflow_trunc <= '0';              
             
            elsif x = Conv_std_logic_vector(27+127,8) then
                    int <= EXT( ('1' & f(22 downto 0) & "0000"),32);        
                    sgl_inexact_trunc  <= '0';
		    sgl_overflow_trunc <= '0'; 

            elsif x = Conv_std_logic_vector(26+127,8) then
                    int <= EXT( ('1' & f(22 downto 0) & "000"),32);
                    sgl_inexact_trunc  <= '0';
                    sgl_overflow_trunc <= '0'; 
                             
            elsif x = Conv_std_logic_vector(25+127,8) then
                    int <= EXT( ('1' & f(22 downto 0) & "00"),32);
                    sgl_inexact_trunc  <= '0';
	            sgl_overflow_trunc <= '0';  

            elsif x = Conv_std_logic_vector(24+127,8) then
                    int <= EXT( ('1' & f(22 downto 0) & '0'),32);
                    sgl_inexact_trunc  <= '0';
                    sgl_overflow_trunc <= '0'; 

            elsif x = Conv_std_logic_vector(23+127,8) then
                    int <= EXT( ('1' & f(22 downto 0)),32);
                    sgl_inexact_trunc  <= '0';
	            sgl_overflow_trunc <= '0'; 

            elsif x = Conv_std_logic_vector(22+127,8) then
                    c := Conv_integer(  unsigned( '1' & f(22 downto 1))  );
                    if f(0) = '0' then
                            -- only one exact case!
                            int <= Conv_std_logic_vector(c,32);
                            sgl_inexact_trunc  <= '0';
                    --------------------------------------------------------------------------
                    -- ROUND TO NEAREST
                    elsif f(1) = '0' then
                            int <= Conv_std_logic_vector(c,32);
                            sgl_inexact_trunc  <= '1';
                    else 
                            int <= Conv_std_logic_vector(c+1,32);
                            sgl_inexact_trunc  <= '1';
                    end if;
		    sgl_overflow_trunc <= '0';            
                               
            elsif x = Conv_std_logic_vector(21+127,8) then
                    c := Conv_integer(  unsigned( '1' & f(22 downto 2))  );
                    if ( f(1 downto 0) = Conv_std_logic_vector(0,2)) then
                            -- only one exact case!
                            int <= Conv_std_logic_vector(c,32);
                            sgl_inexact_trunc  <= '0';
                     --------------------------------------------------------------------------
                     -- ROUND TO NEAREST
		         elsif ( (f(1) = '0') or ( f(2) = '0' and f(1) = '1' and f(0) = '0' ) ) then
		                int <= Conv_std_logic_vector(c,32);
                            sgl_inexact_trunc  <= '1';
                     else 									                   
                            int <= Conv_std_logic_vector(c+1,32);   
                            sgl_inexact_trunc  <= '1';
                     end if;
                     sgl_overflow_trunc <= '0';                          
				 
            elsif x = Conv_std_logic_vector(20+127,8) then
                     c:= Conv_integer(  unsigned( '1' & f(22 downto 3)) );
                     if ( f(2 downto 0) = Conv_std_logic_vector(0,3) ) then
                            -- only one exact case!
                            int <= Conv_std_logic_vector(c,32);
                            sgl_inexact_trunc  <= '0';
                     --------------------------------------------------------------------------
                     -- ROUND TO NEAREST
		         elsif ( (f(2) = '0') or ( f(3) = '0' and f(2) = '1' and f(1 downto 0) = Conv_std_logic_vector(0,2) ) ) then
		            int <= Conv_std_logic_vector(c,32);
                            sgl_inexact_trunc  <= '1';
                     else 									                   
                            int <= Conv_std_logic_vector(c+1,32);   
                            sgl_inexact_trunc  <= '1';
                     end if;
                     sgl_overflow_trunc <= '0';                            

            elsif x = Conv_std_logic_vector(19+127,8) then
                     c:= Conv_integer(  unsigned( '1' & f(22 downto 4)) );
                     if ( f(3 downto 0) = Conv_std_logic_vector(0,4) ) then
                            -- only one exact case!
                            int <= Conv_std_logic_vector(c,32);
                            sgl_inexact_trunc  <= '0';
                     --------------------------------------------------------------------------
                     -- ROUND TO NEAREST
		         elsif ( (f(3) = '0') or ( f(4) = '0' and f(3) = '1' and f(2 downto 0) = Conv_std_logic_vector(0,3) ) ) then
		            int <= Conv_std_logic_vector(c,32);
                            sgl_inexact_trunc  <= '1';
                     else 									                   
                            int <= Conv_std_logic_vector(c+1,32);   
                            sgl_inexact_trunc  <= '1';
                     end if;
                     sgl_overflow_trunc <= '0';                          
               
            elsif x = Conv_std_logic_vector(18+127,8) then
                     c:= Conv_integer( unsigned( '1' & f(22 downto 5)) );
                     if ( f(4 downto 0) = Conv_std_logic_vector(0,5) ) then
                            -- only one exact case!
                            int <= Conv_std_logic_vector(c,32);
                            sgl_inexact_trunc  <= '0';
                     --------------------------------------------------------------------------
                     -- ROUND TO NEAREST
		         elsif ( (f(4) = '0') or ( f(5) = '0' and f(4) = '1' and f(3 downto 0) = Conv_std_logic_vector(0,4) ) ) then
		            int <= Conv_std_logic_vector(c,32);
                            sgl_inexact_trunc  <= '1';
                     else 									                   
                            int <= Conv_std_logic_vector(c+1,32);   
                            sgl_inexact_trunc  <= '1';
                     end if;
                     sgl_overflow_trunc <= '0';                            

            elsif x = Conv_std_logic_vector(17+127,8) then
                     c:= Conv_integer(  unsigned( '1' & f(22 downto 6)) );
                     if ( f(5 downto 0) = Conv_std_logic_vector(0,6) ) then
                             -- only one exact case!
                             int <= Conv_std_logic_vector(c,32);
                             sgl_inexact_trunc  <= '0';
                     --------------------------------------------------------------------------
                     -- ROUND TO NEAREST
		         elsif ( (f(5) = '0') or ( f(6) = '0' and f(5) = '1' and f(4 downto 0) = Conv_std_logic_vector(0,5) ) ) then
		             int <= Conv_std_logic_vector(c,32);
                             sgl_inexact_trunc  <= '1';
                     else 									                   
                             int <= Conv_std_logic_vector(c+1,32);   
                             sgl_inexact_trunc  <= '1';
                     end if;
                     sgl_overflow_trunc <= '0';                          

            elsif x = Conv_std_logic_vector(16+127,8) then
                     c:= Conv_integer(  unsigned( '1' & f(22 downto 7)) );
                     if ( f(6 downto 0) = Conv_std_logic_vector(0,7) ) then
                             -- only one exact case!
                             int <= Conv_std_logic_vector(c,32);
                             sgl_inexact_trunc  <= '0';
                     --------------------------------------------------------------------------
                     -- ROUND TO NEAREST
 		         elsif ( (f(6) = '0') or ( f(7) = '0' and f(6) = '1' and f(5 downto 0) = Conv_std_logic_vector(0,6) ) ) then
		             int <= Conv_std_logic_vector(c,32);
                             sgl_inexact_trunc  <= '1';
                     else 									                   
                             int <= Conv_std_logic_vector(c+1,32);   
                             sgl_inexact_trunc  <= '1';
                     end if;
                     sgl_overflow_trunc <= '0';               
                             
            elsif x = Conv_std_logic_vector(15+127,8) then
                     c:= Conv_integer(  unsigned( '1' & f(22 downto 8)) );
                     if ( f(7 downto 0) = Conv_std_logic_vector(0,8) ) then
                             -- only one exact case!
                             int <= Conv_std_logic_vector(c,32);
                             sgl_inexact_trunc  <= '0';
                     --------------------------------------------------------------------------
                     -- ROUND TO NEAREST
		         elsif ( (f(7) = '0') or ( f(8) = '0' and f(7) = '1' and f(6 downto 0) = Conv_std_logic_vector(0,7) ) ) then
		             int <= Conv_std_logic_vector(c,32);
                             sgl_inexact_trunc  <= '1';
                     else 									                   
                             int <= Conv_std_logic_vector(c+1,32);   
                             sgl_inexact_trunc  <= '1';
                     end if;
                     sgl_overflow_trunc <= '0';             

            elsif x = Conv_std_logic_vector(14+127,8) then
                     c:= Conv_integer( unsigned(  '1' & f(22 downto 9)) );
                     if ( f(8 downto 0) = Conv_std_logic_vector(0,9) ) then
                             -- only one exact case!
                             int <= Conv_std_logic_vector(c,32);
                             sgl_inexact_trunc  <= '0';
                     --------------------------------------------------------------------------
                     -- ROUND TO NEAREST
 	               elsif ( (f(8) = '0') or ( f(9) = '0' and f(8) = '1' and f(7 downto 0) = Conv_std_logic_vector(0,8) ) ) then
		             int <= Conv_std_logic_vector(c,32);
                             sgl_inexact_trunc  <= '1';
                     else 									                   
                             int <= Conv_std_logic_vector(c+1,32);   
                             sgl_inexact_trunc  <= '1';
                     end if;
                     sgl_overflow_trunc <= '0';                      

            elsif x = Conv_std_logic_vector(13+127,8) then
                     c:= Conv_integer(  unsigned( '1' & f(22 downto 10)) );
                     if ( f(9 downto 0) = Conv_std_logic_vector(0,10) ) then
                             -- only one exact case!
                             int <= Conv_std_logic_vector(c,32);
                             sgl_inexact_trunc  <= '0';
                     --------------------------------------------------------------------------
                     -- ROUND TO NEAREST
		         elsif ( (f(9) = '0') or ( f(10) = '0' and f(9) = '1' and f(8 downto 0) = Conv_std_logic_vector(0,9) ) ) then
		             int <= Conv_std_logic_vector(c,32);
                             sgl_inexact_trunc  <= '1';
                     else 									                   
                             int <= Conv_std_logic_vector(c+1,32);   
                             sgl_inexact_trunc  <= '1';
                     end if;
                     sgl_overflow_trunc <= '0';

            elsif x = Conv_std_logic_vector(12+127,8) then
                     c:= Conv_integer(  unsigned( '1' & f(22 downto 11)) );
                     if ( f(10 downto 0) = Conv_std_logic_vector(0,11) ) then
                             -- only one exact case!
                             int <= Conv_std_logic_vector(c,32);
                             sgl_inexact_trunc  <= '0';
                     --------------------------------------------------------------------------
                     -- ROUND TO NEAREST
		         elsif ( (f(10) = '0') or ( f(11) = '0' and f(10) = '1' and f(9 downto 0) = Conv_std_logic_vector(0,10) ) ) then
		             int <= Conv_std_logic_vector(c,32);
                             sgl_inexact_trunc  <= '1';
                     else 									                   
                             int <= Conv_std_logic_vector(c+1,32);   
                             sgl_inexact_trunc  <= '1';
                     end if;
                     sgl_overflow_trunc <= '0';                    
		              
            elsif x = Conv_std_logic_vector(11+127,8) then
                     c:= Conv_integer(  unsigned( '1' & f(22 downto 12)) );
                     if ( f(11 downto 0) = Conv_std_logic_vector(0,12) ) then
                             -- oly one exact case!
                             int <= Conv_std_logic_vector(c,32);
                             sgl_inexact_trunc  <= '0';
                     --------------------------------------------------------------------------
                     -- ROUND TO NEAREST
		         elsif ( (f(11) = '0') or ( f(12) = '0' and f(11) = '1' and f(10 downto 0) = Conv_std_logic_vector(0,11) ) ) then
		             int <= Conv_std_logic_vector(c,32);
                             sgl_inexact_trunc  <= '1';
                     else 									                   
                             int <= Conv_std_logic_vector(c+1,32);   
                             sgl_inexact_trunc  <= '1';
                     end if;
                     sgl_overflow_trunc <= '0'; 
            
            elsif x = Conv_std_logic_vector(10+127,8) then
                     c:= Conv_integer(  unsigned( '1' & f(22 downto 13)) );
                     if ( f(12 downto 0) = Conv_std_logic_vector(0,13) ) then
                             -- only one exact case!
                             int <= Conv_std_logic_vector(c,32);
                             sgl_inexact_trunc  <= '0';
                     --------------------------------------------------------------------------
                     -- ROUND TO NEAREST
		         elsif ( (f(12) = '0') or ( f(13) = '0' and f(12) = '1' and f(11 downto 0) = Conv_std_logic_vector(0,12) ) ) then
		             int <= Conv_std_logic_vector(c,32);
                             sgl_inexact_trunc  <= '1';
                     else 									                   
                             int <= Conv_std_logic_vector(c+1,32);   
                             sgl_inexact_trunc  <= '1';
                     end if;
                     sgl_overflow_trunc <= '0';

            elsif x = Conv_std_logic_vector(9+127,8) then
                     c:= Conv_integer( unsigned( '1' & f(22 downto 14)) );
                     if ( f(13 downto 0) = Conv_std_logic_vector(0,14) ) then
                             -- only one exact case!
                             int <= Conv_std_logic_vector(c,32);
                             sgl_inexact_trunc  <= '0';
                     --------------------------------------------------------------------------
                     -- ROUND TO NEAREST
		         elsif ( (f(13) = '0') or ( f(14) = '0' and f(13) = '1' and f(12 downto 0) = Conv_std_logic_vector(0,13) ) ) then
		             int <= Conv_std_logic_vector(c,32);
                             sgl_inexact_trunc  <= '1';
                     else 									                   
                             int <= Conv_std_logic_vector(c+1,32);   
                             sgl_inexact_trunc  <= '1';
                     end if;
                     sgl_overflow_trunc <= '0';

            elsif x = Conv_std_logic_vector(8+127,8) then
                     c:= Conv_integer(  unsigned( '1' & f(22 downto 15)) );
                     if ( f(14 downto 0) = Conv_std_logic_vector(0,15) ) then
                             -- only one exact case!
                             int <= Conv_std_logic_vector(c,32);
                             sgl_inexact_trunc  <= '0';
                     --------------------------------------------------------------------------
                     -- ROUND TO NEAREST
		         elsif ( (f(14) = '0') or ( f(15) = '0' and f(14) = '1' and f(13 downto 0) = Conv_std_logic_vector(0,14) ) ) then
		             int <= Conv_std_logic_vector(c,32);
                             sgl_inexact_trunc  <= '1';
                     else 									                   
                             int <= Conv_std_logic_vector(c+1,32);   
                             sgl_inexact_trunc  <= '1';
                     end if;
                     sgl_overflow_trunc <= '0';

            elsif x = Conv_std_logic_vector(7+127,8) then
                     c:= Conv_integer(  unsigned( '1' & f(22 downto 16)) );
                     if ( f(15 downto 0) = Conv_std_logic_vector(0,16) ) then
                             -- only one exact case!
                             int <= Conv_std_logic_vector(c,32);
                             sgl_inexact_trunc  <= '0';
                     --------------------------------------------------------------------------
                     -- ROUND TO NEAREST
		         elsif ( (f(15) = '0') or ( f(16) = '0' and f(15) = '1' and f(14 downto 0) = Conv_std_logic_vector(0,15) ) ) then
		             int <= Conv_std_logic_vector(c,32);
                             sgl_inexact_trunc  <= '1';
                     else 									                   
                             int <= Conv_std_logic_vector(c+1,32);   
                             sgl_inexact_trunc  <= '1';
                     end if;
                     sgl_overflow_trunc <= '0';
  
           elsif x = Conv_std_logic_vector(6+127,8) then
                     c:= Conv_integer( unsigned(  '1' & f(22 downto 17)) );
                     if ( f(16 downto 0) = Conv_std_logic_vector(0,17) ) then
                             -- only one exact case!
                             int <= Conv_std_logic_vector(c,32);
                             sgl_inexact_trunc  <= '0';
                     --------------------------------------------------------------------------
                     -- ROUND TO NEAREST
		         elsif ( (f(16) = '0') or ( f(17) = '0' and f(16) = '1' and f(15 downto 0) = Conv_std_logic_vector(0,16) ) ) then 
		             int <= Conv_std_logic_vector(c,32);
                             sgl_inexact_trunc  <= '1';
                     else 									                   
                             int <= Conv_std_logic_vector(c+1,32);   
                             sgl_inexact_trunc  <= '1';
                     end if;
                     sgl_overflow_trunc <= '0';
              
           elsif x = Conv_std_logic_vector(5+127,8) then
                     c:= Conv_integer(  unsigned( '1' & f(22 downto 18)) );
                     if ( f(17 downto 0) = Conv_std_logic_vector(0,18) ) then
                             -- only one exact case!
                             int <= Conv_std_logic_vector(c,32);
                             sgl_inexact_trunc  <= '0';
                     --------------------------------------------------------------------------
                     -- ROUND TO NEAREST
		         elsif ( (f(17) = '0') or ( f(18) = '0' and f(17) = '1' and f(16 downto 0) = Conv_std_logic_vector(0,17) ) ) then
		             int <= Conv_std_logic_vector(c,32);
                             sgl_inexact_trunc  <= '1';
                     else 									                   
                             int <= Conv_std_logic_vector(c+1,32);   
                             sgl_inexact_trunc  <= '1';
                     end if;
                     sgl_overflow_trunc <= '0';

           elsif x = Conv_std_logic_vector(4+127,8) then
                     c:= Conv_integer(  unsigned( '1' & f(22 downto 19)) );
                     if ( f(18 downto 0) = Conv_std_logic_vector(0,19) ) then
                             -- only one exact case!
                             int <= Conv_std_logic_vector(c,32);
                             sgl_inexact_trunc  <= '0';
                     --------------------------------------------------------------------------
                     -- ROUND TO NEAREST
		         elsif ( (f(18) = '0') or ( f(19) = '0' and f(18) = '1' and f(17 downto 0) = Conv_std_logic_vector(0,18) ) ) then
		             int <= Conv_std_logic_vector(c,32);
                             sgl_inexact_trunc  <= '1';
                     else 									                   
                             int <= Conv_std_logic_vector(c+1,32);   
                             sgl_inexact_trunc  <= '1';
                     end if;
                     sgl_overflow_trunc <= '0';

           elsif x = Conv_std_logic_vector(3+127,8) then  -- range: [1000.0 -> 1111.111...1]
                     c:= Conv_integer(  unsigned( '1' & f(22 downto 20)) );
                     if ( f(19 downto 0) = Conv_std_logic_vector(0,20) ) then
                             -- only one exact case!
                             int <= Conv_std_logic_vector(c,32);
                             sgl_inexact_trunc  <= '0';
                     --------------------------------------------------------------------------
                     -- ROUND TO NEAREST
		         elsif ( (f(19) = '0') or ( f(20) = '0' and f(19) = '1' and f(18 downto 0) = Conv_std_logic_vector(0,19) ) ) then
		             int <= Conv_std_logic_vector(c,32);
                             sgl_inexact_trunc  <= '1';
                     else 									                   
                             int <= Conv_std_logic_vector(c+1,32);  
                             sgl_inexact_trunc  <= '1';
                     end if;
                     sgl_overflow_trunc <= '0';
                 
	     elsif x = Conv_std_logic_vector(2+127,8) then  -- range:[100.0 -> 111.111...1]
                     c:= Conv_integer( unsigned('1' & f(22 downto 21)) );
                     if ( f(20 downto 0) = Conv_std_logic_vector(0,21) ) then
                             -- only one exact case!
                             int <= Conv_std_logic_vector(c,32);
                             sgl_inexact_trunc  <= '0';
                     --------------------------------------------------------------------------
                     -- ROUND TO NEAREST
		         elsif ( (f(20) = '0') or ( f(21) = '0' and f(20) = '1' and f(19 downto 0) = Conv_std_logic_vector(0,20) ) ) then   
		             int <= Conv_std_logic_vector(c,32);
                             sgl_inexact_trunc  <= '1';
                     else 									                   
                             int <= Conv_std_logic_vector(c+1,32);  
                             sgl_inexact_trunc  <= '1';
                     end if;
                     sgl_overflow_trunc <= '0';
				 
           elsif x = Conv_std_logic_vector(1+127,8) then  -- range: [10.0 -> 11.111...1]  -- particular case
                     if f(22)='0' then
                             if ( f(21 downto 0) = Conv_std_logic_vector(0,22) ) then
                                     -- only one exact case!
                                     int <= Conv_std_logic_vector(2,32);  
                                     sgl_inexact_trunc  <= '0';
                             --------------------------------------------------------------------------
                             -- ROUND TO NEAREST
                             elsif ( (f(21) = '0') or ( f(22) = '0' and f(21) = '1' and f(20 downto 0) = Conv_std_logic_vector(0,21) ) ) then
		                     int <= Conv_std_logic_vector(2,32);
                                     sgl_inexact_trunc  <= '1';
                             else 									                    
                                     int <= Conv_std_logic_vector(3,32);  
                                     sgl_inexact_trunc  <= '1';
                             end if;
                     else
                             if ( f(21 downto 0) = Conv_std_logic_vector(0,22) ) then
                                     -- only one exact case!
                                     int <= Conv_std_logic_vector(3,32);  
                                     sgl_inexact_trunc  <= '0';
                             --------------------------------------------------------------------------
                             -- ROUND TO NEAREST
                             elsif ( (f(21) = '0') or ( f(22) = '0' and f(21) = '1' and f(20 downto 0) = Conv_std_logic_vector(0,21) ) ) then
		                     int <= Conv_std_logic_vector(3,32);
                                     sgl_inexact_trunc  <= '1';
                             else 									                    
                                     int <= Conv_std_logic_vector(4,32);  
                                     sgl_inexact_trunc  <= '1';
                             end if;
                     end if;
                     sgl_overflow_trunc <= '0';
                   
           elsif x = Conv_std_logic_vector(0+127,8) then  -- range: [1.0 -> 1.111...1]
                     if ( f(22 downto 0) = Conv_std_logic_vector(0,23)  ) then
                             -- only one exact case!   
                             int <= Conv_std_logic_vector(1,32);  
                             sgl_inexact_trunc  <= '0';
                     --------------------------------------------------------------------------
                     -- ROUND TO NEAREST
                     elsif ( f(22) = '0' ) then  
		             int <= Conv_std_logic_vector(1,32); 
                             sgl_inexact_trunc  <= '1';
                     else 									                    
                             int <= Conv_std_logic_vector(2,32); 
                             sgl_inexact_trunc  <= '1';
                     end if;
                     sgl_overflow_trunc <= '0';

           elsif x = Conv_std_logic_vector(-1+127,8) then  -- range: [0.100...0 -> 0.111...1]
                     --------------------------------------------------------------------------
                     -- ROUND TO NEAREST
                     if ( f(22 downto 0) = Conv_std_logic_vector(0,23)  ) then  
                             int <= Conv_std_logic_vector(0,32);             
                             sgl_inexact_trunc  <= '1';
                     else 									                    
                             int <= Conv_std_logic_vector(1,32); 
                             sgl_inexact_trunc  <= '1';
                     end if;
                     sgl_overflow_trunc <= '0';

           elsif ( x < b  and  x/= "00000000") then  -- range: all remaining (smaller) normalized numbers [0.000...1 -> 0.0111...1]
                     int <= Conv_std_logic_vector(0,32); 
                     sgl_inexact_trunc  <= '1';
                     sgl_overflow_trunc <= '0';

           elsif ( x= "00000000" ) and (f /= "00000000000000000000000") then  -- DENORMALIZED (they are flushed to zero)
                     int <= Conv_std_logic_vector(0,32);                      -- (my choice)
                     sgl_inexact_trunc  <= '1';
                     sgl_overflow_trunc <= '0';
			 
	     elsif (x = "00000000") and (f = "00000000000000000000000") then   -- ZERO (exact)         
		     int <= Conv_std_logic_vector(0,32); 
                     sgl_inexact_trunc  <= '0';
		     sgl_overflow_trunc <= '0';
			
	     else
                     int <= "00000000000000000000000000000000"; -- UNDEFINED
                     sgl_inexact_trunc  <= '0';
                     sgl_overflow_trunc <= '0';
			     
  	    end if;
      end process;


      PIPELINE_REG: we_register generic map (reg_width => word_width)
                                port map (clk => clk, reset => reset, we => enable, data_in => int, data_out => pipelined_int);


------------------------------------------------------------------

--------------------------
-- RESULT BUILDING STAGE 
--------------------------

-- "int" value is converted into the 2's comlement integer format

        ABS_TO_2_COMPLEMENT: process(pipelined_s, pipelined_int, pipelined_signselect_in)
        begin
            if pipelined_signselect_in = '1' then
              if pipelined_s = '0' then
                normal <= pipelined_int;
              else
                normal <= (others => '0');
              end if;
            else
              if pipelined_s = '0' then
                      normal <= pipelined_int;      
              else  
                      normal <= Conv_std_logic_vector( (0 - Conv_integer(pipelined_int)),word_width ); 
              end if;
            end if;
        end process;

---------------------------------------------------------------------------------------------
end rtl;
