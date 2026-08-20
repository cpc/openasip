/*
    Copyright (c) 2002-2026 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */

module printchar_always_1 #(
    parameter dataw = 32
)(
    input wire [dataw-1:0] t1data,
    input wire t1load,
    input wire rstx,
    input wire glock,
    input wire clk
);

// synthesis translate_off
`ifndef SYNTHESIS
integer f;

initial begin
    // VHDL stdout uses write_mode (truncate), not append.
    f = $fopen("hdl_sim_stdout.txt", "w");
end

always @(posedge clk) begin
    if (t1load && !glock) begin
        $fwrite(f, "%s", t1data[7:0]);
        $fflush(f);
    end
end
`endif
// synthesis translate_on

endmodule
