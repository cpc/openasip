------------------------------------------------------------------------------------------------
-- Copyright (c) 2011 Tom M. Bruintjes
-- All rights reserved.

-- Redistribution and use in source and binary forms, with or without 
-- modification, are permitted provided that the following conditions are met:
--     * Redistributions of source code must retain the above copyright
--       notice, this list of conditions and the following disclaimer.
--     * Redistributions in binary form must reproduce the above copyright
--       notice, this list of conditions and the following disclaimer in the
--       documentation and/or other materials provided with the distribution.
--     * Neither the name of the copyright holder nor the
--       names of its contributors may be used to endorse or promote products
--       derived from this software without specific prior written permission.

-- THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER ``AS IS'' AND ANY
-- EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
-- IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
-- PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE
-- LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
-- CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
-- SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
-- BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
-- WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
-- OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
-- IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
-- ------------------------------------------------------------------------------------------------
-- ------------------------------------------------------------------------------------------------
--  Instances of components instantiated from the SynopSys DesignWare Library:
--  http://www.synopsys.com/IP/SOCInfrastructureIP/DesignWare/Pages/default.aspx
-- ------------------------------------------------------------------------------------------------

--  Partial product multiplier.  Multiplies (A*B) and returns a partial result consisting of a sum
--  and carries.  After adding the partial results, a full multiplications has been performed. The
--  partial products are now used in a  carry-save adder to elegantly perform multiply-accumulate.
--  The  partial product multiplier  is a  Synopsys DesignWare  DW_02 component.  More information
--  can be found at: https://www.synopsys.com/dw/doc.php/doc/dwf/datasheets/dw02_multp.pdf
library ieee;
use ieee.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
use work.alu_pkg_lvl2.all;

entity multpp is
port(
  a_in    : in  std_logic_vector(SIGNIFICANDWIDTH+HIDDENBIT-1 downto 0);
  b_in    : in  std_logic_vector(SIGNIFICANDWIDTH+HIDDENBIT-1 downto 0);
  tc_in   : in  std_logic; -- selects between signed and unsigned operation
  ppl_out : out std_logic_vector(2*(SIGNIFICANDWIDTH+HIDDENBIT)+GUARDBITS-1 downto 0);
  ppr_out : out std_logic_vector(2*(SIGNIFICANDWIDTH+HIDDENBIT)+GUARDBITS-1 downto 0)
  -- The partial products from this DW component are always signed (even when the performing
  -- unsigned multiplication) and are two bits wider than (two times the input size).
);
end multpp;

--  Carry-save adder (CSA). Adds sum and carry from the partial product multiplier to  the addend C
--  ((A*B)+C).  Returns a redundant  carry-save format  (sum and carries)  like the partial product
--  multiplier.  The carry-save  adder does  not involve a  carry  propagation  which significantly
--  reduces  its  latency  compared to a full  carry-propagation  adder.  This  makes it ideal  for
--  multiply-accumulate.  The sum and carries still have to be added in order to perform a complete
--  addition.  The  carry-save adder  is a  SynopSys DesignWare  DW_01 component.  More information
--  can be found at: https://www.synopsys.com/dw/doc.php/doc/dwf/datasheets/dw01_csa.pdf
library ieee;
use ieee.std_logic_1164.all;
use work.alu_pkg_lvl2.all;

entity csa is
port(a_in      : in  std_logic_vector(3*(SIGNIFICANDWIDTH+HIDDENBIT)+GUARDBITS downto 0);
     b_in      : in  std_logic_vector(3*(SIGNIFICANDWIDTH+HIDDENBIT)+GUARDBITS downto 0);
     c_in      : in  std_logic_vector(3*(SIGNIFICANDWIDTH+HIDDENBIT)+GUARDBITS downto 0);
     ci_in     : in  std_logic;
     sum_out   : out std_logic_vector(3*(SIGNIFICANDWIDTH+HIDDENBIT)+GUARDBITS downto 0);
     carry_out : out std_logic_vector(3*(SIGNIFICANDWIDTH+HIDDENBIT)+GUARDBITS downto 0);
     co_out    : out std_logic
	 );
end csa;

--  Carry-propagate adder.  After the CSA,  the carry and sum need to be added  with a conventional
--  carry-propagate adder.  This can be a ripple-carry adder, a carry-look-ahead adder or any other
--  convenient implementation that propagates the carry.  A DesignWare component is instantiated to
--  be  able to easily  access the carry-out of the adder. More information can be found at:
--  https://www.synopsys.com/dw/doc.php/doc/dwf/datasheets/dw01_add.pdf
library ieee;
use ieee.std_logic_1164.all;
use work.alu_pkg_lvl2.all;

entity add is
port(
  a_in    : in  std_logic_vector(3*(SIGNIFICANDWIDTH+HIDDENBIT)+GUARDBITS downto 0);
  b_in    : in  std_logic_vector(3*(SIGNIFICANDWIDTH+HIDDENBIT)+GUARDBITS downto 0);
  ci_in   : in  std_logic;
  sum_out : out std_logic_vector(3*(SIGNIFICANDWIDTH+HIDDENBIT)+GUARDBITS downto 0);
  co_out  : out std_logic
);
end add;