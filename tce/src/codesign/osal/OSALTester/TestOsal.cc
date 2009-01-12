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
 * @file TestOsal.cc
 *
 * Implementation of test_osal.
 *
 * @author Jussi Nyk�nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J��skel�inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <iostream>

#include "TestOsal.hh"
#include "Operation.hh"
#include "Conversion.hh"
#include "SimValue.hh"
#include "Application.hh"
#include "LineReader.hh"
#include "LineReaderFactory.hh"
#include "CmdHelp.hh"
#include "StringTools.hh"
#include "InterpreterContext.hh"
#include "OperationSimulator.hh"
#include "IdealSRAM.hh"
#include "BaseType.hh"
#include "TCEString.hh"
#include "tce_config.h"

using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::cerr;

//////////////////////////////////////////////////////////////////////////////
// CmdTrigger
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 */
CmdTrigger::CmdTrigger() : CustomCommand("trigger") {
}

/**
 * Copy constructor.
 */
CmdTrigger::CmdTrigger(const CmdTrigger& cmd) : CustomCommand(cmd) {
}

/**
 * Destructor.
 */
CmdTrigger::~CmdTrigger() {
}

/**
 * Executes the trigger command of an operation.
 *
 * @param arguments The arguments for the command.
 * @return True if the command is executed succesfully, otherwise false.
 * @exception NumberFormatException If data object conversion fails.
 */
bool
CmdTrigger::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {

    ScriptInterpreter* scriptInterp = interpreter();
    OsalInterpreter* interp = dynamic_cast<OsalInterpreter*>(scriptInterp);
    assert(interp != NULL);
    DataObject* obj = new DataObject();

    if (arguments.size() < 2) {
        obj->setString("too few arguments");
        interp->setResult(obj);
        return false;
    }

    string opName = arguments[1].stringValue();

    Operation& op = interp->operation(opName);

    if (&op == &NullOperation::instance()) {
        obj->setString("unknown operation \"" + opName + "\"");
        interp->setResult(obj);
        return false;
    }

    // remove "trigger" and operation name.
    vector<DataObject> inputs;
    for (size_t i = 2; i < arguments.size(); i++) {
        inputs.push_back(arguments[i]);
    }

    vector<SimValue*> args;
    TesterContext& context = *dynamic_cast<TesterContext*>(&interp->context());
    unsigned int bitWidth = context.bitWidth();
    OperationContext& opContext = context.operationContext();

    string result = "";

    OperationSimulator& simulator = OperationSimulator::instance();
    InstructionAddress oldPC = opContext.programCounter();
    if (!simulator.simulateTrigger(
            op, inputs, args, opContext, bitWidth, result)) {

        for (unsigned int i = 0; i < args.size(); i++) {
            delete args[i];
        }
        obj->setString(result);
        interp->setResult(obj);
        return false;

    } else {

        for (int i = 0; i < op.numberOfOutputs(); i++) {

            SimValue* current = args[op.numberOfInputs() + i];
            string output = context.toOutputFormat(current);
            result += output;

            // put blank everywhere else but after the last output
            if (i < op.numberOfOutputs() - 1) {
                result += " ";
            }
        }
    }

    for (unsigned int i = 0; i < args.size(); i++) {
        delete args[i];
    }

    if (opContext.saveReturnAddress()) {
        opContext.returnAddress() = oldPC;
    }
    obj->setString(result);
    interp->setResult(obj);
    return true;
}

/**
 * Returns the help text of trigger command.
 *
 * @return The help text.
 */
string
CmdTrigger::helpText() const {
    return "trigger <operation> <operand>...";
}

//////////////////////////////////////////////////////////////////////////////
// CmdReset
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 */
CmdReset::CmdReset() : CustomCommand("reset") {
}

/**
 * Copy constructor.
 *
 * @param cmd Command to be copied.
 */
CmdReset::CmdReset(const CmdReset& cmd) : CustomCommand(cmd) {
}

/**
 * Destructor
 */
CmdReset::~CmdReset() {
}

/**
 * Executes the command.
 *
 * @param arguments Arguments for the command.
 * @return True if the command is executed succesfully, otherwise false.
 */
bool
CmdReset::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {

    ScriptInterpreter* scriptInterp = interpreter();
    OsalInterpreter* interp = dynamic_cast<OsalInterpreter*>(scriptInterp);
    assert(interp != NULL);

    DataObject* result = new DataObject();
    if (arguments.size() != 2) {
        result->setString("wrong number of arguments");
        interp->setResult(result);
	return false;
    }

    string opName = arguments[1].stringValue();

    Operation& op = interp->operation(opName);

    if (&op == &NullOperation::instance()) {
        result->setString("unknown operation \"" + opName + "\"");
        interp->setResult(result);
	return false;
    }

    TesterContext& context = *dynamic_cast<TesterContext*>(&interp->context());
    OperationContext& opContext = context.operationContext();

    OperationBehavior& behavior = op.behavior();
    behavior.deleteState(opContext);
    result->setString("");
    interp->setResult(result);
    return true;
}

/**
 * Returns the help text of the command.
 *
 * @return The help text.
 */
string
CmdReset::helpText() const {
    return "Resets the state of the operation.\n\n"
        "!reset <command name>";
}

//////////////////////////////////////////////////////////////////////////////
// CmdOutput
//////////////////////////////////////////////////////////////////////////////

const string CmdOutput::OUTPUT_FORMAT_INT_SIGNED = "signed";
const string CmdOutput::OUTPUT_FORMAT_INT_UNSIGNED = "unsigned";
const string CmdOutput::OUTPUT_FORMAT_DOUBLE = "double";
const string CmdOutput::OUTPUT_FORMAT_FLOAT = "float";
const string CmdOutput::OUTPUT_FORMAT_BIN = "bin";
const string CmdOutput::OUTPUT_FORMAT_HEX = "hex";

/**
 * Constructor.
 */
CmdOutput::CmdOutput() : CustomCommand("output") {
}

/**
 * Copy constructor.
 *
 * @param cmd Command to be copied.
 */
CmdOutput::CmdOutput(const CmdOutput& cmd) : CustomCommand(cmd) {
}

/**
 * Destructor
 */
CmdOutput::~CmdOutput() {
}

/**
 * Executes the command.
 *
 * @param arguments Arguments for the command.
 * @return True if the command is executed succesfully, otherwise false.
 */
bool
CmdOutput::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {

    ScriptInterpreter* scriptInterp = interpreter();
    OsalInterpreter* interp = dynamic_cast<OsalInterpreter*>(scriptInterp);
    assert(interp != NULL);
    DataObject* result = new DataObject();

    if (arguments.size() != 2) {
        result->setString("wrong number of arguments");
        interp->setResult(result);
        return false;
    }

    string outputFormat = arguments[1].stringValue();

    if (outputFormat == OUTPUT_FORMAT_INT_SIGNED ||
        outputFormat == OUTPUT_FORMAT_INT_UNSIGNED ||
        outputFormat == OUTPUT_FORMAT_DOUBLE ||
        outputFormat == OUTPUT_FORMAT_FLOAT ||
        outputFormat == OUTPUT_FORMAT_BIN ||
        outputFormat == OUTPUT_FORMAT_HEX) {

        TesterContext& context =
            *dynamic_cast<TesterContext*>(&interp->context());
        context.setOutputFormat(outputFormat);
        result->setString("");
        interp->setResult(result);
    } else {
        result->setString("illegal output format \"" +
                          outputFormat +"\"");
        interp->setResult(result);
	return false;
    }

    return true;
}

/**
 * Returns the help text of the command.
 *
 * @return The help text.
 */
string
CmdOutput::helpText() const {

    ScriptInterpreter* scriptInterp = interpreter();
    OsalInterpreter* interp = dynamic_cast<OsalInterpreter*>(scriptInterp);
    TesterContext& context =
        *dynamic_cast<TesterContext*>(&interp->context());

    return
        "Changes the format of the output between "
        "int, double, float, bin and hex.\n"
        "Currently \"" +
        context.outputFormat() +
        "\" is used.\n\n"
        "!output {" +
        OUTPUT_FORMAT_INT_SIGNED + "|" +
        OUTPUT_FORMAT_INT_UNSIGNED + "|" +
        OUTPUT_FORMAT_DOUBLE + "|" +
        OUTPUT_FORMAT_FLOAT + "|" +
        OUTPUT_FORMAT_BIN + "|" +
        OUTPUT_FORMAT_HEX + "}";
}

//////////////////////////////////////////////////////////////////////////////
// OsalInterpreter
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 */
OsalInterpreter::OsalInterpreter() : SimpleScriptInterpreter() {
    pool_ = new OperationPool();
    operation_ = &NullOperation::instance();
}

/**
 * Destructor.
 */
OsalInterpreter::~OsalInterpreter() {
    delete pool_;
}

/**
 * Loads an operation and return a reference to it.
 *
 * @param name The name of the operation.
 * @return The reference to operation.
 */
Operation&
OsalInterpreter::operation(const std::string& name) {

    if (operation_ == &NullOperation::instance() ||
        operation_->name() != name) {
        operation_ = &(pool_->operation(name.c_str()));
    }

    TesterContext& testCont = *dynamic_cast<TesterContext*>(&context());
    OperationContext& opContext = testCont.operationContext();
    if (operation_ != &NullOperation::instance()) {
        OperationBehavior& beh = operation_->behavior();
        beh.createState(opContext);
    }

    return *operation_;
}

//////////////////////////////////////////////////////////////////////////////
// TesterContext
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 */
TesterContext::TesterContext() :
    InterpreterContext(), continue_(true), programCounterStorage_(0),
    outputFormat_(CmdOutput::OUTPUT_FORMAT_INT_SIGNED),
    bitWidth_(32),
    opContext_(NULL, programCounterStorage_, returnAddressStorage_) {

}

/**
 * Destructor.
 */
TesterContext::~TesterContext() {
}

/**
 * Sets the continue_ flag off.
 */
void
TesterContext::stop() {
    continue_ = false;
}

/**
 * Return true, if program can continue, false otherwise.
 *
 * @return True if program can continue, false otherwise.
 */
bool
TesterContext::cont() const {
    return continue_;
}

/**
 * Returns the operation context shared with all operations invoked in
 * the application.
 *
 * @return The operations context shared with all operations invoked
 *         in the application.
 */
OperationContext&
TesterContext::operationContext() {
    return opContext_;
}

/**
 * Returns the output format currently in use.
 *
 * @return The output format currently in use.
 */
string
TesterContext::outputFormat() const {
    return outputFormat_;
}

/**
 * Sets the output format to be used in the future.
 *
 * @param The output format to be used in the future.
 */
void
TesterContext::setOutputFormat(string outputFormat) {
    outputFormat_ = outputFormat;
}

/**
 * Converts given SimValue to output string.
 *
 * @param value Value to be converted.
 * @return The value of SimValue as a string.
 */
string
TesterContext::toOutputFormat(SimValue* value) {
    string output = "";
    if (outputFormat_ == CmdOutput::OUTPUT_FORMAT_INT_SIGNED) {
        output = Conversion::toString(value->intValue());

    } else if(outputFormat_ == CmdOutput::OUTPUT_FORMAT_INT_UNSIGNED) {
        output = Conversion::toString(value->unsignedValue());

    } else if(outputFormat_ == CmdOutput::OUTPUT_FORMAT_DOUBLE) {
        DoubleWord doubleWord = value->doubleWordValue();
        output = Conversion::toString(doubleWord);

    } else if(outputFormat_ == CmdOutput::OUTPUT_FORMAT_FLOAT) {
        FloatWord floatWord = value->floatWordValue();
        output = Conversion::toString(floatWord);

    } else if(outputFormat_ == CmdOutput::OUTPUT_FORMAT_BIN) {
        output = Conversion::toBinString(value->intValue());

    } else if(outputFormat_ == CmdOutput::OUTPUT_FORMAT_HEX) {
        output = Conversion::toHexString(value->unsignedValue());
    }
    return output;
}

/**
 * Returns the bit width of the operands.
 *
 * @return Bit width.
 */
unsigned int
TesterContext::bitWidth() {
    return bitWidth_;
}

/**
 * Sets the bit width to new value.
 *
 * @param bitWidth The new bit width of the operands.
 */
void
TesterContext::setBitWidth(unsigned int bitWidth) {
    bitWidth_ = bitWidth;
}

/**
 * Returns the program counter as SimValue.
 *
 * @return Program counter as SimValue.
 */
InstructionAddress&
TesterContext::programCounter() {
    return opContext_.programCounter();
}

/**
 * Returns the return address as SimValue.
 *
 * @return Return address as SimValue.
 */
SimValue&
TesterContext::returnAddress() {
    return opContext_.returnAddress();
}


//////////////////////////////////////////////////////////////////////////////
// CmdQuit
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 */
CmdQuit::CmdQuit() : CustomCommand("quit") {
}

/**
 * Copy constructor.
 *
 * @param cmd Command to be copied.
 */
CmdQuit::CmdQuit(const CmdQuit& cmd) : CustomCommand(cmd) {
}

/**
 * Destructor.
 */
CmdQuit::~CmdQuit() {
}

/**
 * Executes the command.
 *
 * The execution of the quit command will stop the program.
 *
 * @param arguments Arguments for the command.
 * @exception NumberFormatException If DataObject conversion fails.
 * @return True if execution is successful, false otherwise.
 */
bool
CmdQuit::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {

    ScriptInterpreter* scriptInterp = interpreter();
    OsalInterpreter* interp = dynamic_cast<OsalInterpreter*>(scriptInterp);
    DataObject* result = new DataObject();
    assert(interp != NULL);

    if (arguments.size() != 1) {
        result->setString("wrong number of arguments");
        interp->setResult(result);
	return false;
    }

    InterpreterContext& interpCont = interp->context();
    TesterContext& testCont = *dynamic_cast<TesterContext*>(&interpCont);
    testCont.stop();
    result->setString("");
    interp->setResult(result);
    return true;
}

/**
 * Returns the help text of the command.
 *
 * @return The help text.
 */
string
CmdQuit::helpText() const {
    return "exits the program";
}

//////////////////////////////////////////////////////////////////////////////
// CmdRegister
//////////////////////////////////////////////////////////////////////////////

const string CmdRegister::REGISTER_PROGRAM_COUNTER = "programcounter";
const string CmdRegister::REGISTER_RETURN_ADDRESS = "returnaddress";

/**
 * Constructor.
 */
CmdRegister::CmdRegister() : CustomCommand("register") {
}

/**
 * Copy constructor.
 *
 * @param cmd Command to be copied.
 */
CmdRegister::CmdRegister(const CmdRegister& cmd) : CustomCommand(cmd) {
}

/**
 * Destructor.
 */
CmdRegister::~CmdRegister() {
}

/**
 * Executes the command.
 *
 * Output is the value of given register.
 *
 * @param arguments Arguments for the command.
 * @return True if the execution is successful, false otherwise.
 * @exception NumberFormatException If DataObject conversion fails.
 */
bool
CmdRegister::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {

    ScriptInterpreter* scriptInterp = interpreter();
    OsalInterpreter* interp = dynamic_cast<OsalInterpreter*>(scriptInterp);
    assert(interp != NULL);
    DataObject* result = new DataObject();

    if (arguments.size() != 2) {
        result->setString("wrong number of arguments");
        interp->setResult(result);
        return false;
    }

    string registerValue = arguments[1].stringValue();

    TesterContext& context =
        *(dynamic_cast<TesterContext*>(&interp->context()));

    string output = "";

    if (registerValue == REGISTER_PROGRAM_COUNTER) {
        InstructionAddress& addr = context.programCounter();
        SimValue value(32);
        value = addr;
        output = context.toOutputFormat(&value);
    } else if (registerValue == REGISTER_RETURN_ADDRESS) {
        SimValue& addr = context.returnAddress();
        SimValue value(32);
        value = addr;
        output = context.toOutputFormat(&value);
    } else {
        result->setString("illegal register name \"" +
                          registerValue + "\"");
        interp->setResult(result);
        return false;
    }

    result->setString(output);
    interp->setResult(result);
    return true;
}

/**
 * Returns the help text of the command.
 *
 * @return The help text.
 */
string
CmdRegister::helpText() const {
    return
        "Prints the contents of the register. Register is one of the "
        "following:\n{" +
        REGISTER_PROGRAM_COUNTER + "|" +
        REGISTER_RETURN_ADDRESS + "}";
}

//////////////////////////////////////////////////////////////////////////////
// CmdRegister
//////////////////////////////////////////////////////////////////////////////

const string CmdMem::MEM_BYTE = "byte";
const string CmdMem::MEM_HALF_WORD = "halfword";
const string CmdMem::MEM_WORD = "word";
const string CmdMem::MEM_DOUBLE_WORD = "double";

/**
 * Constructor.
 */
CmdMem::CmdMem() : CustomCommand("mem") {
}

/**
 * Copy constructor.
 *
 * @param cmd Command to be copied.
 */
CmdMem::CmdMem(const CmdMem& cmd) : CustomCommand(cmd) {
}

/**
 * Destructor.
 */
CmdMem::~CmdMem() {
}

/**
 * Executes the command.
 *
 * @param arguments Arguments for the command.
 * @return True if execution is successful, false otherwise.
 * @exception NumberFormatException If DataObject conversion fails.
 */
bool
CmdMem::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {

    ScriptInterpreter* scriptInterp = interpreter();
    OsalInterpreter* interp = dynamic_cast<OsalInterpreter*>(scriptInterp);
    assert(interp != NULL);

    DataObject* result = new DataObject();

    if (arguments.size() != 3) {
        result->setString("wrong number of arguments");
        interp->setResult(result);
        return false;
    }

    string size = arguments[1].stringValue();

    if (size != MEM_BYTE &&
        size != MEM_HALF_WORD &&
        size != MEM_WORD &&
        size != MEM_DOUBLE_WORD) {

        result->setString("invalid memory size: " + size);
        interp->setResult(result);
        return false;
    }

    TesterContext& context =
        *(dynamic_cast<TesterContext*>(&interp->context()));

    Memory& memory = context.operationContext().memory();

    Word address = 0;
    string addressString = arguments[2].stringValue();

    try {
        address = Conversion::toInt(addressString);
    } catch(const NumberFormatException& n) {
        result->setString("illegal address: " + addressString);
        interp->setResult(result);
        return false;
    }

    SimValue resultValue(32);

    try {
        if (size == MEM_BYTE) {
            UIntWord resultInt = 0;
            memory.read(address, 1, resultInt);

            // sign extension, if necessary
            if (context.outputFormat() == CmdOutput::OUTPUT_FORMAT_INT_SIGNED) {
                resultInt = (((int)(resultInt << 24)) >> 24);
            }
            resultValue = resultInt;
        } else if (size == MEM_HALF_WORD) {
            UIntWord resultInt = 0;
            memory.read(address, 2, resultInt);

            // sign extension, if necessary
            if (context.outputFormat() == CmdOutput::OUTPUT_FORMAT_INT_SIGNED) {
                resultInt = (((int)(resultInt << 16)) >> 16);
            }
            resultValue = resultInt;
        } else if (size == MEM_WORD) {
            UIntWord resultInt = 0;
            memory.read(address, 4, resultInt);
            resultValue = resultInt;
        } else if (size == MEM_DOUBLE_WORD) {
            DoubleWord result = 0.0;
            memory.read(address, result);
            resultValue = result;
        }
    } catch (const OutOfRange& o) {
        string addressString = Conversion::toString(address);
        result->setString("address " + addressString + " out of memory bounds");
        interp->setResult(result);
        return false;
    }
    string output = context.toOutputFormat(&resultValue);
    result->setString(output);
    interp->setResult(result);
    return true;
}

/**
 * Returns the help text of the command.
 *
 * @return The help text.
 */
string
CmdMem::helpText() const {
    return
        "Prints the contents of the memory.\n"
        "!mem <size> <address>\n"
        "size is one of the following:\n{" +
        MEM_BYTE + "|" + MEM_HALF_WORD + "|" + MEM_WORD + "|" +
        MEM_DOUBLE_WORD + "}";
}

//////////////////////////////////////////////////////////////////////////////
// CmdAdvanceClock
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 */
CmdAdvanceClock::CmdAdvanceClock() : CustomCommand("advanceclock") {
}

/**
 * Copy constructor.
 *
 * @param cmd Command to be copied.
 */
CmdAdvanceClock::CmdAdvanceClock(const CmdAdvanceClock& cmd) :
    CustomCommand(cmd) {
}

/**
 * Destructor.
 */
CmdAdvanceClock::~CmdAdvanceClock() {
}

/**
 * Executes the command.
 *
 * @param arguments Arguments for the command.
 * @return True if execution is successful.
 * @exception NumberFormatException If DataObject conversion fails.
 */
bool
CmdAdvanceClock::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {

    ScriptInterpreter* scriptInterp = interpreter();
    OsalInterpreter* interp = dynamic_cast<OsalInterpreter*>(scriptInterp);
    assert(interp != NULL);

    DataObject* result = new DataObject();

    if (arguments.size() != 1) {
        result->setString("wrong number of arguments");
        interp->setResult(result);
        return false;
    }

    TesterContext& context =
        *(dynamic_cast<TesterContext*>(&interp->context()));

    Memory& memory = context.operationContext().memory();
    memory.advanceClock();
    context.operationContext().advanceClock();
    result->setString("");
    interp->setResult(result);
    return true;
}

/**
 * Returns the help text of the command.
 *
 * @return The help text.
 */
string
CmdAdvanceClock::helpText() const {
    return "Advances clock per one cycle";
}

//////////////////////////////////////////////////////////////////////////////
// CmdBitWidth
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 */
CmdBitWidth::CmdBitWidth() : CustomCommand("bitwidth") {
}

/**
 * Copy constructor.
 *
 * @param cmd Command to be copied.
 */
CmdBitWidth::CmdBitWidth(const CmdBitWidth& cmd) : CustomCommand(cmd) {
}

/**
 * Destructor.
 */
CmdBitWidth::~CmdBitWidth() {
}

/**
 * Executes the command.
 *
 * @param arguments Arguments for the command.
 * @return True if execution is successful, false otherwise.
 * @exception NumberFormatException If DataObject conversion fails.
 */
bool
CmdBitWidth::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {

    ScriptInterpreter* scriptInterp = interpreter();
    OsalInterpreter* interp = dynamic_cast<OsalInterpreter*>(scriptInterp);
    assert(interp != NULL);

    DataObject* result = new DataObject();

    if (arguments.size() != 2) {
        result->setString("wrong number of arguments");
        interp->setResult(result);
        return false;
    }

    int width = arguments[1].integerValue();

    TesterContext& context =
        *(dynamic_cast<TesterContext*>(&interp->context()));
    context.setBitWidth(width);
    result->setString("");
    interp->setResult(result);
    return true;
}

/**
 * Returns the help text of the command.
 *
 * @return The help text.
 */
string
CmdBitWidth::helpText() const {
    return "!bitwidth <value>\n"
        "Sets new operand bit width.";
}

//////////////////////////////////////////////////////////////////////////////
// OsalCmdLineOptions
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 */
OsalCmdLineOptions::OsalCmdLineOptions() : CmdLineOptions("") {
}

/**
 * Destructor.
 */
OsalCmdLineOptions::~OsalCmdLineOptions() {
}

/**
 * Prints the version of the program.
 */
void
OsalCmdLineOptions::printVersion() const {
    cout << "test_osal - OSAL Tester "
         << Application::TCEVersionString() << endl;
}

/**
 * Prints the help menu of the program.
 */
void
OsalCmdLineOptions::printHelp() const {
    string helpText = "test_osal\n\tcommands:\n";
    helpText += "\t<operation> param...: Executes operation with parameters.\n";
    helpText += "\t!quit: Quits the execution of the program.\n";
    cout << helpText << endl;
}

// function declaration
void executeCommand(OsalInterpreter& interpreter, string command);

/**
 * Main program.
 */
int main(int argc, char* argv[]) {

    OsalCmdLineOptions options;
    try {
        options.parse(argv, argc);
    } catch (ParserStopRequest) {
        return EXIT_SUCCESS;
    } catch (const IllegalCommandLine& i) {
        cerr << i.errorMessage() << endl;
        return EXIT_FAILURE;
    }

    OsalInterpreter interpreter;
    LineReader* reader = LineReaderFactory::lineReader();
    TesterContext context;
    CmdTrigger* trigger = new CmdTrigger();
    CmdHelp* help = new CmdHelp();
    CmdReset* reset = new CmdReset();
    CmdOutput* output = new CmdOutput();
    CmdQuit* quit = new CmdQuit();
    CmdRegister* reg = new CmdRegister();
    CmdMem* mem = new CmdMem();
    CmdAdvanceClock* clock = new CmdAdvanceClock();
    CmdBitWidth* bitWidth = new CmdBitWidth();

    interpreter.initialize(argc, argv, &context, reader);
    interpreter.addCustomCommand(trigger);
    interpreter.addCustomCommand(help);
    interpreter.addCustomCommand(reset);
    interpreter.addCustomCommand(output);
    interpreter.addCustomCommand(quit);
    interpreter.addCustomCommand(reg);
    interpreter.addCustomCommand(mem);
    interpreter.addCustomCommand(clock);
    interpreter.addCustomCommand(bitWidth);
    reader->initialize(">> ");

    // memory is 64k bytes
    IdealSRAM* memory = new IdealSRAM(0, 65535, 8);
    context.operationContext().setMemory(memory);

    if (options.numberOfArguments() > 0) {
        // batch mode

        string command = "trigger ";
        for (int i = 1; i <= options.numberOfArguments(); i++) {
            string token = options.argument(i);
            if (token.substr(token.size() - 1) == ";") {
                // an end of the command
                command += token.substr(0, token.size() - 1);
                executeCommand(interpreter, command);
                command = "trigger ";
            } else {
                command += token + " ";
            }
        }
        if (command != "") {
	    executeCommand(interpreter, command);
        }

    } else {
        // interactive mode
        string command = "";
        InterpreterContext& interpCont = interpreter.context();
        TesterContext& testCont = *dynamic_cast<TesterContext*>(&interpCont);
        while (testCont.cont()) {
            command = reader->readLine();
            command = StringTools::trim(command);
            if (command == "") {
                continue;
            }
            if (command.find("!") == string::npos) {
                command = "trigger " + command;
            } else {
                command = command.substr(1, command.length());
            }
            executeCommand(interpreter, command);
        }
    }
    delete reader;
    delete memory;
    interpreter.finalize();
    return EXIT_SUCCESS;
}

/**
 * Executes given command with interpreter.
 *
 * @param interpreter Interpreter which executes the command.
 * @param command The command to be executed.
 * @param print True, if result is printed.
 */
void
executeCommand(OsalInterpreter& interpreter, string command) {
    bool succeeded = interpreter.interpret(command);
    if (interpreter.result() != "") {
        if (!succeeded) {
            cout << "Error: ";
        }
        cout << interpreter.result() << endl;
    }
}
