avalid_in_1 = 1'b1;
awren_in_1 = 1'b1;
aaddr_in_1 = op1[addrw_c-1:0];
strobe_32b = 4'b0000;
write_data_32b = 32'b00000000000000000000000000000000;
unique case (op1[1:0])
  2'b00:  begin
    strobe_32b[0:0] = 1'b1;
    write_data_32b[7:0] = op2;
  end
  2'b01:  begin
    strobe_32b[1:1] = 1'b1;
    write_data_32b[15:8] = op2;
  end
  2'b10:  begin
    strobe_32b[2:2] = 1'b1;
    write_data_32b[23:16] = op2;
  end
  default:  begin
    strobe_32b[3:3] = 1'b1;
    write_data_32b[31:24] = op2;
  end
endcase
adata_in_1 = write_data_32b;
astrb_in_1 = strobe_32b;
