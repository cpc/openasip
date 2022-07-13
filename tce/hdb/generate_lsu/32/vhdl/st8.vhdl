avalid_in_1 <= '1';
awren_in_1 <= '1';
aaddr_in_1 <= op1(addrw_c-1 downto 0);
strobe_32b <= "0000";
write_data_32b <= "00000000000000000000000000000000";
case op1(1 downto 0) is
  when "00" => 
    strobe_32b(0 downto 0) <= "1";
    write_data_32b(7 downto 0) <= op2;
  when "01" => 
    strobe_32b(1 downto 1) <= "1";
    write_data_32b(15 downto 8) <= op2;
  when "10" => 
    strobe_32b(2 downto 2) <= "1";
    write_data_32b(23 downto 16) <= op2;
  when others => 
    strobe_32b(3 downto 3) <= "1";
    write_data_32b(31 downto 24) <= op2;
end case;
adata_in_1 <= write_data_32b;
astrb_in_1 <= strobe_32b;
