/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file SimulatorToolbox.cc
 *
 * Definition of SimulatorToolbox class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <cstddef>

#include "boost/regex.hpp"

#include "Application.hh"
#include "SimulatorTextGenerator.hh"
#include "SimulatorToolbox.hh"
#include "OperationPool.hh"
#include "SimulationEventHandler.hh"

OperationPool* SimulatorToolbox::pool_(NULL);
SimulatorTextGenerator SimulatorToolbox::textGenerator_;

/**
 * Constructor.
 */
SimulatorToolbox::SimulatorToolbox() {
}

/**
 * Returns the instance of OperationPool.
 *
 * @return The instance of OperationPool.
 */
OperationPool&
SimulatorToolbox::operationPool() {
    if (pool_ == NULL) {
        pool_ = new OperationPool();
    }
    return *pool_;
}

/**
 * Returns the instance of SimulatorTextGenerator.
 *
 * @return The instance of SimulatorTextGenerator.
 */
SimulatorTextGenerator&
SimulatorToolbox::textGenerator() {
    return textGenerator_;
}

/**
 * Returns a regular expression to parse a sequential register 
 * string.
 *
 * @return The regular expression.
 */
boost::regex
SimulatorToolbox::sequentialRegisterRegex() {
    return boost::regex("([rR]|[fF])([0-9]+)");
}

/**
 * Returns a regular expression to parse a FU port string.
 *
 * @return The regular expression.
 */
boost::regex
SimulatorToolbox::fuPortRegex() {
    return boost::regex("(.*)\\.(.*)");
}
