avalid_in_1 <= '1';
awren_in_1 <= '1';
aaddr_in_1 <= op1(addrw_c-1 downto 0);
strobe_32b <= "0000";
write_data_32b <= "00000000000000000000000000000000";
case op1(1 downto 1) is
  when "1" => 
    strobe_32b(1 downto 0) <= "11";
    write_data_32b(15 downto 0) <= op2;
  when others => 
    strobe_32b(3 downto 2) <= "11";
    write_data_32b(31 downto 16) <= op2;
end case;
adata_in_1 <= write_data_32b;
astrb_in_1 <= strobe_32b;
