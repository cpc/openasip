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
 * @file OperationSimulator.cc
 *
 * Declaration of OperationSimulator class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OPERATION_SIMULATOR_HH
#define TTA_OPERATION_SIMULATOR_HH

#include <string>
#include <vector>

#include "DataObject.hh"

class Operation;
class OperationContext;
class SimValue;

/**
 * Contains functionality to simulate operation behavior.
 */
class OperationSimulator {
public:
    static OperationSimulator& instance();

    bool simulateTrigger(
        Operation& op,
        std::vector<DataObject> inputs,
        std::vector<SimValue*>& outputs,
        OperationContext& context,
        unsigned int bitWidth,
        std::string& result);

    bool lateResult(
        Operation& op,
        std::vector<DataObject> inputs,
        std::vector<SimValue*>& outputs,
        OperationContext& context,
        unsigned int bitWidth,
        std::string& result);

    void advanceClock(OperationContext& context);

    bool initializeSimValue(
        std::string value,
        SimValue* sim,
        std::string& result);

private:
    OperationSimulator();
    virtual ~OperationSimulator();

    bool initializeOutputs(
        Operation& op,
        std::vector<DataObject> inputs,
        std::vector<SimValue*>& outpus,
        unsigned int bitWidth,
        std::string& result);

    /// Unique instance.
    static OperationSimulator* instance_;
};

#endif
