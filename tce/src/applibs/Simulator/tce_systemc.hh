/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file tce_systemc.hh
 *
 * A SC_MODULE wrapper for a single TTA core for use in SystemC simulations.
 *
 * @author Pekka J‰‰skel‰inen 2010 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TCE_SYSTEMC_HH
#define TCE_SYSTEMC_HH

#include "SimpleSimulatorFrontend.hh"
#include "systemc.h"

SC_MODULE(TTACore) {
    sc_in<bool> clock;

    void simulateCycle() {
        ttasim_.step();
    }

    SC_HAS_PROCESS(TTACore);

    TTACore(
        sc_module_name name, 
        std::string machineFile, std::string programFile) :
        sc_module(name), ttasim_(machineFile, programFile) {
        SC_METHOD(simulateCycle);
        sensitive << clock;
    }

private:
    SimpleSimulatorFrontend ttasim_;
};

#endif
