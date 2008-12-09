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
 * Definition of OperationSimulator class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "Application.hh"
#include "OperationSimulator.hh"
#include "SimValue.hh"
#include "Conversion.hh"

using std::string;
using std::vector;

OperationSimulator* OperationSimulator::instance_ = NULL;

/**
 * Constructor.
 */
OperationSimulator::OperationSimulator() {
}

/**
 * Destructor.
 */
OperationSimulator::~OperationSimulator() {
}

/**
 * Returns the unique instance of OperationSimulator.
 *
 * @return The instance of OperationSimulator.
 */
OperationSimulator&
OperationSimulator::instance() {
    if (instance_ == NULL) {
        instance_ = new OperationSimulator();
    }
    return *instance_;
}

/**
 * Executes the operation trigger command.
 *
 * @param op Operation to be simulated.
 * @param inputs The inputs for the operation.
 * @param outputs The outputs of the operation.
 * @param context OperationContext used in simulation.
 * @param bitWidth Bit width of the operands.
 * @param result The possible error string.
 * @return True if simulation is successful.
 */
bool
OperationSimulator::simulateTrigger(
    Operation& op,
    std::vector<DataObject> inputs,
    std::vector<SimValue*>& outputs,
    OperationContext& context,
    unsigned int bitWidth,
    std::string& result) {

    if (static_cast<unsigned>(op.numberOfInputs()) != inputs.size()) {
        result = "wrong number of arguments";
        return false;
    }

    if (!initializeOutputs(op, inputs, outputs, bitWidth, result)) {
        return false;
    }

    SimValue** io_ = new SimValue*[outputs.size()];
    for (std::size_t i = 0; i < outputs.size(); ++i) {
        io_[i] = outputs.at(i);
    }
    try {
        bool ready = op.simulateTrigger(io_, context);
        delete[] io_;
        io_ = NULL;
        return ready;
    } catch (const Exception& e) {
        result = e.errorMessage();
        return false;
    }
}

/**
 * Initializes one SimValue.
 *
 * @param value Value.
 * @param sim SimValue to be initialized.
 * @param result Possible error string.
 */
bool
OperationSimulator::initializeSimValue(
    std::string value,
    SimValue* sim,
    std::string& result) {

    bool is_int = value.find(".") == string::npos;
    bool is_float = value.find(".") != string::npos &&
        (value.find("f") != string::npos ||
        value.find("F") != string::npos);

    try {
        if (is_int && !is_float) {
            *sim = Conversion::toInt(value);
        } else if (is_float) {
            *sim = Conversion::toFloat(value.substr(0, value.length()-1));
        } else {
            *sim = Conversion::toDouble(value);
        }
    } catch (const NumberFormatException& n) {
        result = "illegal operand value \"" + value + "\"";
        return false;
    }
    return true;
}

/**
 * Initializes the output values of the operation.
 *
 * @param op Operation in which outputs are initialized.
 * @param inputs Input values.
 * @param outputs Output values to be initialized.
 * @param bitWidth Bit width of the operands.
 * @param result Possible error message string.
 * @return True if initialization is successful.
 */
bool
OperationSimulator::initializeOutputs(
    Operation& op,
    std::vector<DataObject> inputs,
    std::vector<SimValue*>& outputs,
    unsigned int bitWidth,
    std::string& result) {

    for (size_t i = 0; i < inputs.size(); i++) {
        SimValue* sim = new SimValue(bitWidth);

        string value = inputs[i].stringValue();

        if (!initializeSimValue(value, sim, result)) {
            return false;
        }

        outputs.push_back(sim);
    }

    for (int i = 0; i < op.numberOfOutputs(); i++) {
        SimValue* result = new SimValue(bitWidth);
        outputs.push_back(result);
    }
    return true;
}
