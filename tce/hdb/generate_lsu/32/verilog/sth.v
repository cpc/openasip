avalid_in_1 = 1'b1;
awren_in_1 = 1'b1;
aaddr_in_1 = op1[addrw_c-1:0];
strobe_32b = 4'b0000;
write_data_32b = 32'b00000000000000000000000000000000;
unique case (op1[1:1])
  1'b1:  begin
    strobe_32b[1:0] = 2'b11;
    write_data_32b[15:0] = op2;
  end
  default:  begin
    strobe_32b[3:2] = 2'b11;
    write_data_32b[31:16] = op2;
  end
endcase
adata_in_1 = write_data_32b;
astrb_in_1 = strobe_32b;
