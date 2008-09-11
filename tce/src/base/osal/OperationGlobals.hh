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
 * @file OperationGlobals.hh
 *
 * Declaration of OperationGlobals singleton class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen-no.spam-tut.fi)
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
