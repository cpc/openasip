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
 * @file FUGuardEncoding.hh
 *
 * Declaration of FUGuardEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_FU_GUARD_ENCODING_HH
#define TTA_FU_GUARD_ENCODING_HH

#include <string>

#include "GuardEncoding.hh"
#include "Exception.hh"

/**
 * The FUGuardEncoding class maps a guard expression with FU output port term
 * to a control code.
 *
 * Instances of this class are always registered to a guard field.
 */
class FUGuardEncoding : public GuardEncoding {
public:
    FUGuardEncoding(
	const std::string& fu,
	const std::string& port,
	bool inverted,
	unsigned int encoding,
	GuardField& parent)
	throw (ObjectAlreadyExists);
    FUGuardEncoding(const ObjectState* state, GuardField& parent)
	throw (ObjectStateLoadingException, ObjectAlreadyExists);
    virtual ~FUGuardEncoding();

    std::string functionUnit() const;
    std::string port() const;

    virtual ObjectState* saveState() const;

    /// ObjectState name for FU guard encoding.
    static const std::string OSNAME_FU_GUARD_ENCODING;
    /// ObjectState attribute key for the name of the function unit.
    static const std::string OSKEY_FU_NAME;
    /// ObjectState attribute key for the name of the port.
    static const std::string OSKEY_PORT_NAME;

private:
    /// Name of the function unit.
    std::string functionUnit_;
    /// Name of the port.
    std::string port_;
};

#endif
