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
// Converted from VHDL registers-pkg.vhdl
// Include inside a module body.

localparam nof_status_registers_c = 6;
localparam TTA_STATUS        = 0;
localparam TTA_PC            = 1;
localparam TTA_CYCLECNT      = 2;
localparam TTA_CYCLECNT_HIGH = 3;
localparam TTA_LOCKCNT       = 4;
localparam TTA_LOCKCNT_HIGH  = 5;

localparam control_addresspace_start_c = 2**7;
localparam nof_control_registers_c     = 6;

localparam TTA_DEBUG_CMD  = 0 + control_addresspace_start_c;
localparam TTA_PC_START   = 1 + control_addresspace_start_c;
localparam TTA_DEBUG_CTRL = 2 + control_addresspace_start_c;
localparam TTA_DEBUG_BP0  = 3 + control_addresspace_start_c;
localparam TTA_DEBUG_BP1  = 4 + control_addresspace_start_c;
localparam TTA_DEBUG_BP2  = 5 + control_addresspace_start_c;

localparam info_addresspace_start_c = 2**7 + 2**6;

localparam TTA_DEVICECLASS      = 0 + info_addresspace_start_c;
localparam TTA_DEVICE_ID        = 1 + info_addresspace_start_c;
localparam TTA_INTERFACE_TYPE   = 2 + info_addresspace_start_c;
localparam TTA_CORE_COUNT       = 3 + info_addresspace_start_c;
localparam TTA_CTRL_SIZE        = 4 + info_addresspace_start_c;

localparam TTA_IMEM_SIZE        = 5 + info_addresspace_start_c;
localparam TTA_IMEM_START_LOW   = 6 + info_addresspace_start_c;
localparam TTA_IMEM_START_HIGH  = 7 + info_addresspace_start_c;

localparam TTA_CQMEM_SIZE_LOW   = 8  + info_addresspace_start_c;
localparam TTA_CQMEM_SIZE_HIGH  = 9  + info_addresspace_start_c;
localparam TTA_CQMEM_START_LOW  = 10 + info_addresspace_start_c;
localparam TTA_CQMEM_START_HIGH = 11 + info_addresspace_start_c;

localparam TTA_DMEM_SIZE_LOW    = 12 + info_addresspace_start_c;
localparam TTA_DMEM_SIZE_HIGH   = 13 + info_addresspace_start_c;
localparam TTA_DMEM_START_LOW   = 14 + info_addresspace_start_c;
localparam TTA_DMEM_START_HIGH  = 15 + info_addresspace_start_c;

localparam TTA_FEATURE_FLAGS_LOW  = 16 + info_addresspace_start_c;
localparam TTA_FEATURE_FLAGS_HIGH = 17 + info_addresspace_start_c;
localparam TTA_PTR_SIZE           = 18 + info_addresspace_start_c;

localparam TTA_DEBUG_SUPPORT    = 19 + info_addresspace_start_c;
localparam TTA_BP_COUNT         = 20 + info_addresspace_start_c;

localparam DEBUG_CMD_RESET    = 0;
localparam DEBUG_CMD_CONTINUE = 1;
localparam DEBUG_CMD_BREAK    = 2;

localparam FF_AXI_MASTER = 0;
