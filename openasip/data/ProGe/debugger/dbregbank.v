`timescale 1ns/1ps
// Copyright (c) 2013 Nokia Research Center
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
// Converted from VHDL dbregbank.vhdl

module status_register #(
    parameter reg_width_g    = 32,
    parameter output_width_g = 32
) (
    input  wire                    clk,
    input  wire                    nreset,
    input  wire [reg_width_g-1:0]  din,
    output wire [output_width_g-1:0] dout
);
    reg [reg_width_g-1:0] dout_r;

    always @(posedge clk or negedge nreset) begin
        if (nreset == 1'b0) begin
            dout_r <= {reg_width_g{1'b0}};
        end else begin
            dout_r <= din;
        end
    end

    generate
        if (output_width_g > reg_width_g) begin : g_pad
            assign dout = {{(output_width_g-reg_width_g){1'b0}}, dout_r};
        end else begin : g_nopad
            assign dout = dout_r;
        end
    endgenerate
endmodule

module control_register #(
    parameter reg_width_g    = 32,
    parameter output_width_g = 32,
    parameter reset_val_g    = 0
) (
    input  wire                      clk,
    input  wire                      nreset,
    input  wire                      we,
    input  wire [reg_width_g-1:0]    din,
    output wire [output_width_g-1:0] dout
);
    reg [reg_width_g-1:0] dout_r;

    always @(posedge clk or negedge nreset) begin
        if (nreset == 1'b0) begin
            dout_r <= reset_val_g;
        end else begin
            if (we == 1'b1) begin
                dout_r <= din;
            end
        end
    end

    generate
        if (output_width_g > reg_width_g) begin : g_pad
            assign dout = {{(output_width_g-reg_width_g){1'b0}}, dout_r};
        end else begin : g_nopad
            assign dout = dout_r;
        end
    endgenerate
endmodule

module dbregbank #(
    parameter data_width_g         = 32,
    parameter addr_width_g         = 8,
    parameter num_pc_breakpoints_g = 2,
    parameter num_cc_breakpoints_g = 1,
    parameter imem_data_width_g    = 64,
    parameter imem_addr_width_g    = 16,
    parameter dmem_data_width_g    = 32,
    parameter dmem_addr_width_g    = 14,
    parameter pmem_data_width_g    = 32,
    parameter pmem_addr_width_g    = 14,
    parameter bus_count_g          = 3,
    parameter core_count_g         = 1,
    parameter core_id_width_g      = 1,
    parameter core_id_g            = 0,
    parameter reserved_sp_bytes_g  = 0,
    parameter default_aql_len_g    = 0,
    parameter axi_offset_high_g    = 0,
    parameter axi_offset_low_g     = 0
) (
    input  wire                                clk,
    input  wire                                nreset,
    input  wire                                we_if,
    input  wire                                en_if,
    input  wire [addr_width_g-1:0]             addr_if,
    input  wire [data_width_g-1:0]             din_if,
    output reg  [data_width_g-1:0]             dout_if,
    input  wire [core_id_width_g-1:0]          core_sel,
    input  wire [2+num_cc_breakpoints_g+num_pc_breakpoints_g-1:0] bp_hit,
    input  wire [imem_addr_width_g-1:0]        pc,
    input  wire [2*data_width_g-1:0]           cycle_cnt,
    input  wire [2*data_width_g-1:0]           lock_cnt,
    input  wire [bus_count_g*data_width_g-1:0] bustraces,
    output wire [imem_addr_width_g-1:0]        pc_start_address,
    output wire [data_width_g-1:0]             bp0,
    output wire [1:0]                          bp0_type,
    output wire [imem_addr_width_g-1:0]        bp1,
    output wire [imem_addr_width_g-1:0]        bp2,
    output wire [imem_addr_width_g-1:0]        bp3,
    output wire [imem_addr_width_g-1:0]        bp4,
    output wire [num_cc_breakpoints_g+num_pc_breakpoints_g-1:0] bp_enable,
    output wire                                tta_reset,
    output reg                                 tta_continue,
    output reg                                 tta_forcebreak
);

    `include "registers_pkg.vh"
    `include "tce_util_functions.vh"

    function integer mem_size;
        input integer mem_addr_width;
        input integer mem_data_width;
        begin
            if (mem_data_width == 0) begin
                mem_size = 0;
            end else begin
                mem_size = 2 ** (mem_addr_width + bit_width((mem_data_width+7)/8));
            end
        end
    endfunction

    function integer set_aql_reservation_size;
        input integer dmem_addr_width;
        input integer default_aql_length;
        begin
            if (dmem_addr_width != 0) begin
                set_aql_reservation_size = 0;
            end else begin
                // AQL packet size is always 64
                // One packet slot is reserved for queue header
                set_aql_reservation_size = (64 * (default_aql_length + 1));
            end
        end
    endfunction

    function integer integer_ternary;
        input integer condition;
        input integer a;
        input integer b;
        begin
            if (condition != 0) begin
                integer_ternary = a;
            end else begin
                integer_ternary = b;
            end
        end
    endfunction

    // program counter width
    localparam pc_width_c      = imem_addr_width_g;
    localparam deviceclass_c   = 'h774;
    localparam device_id_c     = 'h12345678;
    localparam interface_type_c = 3;
    localparam ctrl_size_c     = 2**10;
    localparam dmem_size_c     = 2**(dmem_addr_width_g + bit_width(dmem_data_width_g/8));
    localparam pmem_size_c     = 2**(pmem_addr_width_g + bit_width(pmem_data_width_g/8));
    localparam imem_size_c     = mem_size(imem_addr_width_g, imem_data_width_g);
    localparam segment_size_c  = return_highest4(ctrl_size_c, imem_size_c, dmem_size_c, pmem_size_c);

    localparam signed [31:0] axi_offset_high_s = axi_offset_high_g;
    localparam signed [31:0] axi_offset_low_s  = axi_offset_low_g;
    localparam [63:0] axi_offset_c = {axi_offset_high_s, axi_offset_low_s};
    localparam [63:0] imem_offset_c = axi_offset_c + segment_size_c;
    localparam [63:0] separate_cqmem_offset_c = axi_offset_c + (2 * segment_size_c);
    localparam [63:0] buffermem_offset_c = axi_offset_c + (3 * segment_size_c);

    localparam reserved_aql_size_c = set_aql_reservation_size(dmem_addr_width_g, default_aql_len_g);

    // Buffermem can exist all in the same pmem-memory component with cq and scratchpad
    localparam buffermem_size_c = pmem_size_c - reserved_sp_bytes_g - reserved_aql_size_c;
    localparam cqmem_size_c     = integer_ternary(dmem_addr_width_g, dmem_size_c, reserved_aql_size_c);
    // VHDL goes through to_integer (32-bit) then to_unsigned(..., 64). Keep 64-bit
    // add so offsets above 2^31-1 are not truncated.
    localparam [63:0] cqmem_offset_c = (dmem_addr_width_g != 0)
        ? separate_cqmem_offset_c
        : (buffermem_offset_c + buffermem_size_c);

    localparam [63:0] feature_flags_c = {{63{1'b0}}, (axi_offset_c == 64'd0) ? 1'b0 : 1'b1};

    localparam TTA_STATUS_BITS        = 6;
    localparam TTA_PC_BITS            = pc_width_c;
    localparam TTA_CYCLECNT_BITS      = 32;
    localparam TTA_CYCLECNT_HIGH_BITS = 32;
    localparam TTA_LOCKCNT_BITS       = 32;
    localparam TTA_LOCKCNT_HIGH_BITS  = 32;
    localparam TTA_DEBUG_CMD_BITS     = 1;
    localparam TTA_PC_START_BITS      = pc_width_c;
    localparam TTA_DEBUG_CTRL_BITS    = 12;
    localparam TTA_DEBUG_BP0_BITS     = 32;
    localparam TTA_DEBUG_BP1_BITS     = pc_width_c;
    localparam TTA_DEBUG_BP2_BITS     = pc_width_c;

    localparam num_breakpoints_c = num_pc_breakpoints_g + num_cc_breakpoints_g;

    wire [data_width_g-1:0] dbstatus  [0:nof_status_registers_c-1];
    wire [data_width_g-1:0] dbcontrol [control_addresspace_start_c:
                                       control_addresspace_start_c+nof_control_registers_c-1];

    reg [control_addresspace_start_c+nof_control_registers_c-1:control_addresspace_start_c] ctrl_we;
    reg [nof_control_registers_c-1:0] ctrl_we_0;

    wire [TTA_STATUS_BITS-1:0] din_tta_status;

    wire [data_width_g-1:0] bustraces_arr [0:bus_count_g-1];

    reg [data_width_g-1:0] dout_if_reg;
    reg [bus_count_g*data_width_g-1:0] bustraces_reg;
    reg [addr_width_g-1:0] addr_delay;

    integer cregix;
    integer regix;
    integer rd_regix;

    genvar i;

    generate
        for (i = 0; i < bus_count_g; i = i + 1) begin : divide_traces
            assign bustraces_arr[i] = bustraces_reg[(i+1)*data_width_g-1:i*data_width_g];
        end
    endgenerate

    // Debugger configuration and command outputs
    // special handling for continue and force break bits (bypassed as pulse)
    always @(posedge clk or negedge nreset) begin
        if (nreset == 1'b0) begin
            tta_continue   <= 1'b0;
            tta_forcebreak <= 1'b0;
        end else begin
            tta_continue   <= 1'b0;
            tta_forcebreak <= 1'b0;
            if (ctrl_we[TTA_DEBUG_CMD] == 1'b1) begin
                tta_continue   <= din_if[DEBUG_CMD_CONTINUE];
                tta_forcebreak <= din_if[DEBUG_CMD_BREAK];
            end
        end
    end

    assign pc_start_address = dbcontrol[TTA_PC_START][pc_width_c-1:0];
    assign bp0              = dbcontrol[TTA_DEBUG_BP0];
    assign bp0_type         = dbcontrol[TTA_DEBUG_CTRL][1:0];
    assign bp1              = dbcontrol[TTA_DEBUG_BP1][pc_width_c-1:0];
    assign bp2              = dbcontrol[TTA_DEBUG_BP2][pc_width_c-1:0];
    // VHDL leaves bp3/bp4 undriven (BP3/BP4 mapping commented out)
    assign bp3              = {imem_addr_width_g{1'b0}};
    assign bp4              = {imem_addr_width_g{1'b0}};
    assign bp_enable        = dbcontrol[TTA_DEBUG_CTRL][num_breakpoints_c+1:2];
    assign tta_reset        = dbcontrol[TTA_DEBUG_CMD][0];

    // ctrl register write encoder
    always @* begin
        cregix = addr_if[addr_width_g-2:0];
        ctrl_we = {(nof_control_registers_c){1'b0}};
        if (we_if == 1'b1 && core_sel == core_id_g[core_id_width_g-1:0]) begin
            if (addr_if[addr_width_g-1] == 1'b1) begin
`ifndef SYNTHESIS
                if (cregix > 63) begin
                    $error("Write request to info register, cregix=%0d", cregix);
                end else begin
                    if (!(cregix < nof_control_registers_c)) begin
                        $error("Write request to non-existing control register, cregix=%0d, #ctrl regs=%0d",
                               cregix, nof_control_registers_c);
                    end
                end
`endif
                if (cregix < nof_control_registers_c) begin
                    ctrl_we_0 = {nof_control_registers_c{1'b0}};
                    ctrl_we_0[cregix] = 1'b1;
                    ctrl_we = ctrl_we_0;
                end
            end
        end
    end

    // read address encoding
    always @(posedge clk or negedge nreset) begin
        if (nreset == 1'b0) begin
            dout_if_reg   <= {data_width_g{1'b0}};
            bustraces_reg <= {(bus_count_g*data_width_g){1'b0}};
            addr_delay    <= {addr_width_g{1'b0}};
        end else begin
            dout_if_reg   <= {data_width_g{1'b0}};
            bustraces_reg <= bustraces;

            if (en_if == 1'b1) begin
                addr_delay <= addr_if;

                if (addr_if == TTA_CTRL_SIZE) begin
                    dout_if_reg <= ctrl_size_c[data_width_g-1:0];
                end else if (addr_if == TTA_IMEM_SIZE) begin
                    dout_if_reg <= imem_size_c[data_width_g-1:0];
                end else if (addr_if == TTA_IMEM_START_LOW) begin
                    dout_if_reg <= imem_offset_c[31:0];
                end else if (addr_if == TTA_IMEM_START_HIGH) begin
                    dout_if_reg <= imem_offset_c[63:32];
                end else if (addr_if == TTA_CQMEM_SIZE_LOW) begin
                    dout_if_reg <= cqmem_size_c[data_width_g-1:0];
                end else if (addr_if == TTA_CQMEM_SIZE_HIGH) begin
                    dout_if_reg <= {data_width_g{1'b0}};
                end else if (addr_if == TTA_CQMEM_START_LOW) begin
                    dout_if_reg <= cqmem_offset_c[31:0];
                end else if (addr_if == TTA_CQMEM_START_HIGH) begin
                    dout_if_reg <= cqmem_offset_c[63:32];
                end else if (addr_if == TTA_DMEM_SIZE_LOW) begin
                    dout_if_reg <= buffermem_size_c[data_width_g-1:0];
                end else if (addr_if == TTA_DMEM_SIZE_HIGH) begin
                    dout_if_reg <= {data_width_g{1'b0}};
                end else if (addr_if == TTA_DMEM_START_LOW) begin
                    dout_if_reg <= buffermem_offset_c[31:0];
                end else if (addr_if == TTA_DMEM_START_HIGH) begin
                    dout_if_reg <= buffermem_offset_c[63:32];
                end else if (addr_if == TTA_FEATURE_FLAGS_LOW) begin
                    dout_if_reg <= feature_flags_c[31:0];
                end else if (addr_if == TTA_FEATURE_FLAGS_HIGH) begin
                    dout_if_reg <= feature_flags_c[63:32];
                end else if (addr_if == TTA_PTR_SIZE) begin
                    dout_if_reg <= 32'd4;
                end else if (addr_if == TTA_DEVICECLASS) begin
                    dout_if_reg <= deviceclass_c[data_width_g-1:0];
                end else if (addr_if == TTA_DEVICE_ID) begin
                    dout_if_reg <= device_id_c[data_width_g-1:0];
                end else if (addr_if == TTA_INTERFACE_TYPE) begin
                    dout_if_reg <= interface_type_c[data_width_g-1:0];
                end else if (addr_if == TTA_CORE_COUNT) begin
                    dout_if_reg <= core_count_g[data_width_g-1:0];
                end else if (addr_if == TTA_DEBUG_SUPPORT) begin
                    dout_if_reg <= 32'd1;
                end else if (addr_if == TTA_BP_COUNT) begin
                    dout_if_reg <= num_pc_breakpoints_g[data_width_g-1:0];
                end else begin
                    rd_regix = addr_if[addr_width_g-2:0];
                    if (addr_if[addr_width_g-1] == 1'b0) begin
                        if (rd_regix <= (nof_status_registers_c-1)) begin
                            dout_if_reg <= dbstatus[rd_regix];
                        end
                    end else begin
                        if (rd_regix < nof_control_registers_c) begin
                            dout_if_reg <= dbcontrol[rd_regix + control_addresspace_start_c];
                        end else begin
`ifndef SYNTHESIS
                            $error("Non-exiting control register read access");
`endif
                        end
                    end
                end
            end
        end
    end

    always @* begin
        regix = addr_delay[addr_width_g-2:0];
        dout_if = dout_if_reg;
        if (addr_delay[addr_width_g-1] == 1'b0 && regix > 15) begin
            if ((regix - 16) < bus_count_g) begin
                dout_if = bustraces_arr[regix-16];
            end else begin
                dout_if = {data_width_g{1'b0}};
            end
        end else begin
            dout_if = dout_if_reg;
        end
    end

    // TTA_STATUS: bp_hit(4 downto 3) & '0' & bp_hit(2 downto 0)
    assign din_tta_status = {bp_hit[4:3], 1'b0, bp_hit[2:0]};

    status_register #(
        .reg_width_g    (TTA_STATUS_BITS),
        .output_width_g (data_width_g)
    ) sreg_tta_status (
        .clk    (clk),
        .nreset (nreset),
        .din    (din_tta_status),
        .dout   (dbstatus[TTA_STATUS])
    );

    status_register #(
        .reg_width_g    (TTA_PC_BITS),
        .output_width_g (data_width_g)
    ) sreg_tta_pc (
        .clk    (clk),
        .nreset (nreset),
        .din    (pc),
        .dout   (dbstatus[TTA_PC])
    );

    status_register #(
        .reg_width_g    (TTA_CYCLECNT_BITS),
        .output_width_g (data_width_g)
    ) sreg_tta_cyclecnt (
        .clk    (clk),
        .nreset (nreset),
        .din    (cycle_cnt[32-1:0]),
        .dout   (dbstatus[TTA_CYCLECNT])
    );

    status_register #(
        .reg_width_g    (TTA_CYCLECNT_HIGH_BITS),
        .output_width_g (data_width_g)
    ) sreg_tta_cyclecnt_high (
        .clk    (clk),
        .nreset (nreset),
        .din    (cycle_cnt[64-1:32]),
        .dout   (dbstatus[TTA_CYCLECNT_HIGH])
    );

    status_register #(
        .reg_width_g    (TTA_LOCKCNT_BITS),
        .output_width_g (data_width_g)
    ) sreg_tta_lockcnt (
        .clk    (clk),
        .nreset (nreset),
        .din    (lock_cnt[32-1:0]),
        .dout   (dbstatus[TTA_LOCKCNT])
    );

    status_register #(
        .reg_width_g    (TTA_LOCKCNT_HIGH_BITS),
        .output_width_g (data_width_g)
    ) sreg_tta_lockcnt_high (
        .clk    (clk),
        .nreset (nreset),
        .din    (lock_cnt[64-1:32]),
        .dout   (dbstatus[TTA_LOCKCNT_HIGH])
    );

    control_register #(
        .reg_width_g    (TTA_PC_START_BITS),
        .output_width_g (data_width_g),
        .reset_val_g    (0)
    ) creg_tta_pc_start (
        .clk    (clk),
        .nreset (nreset),
        .din    (din_if[TTA_PC_START_BITS-1:0]),
        .dout   (dbcontrol[TTA_PC_START]),
        .we     (ctrl_we[TTA_PC_START])
    );

    control_register #(
        .reg_width_g    (TTA_DEBUG_BP0_BITS),
        .output_width_g (data_width_g),
        .reset_val_g    (0)
    ) creg_tta_debug_bp0 (
        .clk    (clk),
        .nreset (nreset),
        .din    (din_if[TTA_DEBUG_BP0_BITS-1:0]),
        .dout   (dbcontrol[TTA_DEBUG_BP0]),
        .we     (ctrl_we[TTA_DEBUG_BP0])
    );

    generate
        for (i = 1; i <= 2; i = i + 1) begin : creg_tta_debug_bpn
            control_register #(
                .reg_width_g    (pc_width_c),
                .output_width_g (data_width_g),
                .reset_val_g    (0)
            ) debug_bpn_reg (
                .clk    (clk),
                .nreset (nreset),
                .din    (din_if[pc_width_c-1:0]),
                .dout   (dbcontrol[TTA_DEBUG_BP1-1+i]),
                .we     (ctrl_we[TTA_DEBUG_BP1-1+i])
            );
        end
    endgenerate

    control_register #(
        .reg_width_g    (TTA_DEBUG_CTRL_BITS),
        .output_width_g (data_width_g),
        .reset_val_g    (0)
    ) creg_tta_debug_ctrl (
        .clk    (clk),
        .nreset (nreset),
        .din    (din_if[TTA_DEBUG_CTRL_BITS-1:0]),
        .dout   (dbcontrol[TTA_DEBUG_CTRL]),
        .we     (ctrl_we[TTA_DEBUG_CTRL])
    );

    control_register #(
        .reg_width_g    (TTA_DEBUG_CMD_BITS),
        .output_width_g (data_width_g),
        .reset_val_g    (1)
    ) creg_tta_debug_cmd (
        .clk    (clk),
        .nreset (nreset),
        .din    (din_if[TTA_DEBUG_CMD_BITS-1:0]),
        .dout   (dbcontrol[TTA_DEBUG_CMD]),
        .we     (ctrl_we[TTA_DEBUG_CMD])
    );

endmodule
