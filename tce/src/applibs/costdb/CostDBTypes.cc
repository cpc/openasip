/*
    Copyright (c) 2002-2009 Tampere University.

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
/**
 * @file CostDBTypes.cc
 *
 * Definitions of CostDBTypes class.
 *
 * @author Tommi Rantanen 2004 (tommi.rantanen-no.spam-tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#include "CostDBTypes.hh"

const std::string CostDBTypes::EK_UNIT = "unit";
const std::string CostDBTypes::EK_RFILE = "rfile";
const std::string CostDBTypes::EK_MBUS = "mbus";
const std::string CostDBTypes::EK_SOCKET = "sock";
const std::string CostDBTypes::EK_INPUT_SOCKET = "in sock";
const std::string CostDBTypes::EK_OUTPUT_SOCKET = "out sock";
const std::string CostDBTypes::EK_INLINE_IMM_SOCKET = "imm sock";
const std::string CostDBTypes::EKF_BIT_WIDTH = "data";
const std::string CostDBTypes::EKF_OPERATIONS = "oper";
const std::string CostDBTypes::EKF_LATENCY = "latency";
const std::string CostDBTypes::EKF_NUM_REGISTERS = "size";
const std::string CostDBTypes::EKF_READ_PORTS = "rd";
const std::string CostDBTypes::EKF_WRITE_PORTS = "wr";
const std::string CostDBTypes::EKF_BIDIR_PORTS = "bidir";
const std::string CostDBTypes::EKF_MAX_READS = "max reads";
const std::string CostDBTypes::EKF_MAX_WRITES = "max writes";
const std::string CostDBTypes::EKF_BUS_FANIN = "bus fanin";
const std::string CostDBTypes::EKF_BUS_FANOUT = "bus fanout";
const std::string CostDBTypes::EKF_INPUT_SOCKET_FANIN = "in sock fanin";
const std::string CostDBTypes::EKF_OUTPUT_SOCKET_FANOUT = "out sock fanout";
const std::string CostDBTypes::EKF_GUARD_SUPPORT = "guard support";
const std::string CostDBTypes::EKF_GUARD_LATENCY = "guard latency";
const std::string CostDBTypes::EKF_FUNCTION_UNIT = "function unit";
