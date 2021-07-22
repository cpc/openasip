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
 * @file BlocksIMM.cc
 *
 * Implementation of BlocksIMM Class.
 *
 * @author Maarten Molendijk 2020 (m.j.molendijk@tue.nl)
 * @author Kanishkan Vadivel 2021 (k.vadivel@tue.nl)
 */

#include "BlocksIMM.hh"

using namespace TTAMachine;

/**
 * BlocksIMM with default instructions
 *
 * @param mach The TTA machine where the IU needs to be added.
 * @param name The name of the IU.
 * @param sources A list of sources that are attached to this unit's input.
 */
BlocksIMM::BlocksIMM(
    Machine& mach, const std::string& name, std::list<std::string> sources) {
    // IU property constants
    unsigned int size = 1;
    unsigned int width = 32;
    unsigned int maxReads = 1;
    unsigned int guardLatency = 1;
    Machine::Extension ext = Machine::Extension::ZERO;

    // Create object
    iu = new ImmediateUnit(name, size, width, maxReads, guardLatency, ext);
    this->sources = sources;
    out0 = new RFPort("out0", *iu);
    mach.addImmediateUnit(*iu);

    // Create sockets
    out0sock = new Socket(name + "_out0");
    mach.addSocket(*out0sock);
    out0->attachSocket(*out0sock);
}
