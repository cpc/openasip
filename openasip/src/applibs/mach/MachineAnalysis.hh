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
 * @file MachineAnalysis.hh
 *
 * Declaration of MachineAnalysis class.
 *
 * @author Heikki Kultala 2008 (heikki.kultala-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef MACHINE_ANALYSIS_HH
#define MACHINE_ANALYSIS_HH

namespace TTAMachine {
    class Machine;
}

class MachineAnalysis {
public:
    MachineAnalysis(const TTAMachine::Machine& machine);

    float connectivity() const { return connectivity_; }
    float bypassability() const { return bypassability_; }
    float averageILP() const { return averageILP_; }

    float busILP() const { return busILP_; }
    float fuILP() const { return fuILP_; }
    float rfILP() const { return rfILP_; }
    float bypassedRfILP() const { return bypassedRfILP_; }
    float guardability() const { return guardability_; }
private:
    float connectivity_;
    float busILP_;
    float fuILP_;
    float rfILP_;
    float bypassedRfILP_;
    float bypassability_;
    float guardability_;

    float averageILP_;
};
#endif
