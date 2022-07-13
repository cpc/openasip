load_data_32b = rdata_out_1;
unique case (addr_low_out_1[1:0])
  2'b11: op2 = {{24{load_data_32b[7]}, load_data_32b[7:0]};
  2'b10: op2 = {{24{load_data_32b[15]}, load_data_32b[15:8]};
  2'b01: op2 = {{24{load_data_32b[23]}, load_data_32b[23:16]};
  default: op2 = {{24{load_data_32b[31]}, load_data_32b[31:24]};
endcase