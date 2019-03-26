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
 * @file Exception.cc
 *
 * Implementations of all exceptions.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2007 (pekka.jaaskelainen-no.spam-tut.fi)
 */

#include <boost/format.hpp>

#include <string>
using std::string;

#include "Exception.hh"
#include "Conversion.hh"

const string Exception::unknownProcMsg_ = "(unknown)";

std::string Exception::lastExceptionInfo_ = "";

///////////////////////////////////////////////////////////////////////////////
// Exception
///////////////////////////////////////////////////////////////////////////////

/**
 * The constructor.
 *
 * @param filename Name of the file in which the exception occurred.
 * @param linenum Line number in the file.
 * @param procname Name of the procedure.
 * @param errorMessage Error message.
 */
Exception::Exception(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    file_(filename), line_(linenum), proc_(procname), 
    errorMessage_(errorMessage), cause_(NULL) {
    lastExceptionInfo_ = 
        filename + ", line " + Conversion::toString(linenum) + ". Message: " + 
        errorMessage;
}

/**
 * Sets cause of exception.
 *
 * @param  aCause Exception that resulted current exception.
 */
void
Exception::setCause(const Exception& aCause) {
    cause_ = new Exception(aCause);
}

/**
 * Returns true if cause is set for current exception.
 *
 * @return  True if cause is set for current exception.
 */
bool
Exception::hasCause() const {
    return (cause_ != NULL);
}

/**
 * Returns Exception that caused throwing current exception.
 *
 * @return  Exception that caused throwing current exception.
 */
const Exception&
Exception::cause() const {
    return *cause_;
}

/**
 * Returns information of the last exception thrown.
 *
 * This is useful in debugging. It's used in Application.hh's unexpected
 * exception handler.
 *
 * @return A string describing the last thrown exception.
 */
std::string
Exception::lastExceptionInfo() {
    return lastExceptionInfo_;
}

/**
 * The destructor.
 * Nothing specific to do in the destructor.
 */
Exception::~Exception() {
}

/**
 * Returns the exception's error message.
 */
string
Exception::errorMessage() const {
    return errorMessage_;
}

/**
 * Returns error message of exception and all error messages of exceptions
 * that caused current exception.
 * 
 * @param messagesOnly if true, only returns error messages, not the exception
 *        names and line numbers
 *
 * @return error message of exception and all error messages of exceptions
 *         that caused current exception.
 */
string
Exception::errorMessageStack(bool messagesOnly) const {

    string result = messagesOnly ?
        errorMessage() :
        (boost::format("%s:%d '%s'\n") % 
         fileName() % lineNum() % errorMessage()).str();
    
    const Exception* exception = this;

    while (exception->hasCause()) {
        exception = &exception->cause();
        assert(exception != NULL);
        result += messagesOnly ?
            exception->errorMessage() :
            (boost::format("%s:%d '%s'\n") % 
             exception->fileName() % exception->lineNum() % 
             exception->errorMessage()).str();
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
// IllegalParameters
///////////////////////////////////////////////////////////////////////////////

/**
 * The constructor for IllegalParameters exception.
 *
 * @param filename Name of the file in which the exception occurred.
 * @param linenum Line number of the file.
 * @param procname Name of the procedure.
 * @param errorMessage Optional error message.
 */
IllegalParameters::IllegalParameters(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    Exception(filename, linenum, procname, errorMessage) {
}

///////////////////////////////////////////////////////////////////////////////
// IOException
///////////////////////////////////////////////////////////////////////////////

/**
 * The constructor.
 *
 * @param filename Name of the file in which the exception occurred.
 * @param linenum Line number of the file.
 * @param procname Name of the procedure.
 * @param errorMessage Optional error message.
 */
IOException::IOException(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    Exception(filename, linenum, procname, errorMessage) {
}

///////////////////////////////////////////////////////////////////////////////
// InvalidData
///////////////////////////////////////////////////////////////////////////////

/**
 * The constructor.
 *
 * @param filename Name of the file in which the exception occurred.
 * @param linenum Line number of the file.
 * @param procname Name of the procedure.
 * @param errorMessage Optional error message.
 */
InvalidData::InvalidData(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    Exception(filename, linenum, procname, errorMessage) {
}

///////////////////////////////////////////////////////////////////////////////
// UnreachableStream
///////////////////////////////////////////////////////////////////////////////

/**
 * The constructor.
 *
 * @param filename Name of the file in which the exception occurred.
 * @param linenum Line number of the file.
 * @param procname Name of the procedure.
 * @param errorMessage Name of the stream (file).
 */
UnreachableStream::UnreachableStream(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    IOException(filename, linenum, procname, errorMessage) {
}

/**
 * The destructor.
 * Nothing specific to do in the destructor.
 */
UnreachableStream::~UnreachableStream() {
}

///////////////////////////////////////////////////////////////////////////////
// EndOfFile
///////////////////////////////////////////////////////////////////////////////

/**
 * The constructor.
 *
 * @param filename Name of the file in which the exception occurred.
 * @param linenum Line number of the file.
 * @param procname Name of the procedure.
 * @param errorMessage Name of the stream (file).
 */
EndOfFile::EndOfFile(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    IOException(filename, linenum, procname, errorMessage) {
}

/**
 * The destructor.
 * Nothing specific to do in the destructor.
 */
EndOfFile::~EndOfFile() {
}

///////////////////////////////////////////////////////////////////////////////
// WritePastEOF
///////////////////////////////////////////////////////////////////////////////

/**
 * The constructor.
 *
 * @param filename Name of the file in which the exception occurred.
 * @param linenum Line number of the file.
 * @param procname Name of the procedure.
 * @param errorMessage Optional error message.
 */
WritePastEOF::WritePastEOF(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    IOException(filename, linenum, procname, errorMessage) {
}

/**
 * The destructor.
 *
 * Nothing specific to do in the destructor.
 */
WritePastEOF::~WritePastEOF() {
}

///////////////////////////////////////////////////////////////////////////////
// FileNotFound
///////////////////////////////////////////////////////////////////////////////

/**
 * The constructor.
 *
 * @param filename The name of the file in which exception was thrown.
 * @param linenum The numberof the line in which exception was thrown.
 * @param procname The name of the procedure in which exception was thrown.
 * @param errorMessage The error message.
 */
FileNotFound::FileNotFound(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    IOException(filename, linenum, procname, errorMessage) {
}

/**
 * The destructor.
 */
FileNotFound::~FileNotFound() {
}

///////////////////////////////////////////////////////////////////////////////
// PathNotFound
///////////////////////////////////////////////////////////////////////////////

/**
 * The constructor.
 *
 * @param filename The name of the file in which exception was thrown.
 * @param linenum The numberof the line in which exception was thrown.
 * @param procname The name of the procedure in which exception was thrown.
 * @param errorMessage The error message.
 * @param path The path that could not be found.
 */
PathNotFound::PathNotFound(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage,
    std::string path) :
    IOException(filename, linenum, procname, errorMessage), path_(path) {
}

/**
 * The destructor.
 */
PathNotFound::~PathNotFound() {
}


///////////////////////////////////////////////////////////////////////////////
// KeyAlreadyExists
///////////////////////////////////////////////////////////////////////////////
/**
 * The constructor for KeyAlreadyExists exception.
 *
 * @param filename Name of the file in which the exception occurred.
 * @param linenum Line number of the file.
 * @param procname Name of the procedure.
 * @param errorMessage The message attached to exception.
 */
KeyAlreadyExists::KeyAlreadyExists(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    IllegalParameters(filename, linenum, procname, errorMessage) {
}

///////////////////////////////////////////////////////////////////////////////
// KeyNotFound
///////////////////////////////////////////////////////////////////////////////
/**
 * The constructor for KeyNotFound exception.
 *
 * @param filename Name of the file in which the exception occurred.
 * @param linenum Line number of the file.
 * @param procname Name of the procedure.
 * @param errorMessage The message attached to exception.
 */
KeyNotFound::KeyNotFound(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    IllegalParameters(filename, linenum, procname, errorMessage) {
}

///////////////////////////////////////////////////////////////////////////////
// InstanceNotFound
///////////////////////////////////////////////////////////////////////////////

/**
 * The constructor for InstanceNotFound exception.
 *
 * @param filename Name of the file in which the exception occurred.
 * @param linenum Line number of the file.
 * @param procname Name of the procedure.
 * @param errorMessage Error message.
 */
InstanceNotFound::InstanceNotFound(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    IllegalParameters(filename, linenum, procname, errorMessage) {
}

/**
 * Destructor.
 */
InstanceNotFound::~InstanceNotFound() {
}

///////////////////////////////////////////////////////////////////////////////
// OutOfRange
///////////////////////////////////////////////////////////////////////////////

/**
 * The constructor.
 *
 * @param filename Name of the file in which the exception occurred.
 * @param linenum Line number of the file.
 * @param procname Name of the procedure.
 * @param errorMessage Optional error message.
 */
OutOfRange::OutOfRange(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    InvalidData(filename, linenum, procname, errorMessage) {
}

///////////////////////////////////////////////////////////////////////////////
// WrongSubclass
///////////////////////////////////////////////////////////////////////////////

/**
 * The constructor.
 *
 * @param filename Name of the file in which the exception occurred.
 * @param linenum Line number of the file.
 * @param procname Name of the procedure.
 * @param errorMessage Optional error message.
 */
WrongSubclass::WrongSubclass(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    InvalidData(filename, linenum, procname, errorMessage) {
}

///////////////////////////////////////////////////////////////////////////////
// NotChunkable
///////////////////////////////////////////////////////////////////////////////

/**
 * The constructor for NotChunkable exception.
 *
 * @param filename Name of the file in which the exception occurred.
 * @param linenum Line number of the file.
 * @param procname Name of the procedure.
 * @param errorMessage Optional error message.
 */
NotChunkable::NotChunkable(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    WrongSubclass(filename, linenum, procname, errorMessage) {
}

///////////////////////////////////////////////////////////////////////////////
// UnresolvedReference
///////////////////////////////////////////////////////////////////////////////
/**
 * The constructor for UnresolvedReferences exception.
 *
 * @param filename Name of the file in which the exception occurred.
 * @param linenum Line number of the file.
 * @param procname Name of the procedure.
 * @param errorMessage The message attached to exception.
 */
UnresolvedReference::UnresolvedReference(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    InvalidData(filename, linenum, procname, errorMessage) {
}


///////////////////////////////////////////////////////////////////////////////
// ErrorInExternalFile
///////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param filename Name of the file in which the exception is created.
 * @param linenum Number of the line in which the exception is created.
 * @param procname Name of the function in which the exception is created.
 * @param errorMessage Error message.
 */
ErrorInExternalFile::ErrorInExternalFile(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    InvalidData(filename, linenum, procname, errorMessage) {
}

/**
 * Destructor.
 */
ErrorInExternalFile::~ErrorInExternalFile() {
}

///////////////////////////////////////////////////////////////////////////////
// MissingKeys
///////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param filename Name of the file in which the exception is created.
 * @param linenum Number of the line in which the exception is created.
 * @param procname Name of the function in which the exception is created.
 * @param errorMessage Error message.
 */
MissingKeys::MissingKeys(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    InvalidData(filename, linenum, procname, errorMessage) {
}

/**
 * Destructor.
 */
MissingKeys::~MissingKeys() {
}

/////////////////////////////////////////////////////////////////////////////
// NumberFormatException
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param filename Name of the file in which the exception is created.
 * @param linenum Number of the line in which the exception is created.
 * @param procname Name of the function in which the exception is created.
 * @param errorMessage Optional error message.
 */
NumberFormatException::NumberFormatException(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    InvalidData(filename, linenum, procname, errorMessage) {
}

/**
 * Destructor.
 */
NumberFormatException::~NumberFormatException() {
}

///////////////////////////////////////////////////////////////////////////////
// IllegalCommandLine
///////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param filename Name of the file in which the exception is created.
 * @param linenum Number of the line in which the exception is created.
 * @param procname Name of the function in which the exception is created.
 * @param errorMessage Error message.
 */
IllegalCommandLine::IllegalCommandLine(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    InvalidData(filename, linenum, procname, errorMessage) {
}

/**
 * Destructor.
 */
IllegalCommandLine::~IllegalCommandLine() {
}

///////////////////////////////////////////////////////////////////////////////
// UnexpectedValue
///////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param filename Name of the file in which the exception is created.
 * @param linenum Number of the line in which the exception is created.
 * @param procname Name of the function in which the exception is created.
 * @param errorMessage Error message.
 */
UnexpectedValue::UnexpectedValue(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    InvalidData(filename, linenum, procname, errorMessage) {
}

/**
 * Destructor.
 */
UnexpectedValue::~UnexpectedValue() {
}


/////////////////////////////////////////////////////////////////////////////
// IllegalConnectivity
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param filename Name of the file in which the exception is created.
 * @param linenum Number of the line in which the exception is created.
 * @param procname Name of the function in which the exception is created.
 * @param errorMessage Error message.
 */
IllegalConnectivity::IllegalConnectivity(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    Exception(filename, linenum, procname, errorMessage) {
}


/**
 * Destructor.
 */
IllegalConnectivity::~IllegalConnectivity() {
}

/////////////////////////////////////////////////////////////////////////////
// ParserStopRequest
/////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 *
 * @param fileName Name of the file in which the exception is created.
 * @param lineNum Number of the line in which the exception is created.
 * @param procName Name of the function in which the exception is created.
 * @param errorMessage Error message.
 */
ParserStopRequest::ParserStopRequest(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    Exception(filename, linenum, procname, errorMessage) {
}


/**
 * Destructor.
 */
ParserStopRequest::~ParserStopRequest() {
}

/////////////////////////////////////////////////////////////////////////////
// ComponentAlreadyExists
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param filename Name of the file in which the exception is created.
 * @param linenum Number of the line in which the exception is created.
 * @param procname Name of the function in which the exception is created.
 * @param errorMessage Error message.
 */
ComponentAlreadyExists::ComponentAlreadyExists(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    IllegalParameters(filename, linenum, procname, errorMessage) {
}


/**
 * Destructor.
 */
ComponentAlreadyExists::~ComponentAlreadyExists() {
}


/////////////////////////////////////////////////////////////////////////////
// IllegalRegistration
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param filename Name of the file in which the exception is created.
 * @param linenum Number of the line in which the exception is created.
 * @param procname Name of the function in which the exception is created.
 * @param errorMessage Error message.
 */
IllegalRegistration::IllegalRegistration(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    InvalidData(filename, linenum, procname, errorMessage) {
}


/**
 * Destructor.
 */
IllegalRegistration::~IllegalRegistration() {
}


/////////////////////////////////////////////////////////////////////////////
// ObjectStateLoadingException
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param filename Name of the file in which the exception is created.
 * @param linenum Number of the line in which the exception is created.
 * @param procname Name of the function in which the exception is created.
 * @param errorMessage Error message.
 */
ObjectStateLoadingException::ObjectStateLoadingException(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    Exception(filename, linenum, procname, errorMessage) {
}


/**
 * Destructor.
 */
ObjectStateLoadingException::~ObjectStateLoadingException() {
}


/////////////////////////////////////////////////////////////////////////////
// NonexistingChild
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param filename Name of the file in which the exception is created.
 * @param linenum Number of the line in which the exception is created.
 * @param procname Name of the function in which the exception is created.
 * @param errorMessage Error message.
 */
NonexistingChild::NonexistingChild(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    IllegalParameters(filename, linenum, procname, errorMessage) {
}


/**
 * Destructor.
 */
NonexistingChild::~NonexistingChild() {
}

/////////////////////////////////////////////////////////////////////////////
// DynamicLibraryException
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param filename Name of the file in which the exception is created.
 * @param linenum Number of the line in which the exception is created.
 * @param procname Name of the function in which the exception is created.
 * @param errorMessage Error message.
 */
DynamicLibraryException::DynamicLibraryException(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    Exception(filename, linenum, procname, errorMessage) {
}


/**
 * Destructor.
 */
DynamicLibraryException::~DynamicLibraryException() {
}

/////////////////////////////////////////////////////////////////////////////
// MultipleInstancesFound
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param filename Name of the file in which the exception is created.
 * @param linenum Number of the line in which the exception is created.
 * @param procname Name of the function in which the exception is created.
 * @param errorMessage Error message.
 */
MultipleInstancesFound::MultipleInstancesFound(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    Exception(filename, linenum, procname, errorMessage) {
}


/**
 * Destructor.
 */
MultipleInstancesFound::~MultipleInstancesFound() {
}


/////////////////////////////////////////////////////////////////////////////
// SymbolNotFound
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param filename Name of the file in which the exception is created.
 * @param linenum Number of the line in which the exception is created.
 * @param procname Name of the function in which the exception is created.
 * @param errorMessage Error message.
 */
SymbolNotFound::SymbolNotFound(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    Exception(filename, linenum, procname, errorMessage) {
}


/**
 * Destructor.
 */
SymbolNotFound::~SymbolNotFound() {
}

/////////////////////////////////////////////////////////////////////////////
// ObjectNotInitialized
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param filename Name of the file in which the exception is created.
 * @param linenum Number of the line in which the exception is created.
 * @param procname Name of the function in which the exception is created.
 * @param errorMessage Error message.
 */
ObjectNotInitialized::ObjectNotInitialized(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    Exception(filename, linenum, procname, errorMessage) {
}


/**
 * Destructor.
 */
ObjectNotInitialized::~ObjectNotInitialized() {
}

/////////////////////////////////////////////////////////////////////////////
// ScriptExecutionFailure
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param filename Name of the file in which the exception is created.
 * @param linenum Number of the line in which the exception is created.
 * @param procname Name of the function in which the exception is created.
 * @param errorMessage Error message.
 */
ScriptExecutionFailure::ScriptExecutionFailure(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    Exception(filename, linenum, procname, errorMessage) {
}


/**
 * Destructor.
 */
ScriptExecutionFailure::~ScriptExecutionFailure() {
}


/////////////////////////////////////////////////////////////////////////////
// SerializerException
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param fileName Name of the file in which the exception is created.
 * @param lineNum Number of the line in which the exception is created.
 * @param procName Name of the function in which the exception is created.
 * @param errorMessage Error message.
 */
SerializerException::SerializerException(
    std::string fileName,
    int lineNum,
    std::string procName,
    std::string errorMessage) :
    Exception(fileName, lineNum, procName, errorMessage) {
}


/**
 * Destructor.
 */
SerializerException::~SerializerException() {
}

/////////////////////////////////////////////////////////////////////////////
// RelationalDBException
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param fileName Name of the file in which the exception is created.
 * @param lineNum Number of the line in which the exception is created.
 * @param procName Name of the function in which the exception is created.
 * @param errorMessage Error message.
 */
RelationalDBException::RelationalDBException(
    std::string fileName,
    int lineNum,
    std::string procName,
    std::string errorMessage) :
    Exception(fileName, lineNum, procName, errorMessage) {
}


/**
 * Destructor.
 */
RelationalDBException::~RelationalDBException() {
}


/////////////////////////////////////////////////////////////////////////////
// StartTooLate
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param fileName Name of the file in which the exception is created.
 * @param lineNum Number of the line in which the exception is created.
 * @param procName Name of the function in which the exception is created.
 * @param errorMessage Error message.
 */
StartTooLate::StartTooLate(
    std::string fileName,
    int lineNum,
    std::string procName,
    std::string errorMessage) :
    InvalidData(fileName, lineNum, procName, errorMessage) {
}


/**
 * Destructor.
 */
StartTooLate::~StartTooLate() {
}


/////////////////////////////////////////////////////////////////////////////
// NotAvailable
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param fileName Name of the file in which the exception is created.
 * @param lineNum Number of the line in which the exception is created.
 * @param procName Name of the function in which the exception is created.
 * @param errorMessage Error message.
 */
NotAvailable::NotAvailable(
    std::string fileName,
    int lineNum,
    std::string procName,
    std::string errorMessage) :
    InvalidData(fileName, lineNum, procName, errorMessage) {
}


/**
 * Destructor.
 */
NotAvailable::~NotAvailable() {
}

/////////////////////////////////////////////////////////////////////////////
// CannotEstimateCost
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param fileName Name of the file in which the exception is created.
 * @param lineNum Number of the line in which the exception is created.
 * @param procName Name of the function in which the exception is created.
 * @param errorMessage Error message.
 */
CannotEstimateCost::CannotEstimateCost(
    std::string fileName,
    int lineNum,
    std::string procName,
    std::string errorMessage) :
    NotAvailable(fileName, lineNum, procName, errorMessage) {
}


/**
 * Destructor.
 */
CannotEstimateCost::~CannotEstimateCost() {
}



/////////////////////////////////////////////////////////////////////////////
// WrongOperandType
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param fileName Name of the file in which the exception is created.
 * @param lineNum Number of the line in which the exception is created.
 * @param procName Name of the function in which the exception is created.
 * @param errorMessage Error message.
 */
WrongOperandType::WrongOperandType(
    std::string fileName,
    int lineNum,
    std::string procName,
    std::string errorMessage) :
    IllegalParameters(fileName, lineNum, procName, errorMessage) {
}


/**
 * Destructor.
 */
WrongOperandType::~WrongOperandType() {
}


/////////////////////////////////////////////////////////////////////////////
// BadOperationModule
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param fileName Name of the file in which the exception is created.
 * @param lineNum Number of the line in which the exception is created.
 * @param procName Name of the function in which the exception is created.
 * @param errorMessage Error message.
 */
BadOperationModule::BadOperationModule(
    std::string fileName,
    int lineNum,
    std::string procName,
    std::string errorMessage) :
    InvalidData(fileName, lineNum, procName, errorMessage) {
}


/**
 * Destructor.
 */
BadOperationModule::~BadOperationModule() {
}


/////////////////////////////////////////////////////////////////////////////
// TypeMismatch
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param fileName Name of the file in which the exception is created.
 * @param lineNum Number of the line in which the exception is created.
 * @param procName Name of the function in which the exception is created.
 * @param errorMessage Error message.
 */
TypeMismatch::TypeMismatch(
    std::string fileName,
    int lineNum,
    std::string procName,
    std::string errorMessage) :
    InvalidData(fileName, lineNum, procName, errorMessage) {
}


/**
 * Destructor.
 */
TypeMismatch::~TypeMismatch() {
}


/////////////////////////////////////////////////////////////////////////////
// InvalidName
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param fileName Name of the file in which the exception is created.
 * @param lineNum Number of the line in which the exception is created.
 * @param procName Name of the function in which the exception is created.
 * @param errorMessage Error message.
 */
InvalidName::InvalidName(
    std::string fileName,
    int lineNum,
    std::string procName,
    std::string errorMessage) :
    InvalidData(fileName, lineNum, procName, errorMessage) {
}


/**
 * Destructor.
 */
InvalidName::~InvalidName() {
}

/////////////////////////////////////////////////////////////////////////////
// IllegalOperationBehavior
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param fileName Name of the file in which the exception is created.
 * @param lineNum Number of the line in which the exception is created.
 * @param procName Name of the function in which the exception is created.
 * @param errorMessage Error message.
 */
IllegalOperationBehavior::IllegalOperationBehavior(
    std::string fileName,
    int lineNum,
    std::string procName,
    std::string errorMessage) :
    Exception(fileName, lineNum, procName, errorMessage) {
}

/**
 * Destructor.
 */
IllegalOperationBehavior::~IllegalOperationBehavior() {
}

/////////////////////////////////////////////////////////////////////////////
// NonexistingSyscall
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param fileName Name of the file in which the exception is created.
 * @param lineNum Number of the line in which the exception is created.
 * @param procName Name of the function in which the exception is created.
 * @param errorMessage Error message.
 */
NonexistingSyscall::NonexistingSyscall(
    std::string fileName,
    int lineNum,
    std::string procName,
    std::string errorMessage) :
    Exception(fileName, lineNum, procName, errorMessage) {
}

/**
 * Destructor.
 */
NonexistingSyscall::~NonexistingSyscall() {
}

/////////////////////////////////////////////////////////////////////////////
// IllegalMachine
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param fileName Name of the file in which the exception is created.
 * @param lineNum Number of the line in which the exception is created.
 * @param procName Name of the function in which the exception is created.
 * @param errorMessage Error message.
 */
IllegalMachine::IllegalMachine(
    std::string fileName,
    int lineNum,
    std::string procName,
    std::string errorMessage) :
    InvalidData(fileName, lineNum, procName, errorMessage) {
}

/**
 * Destructor.
 */
IllegalMachine::~IllegalMachine() {
}

/////////////////////////////////////////////////////////////////////////////
// IllegalProgram
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param fileName Name of the file in which the exception is created.
 * @param lineNum Number of the line in which the exception is created.
 * @param procName Name of the function in which the exception is created.
 * @param errorMessage Error message.
 */
IllegalProgram::IllegalProgram(
    std::string fileName,
    int lineNum,
    std::string procName,
    std::string errorMessage) :
    InvalidData(fileName, lineNum, procName, errorMessage) {
}

/**
 * Destructor.
 */
IllegalProgram::~IllegalProgram() {
}


///////////////////////////////////////////////////////////////////////////////
// SimulationException
///////////////////////////////////////////////////////////////////////////////

/**
 * The constructor.
 *
 * @param filename Name of the file in which the exception occurred.
 * @param linenum Line number of the file.
 * @param procname Name of the procedure.
 * @param errorMessage Name of the stream (file).
 */
SimulationException::SimulationException(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    Exception(filename, linenum, procname, errorMessage) {
}

/**
 * The destructor.
 */
SimulationException::~SimulationException() {
}

///////////////////////////////////////////////////////////////////////////////
// SimulationStillRunning
///////////////////////////////////////////////////////////////////////////////

/**
 * The constructor.
 *
 * @param filename Name of the file in which the exception occurred.
 * @param linenum Line number of the file.
 * @param procname Name of the procedure.
 * @param errorMessage Name of the stream (file).
 */
SimulationStillRunning::SimulationStillRunning(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    SimulationException(filename, linenum, procname, errorMessage) {
}

/**
 * The destructor.
 */
SimulationStillRunning::~SimulationStillRunning() {
}

/////////////////////////////////////////////////////////////////////////////
// SimulationExecutionError
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param filename Name of the file in which the exception is created.
 * @param linenum Number of the line in which the exception is created.
 * @param procname Name of the function in which the exception is created.
 * @param errorMessage Error message.
 */
SimulationExecutionError::SimulationExecutionError(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    SimulationException(filename, linenum, procname, errorMessage) {
}


/**
 * Destructor.
 */
SimulationExecutionError::~SimulationExecutionError() {
}

///////////////////////////////////////////////////////////////////////////////
// SimulationCycleLimitReached
///////////////////////////////////////////////////////////////////////////////

/**
 * The constructor.
 *
 * @param filename Name of the file in which the exception occurred.
 * @param linenum Line number of the file.
 * @param procname Name of the procedure.
 * @param errorMessage Name of the stream (file).
 */
SimulationCycleLimitReached::SimulationCycleLimitReached(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    Exception(filename, linenum, procname, errorMessage) {
}

/**
 * The destructor.
 */
SimulationCycleLimitReached::~SimulationCycleLimitReached() {
}

///////////////////////////////////////////////////////////////////////////////
// SimulationTimeOut
///////////////////////////////////////////////////////////////////////////////

/**
 * The constructor.
 *
 * @param filename Name of the file in which the exception occurred.
 * @param linenum Line number of the file.
 * @param procname Name of the procedure.
 * @param errorMessage Name of the stream (file).
 */
SimulationTimeOut::SimulationTimeOut(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    Exception(filename, linenum, procname, errorMessage) {
}

/**
 * The destructor.
 */
SimulationTimeOut::~SimulationTimeOut() {
}

/////////////////////////////////////////////////////////////////////////////
// ObjectAlreadyExists
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param filename Name of the file in which the exception is created.
 * @param linenum Number of the line in which the exception is created.
 * @param procname Name of the function in which the exception is created.
 * @param errorMessage Error message.
 */
ObjectAlreadyExists::ObjectAlreadyExists(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    IllegalParameters(filename, linenum, procname, errorMessage) {
}


/**
 * Destructor.
 */
ObjectAlreadyExists::~ObjectAlreadyExists() {
}

/////////////////////////////////////////////////////////////////////////////
// CompileError
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param filename Name of the file in which the exception is created.
 * @param linenum Number of the line in which the exception is created.
 * @param procname Name of the function in which the exception is created.
 * @param errorMessage Error message.
 */
CompileError::CompileError(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    Exception(filename, linenum, procname, errorMessage) {
}


/**
 * Destructor.
 */
CompileError::~CompileError() {
}

/**
 * Set's line number where error happened in source code that is compiled.
 *
 * @param lineNum Line number where error happened in code that is compiled.
 */
void
CompileError::setCodeFileLineNumber(int lineNum) {
    codeLineNumber_ = lineNum;
}

/**
 * Returns line number where error happened in source code file that is compiled.
 *
 * @return Line number where error happened in source code file that is compiled.
 */
int
CompileError::codeFileLineNumber() {
    return codeLineNumber_;
}

///////////////////////////////////////////////////////////////////////////////
// ModuleRunTimeError
///////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param filename Name of the file in which the exception is created.
 * @param linenum Number of the line in which the exception is created.
 * @param procname Name of the function in which the exception is created.
 * @param errorMessage Error message.
 */
ModuleRunTimeError::ModuleRunTimeError(
    std::string filename,
    int linenum,
    std::string procname,
    std::string errorMessage) :
    Exception(filename, linenum, procname, errorMessage) {
}
