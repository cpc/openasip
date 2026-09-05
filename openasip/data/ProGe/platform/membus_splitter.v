`timescale 1ns/1ps
// Copyright (c) 2017 Tampere University
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
// Converted from VHDL membus_splitter.vhdl

module membus_splitter #(
    parameter core_count_g      = 1,
    parameter axi_addr_width_g  = 21,
    parameter axi_data_width_g  = 32,
    parameter ctrl_addr_width_g = 8,
    parameter imem_addr_width_g = 16,
    parameter dmem_addr_width_g = 14,
    parameter pmem_addr_width_g = 14,
    parameter CORE_SEL_W = (core_count_g <= 1) ? 1 : $clog2(core_count_g)
) (
    input  wire                         avalid_in,
    output reg                          aready_out,
    input  wire [axi_addr_width_g-2-1:0] aaddr_in,
    output reg                          rvalid_out,
    input  wire                         rready_in,
    output reg  [axi_data_width_g-1:0]  rdata_out,
    output reg                          dmem_avalid_out,
    input  wire                         dmem_aready_in,
    input  wire                         dmem_rvalid_in,
    output reg                          dmem_rready_out,
    input  wire [axi_data_width_g-1:0]  dmem_rdata_in,
    output reg                          pmem_avalid_out,
    input  wire                         pmem_aready_in,
    input  wire                         pmem_rvalid_in,
    output reg                          pmem_rready_out,
    input  wire [axi_data_width_g-1:0]  pmem_rdata_in,
    output reg                          imem_avalid_out,
    input  wire                         imem_aready_in,
    input  wire                         imem_rvalid_in,
    output reg                          imem_rready_out,
    input  wire [axi_data_width_g-1:0]  imem_rdata_in,
    output reg                          ctrl_avalid_out,
    input  wire                         ctrl_aready_in,
    input  wire                         ctrl_rvalid_in,
    output reg                          ctrl_rready_out,
    input  wire [core_count_g*axi_data_width_g-1:0] ctrl_rdata_in,
    output reg  [CORE_SEL_W-1:0]        ctrl_core_sel_out
);

    localparam dataw = axi_data_width_g;
    `include "tce_util_pkg.vh"

    localparam integer CTRL_HIGH = 0;
    localparam integer IMEM_HIGH = 1;
    localparam integer DMEM_HIGH = 2;
    localparam integer PMEM_HIGH = 3;

    localparam integer mem_width_0 = ctrl_addr_width_g * bit_width(core_count_g);
    localparam integer mem_width_1 = imem_addr_width_g;
    localparam integer mem_width_2 = dmem_addr_width_g;
    localparam integer mem_width_3 = pmem_addr_width_g;
    localparam integer mem_width_max01 = (mem_width_0 > mem_width_1) ? mem_width_0 : mem_width_1;
    localparam integer mem_width_max23 = (mem_width_2 > mem_width_3) ? mem_width_2 : mem_width_3;
    localparam integer mem_width_max   = (mem_width_max01 > mem_width_max23) ? mem_width_max01 : mem_width_max23;
    localparam integer axi_addrw_c     = mem_width_max + 2;

    wire [CORE_SEL_W-1:0] core_sel;
    integer core_sel_int;
    integer mem_sel_int;

    assign core_sel = aaddr_in[bit_width(core_count_g)+ctrl_addr_width_g-1 : ctrl_addr_width_g];

    always @(*) begin
        ctrl_avalid_out   = 1'b0;
        ctrl_rready_out   = 1'b0;
        imem_avalid_out   = 1'b0;
        imem_rready_out   = 1'b0;
        dmem_avalid_out   = 1'b0;
        dmem_rready_out   = 1'b0;
        pmem_avalid_out   = 1'b0;
        pmem_rready_out   = 1'b0;
        ctrl_core_sel_out = {CORE_SEL_W{1'b0}};
        aready_out        = 1'b0;
        rvalid_out        = 1'b0;
        rdata_out         = {axi_data_width_g{1'b0}};

        if (core_count_g > 1)
            core_sel_int = core_sel;
        else
            core_sel_int = 0;

        mem_sel_int = aaddr_in[axi_addr_width_g-2-1 : axi_addr_width_g-2-2];
        case (mem_sel_int)
            CTRL_HIGH: begin
                ctrl_avalid_out   = avalid_in;
                aready_out        = ctrl_aready_in;
                rvalid_out        = ctrl_rvalid_in;
                ctrl_rready_out   = rready_in;
                rdata_out         = ctrl_rdata_in[axi_data_width_g*(core_sel_int+1)-1 -: axi_data_width_g];
                ctrl_core_sel_out = core_sel;
            end
            IMEM_HIGH: begin
                imem_avalid_out = avalid_in;
                aready_out      = imem_aready_in;
                rvalid_out      = imem_rvalid_in;
                imem_rready_out = rready_in;
                rdata_out       = imem_rdata_in;
            end
            DMEM_HIGH: begin
                dmem_avalid_out = avalid_in;
                aready_out      = dmem_aready_in;
                rvalid_out      = dmem_rvalid_in;
                dmem_rready_out = rready_in;
                rdata_out       = dmem_rdata_in;
            end
            default: begin
                pmem_avalid_out = avalid_in;
                aready_out      = pmem_aready_in;
                rvalid_out      = pmem_rvalid_in;
                pmem_rready_out = rready_in;
                rdata_out       = pmem_rdata_in;
            end
        endcase
    end

`ifndef SYNTHESIS
    initial begin
        if (axi_addrw_c != axi_addrw_c) begin
            $error("Toplevel generic axi_addr_width_g does not equal address width computed from the individual memories' widths.");
            $finish;
        end
    end
`endif

endmodule
