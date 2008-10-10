/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file Exception.hh
 *
 * Declarations of exception classes.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 */

#ifndef TTA_EXCEPTION_HH
#define TTA_EXCEPTION_HH

#include <string>

///////////////////////////////////////////////////////////////////////////////
// Exception
///////////////////////////////////////////////////////////////////////////////

/**
 * Base class for all exceptions of the system.
 *
 * It contains information common to all exceptions that identifies the spot
 * in the source code in which the execption was thrown: name of the source
 * file, line number and name of the procedure. Also, it may contain an
 * error message.
 */
class Exception {
public:
    Exception(
	std::string filename, int linenum,
	std::string procname = unknownProcMsg_,
	std::string errorMessage = "");

    virtual ~Exception();

    std::string fileName() const;
    int lineNum() const;
    std::string procedureName() const;
    std::string errorMessage() const;
    std::string errorMessageStack(bool messagesOnly = false) const;

    /// Used when no procedure name is given.
    static const std::string unknownProcMsg_;

    /// Returns information of the last thrown exception.
    static std::string lastExceptionInfo();

    void setCause(const Exception &cause);
    bool hasCause() const;
    const Exception& cause() const;

private:
    /// Information of the last thrown exception for easing the debugging.
    static std::string lastExceptionInfo_;
    /// Name of the file where exception occurred.
    std::string file_;
    /// Line number in the file.
    int line_;
    /// Procedure name.
    std::string proc_;
    /// Error message
    std::string errorMessage_;
    /// Exception that caused current exception.
    const Exception *cause_;
};

///////////////////////////////////////////////////////////////////////////////
// IllegalParameters
///////////////////////////////////////////////////////////////////////////////

/**
 * IllegalParameters exception is the base class for all exceptional
 * conditions that are caused by input parameters given to the throwing
 * function.  An IllegalParameters is a valid parameter of its type (in
 * terms of value range and type sanity) that in the context in which the
 * function is called denotes a corrupted state.  IllegalParameters
 * conditions may occur when the throwing function is invoked in ways that
 * violate some constraint, such as for example, an order constraint
 * (precedence) or a logical constraint (conflict between parameter values
 * given at different invocations).
 *
 * An IllegalParameters condition is "by definition" not to be treated as an
 * assert, because the problem is typically originated by the client of the
 * throwing function.
 */
class IllegalParameters : public Exception {
public:
    IllegalParameters(
	std::string filename, int linenum,
	std::string procname = unknownProcMsg_,
	std::string errorMessage = "");
    virtual ~IllegalParameters() {};
};

///////////////////////////////////////////////////////////////////////////////
// IOException
///////////////////////////////////////////////////////////////////////////////

/**
 * Base class for exceptions which are occured when trying to
 * do reading or writing.
 */
class IOException : public Exception {
public:
    IOException(
	std::string filename, int linenum,
	std::string procname = unknownProcMsg_,
	std::string errorMessage = "");
    virtual ~IOException() {};
};

///////////////////////////////////////////////////////////////////////////////
// InvalidData
///////////////////////////////////////////////////////////////////////////////

/**
 * Base class for exceptions which occur because an object contains invalid
 * data. The object could be a parameter passed to a method (invalid
 * parameter), or an object whose a method is invoked (method incompatible
 * with the current object state).
 */
class InvalidData : public Exception {
public:
    InvalidData(
	std::string filename, int linenum,
	std::string procname = unknownProcMsg_,
	std::string errorMessage = "");
    virtual ~InvalidData() {};
};

///////////////////////////////////////////////////////////////////////////////
// UnreachableStream
///////////////////////////////////////////////////////////////////////////////

/**
 * Implements an exception which is thrown when trying to read from
 * or write to a stream that cannot be opened or appears otherwise
 * unreachable.
 *
 * It contains information about the file name, line number and
 * procedure name in which this exception was thrown, and the stream
 * name.
 */
class UnreachableStream : public IOException {
public:
    UnreachableStream(
        std::string filename, int linenum,
	std::string procname = unknownProcMsg_,
	std::string errorMessage = "");

    virtual ~UnreachableStream();
};

///////////////////////////////////////////////////////////////////////////////
// EndOfFile
///////////////////////////////////////////////////////////////////////////////

/**
 * Implements an exception which is thrown when trying to read from
 * stream when end of file has been reached.
 */
class EndOfFile : public IOException {
public:
    EndOfFile(
	std::string filename, int linenum,
	std::string procname = unknownProcMsg_,
	std::string errorMessage = "");
    virtual ~EndOfFile();
};


///////////////////////////////////////////////////////////////////////////////
// WritePastEOF
///////////////////////////////////////////////////////////////////////////////

/**
 * Implements an exception which is thrown when trying to write to
 * a stream when the write cursor has been set past the end of file.
 */
class WritePastEOF : public IOException {
public:
    WritePastEOF(
	std::string filename, int linenum,
	std::string procname = unknownProcMsg_,
	std::string errorMessage = "");
    virtual ~WritePastEOF();
};

///////////////////////////////////////////////////////////////////////////////
// FileNotFound
///////////////////////////////////////////////////////////////////////////////

/**
 * Implements an exception which is thrown when a requested file is not
 * found.
 */
class FileNotFound : public IOException {
public:
    FileNotFound(
        std::string filename,
        int linenum,
        std::string procname = unknownProcMsg_,
        std::string errorMessage = "");
    virtual ~FileNotFound();
};

///////////////////////////////////////////////////////////////////////////////
// PathNotFound
///////////////////////////////////////////////////////////////////////////////

/**
 * Implements an exception which is thrown when a requested path is not
 * found.
 */
class PathNotFound : public IOException {
public:
    PathNotFound(
        std::string filename,
        int linenum,
        std::string procname,
        std::string errorMessage,
	std::string path);
    virtual ~PathNotFound();

    std::string path() const;

private:
    std::string path_;

};


///////////////////////////////////////////////////////////////////////////////
// KeyAlreadyExists
///////////////////////////////////////////////////////////////////////////////

/**
 * Implements an exception which is thrown when trying to add an entry
 * to a reference key map that already contains an entry with given key.
 */
class KeyAlreadyExists : public IllegalParameters {
public:
    KeyAlreadyExists(
	std::string filename, int linenum,
	std::string procname = unknownProcMsg_,
	std::string errorMessage = "");
    virtual ~KeyAlreadyExists() {};
};

///////////////////////////////////////////////////////////////////////////////
// KeyNotFound
///////////////////////////////////////////////////////////////////////////////

/**
 * Implements an exception which is thrown when a requested key for
 * object is not found.
 */
class KeyNotFound : public IllegalParameters {
public:
    KeyNotFound(
	std::string filename, int linenum,
	std::string procname = unknownProcMsg_,
	std::string errorMessage = "");
    virtual ~KeyNotFound() {};
};

///////////////////////////////////////////////////////////////////////////////
// InstanceNotFound
///////////////////////////////////////////////////////////////////////////////

/**
 * Implements an exception which is thrown when can't find
 * an instance of a class that is requested.
 * For example in BinaryReader::readData, Section::createSection and
 * SectionReader::readSection.
 */
class InstanceNotFound : public IllegalParameters {
public:
    InstanceNotFound(
	std::string filename, int linenum,
	std::string procname = unknownProcMsg_,
	std::string errorMessage = "");
    virtual ~InstanceNotFound();
};

///////////////////////////////////////////////////////////////////////////////
// OutOfRange
///////////////////////////////////////////////////////////////////////////////

/**
 * Implements an exception which is thrown when something is out of range.
 */
class OutOfRange : public InvalidData {
public:
    OutOfRange(
	std::string filename, int linenum,
	std::string procname = unknownProcMsg_,
	std::string errorMessage = "");
    virtual ~OutOfRange() {};
};

///////////////////////////////////////////////////////////////////////////////
// WrongSubclass
///////////////////////////////////////////////////////////////////////////////

/**
 * Class is not capable to do that thing that was wanted.
 */
class WrongSubclass : public InvalidData {
public:
    WrongSubclass(
	std::string filename, int linenum,
	std::string procname = unknownProcMsg_,
	std::string errorMessage = "");
    virtual ~WrongSubclass() {};
};

///////////////////////////////////////////////////////////////////////////////
// NotChunkable
///////////////////////////////////////////////////////////////////////////////

/**
 * Implements an exception which is thrown when trying get chunk object
 * from section that does not override base classe's chunk() method.
 */
class NotChunkable : public WrongSubclass {
public:
    NotChunkable(
	std::string filename, int linenum,
	std::string procname = unknownProcMsg_,
	std::string errorMessage = "");
    virtual ~NotChunkable() {};
};

///////////////////////////////////////////////////////////////////////////////
// UnresolvedReference
///////////////////////////////////////////////////////////////////////////////

/**
 * Implements an exception which is thrown when reference manager
 * finds unresolvable references while trying to resolve().
 */
class UnresolvedReference : public InvalidData {
public:
    UnresolvedReference(
	std::string filename, int linenum,
	std::string procname = unknownProcMsg_,
	std::string errorMessage = "");
    virtual ~UnresolvedReference() {};
};


///////////////////////////////////////////////////////////////////////////////
// ErrorInExternalFile
///////////////////////////////////////////////////////////////////////////////

/**
 * Exception which is thrown when Schema parser finds errors in the file.
 */
class ErrorInExternalFile : public InvalidData {
public:
    ErrorInExternalFile(
	std::string filename, int linenum,
	std::string procname = unknownProcMsg_,
	std::string errorMessage = "");
    virtual ~ErrorInExternalFile();
};

///////////////////////////////////////////////////////////////////////////////
// MissingKeys
///////////////////////////////////////////////////////////////////////////////

/**
 * Exception which is thrown when trying to do replacement while writing
 * binary file and reference manager doesn't contain enough keys for
 * computing replacement value.
 */
class MissingKeys : public InvalidData {
public:
    MissingKeys(
	std::string filename, int linenum,
	std::string procname = unknownProcMsg_,
	std::string errorMessage = "");
    virtual ~MissingKeys();
};

/////////////////////////////////////////////////////////////////////////////
// NumberFormatException
/////////////////////////////////////////////////////////////////////////////
/**
 * Exception which is thrown when trying to convert an illegal number to
 * another type.
 */
class NumberFormatException : public InvalidData {
public:
    NumberFormatException(
	std::string filename, int linenum,
	std::string procname = unknownProcMsg_,
	std::string errorMessage = "");
    virtual ~NumberFormatException();
};

///////////////////////////////////////////////////////////////////////////////
// IllegalCommandLine
///////////////////////////////////////////////////////////////////////////////

/**
 * Exception which is thrown when user given command line is somehow
 * erronous.
 */
class IllegalCommandLine : public InvalidData {
public:
    IllegalCommandLine(
	std::string filename, int linenum,
	std::string procname = unknownProcMsg_,
	std::string errorMessage = "");
    virtual ~IllegalCommandLine();
};

///////////////////////////////////////////////////////////////////////////////
// UnexpectedValue
///////////////////////////////////////////////////////////////////////////////

/**
 * Exception which is thrown when user given command line is somehow
 * erronous.
 */
class UnexpectedValue : public InvalidData {
public:
    UnexpectedValue(
	std::string filename, int linenum,
	std::string procname = unknownProcMsg_,
	std::string errorMessage = "");
    virtual ~UnexpectedValue();
};


/////////////////////////////////////////////////////////////////////////////
// IllegalConnectivity
/////////////////////////////////////////////////////////////////////////////

/**
 * Exception which is thrown when tried to make something impossible because
 * of the connectivity of the object.
 */
class IllegalConnectivity : public Exception {
public:
    IllegalConnectivity(
        std::string filename,
        int linenum,
        std::string procname = unknownProcMsg_,
        std::string errorMessage = "");
    virtual ~IllegalConnectivity();
};

/////////////////////////////////////////////////////////////////////////////
// ParserStopRequest
/////////////////////////////////////////////////////////////////////////////

/**
 * Exception which is thrown when parser wants to tell the client that no
 * error is found but the client should not proceed.
 */
class ParserStopRequest : public Exception {
public:
    ParserStopRequest(
        std::string fileName,
        int lineNum,
        std::string procName = unknownProcMsg_,
        std::string errorMessage = "");
    virtual ~ParserStopRequest();
};

/////////////////////////////////////////////////////////////////////////////
// ComponentAlreadyExists
/////////////////////////////////////////////////////////////////////////////

/**
 * Exception which is thrown when trying to add or attach a machine
 * component to another component and corresponding component is already
 * attached/added.
 */
class ComponentAlreadyExists : public IllegalParameters {
public:
    ComponentAlreadyExists(
        std::string filename,
        int linenum,
        std::string procname = unknownProcMsg_,
        std::string errorMessage = "");
    virtual ~ComponentAlreadyExists();
};


/////////////////////////////////////////////////////////////////////////////
// IllegalRegistration
/////////////////////////////////////////////////////////////////////////////

/**
 * Exception which is thrown when an object that can be registered to (that
 * is, owned and managed by) another object happens to have an owner that is
 * incompatible with the operation requested. For example, an operation
 * involving two objects could throw this exception if the objects are
 * registered to different owners.
 */
class IllegalRegistration : public InvalidData {
public:
    IllegalRegistration(
        std::string filename,
        int linenum,
        std::string procname = unknownProcMsg_,
        std::string errorMessage = "");
    virtual ~IllegalRegistration();
};


/////////////////////////////////////////////////////////////////////////////
// ObjectStateLoadingException
/////////////////////////////////////////////////////////////////////////////

/**
 * An exception which is thrown by Serializable::loadState if the loading
 * failed for some reason.
 */
class ObjectStateLoadingException : public Exception {
public:
    ObjectStateLoadingException(
        std::string filename,
        int linenum,
        std::string procname = unknownProcMsg_,
        std::string errorMessage = "");
    virtual ~ObjectStateLoadingException();
};


/////////////////////////////////////////////////////////////////////////////
// NonexistingChild
/////////////////////////////////////////////////////////////////////////////

/**
 * Exception thrown when no child object is found in a graph or tree-like
 * structure where each node can have several child nodes.
 */
class NonexistingChild : public IllegalParameters {
public:
    NonexistingChild(
        std::string filename,
        int linenum,
        std::string procname = unknownProcMsg_,
        std::string errorMessage = "");
    virtual ~NonexistingChild();
};

/////////////////////////////////////////////////////////////////////////////
// DynamicLibraryException
/////////////////////////////////////////////////////////////////////////////

/**
 * Exception which is thrown when errors occurred when handling dynamic
 * libraries.
 */
class DynamicLibraryException : public Exception {
public:
    DynamicLibraryException(
        std::string filename,
        int linenum,
        std::string procname = unknownProcMsg_,
        std::string errorMessage = "");
    virtual ~DynamicLibraryException();
};

/////////////////////////////////////////////////////////////////////////////
// MultipleInstancesFound
/////////////////////////////////////////////////////////////////////////////

/**
 * Exception which is thrown when multiple instances of some kind are found.
 */
class MultipleInstancesFound : public Exception {
public:
    MultipleInstancesFound(
        std::string filename,
        int linenum,
        std::string procname = unknownProcMsg_,
        std::string errorMessage = "");
    virtual ~MultipleInstancesFound();
};


/////////////////////////////////////////////////////////////////////////////
// SymbolNotFound
/////////////////////////////////////////////////////////////////////////////

/**
 * Exception which is thrown when symbol is not found.
 */
class SymbolNotFound : public Exception {
public:
    SymbolNotFound(
        std::string filename,
        int linenum,
        std::string procname = unknownProcMsg_,
        std::string errorMessage = "");
    virtual ~SymbolNotFound();
};

/////////////////////////////////////////////////////////////////////////////
// ObjectNotInitialized
/////////////////////////////////////////////////////////////////////////////

/**
 * Exception which is thrown when object is not properly initialized.
 */
class ObjectNotInitialized : public Exception {
public:
    ObjectNotInitialized(
        std::string filename,
        int linenum,
        std::string procname = unknownProcMsg_,
        std::string errorMessage = "");
    virtual ~ObjectNotInitialized();
};

/////////////////////////////////////////////////////////////////////////////
// ScriptExecutionFailure
/////////////////////////////////////////////////////////////////////////////

/**
 * Exception which is thrown when script execution fails.
 */
class ScriptExecutionFailure : public Exception {
public:
    ScriptExecutionFailure(
        std::string filename,
        int linenum,
        std::string procname = unknownProcMsg_,
        std::string errorMessage = "");
    virtual ~ScriptExecutionFailure();
};


/////////////////////////////////////////////////////////////////////////////
// SerializerException
/////////////////////////////////////////////////////////////////////////////

/**
 * Exception which is thrown by serializers when some error occurs.
 */
class SerializerException : public Exception {
public:
    SerializerException(
        std::string fileName,
        int lineNum,
        std::string procName = unknownProcMsg_,
        std::string errorMessage = "");
    virtual ~SerializerException();
};

/////////////////////////////////////////////////////////////////////////////
// RelationalDBException
/////////////////////////////////////////////////////////////////////////////

/**
 * Exception which is thrown by methods that handle relational databases.
 */
class RelationalDBException : public Exception {
public:
    RelationalDBException(
        std::string fileName,
        int lineNum,
        std::string procName = unknownProcMsg_,
        std::string errorMessage = "");
    virtual ~RelationalDBException();
};


/////////////////////////////////////////////////////////////////////////////
// StartTooLate
/////////////////////////////////////////////////////////////////////////////

/**
 * Exception which is thrown when pipeline does not start at cycle 0 or 1.
 */
class StartTooLate : public InvalidData {
public:
    StartTooLate(
        std::string fileName,
        int lineNum,
        std::string procName = unknownProcMsg_,
        std::string errorMessage = "");
    virtual ~StartTooLate();
};


/////////////////////////////////////////////////////////////////////////////
// NotAvailable
/////////////////////////////////////////////////////////////////////////////

/**
 * Exception which is thrown when something is not available.
 */
class NotAvailable : public InvalidData {
public:
    NotAvailable(
        std::string fileName,
        int lineNum,
        std::string procName = unknownProcMsg_,
        std::string errorMessage = "");
    virtual ~NotAvailable();
};

/////////////////////////////////////////////////////////////////////////////
// CannotEstimateCost
/////////////////////////////////////////////////////////////////////////////

/**
 * Exception which is thrown when cost estimator is unable to estimate
 * a cost for given machine/machine part.
 *
 * Such case is usually due to missing cost data for the given machine part.
 */
class CannotEstimateCost : public NotAvailable {
public:
    CannotEstimateCost(
        std::string fileName,
        int lineNum,
        std::string procName = unknownProcMsg_,
        std::string errorMessage = "");
    virtual ~CannotEstimateCost();
};


/////////////////////////////////////////////////////////////////////////////
// WrongOperandType
/////////////////////////////////////////////////////////////////////////////

/**
 * Exception which is thrown when tried to use input operand as output or
 * vice versa.
 */
class WrongOperandType : public IllegalParameters {
public:
    WrongOperandType(
        std::string fileName,
        int lineNum,
        std::string procName = unknownProcMsg_,
        std::string errorMessage = "");
    virtual ~WrongOperandType();
};


/////////////////////////////////////////////////////////////////////////////
// BadOperationModule
/////////////////////////////////////////////////////////////////////////////

/**
 * Exception which is thrown when operation module is somehow invalid.
 */
class BadOperationModule : public InvalidData {
public:
    BadOperationModule(
        std::string fileName,
        int lineNum,
        std::string procName = unknownProcMsg_,
        std::string errorMessage = "");
    virtual ~BadOperationModule();
};


/////////////////////////////////////////////////////////////////////////////
// TypeMismatch
/////////////////////////////////////////////////////////////////////////////

/**
 * Exception which is thrown when types mismatch.
 */
class TypeMismatch : public InvalidData {
public:
    TypeMismatch(
        std::string fileName,
        int lineNum,
        std::string procName = unknownProcMsg_,
        std::string errorMessage = "");
    virtual ~TypeMismatch();
};


/////////////////////////////////////////////////////////////////////////////
// InvalidName
/////////////////////////////////////////////////////////////////////////////

/**
 * Exception thrown when a string of characters that represents a name
 * does not conform to the expected constraints. Typical lexical constraints
 * are clashings with reserved names (keywords) and presence of illegal
 * characters. A structural constraint may be the presence (for example, a
 * single, mandatory dot `.' to separate the basename from the extension)
 * and the order of expected characters (for example, a digit not allowed in
 * first position).
 */
class InvalidName : public InvalidData {
public:
    InvalidName(
        std::string fileName,
        int lineNum,
        std::string procName = unknownProcMsg_,
        std::string errorMessage = "");
    virtual ~InvalidName();
};

/////////////////////////////////////////////////////////////////////////////
// IllegalBehavior
/////////////////////////////////////////////////////////////////////////////

/**
 * Exception which is thrown when an illegal operation behavior is executed.
 */
class IllegalOperationBehavior : public Exception {
public:
    IllegalOperationBehavior(
        std::string fileName,
        int lineNum,
        std::string procName = unknownProcMsg_,
        std::string errorMessage = "");
    virtual ~IllegalOperationBehavior();
};

/////////////////////////////////////////////////////////////////////////////
// NonexistingSyscall
/////////////////////////////////////////////////////////////////////////////

/**
 * Exception which is thrown when nonexisting syscall is called.
 */
class NonexistingSyscall : public Exception {
public:
    NonexistingSyscall(
        std::string fileName,
        int lineNum,
        std::string procName = unknownProcMsg_,
        std::string errorMessage = "");
    virtual ~NonexistingSyscall();
};

/////////////////////////////////////////////////////////////////////////////
// IllegalMachine
/////////////////////////////////////////////////////////////////////////////

/**
 * Exception which is thrown when machine being used is somehow erronous.
 */
class IllegalMachine : public InvalidData {
public:
    IllegalMachine(
        std::string fileName,
        int lineNum,
        std::string procName = unknownProcMsg_,
        std::string errorMessage = "");
    virtual ~IllegalMachine();
};

/////////////////////////////////////////////////////////////////////////////
// IllegalProgram
/////////////////////////////////////////////////////////////////////////////

/**
 * Exception which is thrown when program being handled is somehow erronous.
 */
class IllegalProgram : public InvalidData {
public:
    IllegalProgram(
        std::string fileName,
        int lineNum,
        std::string procName = unknownProcMsg_,
        std::string errorMessage = "");
    virtual ~IllegalProgram();
};


/////////////////////////////////////////////////////////////////////////////
// SimulationException
/////////////////////////////////////////////////////////////////////////////

/**
 * Base class for exceptions thrown by simulator.
 */
class SimulationException : public Exception {
public:
    SimulationException(
        std::string fileName,
        int lineNum,
        std::string procName = unknownProcMsg_,
        std::string errorMessage = "");
    virtual ~SimulationException();
};

/////////////////////////////////////////////////////////////////////////////
// SimulationStillRunning
/////////////////////////////////////////////////////////////////////////////

/**
 * An exception which is thrown when doing something which requires the
 * simulation not to be in running state.
 */
class SimulationStillRunning : public SimulationException {
public:
    SimulationStillRunning(
        std::string fileName,
        int lineNum,
        std::string procName = unknownProcMsg_,
        std::string errorMessage = "");
    virtual ~SimulationStillRunning();
};

/////////////////////////////////////////////////////////////////////////////
// SimulationExecutionError
/////////////////////////////////////////////////////////////////////////////

/**
 * Exception which is thrown when a runtime error occurs in the simulated
 * program.
 *
 * Runtime error might be, for example, illegal memory access.
 */
class SimulationExecutionError : public SimulationException {
public:
    SimulationExecutionError(
        std::string filename,
        int linenum,
        std::string procname = unknownProcMsg_,
        std::string errorMessage = "");
    virtual ~SimulationExecutionError();
};

/////////////////////////////////////////////////////////////////////////////
// SimulationCycleLimitReached
/////////////////////////////////////////////////////////////////////////////

/**
 * An exception which is thrown when simulation cycle limitation is reached.
 */
class SimulationCycleLimitReached : public Exception {
public:
    SimulationCycleLimitReached(
        std::string fileName,
        int lineNum,
        std::string procName = unknownProcMsg_,
        std::string errorMessage = "");
    virtual ~SimulationCycleLimitReached();
};

/////////////////////////////////////////////////////////////////////////////
// SimulationTimeOut
/////////////////////////////////////////////////////////////////////////////

/**
 * An exception which is thrown when simulation cycle limitation is reached.
 */
class SimulationTimeOut : public Exception {
public:
    SimulationTimeOut(
        std::string fileName,
        int lineNum,
        std::string procName = unknownProcMsg_,
        std::string errorMessage = "");
    virtual ~SimulationTimeOut();
};

/////////////////////////////////////////////////////////////////////////////
// ObjectAlreadyExists
/////////////////////////////////////////////////////////////////////////////

/**
 * Exception which is thrown when something already existing is being added.
 */
class ObjectAlreadyExists : public IllegalParameters {
public:
    ObjectAlreadyExists(
        std::string filename,
        int linenum,
        std::string procname = unknownProcMsg_,
        std::string errorMessage = "");
    virtual ~ObjectAlreadyExists();
};

/////////////////////////////////////////////////////////////////////////////
// CompileError
/////////////////////////////////////////////////////////////////////////////

/**
 * Exception which is thrown when there is error in compilation.
 */
class CompileError : public Exception {
public:
    CompileError(
        std::string filename,
        int linenum,
        std::string procname = unknownProcMsg_,
        std::string errorMessage = "");

    virtual ~CompileError();

    void setCodeFileLineNumber(int lineNum);
    int codeFileLineNumber();

private:
    int codeLineNumber_;
};

///////////////////////////////////////////////////////////////////////////////
// ModuleRunTimeError
///////////////////////////////////////////////////////////////////////////////

/**
 * Base class for handling run-time errors in plugin modules.
 */
class ModuleRunTimeError : public Exception {
public:
    ModuleRunTimeError(
	std::string filename, int linenum,
	std::string procname = unknownProcMsg_,
	std::string errorMessage = "");
    virtual ~ModuleRunTimeError() {};
};

#include "Exception.icc"

#endif
