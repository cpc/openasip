/**
 * @file OperationSimulator.cc
 *
 * Definition of OperationSimulator class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
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
 * Execute the late result command.
 *
 * @param op Operation to be simulated.
 * @param inputs Inputs of the operation.
 * @param outputs Output values of the operation.
 * @param context Context for the operation.
 * @param bitWidth Bit width of the operands.
 * @param result Possible error string.
 * @return True if execution is successful.
 */
bool
OperationSimulator::lateResult(
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
    const bool ready = op.lateResult(io_, context);
    delete[] io_;
    io_ = NULL;
    return ready;
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
        value.find("f") != string::npos ||
        value.find("F") != string::npos;

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
