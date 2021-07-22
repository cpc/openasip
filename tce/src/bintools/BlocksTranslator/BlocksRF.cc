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
 * @file BlocksRF.cc
 *
 * Implementation of BlocksRF Class.
 *
 * @author Maarten Molendijk 2020 (m.j.molendijk@tue.nl)
 * @author Kanishkan Vadivel 2021 (k.vadivel@tue.nl)
 */
#include "BlocksRF.hh"

using namespace TTAMachine;
/**
 * Default blocks Regierfile model
 *
 * @param mach The TTA machine where the RF needs to be added.
 * @param name The name of the RF.
 * @param sources A list of sources that are attached to this unit's input.
 */
BlocksRF::BlocksRF(
    Machine& mach, const std::string& name, std::list<std::string> sources) {
    // Rf property constants
    unsigned int size = 16;
    // fixed for now, TODO: needs to be variable for mixed precision?
    unsigned int width = 32;
    unsigned int maxReads = 1;
    unsigned int maxWrites = 1;
    unsigned int guardLatency = 0;
    TTAMachine::RegisterFile::Type type =
        TTAMachine::RegisterFile::Type::NORMAL;

    rf = new RegisterFile(
        name, size, width, maxReads, maxWrites, guardLatency, type);
    this->sources = sources;
    in1 = new RFPort("in1", *rf);
    out1 = new RFPort("out1", *rf);
    mach.addRegisterFile(*rf);

    // Create sockets
    in1sock = new Socket(name + "_in1t");
    out1sock = new Socket(name + "_out1");
    mach.addSocket(*in1sock);
    mach.addSocket(*out1sock);
    in1->attachSocket(*in1sock);
    out1->attachSocket(*out1sock);
}
