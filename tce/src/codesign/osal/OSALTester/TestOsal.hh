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
 * @file TestOsal.hh
 *
 * Declaration of classes test_osal needs.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_TEST_OSAL_HH
#define TTA_TEST_OSAL_HH

#include <string>
#include <vector>

#include "CustomCommand.hh"
#include "Exception.hh"
#include "DataObject.hh"
#include "OperationPool.hh"
#include "SimpleScriptInterpreter.hh"
#include "CmdLineOptions.hh"
#include "OperationContext.hh"
#include "SimValue.hh"

//////////////////////////////////////////////////////////////////////////////
// CmdTrigger
//////////////////////////////////////////////////////////////////////////////

/**
 * Custom command that executes the trigger command of last loaded
 * operation.
 */
class CmdTrigger : public CustomCommand {
public:
    CmdTrigger();
    explicit CmdTrigger(const CmdTrigger& cmd);
    virtual ~CmdTrigger();

    virtual bool execute(const std::vector<DataObject>& arguments)
        throw (NumberFormatException);
    virtual std::string helpText() const;
};

//////////////////////////////////////////////////////////////////////////////
// CmdReset
//////////////////////////////////////////////////////////////////////////////

/**
 * Custom command that executes the reset command.
 *
 * Reset command resets the state of the operation.
 */
class CmdReset : public CustomCommand {
public:
    CmdReset();
    explicit CmdReset(const CmdReset& cmd);
    virtual ~CmdReset();

    virtual bool execute(const std::vector<DataObject>& arguments)
	throw (NumberFormatException);
    virtual std::string helpText() const;
};

//////////////////////////////////////////////////////////////////////////////
// CmdQuit
//////////////////////////////////////////////////////////////////////////////

/**
 * Command that quits the execution of the program.
 */
class CmdQuit : public CustomCommand {
public:
    CmdQuit();
    explicit CmdQuit(const CmdQuit& cmd);
    virtual ~CmdQuit();

    virtual bool execute(const std::vector<DataObject>& arguments)
	throw (NumberFormatException);
    virtual std::string helpText() const;
};

//////////////////////////////////////////////////////////////////////////////
// CmdOutput
//////////////////////////////////////////////////////////////////////////////

/**
 * Custom command that executes the output command.
 *
 * Output command changes the format of the output. Possible output
 * formats are decimal, binary and hexadecimal.
 */
class CmdOutput : public CustomCommand {
public:

    static const std::string OUTPUT_FORMAT_INT_SIGNED;
    static const std::string OUTPUT_FORMAT_INT_UNSIGNED;
    static const std::string OUTPUT_FORMAT_DOUBLE;
    static const std::string OUTPUT_FORMAT_FLOAT;
    static const std::string OUTPUT_FORMAT_BIN;
    static const std::string OUTPUT_FORMAT_HEX;

    CmdOutput();
    explicit CmdOutput(const CmdOutput& cmd);
    virtual ~CmdOutput();

    virtual bool execute(const std::vector<DataObject>& arguments)
        throw (NumberFormatException);
    virtual std::string helpText() const;
};

//////////////////////////////////////////////////////////////////////////////
// CmdRegister
//////////////////////////////////////////////////////////////////////////////

/**
 * Custom command for showing register values.
 *
 * There are currently two register values which can asked:
 * program counter and return address.
 */
class CmdRegister : public CustomCommand {
public:

    static const std::string REGISTER_PROGRAM_COUNTER;
    static const std::string REGISTER_RETURN_ADDRESS;

    CmdRegister();
    explicit CmdRegister(const CmdRegister& cmd);
    virtual ~CmdRegister();

    virtual bool execute(const std::vector<DataObject>& arguments)
        throw (NumberFormatException);
    virtual std::string helpText() const;
};

//////////////////////////////////////////////////////////////////////////////
// CmdMem
//////////////////////////////////////////////////////////////////////////////

/**
 * Command for viewing memory contents.
 *
 * This implementation expects that MAU is 1 byte.
 */
class CmdMem : public CustomCommand {
public:

    /// Name for byte memory access.
    static const std::string MEM_BYTE;
    /// Name for half word memory access.
    static const std::string MEM_HALF_WORD;
    /// Name for word memory access.
    static const std::string MEM_WORD;
    /// Name for double word memory access.
    static const std::string MEM_DOUBLE_WORD;

    CmdMem();
    explicit CmdMem(const CmdMem& cmd);
    virtual ~CmdMem();

    virtual bool execute(const std::vector<DataObject>& arguments)
        throw (NumberFormatException);
    virtual std::string helpText() const;
};

//////////////////////////////////////////////////////////////////////////////
// CmdAdvanceClock
//////////////////////////////////////////////////////////////////////////////

/**
 * Advances clock by one cycle.
 */
class CmdAdvanceClock : public CustomCommand {
public:
    CmdAdvanceClock();
    explicit CmdAdvanceClock(const CmdAdvanceClock& cmd);
    virtual ~CmdAdvanceClock();

    virtual bool execute(const std::vector<DataObject>& arguments)
        throw (NumberFormatException);
    virtual std::string helpText() const;
};

//////////////////////////////////////////////////////////////////////////////
// CmdBitWidth
//////////////////////////////////////////////////////////////////////////////

/**
 * Sets bit width of the operation operands.
 */
class CmdBitWidth : public CustomCommand {
public:
    CmdBitWidth();
    explicit CmdBitWidth(const CmdBitWidth& cmd);
    virtual ~CmdBitWidth();

    virtual bool execute(const std::vector<DataObject>& arguments)
        throw (NumberFormatException);
    virtual std::string helpText() const;
};

//////////////////////////////////////////////////////////////////////////////
// TesterContext.
//////////////////////////////////////////////////////////////////////////////

/**
 * Operation context for the program.
 */
class TesterContext : public InterpreterContext {
public:
    TesterContext();
    virtual ~TesterContext();
    void stop();
    bool cont() const;
    OperationContext& operationContext();
    std::string outputFormat() const;
    void setOutputFormat(std::string outputFormat);
    std::string toOutputFormat(SimValue* value);
    unsigned int bitWidth();
    void setBitWidth(unsigned int bitWidth);

    InstructionAddress& programCounter();
    SimValue& returnAddress();
    SimValue& syscallHandler();
    SimValue& syscallNumber();

private:
    /// Flag indicating whether program can continue or not.
    bool continue_;
    SimValue returnAddressStorage_;
    InstructionAddress programCounterStorage_;
    /// Indicates which output format is used currently.
    std::string outputFormat_;
    /// Bit width of the operands.
    unsigned int bitWidth_;
    /// The operation context shared with all operations invoked in
    /// the application.
    OperationContext opContext_;
};

//////////////////////////////////////////////////////////////////////////////
// OsalInterpreter
//////////////////////////////////////////////////////////////////////////////

/**
 * Interpreter for test_osal.
 */
class OsalInterpreter : public SimpleScriptInterpreter {
public:
    OsalInterpreter();
    virtual ~OsalInterpreter();

    Operation& operation(const std::string& name);

private:
    /// Used to load operations.
    OperationPool* pool_;
    /// Last loaded operation.
    Operation* operation_;
};

//////////////////////////////////////////////////////////////////////////////
// OsalCmdLineOptions
//////////////////////////////////////////////////////////////////////////////

/**
 * Command line option class for test_osal
 */
class OsalCmdLineOptions : public CmdLineOptions {
public:
    OsalCmdLineOptions();
    virtual ~OsalCmdLineOptions();

    virtual void printVersion() const;
    virtual void printHelp() const;

private:
    /// Copying not allowed.
    OsalCmdLineOptions(const OsalCmdLineOptions&);
    /// Assignment not allowed.
    OsalCmdLineOptions& operator=(const OsalCmdLineOptions&);
};

#endif
