/*
 Copyright (C) 2024-2026 Tampere University.

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
    static inline std::string getFunc3Str(const int encoding);
    static inline std::string getFunc7Str(const int encoding);
    static inline std::string getFunc2Str(const int encoding);
    static inline std::string getOpcodeStr(const int encoding);
    static inline int getFunc3Int(const int encoding);
    static inline int getFunc7Int(const int encoding);
    static inline int getFunc2Int(const int encoding);
    static inline int getOpcodeInt(const int encoding);
    static inline void findCustomOps(
        std::map<std::string, int>& customOps_, BinaryEncoding* bem_);
    static inline R4Instruction decodeR4Instruction(const uint32_t opcode);
};

#include "RISCVTools.icc"

#endif

