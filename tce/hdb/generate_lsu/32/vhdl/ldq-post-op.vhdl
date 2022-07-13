load_data_32b <= rdata_out_1;
case addr_low_out_1(1 downto 0) is
  when "11" => op2 <= (24-1 downto 0 => load_data_32b(7)) & load_data_32b(7 downto 0);
  when "10" => op2 <= (24-1 downto 0 => load_data_32b(15)) & load_data_32b(15 downto 8);
  when "01" => op2 <= (24-1 downto 0 => load_data_32b(23)) & load_data_32b(23 downto 16);
  when others => op2 <= (24-1 downto 0 => load_data_32b(31)) & load_data_32b(31 downto 24);
end case;