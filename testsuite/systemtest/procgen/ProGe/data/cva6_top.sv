// Copyright (c) 2025  Tampere University of Technology.
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

module cva6_top 
import cvxif_sup_pkg::*;
(
  input logic clk_i,
  input logic rst_ni
);

  typedef struct packed {
    logic [15:0]                          instr;
    logic [cvxif_sup_pkg::X_ID_WIDTH-1:0] hartid;
  } x_compressed_req_t;

  typedef struct packed {
    logic [31:0] instr;
    logic        accept;
  } x_compressed_resp_t;

  typedef struct packed {
    logic [31:0]                          instr;
    logic [cvxif_sup_pkg::X_ID_WIDTH-1:0] hartid;
    logic [cvxif_sup_pkg::X_ID_WIDTH-1:0] id;
  } x_issue_req_t;

  typedef struct packed {
    logic         accept;
    logic         writeback;
    logic [2:0] register_read;
  } x_issue_resp_t;

  typedef struct packed {
    logic [cvxif_sup_pkg::X_ID_WIDTH-1:0] hartid;
    logic [cvxif_sup_pkg::X_ID_WIDTH-1:0] id;
    logic                                 commit_kill;
  } x_commit_t;

  typedef struct packed {
    logic [cvxif_sup_pkg::X_ID_WIDTH-1:0] hartid;
    logic [cvxif_sup_pkg::X_ID_WIDTH-1:0] id;
    logic [2:0][31:0]                  rs;
    logic [2:0]                         rs_valid;
  } x_register_t;

  typedef struct packed {
    logic [cvxif_sup_pkg::X_ID_WIDTH-1:0]   hartid;
    logic [cvxif_sup_pkg::X_ID_WIDTH-1:0]   id;
    logic [cvxif_sup_pkg::X_RFW_WIDTH-1:0]  data;
    logic [4:0]                             rd;
    logic                                   we;
  } x_result_t;

  typedef struct packed {
    logic              compressed_valid;
    x_compressed_req_t compressed_req;
    logic              issue_valid;
    x_issue_req_t      issue_req;
    logic              register_valid;
    x_register_t       register;
    logic              commit_valid;
    x_commit_t         commit;
    logic              result_ready;
  } cvxif_req_t;

  typedef struct packed {
    logic               compressed_ready;
    x_compressed_resp_t compressed_resp;
    logic               issue_ready;
    x_issue_resp_t      issue_resp;
    logic               register_ready;
    logic               result_valid;
    x_result_t          result;
  } cvxif_resp_t;

  cvxif_req_t  cvxif_req;
  cvxif_resp_t cvxif_resp;

  custom_coprocessor #(
        .NrRgprPorts          (2),
        .readregflags_t       (logic [1:0]),
        .writeregflags_t      (logic),
        .id_t                 (logic [cvxif_sup_pkg::X_ID_WIDTH-1:0]),
        .hartid_t             (logic [cvxif_sup_pkg::X_ID_WIDTH-1:0]),
        .x_compressed_req_t   (x_compressed_req_t),
        .x_compressed_resp_t  (x_compressed_resp_t),
        .x_issue_req_t        (x_issue_req_t),
        .x_issue_resp_t       (x_issue_resp_t),
        .x_register_t         (x_register_t),
        .x_commit_t           (x_commit_t),
        .x_result_t           (x_result_t),
        .cvxif_req_t          (cvxif_req_t),
        .cvxif_resp_t         (cvxif_resp_t)
      ) i_cvxif_coprocessor (
        .clk_i                (clk_i),
        .rst_ni               (rst_ni),
        .cvxif_req_i          (cvxif_req),
        .cvxif_resp_o         (cvxif_resp)
      );

endmodule
