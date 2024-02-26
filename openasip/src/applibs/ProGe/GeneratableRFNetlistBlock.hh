/*
    Copyright (c) 2002-2024 Tampere University.

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
 * @file GeneratableRFNetlistBlock.hh
 *
 * Declaration of GeneratableRFNetlistBlock.
 *
 * @author Lasse Lehtonen 2017 (lasse.lehtonen-no.spam-tut.fi)
 * @author Joonas Multanen 2024 (joonas.multanen-no.spam-tuni.fi)
 */

#pragma once

#include "RFPort.hh"
#include "NetlistBlock.hh"
#include "NetlistGenerator.hh"
#include "MathTools.hh"

namespace ProGe {

    class GeneratableRFNetlistBlock : public NetlistBlock {
    public:
        GeneratableRFNetlistBlock(const std::string& moduleName,
            const std::string instanceName, NetlistGenerator& generator)
            : NetlistBlock(moduleName, instanceName), generator_(generator) {}

        virtual ~GeneratableRFNetlistBlock() = default;

        void addInOperand(TTAMachine::RFPort* port, int id, int numRegisters) {
            (void) id;
            NetlistPort* dataPort =
                new NetlistPort("data_" + port->name() + "_in",
                    std::to_string(port->width()), BIT_VECTOR, IN, *this);
            NetlistPort* loadPort = new NetlistPort(
                "load_" + port->name() + "_in", "1", BIT, IN, *this);
            generator_.mapLoadPort(*dataPort, *loadPort);
            generator_.mapNetlistPort(*port, *dataPort);
            NetlistPort* opcodePort =
                new NetlistPort("opcode_" + port->name() + "_in",
                MathTools::requiredBits(numRegisters - 1),
                BIT_VECTOR, IN, *this);
            generator_.mapRFOpcodePort(*dataPort, *opcodePort);
        }
        void addOutOperand(TTAMachine::RFPort* port, int numRegisters) {
            NetlistPort* dataPort =
                new NetlistPort("data_" + port->name() + "_out",
                    std::to_string(port->width()), BIT_VECTOR, OUT, *this);
            NetlistPort* loadPort = new NetlistPort(
                "load_" + port->name() + "_in", "1", BIT, IN, *this);
            generator_.mapLoadPort(*dataPort, *loadPort);
            generator_.mapNetlistPort(*port, *dataPort);
            NetlistPort* opcodePort =
                new NetlistPort("opcode_" + port->name() + "_in",
                MathTools::requiredBits(numRegisters - 1),
                BIT_VECTOR, IN, *this);
            generator_.mapRFOpcodePort(*dataPort, *opcodePort);
        }

    private:
        NetlistGenerator& generator_;
    };
}
