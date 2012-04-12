// Copyright (c) 2012 Vinogradov Viacheslav
//
// This file is part of TTA-Based Codesign Environment (TCE).
// 
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//////////////////////////////////////////////////////////////////////////////
// Title      : Load store unit
//////////////////////////////////////////////////////////////////////////////
// File       : lsu.v
// Author     : Vinogradov Viacheslav
//////////////////////////////////////////////////////////////////////////////
// Description:
//
//
//////////////////////////////////////////////////////////////////////////////
// Revisions  :
// 2012-04-04  1.0  Vinogradov 
//////////////////////////////////////////////////////////////////////////////

module fu_ldh_ldhu_ldq_ldqu_ldw_sth_stq_stw_always_3
#(
    parameter dataw = 32,
    parameter addrw = 11
)
(t1data,t1load,t1opcode,o1data,o1load,r1data,data_in,data_out,
addr,mem_en_x,wr_en_x,wr_mask_x,glock,clk,rstx);

    // socket interfaces:
    input[addrw-1 : 0]t1data;
    input t1load;
    input[2 : 0]t1opcode;
    input[dataw-1 : 0]o1data;
    input o1load;
    output[dataw-1 : 0]r1data;

    input[dataw-1 : 0]data_in;
    output[dataw-1 : 0]data_out;
    output[addrw-2-1 : 0]addr;
    output[0 : 0]mem_en_x;// active low
    output[0 : 0]wr_en_x;// active low
    output[dataw-1 : 0]wr_mask_x;
    input     glock;
    input     clk;
    input     rstx;
    
    localparam OPC_LDW  = 4;
    localparam OPC_LDQ  = 2;
    localparam OPC_LDH  = 0;
    localparam OPC_STW  = 7;
    localparam OPC_STQ  = 6;
    localparam OPC_STH  = 5;
    localparam OPC_LDQU = 3;
    localparam OPC_LDHU = 1;

    reg[addrw-2-1 : 0] addr_reg;
    reg[dataw-1 : 0] data_out_reg;
    reg[0 : 0] wr_en_x_reg;
    reg[0 : 0] mem_en_x_reg;
    reg[dataw-1 : 0] wr_mask_x_reg;
    reg[4:0] status_addr_reg[1 : 0];

    wire[1 : 0] t1data_lower_2;

    reg[dataw-1 : 0]o1shadow_reg;
    reg[dataw-1 : 0]r1_reg;

    localparam NOT_LOAD =3'b000;
    localparam LDW      =3'b001;
    localparam LDH      =3'b010;
    localparam LDQ      =3'b011;
    localparam LDQU     =3'b100;
    localparam LDHU     =3'b101;

    wire[31:0] MSW_MASK_BIGENDIAN = 32'h0000_ffff;
    wire[31:0] LSW_MASK_BIGENDIAN = 32'hffff_0000;

    wire[31:0] MSW_MASK_LITTLE_ENDIAN = LSW_MASK_BIGENDIAN;
    wire[31:0] LSW_MASK_LITTLE_ENDIAN = MSW_MASK_BIGENDIAN;

    wire[7:0] ONES  = 8'hff;
    wire[7:0] ZEROS = 8'h00;

    localparam SIZE_OF_BYTE = 8;

    assign t1data_lower_2 = t1data[1 : 0];
  
    always@(posedge clk or negedge rstx)
    if(rstx ==0)// asynchronous reset (active low)
    begin
        addr_reg      <= {addrw-2{1'b0}};
        data_out_reg  <= {dataw{1'b0}};
        // use preset instead of reset
        wr_en_x_reg[0] <= 1'b1;
        mem_en_x_reg[0]<= 1'b1;
        wr_mask_x_reg <= {dataw{1'b1}};
        status_addr_reg[0] <= 5'b0;
        status_addr_reg[1] <= 5'b0;
        o1shadow_reg <= {dataw{1'b0}};
        r1_reg       <= {dataw{1'b0}};
    end
    else  // rising clock edge
    begin
        if(glock ==0)
            if(t1load ==1)
                case(t1opcode)
                    OPC_LDW:
                    begin
                        status_addr_reg[0] <= {LDW , t1data_lower_2};
                        addr_reg           <= t1data[addrw-1 : 2];
                        mem_en_x_reg[0]    <= 1'b0;
                        wr_en_x_reg[0]     <= 1'b1;
                    end
                    OPC_LDQ:
                    begin
                        status_addr_reg[0] <= {LDQ , t1data_lower_2};
                        addr_reg           <= t1data[addrw-1 : 2];
                        mem_en_x_reg[0]    <= 1'b0;
                        wr_en_x_reg[0]     <= 1'b1;
                    end
                    OPC_LDH:
                    begin
                        status_addr_reg[0] <= {LDH , t1data_lower_2};
                        addr_reg           <= t1data[addrw-1 : 2];
                        mem_en_x_reg[0]    <= 1'b0;
                        wr_en_x_reg[0]     <= 1'b1;
                    end
                    OPC_LDQU:
                    begin
                        status_addr_reg[0] <= {LDQU , t1data_lower_2};
                        addr_reg           <= t1data[addrw-1 : 2];
                        mem_en_x_reg[0]    <= 1'b0;
                        wr_en_x_reg[0]     <= 1'b1;
                    end
                    OPC_LDHU:
                    begin
                        status_addr_reg[0] <= {LDHU , t1data_lower_2};
                        addr_reg           <= t1data[addrw-1 : 2];
                        mem_en_x_reg[0]    <= 1'b0;
                        wr_en_x_reg[0]     <= 1'b1;
                    end
                    OPC_STW:
                    begin
                        status_addr_reg[0][4 : 2] <= NOT_LOAD;
                        if(o1load ==1)
                            data_out_reg <= o1data;
                        else
                            data_out_reg <= o1shadow_reg;
                        mem_en_x_reg[0] <= 1'b0;
                        wr_en_x_reg[0]  <= 1'b0;
                        wr_mask_x_reg   <= {dataw{1'b0}};
                        addr_reg        <= t1data[addrw-1 : 2];
                    end
                    OPC_STH:
                    begin
                        status_addr_reg[0][4 : 2] <= NOT_LOAD;
                        // endianes dependent code
                        // DEFAULT ENDIANESS
                        // big endian
                        //        Byte #
                        //        |0|1|2|3|
                        addr_reg <= t1data[addrw-1 : 2];
                        if(o1load ==1)
                            if(t1data[1] ==0)
                            begin
                                wr_mask_x_reg <= MSW_MASK_BIGENDIAN;
                                data_out_reg  <= {o1data[dataw/2-1 : 0],ZEROS,ZEROS};
                            end
                            else
                            begin
                                wr_mask_x_reg <= LSW_MASK_BIGENDIAN;
                                data_out_reg  <= {ZEROS,ZEROS,o1data[dataw/2-1 : 0]};
                            end
                        else
                            // endianes dependent code
                            if(t1data[1]==0)
                            begin
                                wr_mask_x_reg <= MSW_MASK_BIGENDIAN;
                                data_out_reg  <= {o1shadow_reg[dataw/2-1 : 0],ZEROS,ZEROS};
                            end
                            else
                            begin
                                wr_mask_x_reg <= LSW_MASK_BIGENDIAN;
                                data_out_reg  <= {ZEROS,ZEROS,o1shadow_reg[dataw/2-1 : 0]};
                            end
                        mem_en_x_reg[0] <= 1'b0;
                        wr_en_x_reg[0]  <= 1'b0;
                    end
                    OPC_STQ:
                    begin
                        status_addr_reg[0][4 : 2] <= NOT_LOAD;
                        // endianes dependent code
                        // DEFAULT ENDIANESS
                        // big endian
                        //        Byte #
                        //        |0|1|2|3|
                        addr_reg <= t1data[addrw-1 : 2];
                        if(o1load ==1)
                            case(t1data_lower_2)
                                // endianes dependent code
                                0:
                                begin
                                    wr_mask_x_reg <= {ZEROS,ONES,ONES,ONES};
                                    data_out_reg  <= {o1data[SIZE_OF_BYTE-1 : 0],ZEROS,ZEROS,ZEROS};
                                end
                                1:
                                begin
                                    wr_mask_x_reg <= {ONES,ZEROS,ONES,ONES};
                                    data_out_reg  <= {ZEROS,o1data[SIZE_OF_BYTE-1 : 0],ZEROS,ZEROS};
                                 end
                                2:
                                begin
                                    wr_mask_x_reg <= {ONES,ONES,ZEROS,ONES};
                                    data_out_reg  <= {ZEROS,ZEROS,o1data[SIZE_OF_BYTE-1 : 0],ZEROS};
                                end
                                default:
                                begin
                                    wr_mask_x_reg <= {ONES,ONES,ONES,ZEROS};
                                    data_out_reg  <= {ZEROS,ZEROS,ZEROS,o1data[SIZE_OF_BYTE-1 : 0]};
                                end
                            endcase
                      else
                        case(t1data_lower_2)
                          // endianes dependent code                            
                            0:
                            begin
                                wr_mask_x_reg <= {ZEROS,ONES,ONES,ONES};
                                data_out_reg  <= {o1shadow_reg[SIZE_OF_BYTE-1 : 0],ZEROS,ZEROS,ZEROS};
                            end
                            1:
                            begin
                                wr_mask_x_reg <= {ONES,ZEROS,ONES,ONES};
                                data_out_reg  <= {ZEROS,o1shadow_reg[SIZE_OF_BYTE-1 : 0],ZEROS,ZEROS};
                            end
                            2:
                            begin
                                wr_mask_x_reg <= {ONES,ONES,ZEROS,ONES};
                                data_out_reg  <= {ZEROS,ZEROS,o1shadow_reg[SIZE_OF_BYTE-1 : 0],ZEROS};
                            end
                            default:
                            begin
                                wr_mask_x_reg <= {ONES,ONES,ONES,ZEROS};
                                data_out_reg  <= {ZEROS,ZEROS,ZEROS,o1shadow_reg[SIZE_OF_BYTE-1 : 0]};
                            end
                        endcase
                        mem_en_x_reg[0] <= 1'b0;
                        wr_en_x_reg[0]  <= 1'b0;
                    end
                    default:
                    begin
                    end
                endcase
            else
            begin
                status_addr_reg[0][4 : 2] <= NOT_LOAD;
                wr_en_x_reg[0]            <= 1'b1;
                mem_en_x_reg[0]           <= 1'b1;
            end

        if(o1load ==1)
          o1shadow_reg <= o1data;

        status_addr_reg[1] <= status_addr_reg[0];

        if(status_addr_reg[1][4 : 2] == LDW)
            r1_reg <= data_in;
        else if(status_addr_reg[1][4 : 2] == LDH)
        begin
            // endianes dependent code
            // select either upper or lower part of the word
            if(status_addr_reg[1][1]==0)
                r1_reg <= $signed(data_in[dataw-1 : dataw/2]);
            else
                r1_reg <= $signed(data_in[dataw/2-1 : 0]);
        end
        else if(status_addr_reg[1][4 : 2] == LDQ)
          case(status_addr_reg[1][1 : 0])
            // endianes dependent code
             0:r1_reg <= $signed(data_in[dataw-1 : dataw-SIZE_OF_BYTE]);
             1:r1_reg <= $signed(data_in[dataw-SIZE_OF_BYTE-1 : dataw-2*SIZE_OF_BYTE]);
             2:r1_reg <= $signed(data_in[dataw-2*SIZE_OF_BYTE-1 : dataw-3*SIZE_OF_BYTE]);
             default:
              r1_reg <= $signed(data_in[dataw-3*SIZE_OF_BYTE-1 : dataw-4*SIZE_OF_BYTE]);
          endcase
        else if(status_addr_reg[1][4 : 2] == LDHU)
        begin
            // endianes dependent code
            // select either upper or lower part of the word
            if(status_addr_reg[1][1] ==0)
                r1_reg <= $unsigned(data_in[dataw-1 : dataw/2]);
            else
                r1_reg <= $unsigned(data_in[dataw/2-1 : 0]);
        end
        else if(status_addr_reg[1][4 : 2] == LDQU)
          case(status_addr_reg[1][1 : 0])
            // endianes dependent code
             0:r1_reg <= $unsigned(data_in[dataw-1 : dataw-SIZE_OF_BYTE]);
             1:r1_reg <= $unsigned(data_in[dataw-SIZE_OF_BYTE-1 : dataw-2*SIZE_OF_BYTE]);
             2:r1_reg <= $unsigned(data_in[dataw-2*SIZE_OF_BYTE-1 : dataw-3*SIZE_OF_BYTE]);
             default:
              r1_reg <= $unsigned(data_in[dataw-3*SIZE_OF_BYTE-1 : dataw-4*SIZE_OF_BYTE]);
          endcase
    end


  assign mem_en_x  = mem_en_x_reg;
  assign wr_en_x   = wr_en_x_reg;
  assign wr_mask_x = wr_mask_x_reg;
  assign data_out  = data_out_reg;
  assign addr      = addr_reg;
  assign r1data    = r1_reg;

endmodule

