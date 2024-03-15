module synch_byte_mask_sram #(parameter
    // pragma translate_off
    init = 1,
    INITFILENAME = "ram_init",
    trace = 1,
    TRACEFILENAME = "dpram_trace",
    trace_mode = 0,
    access_trace = 1,
    ACCESSTRACEFILENAME = "access_trace",
    // pragma translate_on
    DATAW = 32,
    ADDRW = 7)
(
    input wire   clk,
    input wire   [DATAW-1:0] adata,
    input wire   [ADDRW-1:0] aaddr,
    input wire   avalid,
    input wire   awren,
    input wire   [(DATAW/8)-1:0] astrb,
    output wire  aready,
    output wire  rvalid,
    input wire   rready,
    output wire  [DATAW-1:0] rdata
);

parameter DW = DATAW;
parameter AW = ADDRW;
parameter DW8 = DW / 8;

reg [DW-1:0] mem_r [0:2**AW-1];
reg [DW-1:0] q_r;
reg [DW-1:0] wr_mask;

integer line;

assign line <= aaddr;

always @(posedge clk) begin
    if (avalid && awren) begin
        mem_r[line] <= (adata & wr_mask) | (mem_r[line] & ~wr_mask);
        q_r <= (adata & wr_mask) | (mem_r[line] & ~wr_mask);
    end else if (avalid) begin
        q_r <= mem_r[line];
    end
end

always @* begin
    wr_mask = 0;
    gen_mask_cp: for (int i = 0; i < DW8; i = i + 1) begin
        for (int j = i * 8; j < i * 8 + 8; j = j + 1) begin
            wr_mask[j] = astrb[i];
        end
    end
end

assign rdata = q_r;
assign rvalid = 1;
assign aready = 1;

endmodule
