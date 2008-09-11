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
 * @file UnconditionalGuardEncoding.hh
 *
 * Declaration of UnconditionalGuardEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_UNCONDITIONAL_GUARD_ENCODING_HH
#define TTA_UNCONDITIONAL_GUARD_ENCODING_HH

#include <string>

#include "GuardEncoding.hh"
#include "Exception.hh"

/**
 * The UnconditionalGuardEncoding class maps always true guard term to a
 * control code.
 *
 * Instances of this class are always registered to a guard field.
 */
class UnconditionalGuardEncoding : public GuardEncoding {
public:
    UnconditionalGuardEncoding(
        bool inverted,
        unsigned int encoding,
        GuardField& parent)
        throw (ObjectAlreadyExists);
    UnconditionalGuardEncoding(const ObjectState* state, GuardField& parent)
        throw (ObjectStateLoadingException, ObjectAlreadyExists);
    virtual ~UnconditionalGuardEncoding();

    virtual ObjectState* saveState() const;

    /// ObjectState name for unconditional guard encoding.
    static const std::string OSNAME_UNCONDITIONAL_GUARD_ENCODING;
};

#endif
