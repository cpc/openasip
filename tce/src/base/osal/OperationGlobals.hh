/**
 * @file OperationGlobals.hh
 *
 * Declaration of OperationGlobals singleton class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#ifndef OPERATION_GLOBALS_HH
#define OPERATION_GLOBALS_HH

#include <iostream>

class Operation;

class OperationGlobals {
public:
    static std::ostream& outputStream();
    static void setOutputStream(std::ostream& newOutputStream);
    static void runtimeError(
        const char* message, 
        const char* file, 
        int line,
        const Operation& parent);

private:
    /// Instantiation not allowed.
    OperationGlobals(); 
    /// Copying not allowed.
    OperationGlobals(const OperationGlobals&);
    /// Assignment not allowed.
    OperationGlobals& operator=(const OperationGlobals&);
    
    /// The global output stream, defaults to std::cout
    static std::ostream* outputStream_;
};

#endif
