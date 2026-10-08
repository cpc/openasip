/*
    Copyright (C) 2025-2026 Tampere University.

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
 * @file CoproCusops.hh
 *
 * Assign encodings of custom RISCV instructions extracted from BEM 
 * for CV-X-IF and ROCC FUs
 * @author Tharaka Sampath
 */

#ifndef COPRO_CUSOPS_HH
#define COPRO_CUSOPS_HH

#include <bitset>

#include "BEMGenerator.hh"
#include "BinaryEncoding.hh"
#include "InstructionFormat.hh"
#include "Machine.hh"
#include "MapTools.hh"
#include "RISCVFields.hh"
#include "RISCVTools.hh"

namespace TTAMachine {
class Machine;
}

class BinaryEncoding;
class InstructionFormat;

class CoproCusops {
public:
    CoproCusops(const TTAMachine::Machine& machine, bool roccEn) {
        bem_ = BEMGenerator(machine, roccEn).generate();
        RISCVTools::findCustomOps(Ops_, bem_);
    }

    // Making the Custom RISCV full instruction encoding
    std::string
    cusencode(std::string operation) {
        std::string encode = " Not found";
        std::string reg = "00000";

        for (auto op : Ops_) {
            if (op.first == operation) {
                encode =
                    RISCVTools::getFunc7Str(op.second).erase(0, 2) + reg +
                    reg + RISCVTools::getFunc3Str(op.second).erase(0, 2) +
                    reg + RISCVTools::getOpcodeStr(op.second).erase(0, 2);
            }
        }
        return encode;
    }

private:
    std::map<std::string, int> Ops_;
    BinaryEncoding* bem_;
};

#endif
