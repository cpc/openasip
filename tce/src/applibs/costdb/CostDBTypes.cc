/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
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
