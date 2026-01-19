/*
    Copyright (C) 2025 Tampere University.

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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301 USA
 */
/**
 * @file CoproCusops.hh
 *
 * Assign encodings of the custom RISCV instructions extracted from BEM 
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
