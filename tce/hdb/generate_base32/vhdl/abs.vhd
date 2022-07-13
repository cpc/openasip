if signed(op1) < 0 then
  op2 <= std_logic_vector(to_signed(0,op2'length) - signed(op1));
else
  op2 <= op1;
end if;
