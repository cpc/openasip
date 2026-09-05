`timescale 1ns/1ps
// Copyright (c) 2016 Nokia Research Center
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
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
// Converted from VHDL tta-axislave.vhdl

module tta_axislave
#(
  parameter axi_addrw_g = 17,
  parameter axi_dataw_g = 32,
  parameter axi_idw_g = 12,
  parameter sync_reset_g = 0
)
(
  input clk,
  input rstx,
  output avalid_out,
  input aready_in,
  output [axi_addrw_g-2-1:0] aaddr_out,
  output awren_out,
  output [axi_dataw_g/8-1:0] astrb_out,
  output [axi_dataw_g-1:0] adata_out,
  input rvalid_in,
  output rready_out,
  input [axi_dataw_g-1:0] rdata_in,
  input [axi_idw_g-1:0] s_axi_awid,
  input [axi_addrw_g-1:0] s_axi_awaddr,
  input [7:0] s_axi_awlen,
  input [2:0] s_axi_awsize,
  input [1:0] s_axi_awburst,
  input s_axi_awvalid,
  output s_axi_awready,
  input [31:0] s_axi_wdata,
  input [3:0] s_axi_wstrb,
  input s_axi_wvalid,
  output s_axi_wready,
  output [axi_idw_g-1:0] s_axi_bid,
  output [1:0] s_axi_bresp,
  output s_axi_bvalid,
  input s_axi_bready,
  input [axi_idw_g-1:0] s_axi_arid,
  input [axi_addrw_g-1:0] s_axi_araddr,
  input [7:0] s_axi_arlen,
  input [2:0] s_axi_arsize,
  input [1:0] s_axi_arburst,
  input s_axi_arvalid,
  output s_axi_arready,
  output [axi_idw_g-1:0] s_axi_rid,
  output [31:0] s_axi_rdata,
  output [1:0] s_axi_rresp,
  output s_axi_rlast,
  output s_axi_rvalid,
  input s_axi_rready
);

  localparam FIXED_BURST = 2'b00;
  localparam INCR_BURST  = 2'b01;
  localparam WRAP_BURST  = 2'b10;
  localparam AXI_OKAY    = 2'b00;

  localparam S_READY        = 3'd0;
  localparam S_WRITE_DATA   = 3'd1;
  localparam S_FINISH_WRITE = 3'd2;
  localparam S_READ_DATA    = 3'd3;
  localparam S_FINISH_READ  = 3'd4;

  function [7:0] burst_size_lut;
    input [2:0] sz;
    begin
      case (sz)
        3'd0: burst_size_lut = 8'd1;
        3'd1: burst_size_lut = 8'd2;
        3'd2: burst_size_lut = 8'd4;
        3'd3: burst_size_lut = 8'd8;
        3'd4: burst_size_lut = 8'd16;
        3'd5: burst_size_lut = 8'd32;
        3'd6: burst_size_lut = 8'd64;
        default: burst_size_lut = 8'd128;
      endcase
    end
  endfunction

  function [axi_addrw_g-1:0] wrap_mask_fn;
    input [2:0] axsize;
    input [7:0] axlen;
    integer I;
    integer axsize_int;
    begin
      axsize_int = axsize;
      for (I = 0; I < axi_addrw_g; I = I + 1) begin
        if (I < axsize_int)
          wrap_mask_fn[I] = 1'b0;
        else if (I < axsize_int + 7)
          wrap_mask_fn[I] = axlen[I - axsize_int];
        else
          wrap_mask_fn[I] = 1'b0;
      end
    end
  endfunction

  function [axi_addrw_g-1:0] increment_addr;
    input [1:0] burst_type;
    input [7:0] size;
    input [axi_addrw_g-1:0] wmask;
    input [axi_addrw_g-1:0] address;
    integer I;
    reg [axi_addrw_g-1:0] address_tmp;
    begin
      case (burst_type)
        FIXED_BURST: increment_addr = address;
        INCR_BURST:  increment_addr = address + size;
        WRAP_BURST: begin
          address_tmp = address + size;
          for (I = 0; I < axi_addrw_g; I = I + 1) begin
            if (wmask[I] == 1'b0)
              address_tmp[I] = address[I];
          end
          increment_addr = address_tmp;
        end
        default: begin
`ifndef SYNTHESIS
          $warning("Unrecognized burst type");
`endif
          increment_addr = address;
        end
      endcase
    end
  endfunction

  reg [2:0] state;

  reg s_axi_awready_r;
  reg s_axi_wready_r;
  reg [axi_idw_g-1:0] s_axi_bid_r;
  reg [1:0] s_axi_bresp_r;
  reg s_axi_bvalid_r;
  reg s_axi_arready_r;
  reg [axi_idw_g-1:0] s_axi_rid_r;
  reg [31:0] s_axi_rdata_r;
  reg [1:0] s_axi_rresp_r;
  reg s_axi_rlast_r;
  reg s_axi_rvalid_r;

  reg [axi_dataw_g/8-1:0] astrb_r, stall_strb_r;
  reg [axi_dataw_g-1:0] adata_r, stall_data_r;
  reg [axi_addrw_g-2-1:0] aaddr_r, stall_addr_r;
  reg avalid_r, awren_r, stall_data_valid_r, rready_r;

  reg [7:0] burst_cnt_r;
  reg [7:0] read_cnt_r;
  reg [axi_idw_g-1:0] transaction_r;
  reg [1:0] burst_type_r;
  reg [7:0] burst_size_r;
  reg [axi_addrw_g-1:0] wrap_mask_r;
  reg [axi_addrw_g-1:0] axi_addr_r;
  reg axi_stall_r;

  task reset_regs;
    begin
      s_axi_awready_r <= 1'b0;
      s_axi_wready_r  <= 1'b0;
      s_axi_bid_r     <= {axi_idw_g{1'b0}};
      s_axi_bresp_r   <= 2'b00;
      s_axi_bvalid_r  <= 1'b0;
      s_axi_arready_r <= 1'b0;
      s_axi_rid_r     <= {axi_idw_g{1'b0}};
      s_axi_rdata_r   <= 32'b0;
      s_axi_rresp_r   <= 2'b00;
      s_axi_rlast_r   <= 1'b0;
      s_axi_rvalid_r  <= 1'b0;
      avalid_r <= 1'b0;
      aaddr_r  <= {(axi_addrw_g-2){1'b0}};
      awren_r  <= 1'b0;
      astrb_r  <= {(axi_dataw_g/8){1'b0}};
      adata_r  <= {axi_dataw_g{1'b0}};
      rready_r <= 1'b0;
      axi_addr_r         <= {axi_addrw_g{1'b0}};
      state              <= S_READY;
      burst_cnt_r        <= 8'b0;
      transaction_r      <= {axi_idw_g{1'b0}};
      stall_strb_r       <= {(axi_dataw_g/8){1'b0}};
      stall_data_r       <= {axi_dataw_g{1'b0}};
      stall_addr_r       <= {(axi_addrw_g-2){1'b0}};
      stall_data_valid_r <= 1'b0;
    end
  endtask

  task seq_logic;
    reg [axi_addrw_g-1:0] axi_addr_v;
    begin
      if ((s_axi_arready_r == 1'b1) && (s_axi_arvalid == 1'b1))
        s_axi_arready_r <= 1'b0;
      if ((s_axi_awready_r == 1'b1) && (s_axi_awvalid == 1'b1))
        s_axi_awready_r <= 1'b0;

      s_axi_wready_r <= 1'b0;
      rready_r       <= 1'b0;
      s_axi_rlast_r  <= 1'b0;
      axi_stall_r    <= 1'b0;

      case (state)
        S_READY: begin
          if (s_axi_awvalid == 1'b1) begin
            s_axi_awready_r <= 1'b1;
            axi_addr_r      <= s_axi_awaddr;
            transaction_r   <= s_axi_awid;
            wrap_mask_r     <= wrap_mask_fn(s_axi_awsize, s_axi_awlen);
            burst_size_r    <= burst_size_lut(s_axi_awsize);
            burst_type_r    <= s_axi_awburst;
            burst_cnt_r     <= s_axi_awlen;
            state           <= S_WRITE_DATA;
          end else if (s_axi_arvalid == 1'b1) begin
            s_axi_arready_r <= 1'b1;
            transaction_r   <= s_axi_arid;
            wrap_mask_r     <= wrap_mask_fn(s_axi_arsize, s_axi_arlen);
            axi_addr_r      <= s_axi_araddr;
            aaddr_r         <= s_axi_araddr[axi_addrw_g-1:2];
            avalid_r        <= 1'b1;
            awren_r         <= 1'b0;
            rready_r        <= 1'b1;
            burst_size_r    <= burst_size_lut(s_axi_arsize);
            burst_cnt_r     <= s_axi_arlen;
            read_cnt_r      <= s_axi_arlen;
            burst_type_r    <= s_axi_arburst;
            state           <= S_READ_DATA;
          end
        end

        S_WRITE_DATA: begin
          if ((avalid_r == 1'b0) || (stall_data_valid_r == 1'b0))
            s_axi_wready_r <= 1'b1;
          else
            s_axi_wready_r <= 1'b0;

          if ((avalid_r == 1'b1) && (aready_in == 1'b1)) begin
            if (stall_data_valid_r == 1'b1) begin
              astrb_r <= stall_strb_r;
              adata_r <= stall_data_r;
              awren_r <= 1'b1;
              aaddr_r <= stall_addr_r;
              stall_data_valid_r <= 1'b0;
            end else begin
              avalid_r <= 1'b0;
            end
          end

          if ((s_axi_wvalid == 1'b1) && (s_axi_wready_r == 1'b1)) begin
            if (((aready_in == 1'b1) && (stall_data_valid_r == 1'b0)) || (avalid_r == 1'b0)) begin
              avalid_r <= 1'b1;
              awren_r  <= 1'b1;
              astrb_r  <= s_axi_wstrb;
              adata_r  <= s_axi_wdata;
              aaddr_r  <= axi_addr_r[axi_addrw_g-1:2];
            end else begin
              stall_data_valid_r <= 1'b1;
              stall_data_r       <= s_axi_wdata;
              stall_strb_r       <= s_axi_wstrb;
              stall_addr_r       <= axi_addr_r[axi_addrw_g-1:2];
            end

            if (burst_cnt_r == 8'd0) begin
              s_axi_wready_r <= 1'b0;
              s_axi_bresp_r  <= AXI_OKAY;
              s_axi_bvalid_r <= 1'b1;
              s_axi_bid_r    <= transaction_r;
              state          <= S_FINISH_WRITE;
            end else begin
              axi_addr_r  <= increment_addr(burst_type_r, burst_size_r, wrap_mask_r, axi_addr_r);
              burst_cnt_r <= burst_cnt_r - 8'd1;
            end
          end
        end

        S_FINISH_WRITE: begin
          if (s_axi_bready == 1'b1)
            s_axi_bvalid_r <= 1'b0;

          if ((avalid_r == 1'b1) && (aready_in == 1'b1)) begin
            if (stall_data_valid_r == 1'b1) begin
              astrb_r <= stall_strb_r;
              adata_r <= stall_data_r;
              awren_r <= 1'b1;
              stall_data_valid_r <= 1'b0;
            end else begin
              avalid_r <= 1'b0;
              awren_r  <= 1'b0;
            end
          end

          if ((avalid_r == 1'b0) && (s_axi_bvalid_r == 1'b0))
            state <= S_READY;
        end

        S_READ_DATA: begin
          if ((s_axi_rready == 1'b1) && (s_axi_rvalid_r == 1'b1)) begin
            if (stall_data_valid_r == 1'b1) begin
              s_axi_rdata_r      <= stall_data_r;
              stall_data_valid_r <= 1'b0;
            end else begin
              s_axi_rvalid_r <= 1'b0;
            end
          end

          if ((avalid_r == 1'b1) && (aready_in == 1'b1)) begin
            if (read_cnt_r == 8'd0) begin
              avalid_r <= 1'b0;
            end else begin
              axi_addr_v  = increment_addr(burst_type_r, burst_size_r, wrap_mask_r, axi_addr_r);
              axi_addr_r  <= axi_addr_v;
              aaddr_r     <= axi_addr_v[axi_addrw_g-1:2];
              read_cnt_r  <= read_cnt_r - 8'd1;
            end
          end

          if ((s_axi_rvalid_r == 1'b0) || (stall_data_valid_r == 1'b0))
            rready_r <= 1'b1;
          else
            rready_r <= 1'b0;

          if ((rvalid_in == 1'b1) && (rready_r == 1'b1)) begin
            if (((s_axi_rready == 1'b1) && (stall_data_valid_r == 1'b0)) || (s_axi_rvalid_r == 1'b0)) begin
              s_axi_rvalid_r <= 1'b1;
              s_axi_rdata_r  <= rdata_in;
              s_axi_rresp_r  <= AXI_OKAY;
              s_axi_rid_r    <= transaction_r;
            end else begin
              stall_data_valid_r <= 1'b1;
              stall_data_r       <= rdata_in;
              rready_r           <= 1'b0;
            end

            if (burst_cnt_r == 8'd0) begin
              if (((s_axi_rready == 1'b1) && (stall_data_valid_r == 1'b0)) || (s_axi_rvalid_r == 1'b0))
                s_axi_rlast_r <= 1'b1;
              rready_r <= 1'b0;
              state <= S_FINISH_READ;
            end else begin
              burst_cnt_r <= burst_cnt_r - 8'd1;
            end
          end
        end

        S_FINISH_READ: begin
          if ((s_axi_rready == 1'b1) && (s_axi_rvalid_r == 1'b1)) begin
            if (stall_data_valid_r == 1'b1) begin
              s_axi_rlast_r      <= 1'b1;
              s_axi_rdata_r      <= stall_data_r;
              stall_data_valid_r <= 1'b0;
            end else begin
              s_axi_rvalid_r <= 1'b0;
              s_axi_rlast_r  <= 1'b0;
              state          <= S_READY;
            end
          end
        end

        default: state <= S_READY;
      endcase
    end
  endtask

  generate
    if (sync_reset_g == 0) begin : g_async
      always @(posedge clk or negedge rstx) begin
        if (rstx == 1'b0)
          reset_regs;
        else
          seq_logic;
      end
    end else begin : g_sync
      always @(posedge clk) begin
        if (rstx == 1'b0)
          reset_regs;
        else
          seq_logic;
      end
    end
  endgenerate

  assign s_axi_awready = s_axi_awready_r;
  assign s_axi_wready  = s_axi_wready_r;
  assign s_axi_bid     = s_axi_bid_r;
  assign s_axi_bresp   = s_axi_bresp_r;
  assign s_axi_bvalid  = s_axi_bvalid_r;
  assign s_axi_arready = s_axi_arready_r;
  assign s_axi_rid     = s_axi_rid_r;
  assign s_axi_rdata   = s_axi_rdata_r;
  assign s_axi_rresp   = s_axi_rresp_r;
  assign s_axi_rlast   = s_axi_rlast_r;
  assign s_axi_rvalid  = s_axi_rvalid_r;
  assign avalid_out    = avalid_r;
  assign aaddr_out     = aaddr_r;
  assign awren_out     = awren_r;
  assign astrb_out     = astrb_r;
  assign adata_out     = adata_r;
  assign rready_out    = rready_r;

endmodule
