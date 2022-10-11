/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file NetlistWriter.cc
 *
 * Implementation of NetlistWriter class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Henry Linjamäki 2015 (henry.linjamaki-no.spam.tut.fi)
 * @note rating: red
 */

#include <string>
#include "NetlistWriter.hh"

using std::string;

namespace ProGe {

/**
 * The constructor.
 *
 * @param netlist The netlist to be written in some HDL.
 */
NetlistWriter::NetlistWriter(const BaseNetlistBlock& netlistBlock)
    : netlistBlock_(netlistBlock) {
}


/**
 * The destructor.
 */
NetlistWriter::~NetlistWriter() {
}


/**
 * Returns the netlist.
 *
 * @return The netlist.
 */
const BaseNetlistBlock&
NetlistWriter::targetNetlistBlock() const {
    return netlistBlock_;
}
}
