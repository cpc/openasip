library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

package base is
	
	constant dataw : integer := 32;

	-- logical
	procedure tce_and		(variable i1, i2 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0));
	procedure tce_or		(variable i1, i2 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0));
	procedure tce_ior		(variable i1, i2 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0));
	procedure tce_xor		(variable i1, i2 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0));
	
	-- extend
	procedure tce_sxhw	(variable i1 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0));
	procedure tce_sxqw	(variable i1 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0));
	
	-- cmp
	procedure tce_eq		(variable i1, i2 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0));
	procedure tce_gt		(variable i1, i2 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0));
	procedure tce_gtu		(variable i1, i2 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0));
	procedure tce_lt		(variable i1, i2 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0));
	procedure tce_ltu		(variable i1, i2 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0));
	
	-- mathematic
	procedure tce_abs		(variable i1		 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0));
	procedure tce_add		(variable i1, i2 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0));
	procedure tce_sub		(variable i1, i2 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0));
	procedure tce_mul		(variable i1, i2 : in std_logic_vector(dataw-1 downto 0); variable r1 : out std_logic_vector(dataw-1 downto 0));
	procedure tce_mulshrl	(variable i1, i2, i3 : in std_logic_vector(dataw-1 downto 0); variable r1 : out std_logic_vector(dataw-1 downto 0));
					
	-- max/min		
	procedure tce_max		(variable i1, i2 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0));
	procedure tce_maxu	(variable i1, i2 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0));
	procedure tce_min		(variable i1, i2 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0));
	procedure tce_minu	(variable i1, i2 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0));
			
	-- shifts				
	procedure tce_shl		(variable i1, i2 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0));
	procedure tce_shr		(variable i1, i2 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0));
	procedure tce_shru	(variable i1, i2 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0));
	procedure tce_shrl	(variable i1, i2 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0));
	
	-- count leading zeros
	procedure tce_clz		(variable i1		 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0));
	
	procedure tce_select(variable i1, i2, i3 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0));
	procedure tce_addsubc(variable i1, i2, i3 : in std_logic_vector(dataw-1 downto 0); variable r1 : out std_logic_vector(dataw-1 downto 0));
							
end base;

package body base is

	procedure tce_and (variable i1, i2 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0)) is begin
		r1 := i1 and i2;
	end procedure;
	
	procedure tce_or (variable i1, i2 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0)) is begin
		r1 := i1 or i2;
	end procedure;
	
	procedure tce_ior (variable i1, i2 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0)) is begin
		r1 := i1 or i2;
	end procedure;
	
	procedure tce_xor (variable i1, i2 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0)) is begin
		r1 := i1 xor i2;
	end procedure;
	
	procedure tce_sxhw	(variable i1 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0)) is begin
		r1 := std_logic_vector(resize(signed(i1(15 downto 0)), r1'length));
	end procedure;
	procedure tce_sxqw	(variable i1 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0)) is begin
		r1 := std_logic_vector(resize(signed(i1(7 downto 0)), r1'length));
	end procedure;

	
	procedure tce_eq		(variable i1, i2 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0)) is begin
		r1 := (others => '0');
		if (signed(i1) = signed(i2)) then
			r1(0) := '1';
		else
			r1(0) := '0';
		end if;
	end procedure;
	
	procedure tce_gt		(variable i1, i2 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0)) is begin
		r1 := (others => '0');
		if (signed(i1) > signed(i2)) then
			r1(0) := '1';
		else
			r1(0) := '0';
		end if;
	end procedure;
	
	procedure tce_gtu		(variable i1, i2 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0)) is begin
		r1 := (others => '0');
		if (unsigned(i1) > unsigned(i2)) then
			r1(0) := '1';
		else
			r1(0) := '0';
		end if;
	end procedure;
	
	procedure tce_lt		(variable i1, i2 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0)) is begin
		r1 := (others => '0');
		if (signed(i1) < signed(i2)) then
			r1(0) := '1';
		else
			r1(0) := '0';
		end if;
	end procedure;
	
	procedure tce_ltu		(variable i1, i2 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0)) is begin
		r1 := (others => '0');
		if (unsigned(i1) < unsigned(i2)) then
			r1(0) := '1';
		else
			r1(0) := '0';
		end if;
	end procedure;
	
	procedure tce_abs (variable i1 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0)) is begin
		r1 := std_logic_vector(abs(signed(i1)));
	end procedure;

	procedure tce_add (variable i1, i2 : in std_logic_vector(dataw-1 downto 0); variable r1 : out std_logic_vector(dataw-1 downto 0)) is begin
		r1 := std_logic_vector(signed(i1) + signed(i2));
	end procedure;
	
	procedure tce_sub (variable i1, i2 : in std_logic_vector(dataw-1 downto 0); variable r1 : out std_logic_vector(dataw-1 downto 0)) is begin
		r1 := std_logic_vector(signed(i1) - signed(i2));
	end procedure;
	
	procedure tce_mul (variable i1, i2 : in std_logic_vector(31 downto 0);	variable r1 : out std_logic_vector(31 downto 0)) is
		variable tmp	: std_logic_vector(63 downto 0);
	begin
		tmp := std_logic_vector(signed(i1) * signed(i2));
		r1 := tmp(31 downto 0);
	end procedure;
	
	procedure tce_mulshrl (variable i1, i2, i3 : in std_logic_vector(31 downto 0);	variable r1 : out std_logic_vector(31 downto 0)) is
		variable prod	: std_logic_vector(63 downto 0);
		variable tmp	: std_logic_vector(63 downto 0);
	begin
		prod := std_logic_vector(signed(i1) * signed(i2));
		if (signed(i3) >= 0) then
			tmp := std_logic_vector(SHIFT_RIGHT(signed(prod), to_integer(unsigned(i3))));
		else
			tmp := std_logic_vector(SHIFT_LEFT(signed(prod), to_integer(abs(signed(i3)))));
		end if;
		r1 := tmp(31 downto 0);
	end procedure;
	
	procedure tce_max (variable i1, i2 : in std_logic_vector(dataw-1 downto 0); variable r1 : out std_logic_vector(dataw-1 downto 0)) is begin
		if (signed(i1) >= signed(i2)) then
			r1 := i1;
		else
			r1 := i2;
		end if;
	end procedure;
	
	procedure tce_maxu (variable i1, i2 : in std_logic_vector(dataw-1 downto 0); variable r1 : out std_logic_vector(dataw-1 downto 0)) is begin
		if (unsigned(i1) >= unsigned(i2)) then
			r1 := i1;
		else
			r1 := i2;
		end if;
	end procedure;
	
	procedure tce_min (variable i1, i2 : in std_logic_vector(dataw-1 downto 0); variable r1 : out std_logic_vector(dataw-1 downto 0)) is begin
		if (signed(i1) <= signed(i2)) then
			r1 := i1;
		else
			r1 := i2;
		end if;
	end procedure;
	
	procedure tce_minu (variable i1, i2 : in std_logic_vector(dataw-1 downto 0); variable r1 : out std_logic_vector(dataw-1 downto 0)) is begin
		if (unsigned(i1) <= unsigned(i2)) then
			r1 := i1;
		else
			r1 := i2;
		end if;
	end procedure;

	procedure tce_shl (variable i1, i2 : in std_logic_vector(dataw-1 downto 0); variable r1 : out std_logic_vector(dataw-1 downto 0)) is begin
		r1 := std_logic_vector(unsigned(i1) sll to_integer(unsigned(i2)));
	end procedure;

	procedure tce_shr (variable i1, i2 : in std_logic_vector(dataw-1 downto 0); variable r1 : out std_logic_vector(dataw-1 downto 0)) is begin
		r1 := std_logic_vector(SHIFT_RIGHT(signed(i1), to_integer(unsigned(i2))));
	end procedure;

	procedure tce_shru (variable i1, i2 : in std_logic_vector(dataw-1 downto 0); variable r1 : out std_logic_vector(dataw-1 downto 0)) is begin
		r1 := std_logic_vector(SHIFT_RIGHT(unsigned(i1), to_integer(unsigned(i2))));
	end procedure;
	
	procedure tce_shrl (variable i1, i2 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0)) is begin
		if (signed(i2) >= 0) then
			r1 := std_logic_vector(SHIFT_RIGHT(signed(i1), to_integer(unsigned(i2))));
		else
			r1 := std_logic_vector(SHIFT_LEFT(signed(i1), to_integer(abs(signed(i2)))));
		end if;
	end procedure;
	
	procedure tce_clz (variable i1 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0)) is
		variable j : integer range 0 to 31 := 0;
	begin
		r1(dataw-1 downto 0) := std_logic_vector(to_unsigned(32, r1'length));
		clz_loop: for j in dataw-1 downto 0 loop
			if i1(j) = '1' then
				r1(dataw-1 downto 0) := std_logic_vector(to_unsigned(dataw-1, dataw) - to_unsigned(j, dataw));
				exit clz_loop;
			end if;
		end loop;
	end procedure;
	
	procedure tce_select(variable i1, i2, i3 : in std_logic_vector(dataw-1 downto 0);	variable r1 : out std_logic_vector(dataw-1 downto 0)) is begin
		if i3(0) = '1' then
			r1 := i1;
		else
			r1 := i2;
		end if;
	end procedure;
	
	procedure tce_addsubc (variable i1, i2, i3 : in std_logic_vector(dataw-1 downto 0); variable r1 : out std_logic_vector(dataw-1 downto 0)) is begin
		if i3(0) = '1' then
			tce_add(i1, i2, r1);
		else
			tce_sub(i1, i2, r1);
		end if;
	end procedure;
	
end base;
