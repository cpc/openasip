/*
 Copyright (C) 2024 Tampere University.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */
/**
 * @file RISCVTools.hh
 *
 * Declaration of RISCVTools class.
 *
 * @author Kari Hepola 2024 (kari.hepola@tuni.fi)
 * @note rating: red
 */


#ifndef RISCV_TOOLS_HH
#define RISCV_TOOLS_HH

#include <string>

class InstructionFormat;

struct R4Instruction {
    int baseopcode;
    int funct3;
    int funct7;
    int funct2;
};

class RISCVTools {
public:
    static std::string getFunc3Str(const int encoding);
    static std::string getFunc7Str(const int encoding);
    static std::string getFunc2Str(const int encoding);
    static std::string getOpcodeStr(const int encoding);
    static int getFunc3Int(const int encoding);
    static int getFunc7Int(const int encoding);
    static int getFunc2Int(const int encoding);
    static int getOpcodeInt(const int encoding);
    static inline void findCustomOps(
        std::map<std::string, int>& customOps_, BinaryEncoding* bem_);
    static R4Instruction decodeR4Instruction(const uint32_t opcode);
};

#include "RISCVTools.icc"

#endif

