module fu_reflect
#(parameter  busw = 3)
(
    input[busw-1:0] t1data,
    input t1load,
    input[0 : 0] t1opcode,
    output[busw-1:0] r1data,
    input clk,
    input rstx,
    input glock
);
    parameter OPC_REFLECT32=0;
    parameter OPC_REFLECT8=1;

    reg[busw-1:0] r1reg;
    integer i;

    always@(posedge clk or negedge rstx)
    if(~rstx)// asynchronous reset (active low)
       r1reg <={busw{1'b0}};
    else //- rising clock edge
    begin
      if(~glock)
        if(t1load)
         case(t1opcode)
            OPC_REFLECT8:
               begin
                 for(i=0;i<8;i=i+1)
                     r1reg[i] <= t1data[7-i];
                end
            OPC_REFLECT32:
              for(i=0;i<busw;i=i+1)
                r1reg[i] <= t1data[busw-1-i];
        endcase
    end

  assign r1data = r1reg;

endmodule
