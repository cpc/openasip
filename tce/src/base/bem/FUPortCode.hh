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
