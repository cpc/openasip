/*
    Copyright (c) 2002-2017 Tampere University.

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
 * @file GeneratableFUNetlistBlock.hh
 *
 * Declaration of GeneratableFUNetlistBlock.
 *
 * @author Lasse Lehtonen 2017 (lasse.lehtonen-no.spam-tut.fi)
 */

#pragma once

#include "FUPort.hh"
#include "NetlistBlock.hh"
#include "NetlistGenerator.hh"

namespace ProGe {

    class GeneratableFUNetlistBlock : public NetlistBlock {
    public:
        GeneratableFUNetlistBlock(const std::string& moduleName,
            const std::string instanceName, NetlistGenerator& generator)
            : NetlistBlock(moduleName, instanceName), generator_(generator) {}

        virtual ~GeneratableFUNetlistBlock() = default;

        void addInOperand(TTAMachine::FUPort* port, int id) {
            (void) id;
            NetlistPort* dataPort =
                new NetlistPort("data_" + port->name() + "_in",
                    std::to_string(port->width()), BIT_VECTOR, IN, *this);
            NetlistPort* loadPort = new NetlistPort(
                "load_" + port->name() + "_in", "1", BIT, IN, *this);
            generator_.mapLoadPort(*dataPort, *loadPort);
            generator_.mapNetlistPort(*port, *dataPort);
        }
        void addOutOperand(TTAMachine::FUPort* port) {
            NetlistPort* dataPort =
                new NetlistPort("data_" + port->name() + "_out",
                    std::to_string(port->width()), BIT_VECTOR, OUT, *this);
            generator_.mapNetlistPort(*port, *dataPort);
        }

    private:
        NetlistGenerator& generator_;
    };
}
