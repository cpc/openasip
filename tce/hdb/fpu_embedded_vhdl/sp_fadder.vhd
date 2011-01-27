-----------------------------------------------------------------------------------------------------------
--
--				           SINGLE PRECISION FP NUMBERS ADDER
--                         
-- Created by Claudio Brunelli, 2004
--
-----------------------------------------------------------------------------------------------------------
-- The chosen (default) rounding policy is "Round to nearest even" as specified by 
-- IEEE Std 754 (1985)

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
use work.cop_definitions.all;
use work.cop_components.all;

entity sp_fadder is
    port( clk,reset,enable          : in  std_logic;
          sub_sel                   : in  std_logic;
          operand_a,operand_b       : in  std_logic_vector(word_width-1 downto 0);
          add_result                : out std_logic_vector(word_width-1 downto 0);
          exc_overflow_add          : out std_logic;  
          exc_underflow_add         : out std_logic;  
          exc_inexact_add           : out std_logic;
          exc_invalid_operation_add : out std_logic ); 
end sp_fadder ;

-------------------------------------------------------------------------------
architecture rtl of sp_fadder is

    component normalizer
        port( denormal_operand    : in  std_logic_vector(word_width-2 downto 0);
              normalized_mantissa : out std_logic_vector(23 downto 0);
              normalized_exponent : out std_logic_vector(8 downto 0)
            );   
    end component;

    component add_packer
        port( internal_exponent      : in std_logic_vector(8 downto 0);
              internal_mantissa      : in std_logic_vector(47 downto 0);
              packed_exponent        : out std_logic_vector(7 downto 0);
              packed_fractional_part : out std_logic_vector(22 downto 0);
              exc_overflow           : out std_logic;
              exc_underflow          : out std_logic;
              exc_inexact            : out std_logic
        );
    end component;

      
    signal op_a, op_b                 : std_logic_vector(word_width-1 downto 0);
    signal pipelined_op_a,
           pipelined_op_b             : std_logic_vector(word_width-1 downto 0);
    signal  temp_op_b             : std_logic_vector(word_width-1 downto 0);
    signal delayed_op_a,
           delayed_op_b               : std_logic_vector(word_width-1 downto 0);
    signal sa, sb                     : std_logic;
    signal pipelined_sa,
           pipelined_sb               : std_logic;
    signal xa, xb                     : std_logic_vector(8 downto 0);
    signal pipelined_xa,
           pipelined_xb               : std_logic_vector(8 downto 0);
    signal ma, mb                     : std_logic_vector(23 downto 0);
    signal pipelined_ma,
           pipelined_mb               : std_logic_vector(23 downto 0);
    signal signs_xor,
           pipelined_signs_xor,
           delayed_signs_xor,
           previous_signs_xor         : std_logic;
    signal shift_amount               : integer RANGE 0 to 512;
    signal xmin,
           delayed_xmin               : std_logic_vector(8 downto 0);
    signal aligned_ma,
           aligned_mb                 : std_logic_vector(47 downto 0);  
    signal pipelined_aligned_ma,
           pipelined_aligned_mb       : std_logic_vector(47 downto 0); 
    signal xa_is_bigger,
           xb_is_bigger,
           xa_equal_xb,
           ma_is_bigger,
           mb_is_bigger,
           op_a_is_bigger,
           op_b_is_bigger             : std_logic;
    signal integer_add, integer_sub,
           temp_integer_sub           : std_logic_vector(47 downto 0);    
    signal integer_result             : std_logic_vector(47 downto 0);
    signal pipelined_integer_result   : std_logic_vector(47 downto 0);

    signal normalized_xmin            : std_logic_vector(8 downto 0);
    signal pipelined_normalized_xmin  : std_logic_vector(8 downto 0);
    signal normalized_add             : std_logic_vector(47 downto 0);
    signal pipelined_normalized_add   : std_logic_vector(47 downto 0);
    signal internal_result            : std_logic_vector(word_width-1 downto 0);
    signal zero                       : std_logic_vector(word_width-1 downto 0);
    signal unsigned_zero              : std_logic_vector(word_width-2 downto 0);
    signal infinity                   : std_logic_vector(word_width-2 downto 0); 
    signal s, delayed_s               : std_logic;
    signal sgl_packed_fractional_part : std_logic_vector(22 downto 0);
    signal sgl_packed_exponent        : std_logic_vector(7 downto 0);
    signal sgl_exc_inexact            : std_logic;
    signal op_neglegted,
           sgl_overflow_sum,
           sgl_underflow_sum,
           sgl_inexact_sum            : std_logic;
    signal fixed_enable               : std_logic;

    begin
                
-----------------------------------------------------------------------------------------------------------------  

        zero          <= conv_std_logic_vector(0,32);
        unsigned_zero <= conv_std_logic_vector(0,31);
        infinity      <= "1111111100000000000000000000000";  -- The sign bit is specified elsewhere
        fixed_enable  <= '1';
        
-----------------
-- INPUT STAGE
-----------------
        
        op_a <= operand_a;
        op_b <= operand_b;
        
-- operands' signs extraction and elaboration:

        sa <= op_a(word_width-1);
 
        process(sub_sel,op_b)
        begin
            if (sub_sel = '0') then
                    -- a sum has to be performed
                    sb <= op_b(word_width-1);
            else
                    -- a subtraction has to be performed
                    sb <= not( op_b(word_width-1) );
            end if;
        end process;


        signs_xor <= (sa xor sb);

        
        SYNCHRONIZATION_FF_SA: wedff port map (clk => clk, reset => reset, we => enable, d => sa, q => pipelined_sa);
        SYNCHRONIZATION_FF_SB: wedff port map (clk => clk, reset => reset, we => enable, d => sb, q => pipelined_sb);


        process(xa_is_bigger, xb_is_bigger, xa_equal_xb, ma_is_bigger, mb_is_bigger)
        begin

            if (xa_is_bigger = '1') then
                    op_a_is_bigger <= '1';
                    op_b_is_bigger <= '0';
                    -- op_a_equal_op_b <= '0';  -- note: equal operands with opposite signs is a special case calculated by "special operands logic"
            elsif (xb_is_bigger = '1') then
                    op_a_is_bigger <= '0';
                    op_b_is_bigger <= '1';
                    -- op_a_equal_op_b <= '0';
            elsif ((xa_equal_xb = '1') and (ma_is_bigger = '1')) then
                    op_a_is_bigger <= '1';
                    op_b_is_bigger <= '0';
                    -- op_a_equal_op_b <= '0';
            elsif ((xa_equal_xb = '1') and (mb_is_bigger = '1')) then
                    op_a_is_bigger <= '0';
                    op_b_is_bigger <= '1';
                    -- op_a_equal_op_b <= '0';
            else
                    op_a_is_bigger <= '0';      -- equal operands
                    op_b_is_bigger <= '0';
                    -- op_a_equal_op_b <= '1';
            end if;

        end process;


        process(pipelined_sa, pipelined_sb, op_a_is_bigger, op_b_is_bigger)
        begin
        if ( (pipelined_sa = '1') and (pipelined_sb = '1') ) then  -- sum of two negative operands; it can be turned in a sum of positive operands with final sign inverted (that is always negative)
                s <= '1';
        elsif ( (pipelined_sa = '0') and (pipelined_sb = '0') ) then  -- sum of two positive operands
                s <= '0';
        elsif ( (pipelined_sa = '0') and (pipelined_sb = '1') and (op_a_is_bigger = '1') ) then  -- if "operand a" is bigger than "operand b" the result is positive
                s <= '0';
        elsif ( (pipelined_sa = '0') and (pipelined_sb = '1') and (op_b_is_bigger = '1') ) then  -- if "operand b" is bigger than "operand a" the result is negative
                s <= '1';
        elsif ( (pipelined_sa = '1') and (pipelined_sb = '0') and (op_a_is_bigger = '1') ) then  -- if "operand a" is bigger than "operand b" the result is negative
                s <= '1';
        else                                                                 -- if "operand b" is bigger than "operand a" the result is negative
                s <= '0';
        end if;
        end process;


        PIPELINE_FF_CHAIN_S: ff_chain generic map (length => 3)
                                                 port map (clk => clk, reset => reset, enable => fixed_enable, ff_chain_in => s, ff_chain_out => delayed_s);
        
-----------------------         
-- OPERANDS UNPACKING
-----------------------

        ADD_OP_A_NORMALIZATION: normalizer 
                                port map ( denormal_operand => op_a(30 downto 0), normalized_mantissa => ma, normalized_exponent => xa);

        ADD_OP_B_NORMALIZATION: normalizer 
                                port map ( denormal_operand => op_b(30 downto 0), normalized_mantissa => mb, normalized_exponent => xb);

        ----------------------------------------------------------

        FIRST_PIPELINE_REG_CHAIN_OP_A: we_register generic map (reg_width => word_width)
                                                   port map (clk => clk, reset => reset, we => enable, data_in => op_a, data_out => pipelined_op_a);
        PIPELINE_REG_CHAIN_OP_A: simple_register_chain generic map (length => add_clk_cycles-2, width => word_width)
                                                       port map (clk => clk, reset => reset, enable => fixed_enable, reg_chain_in => pipelined_op_a, reg_chain_out => delayed_op_a);

        FIRST_PIPELINE_REG_CHAIN_OP_B: we_register generic map (reg_width => word_width)
                                                   port map (clk => clk, reset => reset, we => enable, data_in => op_b, data_out => pipelined_op_b);
        temp_op_b <= pipelined_sb & pipelined_op_b(30 downto 0);
        PIPELINE_REG_CHAIN_OP_B: simple_register_chain generic map (length => add_clk_cycles-2, width => word_width)
                                                                port map (clk => clk, reset => reset, enable => fixed_enable, reg_chain_in => temp_op_b, reg_chain_out => delayed_op_b);


        FIRST_FF_SIGNS_XOR: wedff port map (clk => clk, reset => reset, we => enable, d => signs_xor, q => pipelined_signs_xor);
        PIPELINE_FF_SIGNS_XOR: ff_chain generic map (length => add_clk_cycles-2)
                                                     port map (clk => clk, reset => reset, enable => fixed_enable, ff_chain_in => pipelined_signs_xor, ff_chain_out => delayed_signs_xor);
        SHORT_CHAIN_SIGNS_XOR: ff_chain generic map (length => 1)
                                                     port map (clk => clk, reset => reset, enable => fixed_enable, ff_chain_in => pipelined_signs_xor, ff_chain_out => previous_signs_xor);

-- special cases handling:

        SPECIAL_OP_DETECTION: process(delayed_op_a,delayed_op_b,internal_result,zero,unsigned_zero,infinity,sgl_overflow_sum,sgl_underflow_sum,sgl_inexact_sum,delayed_signs_xor)  
        begin
             
    -- (delayed_op_a is zero)      
            if ( delayed_op_a(30 downto 0) = unsigned_zero ) then
                    -- delayed_op_a is +0 or -0
                    if ( (delayed_signs_xor='1') and (delayed_op_a(30 downto 0)=delayed_op_b(30 downto 0)) ) then
                            add_result <= zero;
                    else
                            add_result <= delayed_op_b;
                    end if;
                    exc_overflow_add <= '0'; exc_underflow_add <= '0'; exc_inexact_add <= '0';
                    exc_invalid_operation_add <= '0';   
        
    -- (delayed_op_b is zero)
            elsif ( delayed_op_b(30 downto 0) = unsigned_zero ) then
                    -- delayed_op_b is +0 or -0
                    add_result <= delayed_op_a;
                    exc_overflow_add <= '0'; exc_underflow_add <= '0'; exc_inexact_add <= '0';
                    exc_invalid_operation_add <= '0';

-- Removed handling of +INF, -INF and NAN (optional in OpenCL Embedded) -Timo

    -- (delayed_op_a is positive infinity)
--            elsif ( (delayed_op_a(31)='0') and (delayed_op_a(30 downto 0)=infinity) and (delayed_op_b(31)='0') and (delayed_op_b(30 downto 0)=infinity) ) then
--                    -- both operands are positive infinity
--                    -- arithmetic on infinite operands is exact when they are valid, no overflow is raised
--                   add_result <= delayed_op_a;
--                    exc_overflow_add <= '0'; exc_underflow_add <= '0'; exc_inexact_add <= '0';
--                    exc_invalid_operation_add <= '0';
--
--            elsif ( (delayed_op_a(31)='0') and (delayed_op_a(30 downto 0)=infinity) and (delayed_op_b(31)='1') and (delayed_op_b(30 downto 0)=infinity) ) then
--                    -- positive infinity plus negative infinity: INVALID OPERATION!
--                    add_result <= "01111111110000000000000000000001";  -- QNaN, if trap disabled (default)
--                    exc_overflow_add <= '0'; exc_underflow_add <= '0'; exc_inexact_add <= '0';
--                    exc_invalid_operation_add <= '1';
--
--            elsif ( (delayed_op_a(31)='0') and (delayed_op_a(30 downto 0)=infinity) and (delayed_op_b(30 downto 0)/=unsigned_zero) and (delayed_op_b(30 downto 23)/="11111111") ) then
--                    -- positive infinity plus finite nonzero number
--                    -- arithmetic on infinite operands is exact when they are valid, no overflow is raised
--                   add_result <= delayed_op_a;
--                    exc_overflow_add <= '0'; exc_underflow_add <= '0'; exc_inexact_add <= '0';
--                    exc_invalid_operation_add <= '0';
--           
--    -- (delayed_op_b is positive infinity)
--            elsif ( (delayed_op_b(31)='0') and (delayed_op_b(30 downto 0)=infinity) and (delayed_op_a(31)='1') and (delayed_op_a(30 downto 0)=infinity) ) then
--                    -- positive infinity plus negative infinity: INVALID OPERATION!
--                    add_result <= "01111111110000000000000000000001";  -- QNaN, if trap disabled (default)
--                    exc_overflow_add <= '0'; exc_underflow_add <= '0'; exc_inexact_add <= '0';
--                    exc_invalid_operation_add <= '1';            
--
--            elsif ( (delayed_op_b(31)='0') and (delayed_op_b(30 downto 0)=infinity) and (delayed_op_a(30 downto 0)/=unsigned_zero) and (delayed_op_a(30 downto 23)/="11111111") ) then
--                    -- positive infinity plus finite nonzero number
--                    -- arithmetic on infinite operands is exact when they are valid, no overflow is raised
--                    add_result <= delayed_op_b;
--                    exc_overflow_add <= '0'; exc_underflow_add <= '0'; exc_inexact_add <= '0';
--                    exc_invalid_operation_add <= '0';
--
--    -- (delayed_op_a is negative infinity)
--           elsif ( (delayed_op_a(31)='1') and (delayed_op_a(30 downto 0)=infinity) and (delayed_op_b(31)='1') and (delayed_op_b(30 downto 0)=infinity) ) then
--                    -- both operands are negative infinity
--                    -- arithmetic on infinite operands is exact when they are valid, no overflow is raised
--                    add_result <= delayed_op_a;
--                    exc_overflow_add <= '0'; exc_underflow_add <= '0'; exc_inexact_add <= '0';
--                    exc_invalid_operation_add <= '0';
--
--            elsif ( (delayed_op_a(31)='1') and (delayed_op_a(30 downto 0)=infinity) and (delayed_op_b(30 downto 0)/=unsigned_zero) and (delayed_op_b(30 downto 23)/="11111111") ) then
--                    -- negative infinity plus finite nonzero number
--                    -- arithmetic on infinite operands is exact when they are valid, no overflow is raised
--                    add_result <= delayed_op_a;
--                    exc_overflow_add <= '0'; exc_underflow_add <= '0'; exc_inexact_add <= '0';
--                    exc_invalid_operation_add <= '0';
--
--    -- delayed_op_b is negative infinity:
--           elsif ( (delayed_op_b(31)='1') and (delayed_op_b(30 downto 0)=infinity) and (delayed_op_a(30 downto 0)/=unsigned_zero) and (delayed_op_a(30 downto 23)/="11111111") ) then
--                    -- negative infinity plus finite nonzero number
--                    -- arithmetic on infinite operands is exact when they are valid, no overflow is raised
--                    add_result <= delayed_op_b;
--                    exc_overflow_add <= '0'; exc_underflow_add <= '0'; exc_inexact_add <= '0';
--                    exc_invalid_operation_add <= '0';
--
--    -- at least one of the operands is a SNaN:
--            elsif ( ((delayed_op_a(30 downto 23)="11111111") and (delayed_op_a(22)='0') and (delayed_op_a(21 downto 0)/=conv_std_logic_vector(0,22)))
--                   or ((delayed_op_b(30 downto 23)="11111111") and (delayed_op_b(22)='0') and (delayed_op_b(21 downto 0)/=conv_std_logic_vector(0,22))) ) then
--                    -- at least one operand is a signaling NaN -> invalid operation exception is raised; output is a QNaN
--                    add_result <= "01111111110000000000000000000001";  -- QNaN, if trap disabled (default)
--                    exc_overflow_add <= '0'; exc_underflow_add <= '0'; exc_inexact_add <= '0';
--                    exc_invalid_operation_add <= '1';
--
--            elsif ( (delayed_op_a(30 downto 22)="111111111") or (delayed_op_b(30 downto 22)="111111111") ) then
--                    -- at least one operand is a quiet NaN; output is one of the input NaNs
--                    if ( delayed_op_a(30 downto 22)="111111111" ) then
--                            add_result <= delayed_op_a;  -- QNaN, if trap disabled (default)
--                    else
--                            add_result <= delayed_op_b;  -- QNaN, if trap disabled (default)
--                    end if;
--                    exc_overflow_add <= '0'; exc_underflow_add <= '0'; exc_inexact_add <= '0';
--                    exc_invalid_operation_add <= '0';

    -- operands have the same absolute value, but opposite sign:
            elsif ( (delayed_signs_xor='1') and (delayed_op_a(30 downto 0)=delayed_op_b(30 downto 0)) ) then
                    add_result <= zero;
                    exc_overflow_add <= '0'; exc_underflow_add <= '0'; exc_inexact_add <= '0';
                    exc_invalid_operation_add <= '0';
    
            else
    -- DEFAULT: the result and state is truely calculated by the following stages
                    add_result <= internal_result;
                    exc_overflow_add <= sgl_overflow_sum; exc_underflow_add <= sgl_underflow_sum; exc_inexact_add <= sgl_inexact_sum;
                    exc_invalid_operation_add <= '0';
            end if;

        end process;

----------------------------------         
-- OPERANDS ALIGNEMENT STAGE
----------------------------------

        PIPELINE_REG_XA: we_register generic map (reg_width => 9)
                                       port map (clk => clk, reset => reset, we => enable, data_in => xa, data_out => pipelined_xa);
        PIPELINE_REG_XB: we_register generic map (reg_width => 9)
                                       port map (clk => clk, reset => reset, we => enable, data_in => xb, data_out => pipelined_xb);
        PIPELINE_REG_MA: we_register generic map (reg_width => 24)
                                       port map (clk => clk, reset => reset, we => enable, data_in => ma, data_out => pipelined_ma);
        PIPELINE_REG_MB: we_register generic map (reg_width => 24)
                                       port map (clk => clk, reset => reset, we => enable, data_in => mb, data_out => pipelined_mb);


-- The operand with greater exponent is modified in order to make it aligned with the other one. Its exponent is equalized
-- to the value of the samller one, and its mantissa is shifted left in a proportional way: 

        process(pipelined_xa, pipelined_xb, pipelined_ma, pipelined_mb, shift_amount, xa_is_bigger, xb_is_bigger, xa_equal_xb, ma_is_bigger, mb_is_bigger)
        begin
            if ( unsigned(pipelined_xa) > unsigned(pipelined_xb) ) then
                    xa_is_bigger <= '1';
                    xb_is_bigger <= '0';
                    xa_equal_xb  <= '0';
                    ma_is_bigger <= '0'; 
                    mb_is_bigger <= '0';
                    
                    -- SHL(operand_a) in order to obtain pipelined_xa = pipelined_xb = xmin
                    shift_amount <= conv_integer( unsigned(pipelined_xa) - unsigned(pipelined_xb) );

                    if (shift_amount = 1) then
                            aligned_ma <= ( conv_std_logic_vector(0,23) & pipelined_ma & conv_std_logic_vector(0,1) );
                            aligned_mb <= ( conv_std_logic_vector(0,24) & pipelined_mb );
                            xmin <= pipelined_xb;
                            op_neglegted <= '0';
                    elsif (shift_amount = 2) then
                            aligned_ma <= ( conv_std_logic_vector(0,22) & pipelined_ma & conv_std_logic_vector(0,2) );
                            aligned_mb <= ( conv_std_logic_vector(0,24) & pipelined_mb );
                            xmin <= pipelined_xb;
                            op_neglegted <= '0';
                    elsif (shift_amount = 3) then
                            aligned_ma <= ( conv_std_logic_vector(0,21) & pipelined_ma & conv_std_logic_vector(0,3) );
                            aligned_mb <= ( conv_std_logic_vector(0,24) & pipelined_mb );
                            xmin <= pipelined_xb;
                            op_neglegted <= '0';
                    elsif (shift_amount = 4) then
                            aligned_ma <= ( conv_std_logic_vector(0,20) & pipelined_ma & conv_std_logic_vector(0,4) );
                            aligned_mb <= ( conv_std_logic_vector(0,24) & pipelined_mb );
                            xmin <= pipelined_xb;
                            op_neglegted <= '0';
                    elsif (shift_amount = 5) then
                            aligned_ma <= ( conv_std_logic_vector(0,19) & pipelined_ma & conv_std_logic_vector(0,5) );
                            aligned_mb <= ( conv_std_logic_vector(0,24) & pipelined_mb );
                            xmin <= pipelined_xb;
                            op_neglegted <= '0';
                    elsif (shift_amount = 6) then
                            aligned_ma <= ( conv_std_logic_vector(0,18) & pipelined_ma & conv_std_logic_vector(0,6) );
                            aligned_mb <= ( conv_std_logic_vector(0,24) & pipelined_mb );
                            xmin <= pipelined_xb;
                            op_neglegted <= '0';
                    elsif (shift_amount = 7) then
                            aligned_ma <= ( conv_std_logic_vector(0,17) & pipelined_ma & conv_std_logic_vector(0,7) );
                            aligned_mb <= ( conv_std_logic_vector(0,24) & pipelined_mb );
                            xmin <= pipelined_xb;
                            op_neglegted <= '0';
                    elsif (shift_amount = 8) then
                            aligned_ma <= ( conv_std_logic_vector(0,16) & pipelined_ma & conv_std_logic_vector(0,8) );
                            aligned_mb <= ( conv_std_logic_vector(0,24) & pipelined_mb );
                            xmin <= pipelined_xb;
                            op_neglegted <= '0';
                    elsif (shift_amount = 9) then
                            aligned_ma <= ( conv_std_logic_vector(0,15) & pipelined_ma & conv_std_logic_vector(0,9) );
                            aligned_mb <= ( conv_std_logic_vector(0,24) & pipelined_mb );
                            xmin <= pipelined_xb;
                            op_neglegted <= '0';
                    elsif (shift_amount = 10) then
                            aligned_ma <= ( conv_std_logic_vector(0,14) & pipelined_ma & conv_std_logic_vector(0,10) );
                            aligned_mb <= ( conv_std_logic_vector(0,24) & pipelined_mb );
                            xmin <= pipelined_xb;
                            op_neglegted <= '0';
                    elsif (shift_amount = 11) then
                            aligned_ma <= ( conv_std_logic_vector(0,13) & pipelined_ma & conv_std_logic_vector(0,11) );
                            aligned_mb <= ( conv_std_logic_vector(0,24) & pipelined_mb );
                            xmin <= pipelined_xb;
                            op_neglegted <= '0';
                    elsif (shift_amount = 12) then
                            aligned_ma <= ( conv_std_logic_vector(0,12) & pipelined_ma & conv_std_logic_vector(0,12) );
                            aligned_mb <= ( conv_std_logic_vector(0,24) & pipelined_mb );
                            xmin <= pipelined_xb;
                            op_neglegted <= '0';
                    elsif (shift_amount = 13) then
                            aligned_ma <= ( conv_std_logic_vector(0,11) & pipelined_ma & conv_std_logic_vector(0,13) );
                            aligned_mb <= ( conv_std_logic_vector(0,24) & pipelined_mb );
                            xmin <= pipelined_xb;
                            op_neglegted <= '0';
                    elsif (shift_amount = 14) then
                            aligned_ma <= ( conv_std_logic_vector(0,10) & pipelined_ma & conv_std_logic_vector(0,14) );
                            aligned_mb <= ( conv_std_logic_vector(0,24) & pipelined_mb );
                            xmin <= pipelined_xb;
                            op_neglegted <= '0';
                    elsif (shift_amount = 15) then
                            aligned_ma <= ( conv_std_logic_vector(0,9) & pipelined_ma & conv_std_logic_vector(0,15) );
                            aligned_mb <= ( conv_std_logic_vector(0,24) & pipelined_mb );
                            xmin <= pipelined_xb;
                            op_neglegted <= '0';
                    elsif (shift_amount = 16) then
                            aligned_ma <= ( conv_std_logic_vector(0,8) & pipelined_ma & conv_std_logic_vector(0,16) );
                            aligned_mb <= ( conv_std_logic_vector(0,24) & pipelined_mb );
                            xmin <= pipelined_xb;
                            op_neglegted <= '0';
                    elsif (shift_amount = 17) then
                            aligned_ma <= ( conv_std_logic_vector(0,7) & pipelined_ma & conv_std_logic_vector(0,17) );
                            aligned_mb <= ( conv_std_logic_vector(0,24) & pipelined_mb );
                            xmin <= pipelined_xb;
                            op_neglegted <= '0';
                    elsif (shift_amount = 18) then
                            aligned_ma <= ( conv_std_logic_vector(0,6) & pipelined_ma & conv_std_logic_vector(0,18) );
                            aligned_mb <= ( conv_std_logic_vector(0,24) & pipelined_mb );
                            xmin <= pipelined_xb;
                            op_neglegted <= '0';
                    elsif (shift_amount = 19) then
                            aligned_ma <= ( conv_std_logic_vector(0,5) & pipelined_ma & conv_std_logic_vector(0,19) );
                            aligned_mb <= ( conv_std_logic_vector(0,24) & pipelined_mb );
                            xmin <= pipelined_xb;
                            op_neglegted <= '0';
                    elsif (shift_amount = 20) then
                            aligned_ma <= ( conv_std_logic_vector(0,4) & pipelined_ma & conv_std_logic_vector(0,20) );
                            aligned_mb <= ( conv_std_logic_vector(0,24) & pipelined_mb );
                            xmin <= pipelined_xb;
                            op_neglegted <= '0';
                    elsif (shift_amount = 21) then
                            aligned_ma <= ( conv_std_logic_vector(0,3) & pipelined_ma & conv_std_logic_vector(0,21) );
                            aligned_mb <= ( conv_std_logic_vector(0,24) & pipelined_mb );
                            xmin <= pipelined_xb;
                            op_neglegted <= '0';
                    elsif (shift_amount = 22) then
                            aligned_ma <= ( conv_std_logic_vector(0,2) & pipelined_ma & conv_std_logic_vector(0,22) );
                            aligned_mb <= ( conv_std_logic_vector(0,24) & pipelined_mb );
                            xmin <= pipelined_xb;
                            op_neglegted <= '0';
                    elsif (shift_amount = 23) then
                            aligned_ma <= ( '0' & pipelined_ma & conv_std_logic_vector(0,23) );
                            aligned_mb <= ( conv_std_logic_vector(0,24) & pipelined_mb );
                            xmin <= pipelined_xb;
                            op_neglegted <= '0';
                    elsif (shift_amount = 24) then
                            aligned_ma <= ( pipelined_ma & conv_std_logic_vector(0,24) );
                            aligned_mb <= ( conv_std_logic_vector(0,24) & pipelined_mb );
                            xmin <= pipelined_xb;
                            op_neglegted <= '0';
                    else 
                            -- operand_b is so small that it can be neglected
                            aligned_ma <= ( conv_std_logic_vector(0,24) & pipelined_ma );
                            aligned_mb <= ( conv_std_logic_vector(0,48) );
                            xmin <= pipelined_xa;
                            op_neglegted <= '1';
                    end if;
            elsif ( unsigned(pipelined_xa) < unsigned(pipelined_xb) ) then
                    xa_is_bigger <= '0';
                    xb_is_bigger <= '1';
                    xa_equal_xb  <= '0';
                    ma_is_bigger <= '0'; 
                    mb_is_bigger <= '0';

                    -- SHL(operand_b) in order to obtain pipelined_xa = pipelined_xb = xmin
                    shift_amount <= conv_integer( unsigned(pipelined_xb) - unsigned(pipelined_xa) );

                    if (shift_amount = 1) then
                            aligned_mb <= ( conv_std_logic_vector(0,23) & pipelined_mb & conv_std_logic_vector(0,1) );
                            aligned_ma <= ( conv_std_logic_vector(0,24) & pipelined_ma );
                            xmin <= pipelined_xa;
                            op_neglegted <= '0';
                    elsif (shift_amount = 2) then
                            aligned_mb <= ( conv_std_logic_vector(0,22) & pipelined_mb & conv_std_logic_vector(0,2) );
                            aligned_ma <= ( conv_std_logic_vector(0,24) & pipelined_ma );
                            xmin <= pipelined_xa;
                            op_neglegted <= '0';
                    elsif (shift_amount = 3) then
                            aligned_mb <= ( conv_std_logic_vector(0,21) & pipelined_mb & conv_std_logic_vector(0,3) );
                            aligned_ma <= ( conv_std_logic_vector(0,24) & pipelined_ma );
                            xmin <= pipelined_xa;
                            op_neglegted <= '0';
                    elsif (shift_amount = 4) then
                            aligned_mb <= ( conv_std_logic_vector(0,20) & pipelined_mb & conv_std_logic_vector(0,4) );
                            aligned_ma <= ( conv_std_logic_vector(0,24) & pipelined_ma );
                            xmin <= pipelined_xa;
                            op_neglegted <= '0';
                    elsif (shift_amount = 5) then
                            aligned_mb <= ( conv_std_logic_vector(0,19) & pipelined_mb & conv_std_logic_vector(0,5) );
                            aligned_ma <= ( conv_std_logic_vector(0,24) & pipelined_ma );
                            xmin <= pipelined_xa;
                            op_neglegted <= '0';
                    elsif (shift_amount = 6) then
                            aligned_mb <= ( conv_std_logic_vector(0,18) & pipelined_mb & conv_std_logic_vector(0,6) );
                            aligned_ma <= ( conv_std_logic_vector(0,24) & pipelined_ma );
                            xmin <= pipelined_xa;
                            op_neglegted <= '0';
                    elsif (shift_amount = 7) then
                            aligned_mb <= ( conv_std_logic_vector(0,17) & pipelined_mb & conv_std_logic_vector(0,7) );
                            aligned_ma <= ( conv_std_logic_vector(0,24) & pipelined_ma );
                            xmin <= pipelined_xa;
                            op_neglegted <= '0';
                    elsif (shift_amount = 8) then
                            aligned_mb <= ( conv_std_logic_vector(0,16) & pipelined_mb & conv_std_logic_vector(0,8) );
                            aligned_ma <= ( conv_std_logic_vector(0,24) & pipelined_ma );
                            xmin <= pipelined_xa;
                            op_neglegted <= '0';
                    elsif (shift_amount = 9) then
                            aligned_mb <= ( conv_std_logic_vector(0,15) & pipelined_mb & conv_std_logic_vector(0,9) );
                            aligned_ma <= ( conv_std_logic_vector(0,24) & pipelined_ma );
                            xmin <= pipelined_xa;
                            op_neglegted <= '0';
                    elsif (shift_amount = 10) then
                            aligned_mb <= ( conv_std_logic_vector(0,14) & pipelined_mb & conv_std_logic_vector(0,10) );
                            aligned_ma <= ( conv_std_logic_vector(0,24) & pipelined_ma );
                            xmin <= pipelined_xa;
                            op_neglegted <= '0';
                    elsif (shift_amount = 11) then
                            aligned_mb <= ( conv_std_logic_vector(0,13) & pipelined_mb & conv_std_logic_vector(0,11) );
                            aligned_ma <= ( conv_std_logic_vector(0,24) & pipelined_ma );
                            xmin <= pipelined_xa;
                            op_neglegted <= '0';
                    elsif (shift_amount = 12) then
                            aligned_mb <= ( conv_std_logic_vector(0,12) & pipelined_mb & conv_std_logic_vector(0,12) );
                            aligned_ma <= ( conv_std_logic_vector(0,24) & pipelined_ma );
                            xmin <= pipelined_xa;
                            op_neglegted <= '0';
                    elsif (shift_amount = 13) then
                            aligned_mb <= ( conv_std_logic_vector(0,11) & pipelined_mb & conv_std_logic_vector(0,13) );
                            aligned_ma <= ( conv_std_logic_vector(0,24) & pipelined_ma );
                            xmin <= pipelined_xa;
                            op_neglegted <= '0';
                    elsif (shift_amount = 14) then
                            aligned_mb <= ( conv_std_logic_vector(0,10) & pipelined_mb & conv_std_logic_vector(0,14) );
                            aligned_ma <= ( conv_std_logic_vector(0,24) & pipelined_ma );
                            xmin <= pipelined_xa;
                            op_neglegted <= '0';
                    elsif (shift_amount = 15) then
                            aligned_mb <= ( conv_std_logic_vector(0,9) & pipelined_mb & conv_std_logic_vector(0,15) );
                            aligned_ma <= ( conv_std_logic_vector(0,24) & pipelined_ma );
                            xmin <= pipelined_xa;
                            op_neglegted <= '0';
                    elsif (shift_amount = 16) then
                            aligned_mb <= ( conv_std_logic_vector(0,8) & pipelined_mb & conv_std_logic_vector(0,16) );
                            aligned_ma <= ( conv_std_logic_vector(0,24) & pipelined_ma );
                            xmin <= pipelined_xa;
                            op_neglegted <= '0';
                    elsif (shift_amount = 17) then
                            aligned_mb <= ( conv_std_logic_vector(0,7) & pipelined_mb & conv_std_logic_vector(0,17) );
                            aligned_ma <= ( conv_std_logic_vector(0,24) & pipelined_ma );
                            xmin <= pipelined_xa;
                            op_neglegted <= '0';
                    elsif (shift_amount = 18) then
                            aligned_mb <= ( conv_std_logic_vector(0,6) & pipelined_mb & conv_std_logic_vector(0,18) );
                            aligned_ma <= ( conv_std_logic_vector(0,24) & pipelined_ma );
                            xmin <= pipelined_xa;
                            op_neglegted <= '0';
                    elsif (shift_amount = 19) then
                            aligned_mb <= ( conv_std_logic_vector(0,5) & pipelined_mb & conv_std_logic_vector(0,19) );
                            aligned_ma <= ( conv_std_logic_vector(0,24) & pipelined_ma );
                            xmin <= pipelined_xa;
                            op_neglegted <= '0';
                    elsif (shift_amount = 20) then
                            aligned_mb <= ( conv_std_logic_vector(0,4) & pipelined_mb & conv_std_logic_vector(0,20) );
                            aligned_ma <= ( conv_std_logic_vector(0,24) & pipelined_ma );
                            xmin <= pipelined_xa;
                            op_neglegted <= '0';
                    elsif (shift_amount = 21) then
                            aligned_mb <= ( conv_std_logic_vector(0,3) & pipelined_mb & conv_std_logic_vector(0,21) );
                            aligned_ma <= ( conv_std_logic_vector(0,24) & pipelined_ma );
                            xmin <= pipelined_xa;
                            op_neglegted <= '0';
                    elsif (shift_amount = 22) then
                            aligned_mb <= ( conv_std_logic_vector(0,2) & pipelined_mb & conv_std_logic_vector(0,22) );
                            aligned_ma <= ( conv_std_logic_vector(0,24) & pipelined_ma );
                            xmin <= pipelined_xa;
                            op_neglegted <= '0';
                    elsif (shift_amount = 23) then
                            aligned_mb <= ( '0' & pipelined_mb & conv_std_logic_vector(0,23) );
                            aligned_ma <= ( conv_std_logic_vector(0,24) & pipelined_ma );
                            xmin <= pipelined_xa;
                            op_neglegted <= '0';
                    elsif (shift_amount = 24) then
                            aligned_mb <= ( pipelined_mb & conv_std_logic_vector(0,24) );
                            aligned_ma <= ( conv_std_logic_vector(0,24) & pipelined_ma );
                            xmin <= pipelined_xa;
                            op_neglegted <= '0';
                    else 
                            -- operand_a is so small that it can be neglected
                            aligned_mb <= ( conv_std_logic_vector(0,24) & pipelined_mb );
                            aligned_ma <= ( conv_std_logic_vector(0,48) );
                            xmin <= pipelined_xb;
                            op_neglegted <= '1';
                    end if;
            else
                    xa_is_bigger <= '0';
                    xb_is_bigger <= '0';
                    xa_equal_xb  <= '1';

                    -- operands are already aligned
                    shift_amount <= 0;

                    -- mantissas might be swapped to allow the smaller one to be subtracted from the bigger one
                    if ( unsigned(pipelined_ma) > unsigned(pipelined_mb) ) then
                            ma_is_bigger <= '1';
                            mb_is_bigger <= '0';
                            aligned_ma <= ( conv_std_logic_vector(0,24) & pipelined_ma );
                            aligned_mb <= ( conv_std_logic_vector(0,24) & pipelined_mb );
                    else
                            ma_is_bigger <= '0'; 
                            mb_is_bigger <= '1';
                            aligned_ma <= ( conv_std_logic_vector(0,24) & pipelined_mb );
                            aligned_mb <= ( conv_std_logic_vector(0,24) & pipelined_ma );
                    end if;

                    xmin <= pipelined_xa;
                    op_neglegted <= '0';
            end if;
        end process;


        PIPELINE_REG_ALIGNED_MA: data_register generic map (reg_width => 48)
                                               port map (clk => clk, reset => reset, data_in => aligned_ma, data_out => pipelined_aligned_ma);
        PIPELINE_REG_ALIGNED_MB: data_register generic map (reg_width => 48)
                                               port map (clk => clk, reset => reset, data_in => aligned_mb, data_out => pipelined_aligned_mb);

        PIPELINE_CHAIN_XMIN: simple_register_chain generic map (length => 2, width => 9)
                                                 port map (clk => clk, reset => reset, enable => fixed_enable, reg_chain_in => xmin, reg_chain_out => delayed_xmin);


----------------------------------         
-- EXTENDED SUM CALCULATION
----------------------------------

-- fractional parts sum

        integer_add <= ( unsigned(pipelined_aligned_ma) + unsigned(pipelined_aligned_mb) );


-- fractional parts subtraction

        temp_integer_sub <= ( unsigned(pipelined_aligned_ma) - unsigned(pipelined_aligned_mb) ) ;
        integer_sub <= abs( signed(temp_integer_sub) );


-- sum/sub selection

        process (previous_signs_xor, integer_add, integer_sub)
        begin
          
            if (previous_signs_xor = '0') then
                    integer_result <= integer_add;
            else
                    integer_result <= integer_sub;
            end if;
            
        end process;


        PIPELINE_REG_INTEGER_RESULT: data_register generic map (reg_width => 48)
                                                   port map (clk => clk, reset => reset, data_in => integer_result, data_out => pipelined_integer_result);


------------------------
-- NORMALIZATION STAGE 
------------------------

-- partial result normalization
-- Note: since the operands has been implicitly multiplied by a 2^-23 factor, their sum will be implicitly 
--       multiplied by a 2^23 factor to restore the correct value. Anyway, this moves the implicit fraction
--       dot between bits 23 and 22 of the 48-bit mantissas sum (extended_add) so that bits 22 downto 0 
--       are the "fractional part"; normalization process is intended to move the fraction point just after
--       bit 47 in order to make it to be the leading bit. thus the exponent has to be incremented by 24.
--       Anyway, exponent adjustment is also required following possible "SHL" of extended_add: the exponent 
--       should be decremented by one if leading bit is extended_add(46), by two if leading bit is
--       extended_add(45), and so on; this way, it remains untouched whenever extended_prod(23) is the 
--       leading bit.

    process(pipelined_integer_result,delayed_xmin) 
    begin
        if ( (pipelined_integer_result(47 downto 1) = conv_std_logic_vector(0,47)) and (pipelined_integer_result(0)='1') ) then
                -- extended_add(0) is the leading bit; exp <- (exp - 23)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin) - 23 ), 9);
                normalized_add <= ( pipelined_integer_result(0) & conv_std_logic_vector(0,47) );

        elsif ( (pipelined_integer_result(47 downto 2) = conv_std_logic_vector(0,46)) and (pipelined_integer_result(1)='1') ) then
                -- extended_add(1) is the leading bit; exp <- (exp - 22)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin) - 22 ), 9);
                normalized_add <= ( pipelined_integer_result(1 downto 0) & conv_std_logic_vector(0,46) );

        elsif ( (pipelined_integer_result(47 downto 3) = conv_std_logic_vector(0,45)) and (pipelined_integer_result(2)='1') ) then
                -- extended_add(2) is the leading bit; exp <- (exp - 21)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin) - 21 ), 9);
                normalized_add <= ( pipelined_integer_result(2 downto 0) & conv_std_logic_vector(0,45) );

        elsif ( (pipelined_integer_result(47 downto 4) = conv_std_logic_vector(0,44)) and (pipelined_integer_result(3)='1') ) then
                -- extended_add(3) is the leading bit; exp <- (exp - 20)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin) - 20 ), 9);
                normalized_add <= ( pipelined_integer_result(3 downto 0) & conv_std_logic_vector(0,44) );

        elsif ( (pipelined_integer_result(47 downto 5) = conv_std_logic_vector(0,43)) and (pipelined_integer_result(4)='1') ) then
                -- extended_add(4) is the leading bit; exp <- (exp - 19)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin) - 19 ), 9);
                normalized_add <= ( pipelined_integer_result(4 downto 0) & conv_std_logic_vector(0,43) );  

        elsif ( (pipelined_integer_result(47 downto 6) = conv_std_logic_vector(0,42)) and (pipelined_integer_result(5)='1') ) then
                -- extended_add(5) is the leading bit; exp <- (exp - 18)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin) - 18 ), 9);
                normalized_add <= ( pipelined_integer_result(5 downto 0) & conv_std_logic_vector(0,42) );

        elsif ( (pipelined_integer_result(47 downto 7) = conv_std_logic_vector(0,41)) and (pipelined_integer_result(6)='1') ) then
                -- extended_add(6) is the leading bit; exp <- (exp - 17)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin) - 17 ), 9);
                normalized_add <= ( pipelined_integer_result(6 downto 0) & conv_std_logic_vector(0,41) );

        elsif ( (pipelined_integer_result(47 downto 8) = conv_std_logic_vector(0,40)) and (pipelined_integer_result(7)='1') ) then
                -- extended_add(7) is the leading bit; exp <- (exp - 16)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin) - 16 ), 9);
                normalized_add <= ( pipelined_integer_result(7 downto 0) & conv_std_logic_vector(0,40) );

        elsif ( (pipelined_integer_result(47 downto 9) = conv_std_logic_vector(0,39)) and (pipelined_integer_result(8)='1') ) then
                -- extended_add(8) is the leading bit; exp <- (exp - 15)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin) - 15 ), 9);
                normalized_add <= ( pipelined_integer_result(8 downto 0) & conv_std_logic_vector(0,39) );

        elsif ( (pipelined_integer_result(47 downto 10) = conv_std_logic_vector(0,38)) and (pipelined_integer_result(9)='1') ) then
                -- extended_add(9) is the leading bit; exp <- (exp - 14)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin) - 14 ), 9);
                normalized_add <= ( pipelined_integer_result(9 downto 0) & conv_std_logic_vector(0,38) );

        elsif ( (pipelined_integer_result(47 downto 11) = conv_std_logic_vector(0,37)) and (pipelined_integer_result(10)='1') ) then
                -- extended_add(10) is the leading bit; exp <- (exp - 13)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin) - 13 ), 9);
                normalized_add <= ( pipelined_integer_result(10 downto 0) & conv_std_logic_vector(0,37) );

        elsif ( (pipelined_integer_result(47 downto 12) = conv_std_logic_vector(0,36)) and (pipelined_integer_result(11)='1') ) then
                -- extended_add(11) is the leading bit; exp <- (exp - 12)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin) - 12 ), 9);
                normalized_add <= ( pipelined_integer_result(11 downto 0) & conv_std_logic_vector(0,36) );

        elsif ( (pipelined_integer_result(47 downto 13) = conv_std_logic_vector(0,35)) and (pipelined_integer_result(12)='1') ) then
                -- extended_add(12) is the leading bit; exp <- (exp - 11)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin) - 11 ), 9);
                normalized_add <= ( pipelined_integer_result(12 downto 0) & conv_std_logic_vector(0,35) );

        elsif ( (pipelined_integer_result(47 downto 14) = conv_std_logic_vector(0,34)) and (pipelined_integer_result(13)='1') ) then
                -- extended_add(13) is the leading bit; exp <- (exp - 10)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin) - 10 ), 9);
                normalized_add <= ( pipelined_integer_result(13 downto 0) & conv_std_logic_vector(0,34) );

        elsif ( (pipelined_integer_result(47 downto 15) = conv_std_logic_vector(0,33)) and (pipelined_integer_result(14)='1') ) then
                -- extended_add(14) is the leading bit; exp <- (exp - 9)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin) - 9 ), 9);
                normalized_add <= ( pipelined_integer_result(14 downto 0) & conv_std_logic_vector(0,33) );

        elsif ( (pipelined_integer_result(47 downto 16) = conv_std_logic_vector(0,32)) and (pipelined_integer_result(15)='1') ) then
                -- extended_add(15) is the leading bit; exp <- (exp - 8)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin) - 8 ), 9);
                normalized_add <= ( pipelined_integer_result(15 downto 0) & conv_std_logic_vector(0,32) );

        elsif ( (pipelined_integer_result(47 downto 17) = conv_std_logic_vector(0,31)) and (pipelined_integer_result(16)='1') ) then
                -- extended_add(16) is the leading bit; exp <- (exp - 7)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin) - 7 ), 9);
                normalized_add <= ( pipelined_integer_result(16 downto 0) & conv_std_logic_vector(0,31) );

        elsif ( (pipelined_integer_result(47 downto 18) = conv_std_logic_vector(0,30)) and (pipelined_integer_result(17)='1') ) then
                -- extended_add(17) is the leading bit; exp <- (exp - 6)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin) - 6 ), 9);
                normalized_add <= ( pipelined_integer_result(17 downto 0) & conv_std_logic_vector(0,30) ); 

        elsif ( (pipelined_integer_result(47 downto 19) = conv_std_logic_vector(0,29)) and (pipelined_integer_result(18)='1') ) then
                -- extended_add(18) is the leading bit; exp <- (exp - 5)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin) - 5 ), 9);
                normalized_add <= ( pipelined_integer_result(18 downto 0) & conv_std_logic_vector(0,29) ); 

        elsif ( (pipelined_integer_result(47 downto 20) = conv_std_logic_vector(0,28)) and (pipelined_integer_result(19)='1') ) then
                -- extended_add(19) is the leading bit; exp <- (exp - 4)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin) - 4 ), 9);
                normalized_add <= ( pipelined_integer_result(19 downto 0) & conv_std_logic_vector(0,28) ); 

        elsif ( (pipelined_integer_result(47 downto 21) = conv_std_logic_vector(0,27)) and (pipelined_integer_result(20)='1') ) then
                -- extended_add(20) is the leading bit; exp <- (exp - 3)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin) - 3 ), 9);
                normalized_add <= ( pipelined_integer_result(20 downto 0) & conv_std_logic_vector(0,27) ); 

        elsif ( (pipelined_integer_result(47 downto 22) = conv_std_logic_vector(0,26)) and (pipelined_integer_result(21)='1') ) then
                -- extended_add(21) is the leading bit; exp <- (exp - 2)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin) - 2 ), 9);
                normalized_add <= ( pipelined_integer_result(21 downto 0) & conv_std_logic_vector(0,26) );  

        elsif ( (pipelined_integer_result(47 downto 23) = conv_std_logic_vector(0,25)) and (pipelined_integer_result(22)='1') ) then
                -- extended_add(22) is the leading bit; exp <- (exp - 1)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin) - 1 ), 9);
                normalized_add <= ( pipelined_integer_result(22 downto 0) & conv_std_logic_vector(0,25) );  

        elsif ( (pipelined_integer_result(47 downto 24) = conv_std_logic_vector(0,24)) and (pipelined_integer_result(23)='1') ) then
                -- extended_add(23) is the leading bit => SHL(pipelined_integer_result), exp <- (exp - 24).  
                -- Anyway 24 should be also added following fraction dot moving, so that globally
                -- the exponent is not modified
                normalized_xmin <= delayed_xmin;
                normalized_add <= ( pipelined_integer_result(23 downto 0) & conv_std_logic_vector(0,24) );

        elsif ( (pipelined_integer_result(47 downto 25) = conv_std_logic_vector(0,23)) and (pipelined_integer_result(24)='1') ) then
                -- pipelined_integer_result(24) is the leading bit; exp <- (exp + 1)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin)+ 1 ), 9);
                normalized_add <= ( pipelined_integer_result(24 downto 0) & conv_std_logic_vector(0,23) );  

        elsif ( (pipelined_integer_result(47 downto 26) = conv_std_logic_vector(0,22)) and (pipelined_integer_result(25)='1') ) then
                -- extended_add(25) is the leading bit; exp <- (exp + 2)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin)+ 2 ), 9);
                normalized_add <= ( pipelined_integer_result(25 downto 0) & conv_std_logic_vector(0,22) ); 

        elsif ( (pipelined_integer_result(47 downto 27) = conv_std_logic_vector(0,21)) and (pipelined_integer_result(26)='1') ) then
                -- extended_add(26) is the leading bit; exp <- (exp + 3)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin)+ 3 ), 9);
                normalized_add <= ( pipelined_integer_result(26 downto 0) & conv_std_logic_vector(0,21) ); 

        elsif ( (pipelined_integer_result(47 downto 28) = conv_std_logic_vector(0,20)) and (pipelined_integer_result(27)='1') ) then
                -- extended_add(27) is the leading bit; exp <- (exp + 4)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin)+ 4 ), 9);
                normalized_add <= ( pipelined_integer_result(27 downto 0) & conv_std_logic_vector(0,20) ); 

        elsif ( (pipelined_integer_result(47 downto 29) = conv_std_logic_vector(0,19)) and (pipelined_integer_result(28)='1') ) then
                -- extended_add(28) is the leading bit; exp <- (exp + 5)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin)+ 5 ), 9);
                normalized_add <= ( pipelined_integer_result(28 downto 0) & conv_std_logic_vector(0,19) ); 

        elsif ( (pipelined_integer_result(47 downto 30) = conv_std_logic_vector(0,18)) and (pipelined_integer_result(29)='1') ) then
                -- extended_add(29) is the leading bit; exp <- (exp + 6)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin)+ 6 ), 9);
                normalized_add <= ( pipelined_integer_result(29 downto 0) & conv_std_logic_vector(0,18) ); 

        elsif ( (pipelined_integer_result(47 downto 31) = conv_std_logic_vector(0,17)) and (pipelined_integer_result(30)='1') ) then
                -- extended_add(30) is the leading bit; exp <- (exp + 7)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin)+ 7 ), 9);
                normalized_add <= ( pipelined_integer_result(30 downto 0) & conv_std_logic_vector(0,17) ); 

        elsif ( (pipelined_integer_result(47 downto 32) = conv_std_logic_vector(0,16)) and (pipelined_integer_result(31)='1') ) then
                -- extended_add(31) is the leading bit; exp <- (exp + 8)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin)+ 8 ), 9);
                normalized_add <= ( pipelined_integer_result(31 downto 0) & conv_std_logic_vector(0,16) ); 

        elsif ( (pipelined_integer_result(47 downto 33) = conv_std_logic_vector(0,15)) and (pipelined_integer_result(32)='1') ) then
                -- extended_add(32) is the leading bit; exp <- (exp + 9)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin)+ 9 ), 9);
                normalized_add <= ( pipelined_integer_result(32 downto 0) & conv_std_logic_vector(0,15) ); 

        elsif ( (pipelined_integer_result(47 downto 34) = conv_std_logic_vector(0,14)) and (pipelined_integer_result(33)='1') ) then
                -- extended_add(33) is the leading bit; exp <- (exp + 10)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin)+ 10 ), 9);
                normalized_add <= ( pipelined_integer_result(33 downto 0) & conv_std_logic_vector(0,14) ); 

        elsif ( (pipelined_integer_result(47 downto 35) = conv_std_logic_vector(0,13)) and (pipelined_integer_result(34)='1') ) then
                -- extended_add(34) is the leading bit; exp <- (exp + 11)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin)+ 11 ), 9);
                normalized_add <= ( pipelined_integer_result(34 downto 0) & conv_std_logic_vector(0,13) );

        elsif ( (pipelined_integer_result(47 downto 36) = conv_std_logic_vector(0,12)) and (pipelined_integer_result(35)='1') ) then
                -- extended_add(35) is the leading bit; exp <- (exp + 12)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin)+ 12 ), 9);
                normalized_add <= ( pipelined_integer_result(35 downto 0) & conv_std_logic_vector(0,12) ); 
        
        elsif ( (pipelined_integer_result(47 downto 37) = conv_std_logic_vector(0,11)) and (pipelined_integer_result(36)='1') ) then
                -- extended_add(36) is the leading bit; exp <- (exp + 13)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin)+ 13 ), 9);
                normalized_add <= ( pipelined_integer_result(36 downto 0) & conv_std_logic_vector(0,11) ); 

        elsif ( (pipelined_integer_result(47 downto 38) = conv_std_logic_vector(0,10)) and (pipelined_integer_result(37)='1') ) then
                -- extended_add(37) is the leading bit; exp <- (exp + 14)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin)+ 14 ), 9);
                normalized_add <= ( pipelined_integer_result(37 downto 0) & conv_std_logic_vector(0,10) ); 

        elsif ( (pipelined_integer_result(47 downto 39) = conv_std_logic_vector(0,9)) and (pipelined_integer_result(38)='1') ) then
                -- extended_add(38) is the leading bit; exp <- (exp + 15)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin)+ 15 ), 9);
                normalized_add <= ( pipelined_integer_result(38 downto 0) & conv_std_logic_vector(0,9) ); 

        elsif ( (pipelined_integer_result(47 downto 40) = conv_std_logic_vector(0,8)) and (pipelined_integer_result(39)='1') ) then
                -- extended_add(39) is the leading bit; exp <- (exp + 16)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin)+ 16 ), 9);
                normalized_add <= ( pipelined_integer_result(39 downto 0) & conv_std_logic_vector(0,8) ); 

        elsif ( (pipelined_integer_result(47 downto 41) = conv_std_logic_vector(0,7)) and (pipelined_integer_result(40)='1') ) then
                -- extended_add(40) is the leading bit; exp <- (exp + 17)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin)+ 17 ), 9);
                normalized_add <= ( pipelined_integer_result(40 downto 0) & conv_std_logic_vector(0,7) ); 

        elsif ( (pipelined_integer_result(47 downto 42) = conv_std_logic_vector(0,6)) and (pipelined_integer_result(41)='1') ) then
                -- extended_add(41) is the leading bit; exp <- (exp + 18)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin)+ 18 ), 9);
                normalized_add <= ( pipelined_integer_result(41 downto 0) & conv_std_logic_vector(0,6) ); 

        elsif ( (pipelined_integer_result(47 downto 43) = conv_std_logic_vector(0,5)) and (pipelined_integer_result(42)='1') ) then
                -- extended_add(42) is the leading bit; exp <- (exp + 19)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin)+ 19 ), 9);
                normalized_add <= ( pipelined_integer_result(42 downto 0) & conv_std_logic_vector(0,5) ); 

        elsif ( (pipelined_integer_result(47 downto 44) = conv_std_logic_vector(0,4)) and (pipelined_integer_result(43)='1') ) then
                -- extended_add(43) is the leading bit; exp <- (exp + 20)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin)+ 20 ), 9);
                normalized_add <= ( pipelined_integer_result(43 downto 0) & conv_std_logic_vector(0,4) ); 

        elsif ( (pipelined_integer_result(47 downto 45) = conv_std_logic_vector(0,3)) and (pipelined_integer_result(44)='1') ) then
                -- extended_add(44) is the leading bit; exp <- (exp + 21)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin)+ 21 ), 9);
                normalized_add <= ( pipelined_integer_result(44 downto 0) & conv_std_logic_vector(0,3) ); 

        elsif ( (pipelined_integer_result(47 downto 46) = conv_std_logic_vector(0,2)) and (pipelined_integer_result(45)='1') ) then
                -- extended_add(25) is the leading bit; exp <- (exp + 22)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin)+ 22 ), 9);
                normalized_add <= ( pipelined_integer_result(45 downto 0) & conv_std_logic_vector(0,2) ); 

        elsif ( (pipelined_integer_result(47) = '0' and (pipelined_integer_result(46)='1')) ) then
                -- extended_add(46) is the leading bit; exp <- (exp + 23)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin)+ 23 ), 9);
                normalized_add <= ( pipelined_integer_result(46 downto 0) & conv_std_logic_vector(0,1) ); 

        elsif (pipelined_integer_result(47) = '1') then
                -- pipelined_integer_result(47) is the leading bit (already normalized); exp <- (exp + 24)
                normalized_xmin <= conv_std_logic_vector( (unsigned(delayed_xmin)+ 24 ), 9);
                normalized_add <= pipelined_integer_result; 
        else 
                normalized_xmin <= conv_std_logic_vector(0,9);
                normalized_add <= conv_std_logic_vector(0,48);             
        end if;
    end process;            

-----------------------------
-- RESULT GENERATION STAGE 
-----------------------------

        PIPELINE_REG_NORMALIZED_ADD: data_register generic map (reg_width => 48)
                                                   port map (clk => clk, reset => reset, data_in => normalized_add, data_out => pipelined_normalized_add);
        PIPELINE_REG_NORMALIZED_XMIN: data_register generic map (reg_width => 9)
                                               port map (clk => clk, reset => reset, data_in => normalized_xmin, data_out => pipelined_normalized_xmin);



        RESULT_PACKING_LOGIC: add_packer 
                              port map (internal_exponent => pipelined_normalized_xmin, internal_mantissa => pipelined_normalized_add, 
                                        packed_exponent => sgl_packed_exponent, packed_fractional_part => sgl_packed_fractional_part,
                                        exc_overflow => sgl_overflow_sum, exc_underflow => sgl_underflow_sum, exc_inexact => sgl_exc_inexact);


        sgl_inexact_sum <= ( sgl_exc_inexact or op_neglegted);

-- final result assembling:

        internal_result <= ( delayed_s & sgl_packed_exponent & sgl_packed_fractional_part);
 
-----------------------------------------------------------------------------------------------------------------       

end rtl;

