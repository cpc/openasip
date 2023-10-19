/*
    Copyright (c) 2002-2020 Tampere University.

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
 * @file ExplorationTools.hh
 *
 * Declaration of ExplorationTools class that includes some helper functions
 * used in automated design space exploration.
 *
 * @author Alex Hirvonen (alex.hirvonen-no.spam-gmail.com)
 * @note rating: red
 */

#ifndef TTA_EXPLORATION_TOOLS_HH
#define TTA_EXPLORATION_TOOLS_HH

#include "Machine.hh"
#include "Operation.hh"

using namespace TTAMachine;

class ExplorationTools {
public:
    static FunctionUnit* createOperationFU(
        Machine* mach, const Operation& osalOp);

    static void connectPortToBus(
        Port* port, Bus* bus, Socket::Direction direction);

    static std::vector<Operation> getArchOperations(Machine* mach);
    static int opCycleStart(TCEString opName, bool isReadCycle);
    static std::vector<TCEString> sortRFsByWidth(Machine* mach);
};

#endif
