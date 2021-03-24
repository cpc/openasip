/*
    Copyright (c) 2002-2021 Tampere University.

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
 * @file BlocksFU.hh
 *
 * Declaration of the virtual BlocksFU Class.
 * This class is the parent class of ALU, LSU & MUL units.
 *
 * @author Maarten Molendijk 2020 (m.j.molendijk@tue.nl)
 * @author Kanishkan Vadivel 2021 (k.vadivel@tue.nl)
 */

#ifndef BLOCKS_FU_HH
#define BLOCKS_FU_HH

#include <memory>

#include "BlocksGCU.hh"
#include "Machine.hh"
#include "Socket.hh"

class BlocksFU {
protected:
    BlocksFU(
        TTAMachine::Machine& mach, const std::string& name,
        std::list<std::string> sources,
        std::shared_ptr<TTAMachine::Socket> in1sock,
        std::shared_ptr<TTAMachine::Socket> in2sock);
    ~BlocksFU() {
        delete in1;
        delete in2;
        delete out;
        delete outsock;
        delete fu;
    }

public:
    std::vector<TTAMachine::HWOperation*> ops;
    TTAMachine::FunctionUnit* fu;
    TTAMachine::FUPort* in1;
    TTAMachine::FUPort* in2;
    TTAMachine::FUPort* out;
    std::shared_ptr<TTAMachine::Socket> in1sock;
    std::shared_ptr<TTAMachine::Socket> in2sock;
    TTAMachine::Socket* outsock;
    std::list<std::string> sources;
    std::string name;
};
#endif
