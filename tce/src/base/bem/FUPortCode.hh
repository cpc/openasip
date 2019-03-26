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
 * @file FUPortCode.hh
 *
 * Declaration of FUPortCode class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_FU_PORT_CODE_HH
#define TTA_FU_PORT_CODE_HH

#include <string>

#include "PortCode.hh"
#include "Exception.hh"

class SocketCodeTable;

/**
 * Class FUPortCode represents the control code that identifies an FU port
 * or, if the port carries an opcode, the combination of port and operation.
 */
class FUPortCode : public PortCode {
public:
    FUPortCode(
	const std::string& fu,
	const std::string& port,
	unsigned int encoding,
	unsigned int extraBits,
	SocketCodeTable& parent)
	throw (ObjectAlreadyExists, OutOfRange);
    FUPortCode(
	const std::string& fu,
	const std::string& port,
	const std::string& operation,
	unsigned int encoding,
	unsigned int extraBits,
	SocketCodeTable& parent)
	throw (ObjectAlreadyExists, OutOfRange);
    FUPortCode(const ObjectState* state, SocketCodeTable& parent)
	throw (ObjectStateLoadingException, ObjectAlreadyExists);
    virtual ~FUPortCode();

    std::string portName() const;
    std::string operationName() const
	throw (InstanceNotFound);
    bool hasOperation() const;

    virtual ObjectState* saveState() const;

    /// ObjectState name for FU port code.
    static const std::string OSNAME_FU_PORT_CODE;
    /// ObjectState attribute key for the name of the port.
    static const std::string OSKEY_PORT_NAME;
    /// ObjectState attribute key for the name of the operation.
    static const std::string OSKEY_OPERATION_NAME;

private:
    /// Name of the port.
    std::string port_;
    /// Name of the operation.
    std::string opName_;
};

#endif
