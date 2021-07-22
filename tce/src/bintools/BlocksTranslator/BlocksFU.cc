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
 * @file BlocksFU.cc
 *
 * Implementation of the virtual BlocksFU Class.
 * This class is the parent class of ALU, LSU & MUL units.
 *
 * @author Maarten Molendijk 2020 (m.j.molendijk@tue.nl)
 * @author Kanishkan Vadivel 2021 (k.vadivel@tue.nl)
 */

#include "BlocksFU.hh"

#include "Segment.hh"

using namespace TTAMachine;

/**
 * This constructor should not be called directly.
 */
BlocksFU::BlocksFU(
    TTAMachine::Machine& mach, const std::string& name,
    std::list<std::string> sources,
    std::shared_ptr<TTAMachine::Socket> in1sock,
    std::shared_ptr<TTAMachine::Socket> in2sock)
    : in1sock(in1sock), in2sock(in2sock) {
    // Function unit and ports
    fu = new FunctionUnit(name);
    this->name = name;
    this->sources = sources;

    // output port suffix
    std::string outputSuffix = name.substr(name.find("_") + 1);

    in1 = new FUPort("in1t", 32, *fu, true, true, true);
    in2 = new FUPort("in2", 32, *fu, false, false, true);
    out = new FUPort(outputSuffix, 32, *fu, false, false, false);

    // Add to machine
    mach.addFunctionUnit(*(this->fu));

    // Create output socket
    outsock = new Socket(name);
    mach.addSocket(*outsock);
    in1->attachSocket(*(in1sock.get()));
    in2->attachSocket(*(in2sock.get()));
    out->attachSocket(*outsock);
}
