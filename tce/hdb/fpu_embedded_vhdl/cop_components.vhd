-----------------------------------------------------------------------------------------------------------
--
--                	                         COMPONENTS PACKAGE
--
-- This package contains several frequently used basic cop_components.
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


-----------------------------------------------------------------------------------------------------------
--
--				                          OE_register
--
-- General purpose register, "reg_width" wide. WRITE ENABLE, OUTPUT ENABLE and RESET commands are provided.
-- Default content at RESET is "zero". WRITE ENABLE, OUTPUT ENABLE and RESET commands polarity are
-- specified by dedicated constants defined in "cop_definitions" package.
--
-----------------------------------------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use work.cop_definitions.all;

entity OE_register is
    generic(reg_width: Integer := word_width);
    port( clk      : in    Std_logic;
          reset    : in    Std_logic;
          we       : in    Std_logic;
	  oe       : in    Std_logic;
          data_in  : in    Std_logic_vector(reg_width-1 downto 0);
          data_out : out   Std_logic_vector(reg_width-1 downto 0) );
end OE_register;
 

architecture rtl of OE_register is

    signal regout: std_logic_vector(reg_width-1 downto 0);

    begin

	    process(clk, reset)
          begin
	        if reset = reset_active then 
		          regout <= Conv_std_logic_vector(0, reg_width);
		  elsif CLK'EVENT and CLK='1' then 
                      if we = we_active then
                              regout <= data_in;
                      end if;
		  end if; 
          end process;  

	    process (oe, regout)
	    begin 
              if oe = oe_active then
		          data_out <= regout;
		  else
		          data_out <= (others => 'Z');
		  end if;
	    end process;

end rtl;


-----------------------------------------------------------------------------------------------------------
--
--							      WE_register
--
-- General purpose register, "reg_width" wide. WRITE ENABLE and RESET commands are provided.
-- Default content at RESET is "zero". WRITE ENABLE and RESET commands polarity are
-- specified by dedicated constants defined in "cop_definitions" package.
--
-----------------------------------------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use work.cop_definitions.all;

entity WE_register is
    generic(reg_width: Integer := word_width);
    port(  clk      : in    Std_logic;
           reset    : in    Std_logic;
           we       : in    Std_logic;
	     data_in  : in    Std_logic_vector(reg_width-1 downto 0);
           data_out : out   Std_logic_vector(reg_width-1 downto 0)  );
end WE_register;
 

architecture rtl of WE_register is

    begin

        process(clk, reset)
        begin
		    if reset = reset_active then 
		        data_out <= Conv_std_logic_vector(0, reg_width);
		    elsif CLK'EVENT and CLK='1' then 
                    if we = we_active then data_out <= data_in;
                    end if;
		    end if; 
        end process;  

end rtl;

-----------------------------------------------------------------------------------------------------------
--
--							  SIMPLE DATA REGISTER
--
-----------------------------------------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use work.cop_definitions.all;

entity data_register is
    generic(reg_width: integer :=8);
    port( 
             clk      : in    Std_logic;
             reset    : in    Std_logic;
 	     data_in  : in    Std_logic_vector(reg_width-1 downto 0);
             data_out : out   Std_logic_vector(reg_width-1 downto 0)
        );
end data_register;
 

architecture rtl of data_register is

    begin

        process(clk, reset)
        begin
	      if reset = reset_active then 
		        data_out <= Conv_std_logic_vector(0,reg_width);
		elsif CLK'EVENT and CLK='1' then 
                    data_out <= data_in;
		end if; 
        end process;  

end rtl;


-----------------------------------------------------------------------------------------------------------
--
--							  NOP REGISTER
--
-----------------------------------------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use work.cop_definitions.all;

entity nop_register is
    generic(reg_width: integer :=word_width);
    port( 
             clk      : in    Std_logic;
             reset    : in    Std_logic;
 	     data_in  : in    Std_logic_vector(reg_width-1 downto 0);
             data_out : out   Std_logic_vector(reg_width-1 downto 0)
        );
end nop_register;
 

architecture rtl of nop_register is

    begin

        process(clk, reset)
        begin
	      if reset = reset_active then 
		        data_out <= "00000000000000000000000000001000";  -- NOP configuration
		elsif CLK'EVENT and CLK='1' then 
                    data_out <= data_in;
		end if; 
        end process;  

end rtl;


----------------------------------------------------------------------------------------------------------
--
--					D-type Flip-Flop  provided with Write Enable command
--
-----------------------------------------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use work.cop_definitions.all;

entity WEDFF is
	port( clk   : in    Std_logic;
            reset : in    Std_logic;
            we    : in    Std_logic;
		d     : in    Std_logic;
            q     : out   Std_logic );
end WEDFF;
 

architecture rtl of WEDFF is

    begin

    process(clk, reset)
    begin
        if reset = reset_active then 
	          q <= '0';
        elsif CLK'EVENT and CLK='1' then 
                if we = we_active then 
                        q <= d;
                end if;
        end if; 
    end process;  

end rtl;



-----------------------------------------------------------------------------------------------------------
--
--							D-type Flip-Flop reset
--							value ='0'
--
-----------------------------------------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use work.cop_definitions.all;

entity data_ff is

  port( clk   : in    Std_logic;
        reset : in    Std_logic;
        d     : in    Std_logic;
        q     : out   Std_logic );
  
end data_ff;
 

architecture rtl of data_ff is

    begin

        process(clk, reset)
        begin
	      if reset = reset_active then 
		        q <= '0';      
		elsif CLK'EVENT and CLK='1' then
		        q <= d;
   	      end if; 
        end process;  

end rtl;

-----------------------------------------------------------------------------------------------------------
--
--							D-type Flip-Flop reset value = '1'
--
-----------------------------------------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use work.cop_definitions.all;

entity data_ff_1 is
  
  port( clk   : in    Std_logic;
        reset : in    Std_logic;
        d     : in    Std_logic;
        q     : out   Std_logic );
  
end data_ff_1;
 

architecture rtl of data_ff_1 is

    begin

        process(clk, reset)
        begin
	      if reset = reset_active then 
		        q <= '1';
		elsif CLK'EVENT and CLK='1' then
		        q <= d;
   	      end if; 
        end process;  

end rtl;

-----------------------------------------------------------------------------------------------------------
--
--			                           DECODER 3:8
--
-- The output addressed by dec_addr is the only one to get an "high" logic value
--
-----------------------------------------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use work.cop_definitions.all;

entity decoder3_8 is
    port( dec_addr : in std_logic_vector(2 downto 0);
	  dec_out  : out std_logic_vector(7 downto 0) );
end decoder3_8;


architecture rtl of decoder3_8 is

    begin

        process(dec_addr)
	  begin
	        dec_out(7) <= (dec_addr(2) and dec_addr(1) and dec_addr(0));
		dec_out(6) <= (dec_addr(2) and dec_addr(1) and not (dec_addr(0)));
		dec_out(5) <= (dec_addr(2) and not (dec_addr(1)) and dec_addr(0));
		dec_out(4) <= (dec_addr(2) and not (dec_addr(1)) and not (dec_addr(0)));
		dec_out(3) <= (not(dec_addr(2)) and dec_addr(1) and dec_addr(0));
		dec_out(2) <= (not(dec_addr(2)) and dec_addr(1) and not(dec_addr(0)));
		dec_out(1) <= (not(dec_addr(2)) and not(dec_addr(1)) and dec_addr(0));
		dec_out(0) <= (not(dec_addr(2)) and not(dec_addr(1)) and not(dec_addr(0)));
	end process;

end rtl;

-----------------------------------------------------------------------------------------------------------
--
--				                   TRISTATE BUFFER
--
-- It's a tristate buffer which is "buffer_width" wide. Output is set to Hi-Z value when its output enable
-- control pin is set to "oe_active" value.
--
-----------------------------------------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use work.cop_definitions.all;

entity tristate_buffer is
   generic(width : integer := buffer_width);
   port( 
          tristate_buffer_oe       : in std_logic;
          tristate_buffer_data_in  : in std_logic_vector(buffer_width-1 downto 0);
          tristate_buffer_data_out : out std_logic_vector(buffer_width-1 downto 0)
        );
end tristate_buffer ;


architecture rtl of tristate_buffer is

    begin

        process (tristate_buffer_oe, tristate_buffer_data_in)
            begin 
                if tristate_buffer_oe = oe_active then
	                  tristate_buffer_data_out <= tristate_buffer_data_in;
	          else
	                  tristate_buffer_data_out <= (others => 'Z');
	          end if;
        end process;

end rtl;


-----------------------------------------------------------------------------------------------------------
--
-- 							8_INPUTS MULTIPLEXER
--
-----------------------------------------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all; 
use work.cop_definitions.all;

entity MY_8IN_MUX is
    generic (width : integer :=word_width);
    port( in_a   : in Std_logic_vector(width-1 downto 0);
          in_b   : in Std_logic_vector(width-1 downto 0);
          in_c   : in Std_logic_vector(width-1 downto 0);
          in_d   : in Std_logic_vector(width-1 downto 0);
          in_e   : in Std_logic_vector(width-1 downto 0);
          in_f   : in Std_logic_vector(width-1 downto 0);
          in_g   : in Std_logic_vector(width-1 downto 0);
          in_h   : in Std_logic_vector(width-1 downto 0);
          sel    : in Std_logic_vector(2 downto 0);
          output : out Std_logic_vector(width-1 downto 0) );
end MY_8IN_MUX;


architecture rtl of MY_8IN_MUX is

    begin  

    process(in_a,in_b,in_c,in_d,in_e,in_f,in_g,in_h,sel)
    begin
        if sel = "000" then
                output <= in_a;
        elsif sel = "001" then
                output <= in_b;
        elsif sel = "010" then
                output <= in_c;
        elsif sel = "011" then
                output <= in_d;
        elsif sel = "100" then
                output <= in_e;
        elsif sel = "101" then
                output <= in_f;
        elsif sel = "110" then
                output <= in_g;
        else
                output <= in_h;
        end if;
    end process;
  
end rtl;

-----------------------------------------------------------------------------------------------------------
--
-- 							CTRL_LOGIC MULTIPLEXER
--
-----------------------------------------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all; 
use work.cop_definitions.all;

entity DELAY_SELECT_MUX is
    generic (width : integer := 8);
    port( in_a   : in Std_logic_vector(width-1 downto 0);
          in_b   : in Std_logic_vector(width-1 downto 0);
          in_c   : in Std_logic_vector(width-1 downto 0);
          in_d   : in Std_logic_vector(width-1 downto 0);
          in_e   : in Std_logic_vector(width-1 downto 0);
          in_f   : in Std_logic_vector(width-1 downto 0);
          in_g   : in Std_logic_vector(width-1 downto 0);
          in_h   : in Std_logic_vector(width-1 downto 0);
          sel    : in Std_logic_vector(6 downto 0);
          output : out Std_logic_vector(width-1 downto 0) 
        );
end DELAY_SELECT_MUX;


architecture rtl of DELAY_SELECT_MUX is

    begin  

    process(in_a,in_b,in_c,in_d,in_e,in_f,in_g,in_h,sel)
    begin

        if sel = "0000001" then
                output <= in_b;
        elsif sel = "0000010" then  
                output <= in_c;
        elsif sel = "0000100" then 
                output <= in_d;
        elsif sel = "0001000" then 
                output <= in_e;
        elsif sel = "0010000" then 
                output <= in_f;
        elsif sel = "0100000" then 
                output <= in_g;
        elsif sel = "1000000" then 
                output <= in_h;
        else
                output <= in_a;
        end if;

    end process;
  
end rtl;


-----------------------------------------------------------------------------------------------------------
--
-- 							2_INPUTS MULTIPLEXER
--
-----------------------------------------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all; 
use work.cop_definitions.all;

entity MY_MUX is
    generic (width : integer :=8);
    port( in_a   : in Std_logic_vector(width-1 downto 0);
          in_b   : in Std_logic_vector(width-1 downto 0);
          sel    : in Std_logic;
          output : out Std_logic_vector(width-1 downto 0) );
end MY_MUX;


architecture rtl of MY_MUX is

    begin  

    process(in_a,in_b,sel)
    begin
        if sel = '0' then
                output <= in_a;
        else
                output <= in_b;
        end if;
    end process;
  
end rtl;

-----------------------------------------------------------------------------------------------------------
--
-- 						2_INPUTS MULTIPLEXER (1 bit-wide inputs)
--
-----------------------------------------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all; 
use work.cop_definitions.all;

entity SIMPLE_MUX_2 is
	port( in_a   : in Std_logic;
            in_b   : in Std_logic;
            sel    : in Std_logic;
            output : out Std_logic );
end SIMPLE_MUX_2;


architecture rtl of SIMPLE_MUX_2 is

     begin  

         process(in_a,in_b,sel)
         begin
             if sel = '0' then
                     output <= in_a;
             else
                     output <= in_b;
             end if;
        end process;
  
end rtl;


-----------------------------------------------------------------------------------------------------------
--
-- 					       REGISTER DELAY CHAIN (ctrl_logic)
--
-----------------------------------------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all; 
use IEEE.std_logic_arith.all;
use work.cop_definitions.all;


entity register_chain is
    generic ( length : integer := m;
              width  : integer := n );
    port( clk           : in std_logic;
          reset         : in std_logic;
          enable        : in Std_logic;
          reg_chain_in  : in Std_logic_vector(width-1 downto 0);
          reg_chain_out : out bus_mxn );
end register_chain;


architecture rtl of register_chain is
    signal ctrl_internal_bus : bus_mxn;

    begin  

        ctrl_internal_bus(0) <= reg_chain_in;

        WRBCK_DELAY_CHAIN: for i in 0 to (length-1) generate   
            process(clk, reset)
            begin
		    if reset = reset_active then 
		            ctrl_internal_bus(i+1) <= conv_std_logic_vector(0, width);
		    elsif CLK'EVENT and CLK='1' then 
                            if enable = we_active then 
                                    ctrl_internal_bus(i+1) <= ctrl_internal_bus(i);
                            end if;
                    end if; 
            end process;  
        end generate WRBCK_DELAY_CHAIN;

        reg_chain_out <= ctrl_internal_bus;
  
end rtl;


-----------------------------------------------------------------------------------------------------------
--
-- 					     SIMPLE REGISTER DELAY CHAIN
--
-----------------------------------------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all; 
use IEEE.std_logic_arith.all;
use work.cop_definitions.all;


entity simple_register_chain is
    generic ( length : integer := m;
              width  : integer := n );
    port( clk           : in std_logic;
          reset         : in std_logic;
          enable        : in Std_logic;
          reg_chain_in  : in Std_logic_vector(width-1 downto 0);
          reg_chain_out : out Std_logic_vector(width-1 downto 0) );
end simple_register_chain;


architecture rtl of simple_register_chain is

    type bus_lengthXwidth is array (length downto 0) of std_logic_vector(width-1 downto 0);

    signal ctrl_internal_bus : bus_lengthXwidth;

    begin  

        ctrl_internal_bus(0) <= reg_chain_in;

        WRBCK_DELAY_CHAIN: for i in 0 to (length-1) generate   
            process(clk, reset)
            begin
		    if reset = reset_active then 
		            ctrl_internal_bus(i+1) <= conv_std_logic_vector(0, width);
		    elsif CLK'EVENT and CLK='1' then 
                            if enable = we_active then 
                                    ctrl_internal_bus(i+1) <= ctrl_internal_bus(i);
                            end if;
                    end if; 
            end process;  
        end generate WRBCK_DELAY_CHAIN;

        reg_chain_out <= ctrl_internal_bus(length);
  
end rtl;



-----------------------------------------------------------------------------------------------------------
--
-- 					         FF DELAY CHAIN
--
-----------------------------------------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all; 
use IEEE.std_logic_arith.all;
use work.cop_definitions.all;


entity ff_variable_chain is
    generic ( length : integer := m);
    port( clk           : in std_logic;
          reset         : in std_logic;
          enable        : in Std_logic;
          ff_chain_in   : in Std_logic;
          ff_chain_out  : out Std_logic_vector(length downto 0) );
end ff_variable_chain;


architecture rtl of ff_variable_chain is
    signal ctrl_internal_bus : Std_logic_vector(length downto 0);

    begin  

        ctrl_internal_bus(0) <= ff_chain_in;

        VARIABLE_DELAY_CHAIN: for i in 0 to (length-1) generate   
            process(clk, reset)
            begin
		    if reset = reset_active then 
		            ctrl_internal_bus(i+1) <= '0';
		    elsif CLK'EVENT and CLK='1' then 
                            if enable = we_active then 
                                    ctrl_internal_bus(i+1) <= ctrl_internal_bus(i);
                            end if;
                    end if; 
            end process;  
        end generate VARIABLE_DELAY_CHAIN;

        ff_chain_out <= ctrl_internal_bus;
  
end rtl;



-----------------------------------------------------------------------------------------------------------
--
-- 					      SIMPLE FF DELAY CHAIN
--
-----------------------------------------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all; 
use IEEE.std_logic_arith.all;
use work.cop_definitions.all;


entity ff_chain is
    generic ( length : integer := m );
    port( clk          : in std_logic;
          reset        : in std_logic;
          enable       : in Std_logic;
          ff_chain_in  : in Std_logic;
          ff_chain_out : out std_logic );
end ff_chain;


architecture rtl of ff_chain is
    signal ff_chain_internal_signals : std_logic_vector(length downto 0);

    begin  

        ff_chain_internal_signals(0) <= ff_chain_in;

        WRBCK_DELAY_CHAIN: for i in 0 to (length-1) generate   
            process(clk, reset)
            begin
		    if reset = reset_active then 
		            ff_chain_internal_signals(i+1) <= '0';
		    elsif CLK'EVENT and CLK='1' then 
                            if enable = we_active then 
                                    ff_chain_internal_signals(i+1) <= ff_chain_internal_signals(i);
                            end if;
                    end if; 
            end process;  
        end generate WRBCK_DELAY_CHAIN;

        ff_chain_out <= ff_chain_internal_signals(length);
  
end rtl;


-----------------------------------------------------------------------------------------------------------
--
-- 							    PACKAGE HEADER
--
-----------------------------------------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use work.cop_definitions.all;

package cop_components is

    component OE_register 
        generic(reg_width: Integer := word_width);
        port( clk      : in    Std_logic;
              reset    : in    Std_logic;
              we       : in    Std_logic;
		  oe       : in    Std_logic;
              data_in  : in    Std_logic_vector(reg_width-1 downto 0);
              data_out : out   Std_logic_vector(reg_width-1 downto 0) );
    end component;


    component WE_register 
        generic(reg_width: Integer := word_width);
	  port( clk      : in    Std_logic;
              reset    : in    Std_logic;
              we       : in    Std_logic;
	        data_in  : in    Std_logic_vector(reg_width-1 downto 0);
              data_out : out   Std_logic_vector(reg_width-1 downto 0) );
    end component;


    component data_register 
        generic(reg_width: Integer);
	  port( clk      : in    Std_logic;
              reset    : in    Std_logic;
		  data_in  : in    Std_logic_vector(reg_width-1 downto 0);
              data_out : out   Std_logic_vector(reg_width-1 downto 0) );
    end component;


    component nop_register
        generic(reg_width: integer :=word_width);
          port( 
               clk      : in    Std_logic;
               reset    : in    Std_logic;
 	       data_in  : in    Std_logic_vector(reg_width-1 downto 0);
               data_out : out   Std_logic_vector(reg_width-1 downto 0)
              );
    end component;


    component WEDFF
        port( clk   : in    Std_logic;
              reset : in    Std_logic;
              we    : in    Std_logic;
		  d     : in    Std_logic;
              q     : out   Std_logic );
    end component;


    component data_ff
         port( clk   : in    Std_logic;
               reset : in    Std_logic;
	       d     : in    Std_logic;
               q     : out   Std_logic );
    end component;

    component data_ff_1
         port( clk   : in    Std_logic;
               reset : in    Std_logic;
	       d     : in    Std_logic;
               q     : out   Std_logic );
    end component;

    component decoder3_8
        port( dec_addr : in std_logic_vector(2 downto 0);
		  dec_out  : out std_logic_vector(7 downto 0) );
    end component;


    component tristate_buffer
        generic(width : integer := buffer_width);
	  port( tristate_buffer_oe       : in std_logic;
              tristate_buffer_data_in  : in std_logic_vector(buffer_width-1 downto 0);
              tristate_buffer_data_out : out std_logic_vector(buffer_width-1 downto 0) );
    end component;


    component MY_8IN_MUX
        generic (width : integer :=word_width);
          port( in_a   : in Std_logic_vector(width-1 downto 0);
                in_b   : in Std_logic_vector(width-1 downto 0);
                in_c   : in Std_logic_vector(width-1 downto 0);
                in_d   : in Std_logic_vector(width-1 downto 0);
                in_e   : in Std_logic_vector(width-1 downto 0);
                in_f   : in Std_logic_vector(width-1 downto 0);
                in_g   : in Std_logic_vector(width-1 downto 0);
                in_h   : in Std_logic_vector(width-1 downto 0);
                sel    : in Std_logic_vector(2 downto 0);
                output : out Std_logic_vector(width-1 downto 0) );
    end component;

    component DELAY_SELECT_MUX
    generic (width : integer := 8);
    port( in_a   : in Std_logic_vector(width-1 downto 0);
          in_b   : in Std_logic_vector(width-1 downto 0);
          in_c   : in Std_logic_vector(width-1 downto 0);
          in_d   : in Std_logic_vector(width-1 downto 0);
          in_e   : in Std_logic_vector(width-1 downto 0);
          in_f   : in Std_logic_vector(width-1 downto 0);
          in_g   : in Std_logic_vector(width-1 downto 0);
          in_h   : in Std_logic_vector(width-1 downto 0);
          sel    : in Std_logic_vector(6 downto 0);
          output : out Std_logic_vector(width-1 downto 0) 
        );
    end component;

    component MY_MUX
        generic ( width : integer);
	  port(in_a : in Std_logic_vector(word_width-1 downto 0);
             in_b   : in Std_logic_vector(word_width-1 downto 0);
             sel    : in Std_logic;
             output : out Std_logic_vector(word_width-1 downto 0) );
    end component;

    component SIMPLE_MUX_2
        port( in_a   : in Std_logic;
              in_b   : in Std_logic;
              sel    : in Std_logic;
              output : out Std_logic );
    end component;

    component register_chain
    generic ( length : integer := m;
              width  : integer := n );
    port( clk           : in std_logic;
          reset         : in std_logic;
          enable        : in Std_logic;
          reg_chain_in  : in Std_logic_vector(n-1 downto 0);
          reg_chain_out : out bus_mxn );
    end component;

    component simple_register_chain
    generic ( length : integer := m;
              width  : integer := n );
    port( clk           : in std_logic;
          reset         : in std_logic;
          enable        : in Std_logic;
          reg_chain_in  : in Std_logic_vector(width-1 downto 0);
          reg_chain_out : out Std_logic_vector(width-1 downto 0) );
    end component;

    component ff_variable_chain
    generic ( length : integer := m);
    port( clk           : in std_logic;
          reset         : in std_logic;
          enable        : in Std_logic;
          ff_chain_in   : in Std_logic;
          ff_chain_out  : out Std_logic_vector(length downto 0) );
    end component;

    component ff_chain
    generic ( length : integer := m );
    port( clk          : in std_logic;
          reset        : in std_logic;
          enable       : in Std_logic;
          ff_chain_in  : in Std_logic;
          ff_chain_out : out std_logic );
    end component;


end cop_components;


package body cop_components is
end cop_components;


