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

class OperationGlobals {
public:
    static std::ostream& outputStream();
    static void setOutputStream(std::ostream& newOutputStream);

private:
    /// Instantiation not allowed.
    OperationGlobals(); 
    /// Copying not allowed.
    OperationGlobals(const OperationGlobals&);
    /// Assignment not allowed.
    OperationGlobals& operator=(const OperationGlobals&);
    
    static std::ostream* outputStream_;
};

#endif
