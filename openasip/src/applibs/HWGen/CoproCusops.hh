/*
    Copyright (c) 2024-2025 Tampere University.

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
* @file CoproCusops.hh
* 
* Custom Instruction encoding assigning 
* @author Tharaka Sampath 
*/

#ifndef COPRO_CUSOPS_HH
#define COPRO_CUSOPS_HH

#include "BEMGenerator.hh"
#include "BinaryEncoding.hh"
#include "InstructionFormat.hh"
#include "Machine.hh"
#include "MapTools.hh"
#include "RISCVFields.hh"
#include <bitset>


namespace TTAMachine {
    class Machine;
}

class BinaryEncoding;
class InstructionFormat;

class CoproCusops {
public:
    
    CoproCusops(const TTAMachine::Machine& machine, bool roccEn) 
    {
        bem_ = BEMGenerator(machine, roccEn).generate();
        findCustomOps();
    }

    // Making the Custom RISCV full instruction encoding
    std::string cusencode(std::string operation){
        std::string encode = " Not found";
        std::string reg = "00000";
        int maskF3 = 0b1110000000;
        int maskF7 = 0b11111110000000000;
        int opmask = 0b1111111;
        int encF3;
        int encF7;
        int opcode;

        for(auto op : Ops_){
            if(op.first == operation){
                encF3   = (op.second & maskF3) >> 7;
                encF7   = (op.second & maskF7) >> 10;
                opcode  = (op.second & opmask);
                
                encode  = std::bitset<7>(encF7).to_string() + reg + reg + std::bitset<3>(encF3).to_string() + reg + 
                        std::bitset<7>(opcode).to_string();
                
            }
        }
        return encode;
    }

protected:
    // Finding RISCV custom Operations as in RISCVTDGEN
    void findCustomOps() {
        Ops_.clear();
        const std::vector<std::string> formatsToSearch = {
            RISCVFields::RISCV_R_TYPE_NAME,
            RISCVFields::RISCV_R1R_TYPE_NAME,
            RISCVFields::RISCV_R1_TYPE_NAME,
            RISCVFields::RISCV_R3R_TYPE_NAME
        };
        for (const std::string& fName : formatsToSearch) {
            InstructionFormat* format = findFormat(fName);
            if (format == NULL) {
                continue;
            }
            for (int i = 0; i < format->operationCount(); i++) {
                const std::string op = format->operationAtIndex(i);
                if (!MapTools::containsKey(RISCVFields::RISCVRTypeOperations, op)) {
                    Ops_.insert({op, format->encoding(op)});
                }
            }
        }
    }

private:
    std::map<std::string, int> Ops_;
    BinaryEncoding* bem_;

    // RISCV format finder as in RISCVTDGEN
    InstructionFormat* findFormat(const std::string name) const {
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