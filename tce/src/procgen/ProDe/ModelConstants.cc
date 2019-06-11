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
 * @file ModelConstants.cc
 *
 * Definition of ModelConstants class.
 *
 * @author Tommi Rantanen 2004 (tommi.rantanen-no.spam-tut.fi)
 */

#include <string>
#include "ModelConstants.hh"
#include "RegisterFile.hh"

using std::string;

const int ModelConstants::DEFAULT_WIDTH = 32;
const int ModelConstants::DEFAULT_RF_SIZE = 8;
const int ModelConstants::DEFAULT_RF_MAX_READS = 0;
const int ModelConstants::DEFAULT_RF_MAX_WRITES = 0;
const int ModelConstants::DEFAULT_RF_GUARD_LATENCY = 0;
const int ModelConstants::DEFAULT_IU_SIZE = 4;
const int ModelConstants::DEFAULT_IU_CYCLES = 1;
const int ModelConstants::DEFAULT_IU_MAX_READS = 1;
const int ModelConstants::DEFAULT_IU_MAX_WRITES = 1;
const int ModelConstants::DEFAULT_IU_GUARD_LATENCY = 0;
const int ModelConstants::DEFAULT_AS_MAU_WIDTH = 8;
const int ModelConstants::DEFAULT_AS_MIN_ADDRESS = 0;
const int ModelConstants::DEFAULT_AS_MAX_ADDRESS = 1048575;
const int ModelConstants::DEFAULT_AS_INTERLEAVING_FACTOR = 1;

