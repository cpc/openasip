load_data_32b = rdata_out_1;
unique case (addr_low_out_1[1:1])
  1'b0: op2 = {{16{load_data_32b[15]}, load_data_32b[15:0]};
  default: op2 = {{16{load_data_32b[31]}, load_data_32b[31:16]};
endcase