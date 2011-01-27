-----------------------------------------------------------------------------------------------------------
--
-- 				  INTEGER TO SINGLE PRECISION FP NUMBER CONVERSION LOGIC
--
-- Created by Claudio Brunelli, 2004
--
-----------------------------------------------------------------------------------------------------------

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


-- Modified to accept 33-bit signed ints instead of 32, because this appeared to be
-- the fastest way to add unsigned int support.
-- The TCE wrapper must sign-extend a signed number, or prepend a "0" bit to unsigned.
-- -Timo


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_signed.all;
use work.cop_definitions.all;
use work.cop_components.all;

entity int_to_single_conv is
    port( clk,reset,enable : in std_logic;
          cvt_s_in         : in std_logic_vector(word_width-1 downto 0);
          signselect_in    : in std_logic;
          cvt_s_out        : out std_logic_vector(word_width-1 downto 0);
          exc_inexact_conv : out std_logic  );
end int_to_single_conv ;

-----------------------------------------------------------------------------------
architecture rtl of int_to_single_conv is

      signal s, pipelined_s : std_logic;
      signal x              : std_logic_vector(7 downto 0);
      signal f              : std_logic_vector(22 downto 0);
      signal int,
             pipelined_int  : std_logic_vector(word_width+1-1 downto 0);
      
      begin

------------------------------------------------------------------



----------------------
-- ABS(cvt_s_in) 
----------------------
            process( signselect_in, cvt_s_in )
            begin
              if signselect_in ='1' then -- unsigned input
                int <= ( abs('0'&cvt_s_in) );
                s <= '0';
              else -- signed input
                int <= ( abs(cvt_s_in(31)&cvt_s_in) );
                s <= cvt_s_in(31);
              end if;
            end process;

            PIPELINE_REG: we_register generic map (reg_width => word_width+1)
                                      port map (clk => clk, reset => reset, we => enable, data_in => int, data_out => pipelined_int);


------------------------------
-- x AND f FIELDS EXTRACTION
------------------------------

	    FRACTION_DETERMINATION: process(pipelined_int)      
	    begin
           if pipelined_int(32 downto 31) = "01" then                            -- if f = "11...1"  =>  MAXINT
                    if ( pipelined_int(6 downto 0)=Conv_std_logic_vector(0,7) ) then
                            x <= Conv_std_logic_vector(31+127,8);
                      f <= pipelined_int(30 downto 8);
                            exc_inexact_conv <= '0';
                    elsif ( (pipelined_int(6)='0') or ((pipelined_int(6)='1') and (pipelined_int(5 downto 0)="000000") and (pipelined_int(7)='0')) ) then
                            -- cut
                            x <= Conv_std_logic_vector(31+127,8);
                      f <= pipelined_int(30 downto 8);
                            exc_inexact_conv <= '1';
                    else
                            -- round
                            if ( pipelined_int(30 downto 8)="11111111111111111111111" ) then
                                    x <= Conv_std_logic_vector(32+127,8);
                              f <= (others => '0');
                            else
                                    x <= Conv_std_logic_vector(31+127,8);
                                    f <= Conv_std_logic_vector((unsigned(pipelined_int(30 downto 8))+1), 23);
                            end if;
                            exc_inexact_conv <= '1';
                    end if;
	        elsif pipelined_int(32 downto 30) = "001" then                            -- if f = "11...1"  =>  MAXINT
                    if ( pipelined_int(6 downto 0)=Conv_std_logic_vector(0,7) ) then
                            x <= Conv_std_logic_vector(30+127,8);
		                f <= pipelined_int(29 downto 7);
                            exc_inexact_conv <= '0';
                    elsif ( (pipelined_int(6)='0') or ((pipelined_int(6)='1') and (pipelined_int(5 downto 0)="000000") and (pipelined_int(7)='0')) ) then
                            -- cut
                            x <= Conv_std_logic_vector(30+127,8);
		                f <= pipelined_int(29 downto 7);
                            exc_inexact_conv <= '1';
                    else
                            -- round
                            if ( pipelined_int(29 downto 7)="11111111111111111111111" ) then
                                    x <= Conv_std_logic_vector(31+127,8);
		                        f <= (others => '0');
                            else
                                    x <= Conv_std_logic_vector(30+127,8);
                                    f <= Conv_std_logic_vector((unsigned(pipelined_int(29 downto 7))+1), 23);
                            end if;
                            exc_inexact_conv <= '1';
                    end if;
              elsif pipelined_int(32 downto 29) = "0001" then
                    if ( pipelined_int(5 downto 0)=Conv_std_logic_vector(0,6) ) then
                            x <= Conv_std_logic_vector(29+127,8);  
          		          f <= pipelined_int(28 downto 6);
                            exc_inexact_conv <= '0';
                    elsif ( (pipelined_int(5)='0') or ((pipelined_int(5)='1') and (pipelined_int(4 downto 0)="00000") and (pipelined_int(6)='0')) ) then
                            -- cut
                            x <= Conv_std_logic_vector(29+127,8);
		                f <= pipelined_int(28 downto 6);
                            exc_inexact_conv <= '1';
                    else
                            -- round
                            if ( pipelined_int(28 downto 6)="11111111111111111111111" ) then
                                    x <= Conv_std_logic_vector(30+127,8);
		                        f <= (others => '0');
                            else
                                    x <= Conv_std_logic_vector(29+127,8);
                                    f <= Conv_std_logic_vector((unsigned(pipelined_int(28 downto 6))+1), 23);
                            end if;
                            exc_inexact_conv <= '1';
                    end if;            
    	        elsif pipelined_int(32 downto 28) = "00001" then
                    if ( pipelined_int(4 downto 0)=Conv_std_logic_vector(0,5) ) then
                            x <= Conv_std_logic_vector(28+127,8);
                            f <= pipelined_int(27 downto 5);
                            exc_inexact_conv <= '0';
                    elsif ( (pipelined_int(4)='0') or ((pipelined_int(4)='1') and (pipelined_int(3 downto 0)="0000") and (pipelined_int(5)='0')) ) then
                            -- cut
                            x <= Conv_std_logic_vector(28+127,8);
		                f <= pipelined_int(27 downto 5);
                            exc_inexact_conv <= '1';
                    else
                            -- round
                            if ( pipelined_int(27 downto 5)="11111111111111111111111" ) then
                                    x <= Conv_std_logic_vector(29+127,8);
		                        f <= (others => '0');
                            else
                                    x <= Conv_std_logic_vector(28+127,8);
                                    f <= Conv_std_logic_vector((unsigned(pipelined_int(27 downto 5))+1), 23);
                            end if;
                            exc_inexact_conv <= '1';
                    end if;
	        elsif pipelined_int(32 downto 27) = "000001" then
                    if ( pipelined_int(3 downto 0)=Conv_std_logic_vector(0,4) ) then
                            x <= Conv_std_logic_vector(27+127,8);
		                f <= pipelined_int(26 downto 4);
                            exc_inexact_conv <= '0';
                    elsif ( (pipelined_int(3)='0') or ((pipelined_int(3)='1') and (pipelined_int(2 downto 0)="000") and (pipelined_int(4)='0')) ) then
                            -- cut
                            x <= Conv_std_logic_vector(27+127,8);
		                f <= pipelined_int(26 downto 4);
                            exc_inexact_conv <= '1';
                    else
                            -- round
                            if ( pipelined_int(26 downto 4)="11111111111111111111111" ) then
                                    x <= Conv_std_logic_vector(28+127,8);
		                        f <= (others => '0');
                            else
                                    x <= Conv_std_logic_vector(27+127,8);
                                    f <= Conv_std_logic_vector((unsigned(pipelined_int(26 downto 4))+1), 23);
                            end if;
                            exc_inexact_conv <= '1';
                    end if;
	        elsif pipelined_int(32 downto 26) = "0000001" then
                    if ( pipelined_int(2 downto 0)=Conv_std_logic_vector(0,3) ) then
                            x <= Conv_std_logic_vector(26+127,8);
		                f <= pipelined_int(25 downto 3);
                            exc_inexact_conv <= '0';
                    elsif ( (pipelined_int(2)='0') or ((pipelined_int(2)='1') and (pipelined_int(1 downto 0)="00") and (pipelined_int(3)='0')) ) then
                            -- cut
                            x <= Conv_std_logic_vector(26+127,8);
		                f <= pipelined_int(25 downto 3);
                            exc_inexact_conv <= '1';
                    else   
                            -- round
                            if ( pipelined_int(25 downto 3)="11111111111111111111111" ) then
                                    x <= Conv_std_logic_vector(27+127,8);
		                        f <= (others => '0');
                            else
                                    x <= Conv_std_logic_vector(26+127,8);
                                    f <= Conv_std_logic_vector((unsigned(pipelined_int(25 downto 3))+1), 23);
                            end if;                     
                            exc_inexact_conv <= '1';
                    end if;
              elsif pipelined_int(32 downto 25) = "00000001" then
                    if ( pipelined_int(1 downto 0)=Conv_std_logic_vector(0,2) ) then
                            x <= Conv_std_logic_vector(25+127,8);
		                f <= pipelined_int(24 downto 2);
                            exc_inexact_conv <= '0';
                    elsif ( (pipelined_int(1)='0') or ((pipelined_int(1)='1') and (pipelined_int(0)='0') and (pipelined_int(2)='0')) ) then
                            -- cut
                            x <= Conv_std_logic_vector(25+127,8);
		                f <= pipelined_int(24 downto 2);
                            exc_inexact_conv <= '1';
                    else 
                            -- round
                            if ( pipelined_int(24 downto 2)="11111111111111111111111" ) then
                                    x <= Conv_std_logic_vector(26+127,8);
		                        f <= (others => '0');
                            else
                                    x <= Conv_std_logic_vector(25+127,8);
                                    f <= Conv_std_logic_vector((unsigned(pipelined_int(24 downto 2))+1), 23);
                            end if;
                            exc_inexact_conv <= '1';
                    end if;
	        elsif pipelined_int(32 downto 24) = "000000001" then
                    if ( pipelined_int(0)='0' ) then
                            x <= Conv_std_logic_vector(24+127,8);
		                f <= pipelined_int(23 downto 1);
                            exc_inexact_conv <= '0';
                    else  
                            if (pipelined_int(1)='0') then
                                    --cut
                                    x <= Conv_std_logic_vector(24+127,8);
		                        f <= pipelined_int(23 downto 1);
                                    exc_inexact_conv <= '1';
                            else
                                    -- round
                                    if ( pipelined_int(23 downto 1)="11111111111111111111111" ) then
                                            x <= Conv_std_logic_vector(25+127,8);
		                                f <= (others => '0');
                                    else
                                            x <= Conv_std_logic_vector(24+127,8);
                                            f <= Conv_std_logic_vector((unsigned(pipelined_int(23 downto 1))+1), 23);
                                    end if;
                            end if;
                            exc_inexact_conv <= '1';
                    end if;
	        elsif pipelined_int(32 downto 23) = "0000000001" then
		        x <= Conv_std_logic_vector(23+127,8);
		        f <= pipelined_int(22 downto 0);
                    exc_inexact_conv <= '0';
              elsif pipelined_int(32 downto 22) = "00000000001" then
	   	        x <= Conv_std_logic_vector(22+127,8);
		        f <= pipelined_int(21 downto 0)&'0';
                    exc_inexact_conv <= '0';  
	        elsif pipelined_int(32 downto 21) = "000000000001" then
		        x <= Conv_std_logic_vector(21+127,8);
		        f <= pipelined_int(20 downto 0)&("00");
                    exc_inexact_conv <= '0';
	        elsif pipelined_int(32 downto 20) = "0000000000001" then
		        x <= Conv_std_logic_vector(20+127,8);
		        f <= pipelined_int(19 downto 0)&("000");
                    exc_inexact_conv <= '0';
	        elsif pipelined_int(32 downto 19) = "00000000000001" then
		        x <= Conv_std_logic_vector(19+127,8);
		        f <= pipelined_int(18 downto 0)&("0000");
                    exc_inexact_conv <= '0';
	        elsif pipelined_int(32 downto 18) = "000000000000001" then
		        x <= Conv_std_logic_vector(18+127,8);
		        f <= pipelined_int(17 downto 0)&("00000");
                    exc_inexact_conv <= '0';
              elsif pipelined_int(32 downto 17) = "0000000000000001" then
		        x <= Conv_std_logic_vector(17+127,8);
		        f <= pipelined_int(16 downto 0)&("000000");
                    exc_inexact_conv <= '0';
              elsif pipelined_int(32 downto 16) = "00000000000000001" then
		        x <= Conv_std_logic_vector(16+127,8);
		        f <= pipelined_int(15 downto 0)&("0000000");
                    exc_inexact_conv <= '0';
	        elsif pipelined_int(32 downto 15) = "000000000000000001" then                     --   [(2^15) .. (2^16)-1] 
		        x <= Conv_std_logic_vector(15+127,8);
		        f <= pipelined_int(14 downto 0)&("00000000");
                    exc_inexact_conv <= '0';
	        elsif pipelined_int(32 downto 14) = "0000000000000000001" then
		        x <= Conv_std_logic_vector(14+127,8);
		        f <= pipelined_int(13 downto 0)&("000000000");
                    exc_inexact_conv <= '0';
	        elsif pipelined_int(32 downto 13) = "00000000000000000001" then
		        x <= Conv_std_logic_vector(13+127,8);
		        f <= pipelined_int(12 downto 0)&("0000000000");
                    exc_inexact_conv <= '0';
	        elsif pipelined_int(32 downto 12) = "000000000000000000001" then
		        x <= Conv_std_logic_vector(12+127,8);
		        f <= pipelined_int(11 downto 0)&("00000000000");
                    exc_inexact_conv <= '0';
	        elsif pipelined_int(32 downto 11) = "0000000000000000000001" then
		        x <= Conv_std_logic_vector(11+127,8);
		        f <= pipelined_int(10 downto 0)&("000000000000");
                    exc_inexact_conv <= '0';
	        elsif pipelined_int(32 downto 10) = "00000000000000000000001" then               --   [1024-2047]
		        x <= Conv_std_logic_vector(10+127,8);
		        f <= pipelined_int(9 downto 0)&("0000000000000");
                    exc_inexact_conv <= '0';
	        elsif pipelined_int(32 downto 9) = "000000000000000000000001" then               --   [512-1023]
		        x <= Conv_std_logic_vector(9+127,8);
		        f <= pipelined_int(8 downto 0)&("00000000000000");
                    exc_inexact_conv <= '0';
              elsif pipelined_int(32 downto 8) = "0000000000000000000000001" then              --   [256-511]
		        x <= Conv_std_logic_vector(8+127,8);
		        f <= pipelined_int(7 downto 0)&("000000000000000");
                    exc_inexact_conv <= '0';
	        elsif pipelined_int(32 downto 7) = "00000000000000000000000001" then             --   [128-255]
		        x <= Conv_std_logic_vector(7+127,8);
		        f <= pipelined_int(6 downto 0)&("0000000000000000");
                    exc_inexact_conv <= '0';
	        elsif pipelined_int(32 downto 6) = "000000000000000000000000001" then            --   [64-127]
		        x <= Conv_std_logic_vector(6+127,8);
		        f <= pipelined_int(5 downto 0)&("00000000000000000");
                    exc_inexact_conv <= '0';
	        elsif pipelined_int(32 downto 5) = "0000000000000000000000000001" then           --   [32-63]
		        x <= Conv_std_logic_vector(5+127,8);
		        f <= pipelined_int(4 downto 0)&("000000000000000000");
                    exc_inexact_conv <= '0';
              elsif pipelined_int(32 downto 4) = "00000000000000000000000000001" then          --   [16-31]
		        x <= Conv_std_logic_vector(4+127,8);
		        f <= pipelined_int(3 downto 0)&("0000000000000000000");
                    exc_inexact_conv <= '0';
              elsif pipelined_int(32 downto 3) = "000000000000000000000000000001" then         --   [8-15]
		        x <= Conv_std_logic_vector(3+127,8);
		        f <= pipelined_int(2 downto 0)&("00000000000000000000");
                    exc_inexact_conv <= '0';
              elsif pipelined_int(32 downto 2) = "0000000000000000000000000000001" then        --   [4-7]
		        x <= Conv_std_logic_vector(2+127,8);
		        f <= pipelined_int(1 downto 0)&("000000000000000000000");
                    exc_inexact_conv <= '0';
	        elsif pipelined_int(32 downto 1) = "00000000000000000000000000000001" then       --   [2-3]
		        x <= Conv_std_logic_vector(1+127,8);
		        f <= pipelined_int(0)&("0000000000000000000000");
                    exc_inexact_conv <= '0';
	        elsif pipelined_int(32 downto 0) = "000000000000000000000000000000001" then      --   ONE;  x = 127   =>   e =0 
		        x <= Conv_std_logic_vector(0+127,8);                          
	 	        f <= (others => '0');
                    exc_inexact_conv <= '0';
              elsif pipelined_int (32 downto 0) = "100000000000000000000000000000000" then     --   MININT  =  -(MAXINT + 1)
                    x <= Conv_std_logic_vector(31+127,8);                          
                    f <= (others => '0');
                    exc_inexact_conv <= '0';
              else 											 
		        x <= (others => '0');                                           --   ZERO
		        f <= (others => '0');
                    exc_inexact_conv <= '0';
	        end	if;

        end process;

--------------------
-- RESULT PACKING
--------------------


          PIPELINE_FF: wedff port map (clk => clk, reset => reset, we => enable, d => s, q => pipelined_s);

	  cvt_s_out <= (pipelined_s & x & f);

-----------------------------------------------------------------------------------------------------------------
end rtl;
