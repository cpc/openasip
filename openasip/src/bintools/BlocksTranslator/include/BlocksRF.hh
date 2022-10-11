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
 * @file BlocksRF.hh
 *
 * Declaration of BlocksRF Class.
 *
 * @author Maarten Molendijk 2020 (m.j.molendijk@tue.nl)
 * @author Kanishkan Vadivel 2021 (k.vadivel@tue.nl)
 */

#ifndef BLOCKS_RF_HH
#define BLOCKS_RF_HH

#include "HWOperation.hh"
#include "Machine.hh"
#include "Socket.hh"
#include "SpecialRegisterPort.hh"

class BlocksRF {
public:
    TTAMachine::RegisterFile* rf;
    BlocksRF(
        TTAMachine::Machine& mach, const std::string& name,
        std::list<std::string> sources);
    TTAMachine::RFPort* in1;
    TTAMachine::RFPort* out1;
    TTAMachine::Socket* in1sock;
    TTAMachine::Socket* out1sock;
    std::list<std::string> sources;

    ~BlocksRF() {
        delete out1;
        delete in1;
        delete in1sock;
        delete out1sock;
        delete rf;
    }
};
#endif
