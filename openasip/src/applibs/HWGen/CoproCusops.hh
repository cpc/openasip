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
 * Custom Instruction encoding assigning
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

namespace TTAMachine {
class Machine;
}

class BinaryEncoding;
class InstructionFormat;

class CoproCusops {
public:
    CoproCusops(const TTAMachine::Machine& machine, bool roccEn) {
        bem_ = BEMGenerator(machine, roccEn).generate();
        findCustomOps();
    }

    // Making the Custom RISCV full instruction encoding
    std::string
    cusencode(std::string operation) {
        std::string encode = " Not found";
        std::string reg = "00000";
        int maskF3 = 0b1110000000;
        int maskF7 = 0b11111110000000000;
        int opmask = 0b1111111;
        int encF3;
        int encF7;
        int opcode;

        for (auto op : Ops_) {
            if (op.first == operation) {
                encF3 = (op.second & maskF3) >> 7;
                encF7 = (op.second & maskF7) >> 10;
                opcode = (op.second & opmask);

                encode = std::bitset<7>(encF7).to_string() + reg + reg +
                         std::bitset<3>(encF3).to_string() + reg +
                         std::bitset<7>(opcode).to_string();
            }
        }
        return encode;
    }

protected:
    // Finding RISCV custom Operations as in RISCVTDGEN
    void
    findCustomOps() {
        Ops_.clear();
        const std::vector<std::string> formatsToSearch = {
            RISCVFields::RISCV_R_TYPE_NAME, RISCVFields::RISCV_R1R_TYPE_NAME,
            RISCVFields::RISCV_R1_TYPE_NAME,
            RISCVFields::RISCV_R3R_TYPE_NAME};
        for (const std::string& fName : formatsToSearch) {
            InstructionFormat* format = findFormat(fName);
            if (format == NULL) {
                continue;
            }
            for (int i = 0; i < format->operationCount(); i++) {
                const std::string op = format->operationAtIndex(i);
                if (!MapTools::containsKey(
                        RISCVFields::RISCVRTypeOperations, op)) {
                    Ops_.insert({op, format->encoding(op)});
                }
            }
        }
    }

private:
    std::map<std::string, int> Ops_;
    BinaryEncoding* bem_;

    // RISCV format finder as in RISCVTDGEN
    InstructionFormat*
    findFormat(const std::string name) const {
        InstructionFormat* format = NULL;
        for (int f = 0; f < bem_->instructionFormatCount(); f++) {
            if (bem_->instructionFormat(f).name() == name) {
                format = &bem_->instructionFormat(f);
                break;
            }
        }
        return format;
    }
};

#endif