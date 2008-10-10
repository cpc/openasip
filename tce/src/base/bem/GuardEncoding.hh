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
 * @file GuardEncoding.hh
 *
 * Declaration of GuardEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_GUARD_ENCODING_HH
#define TTA_GUARD_ENCODING_HH

#include "Exception.hh"

class GuardField;
class ObjectState;

/**
 * GuardEncoding class captures the properties shared by all types of guard
 * encoding.
 */
class GuardEncoding {
public:
    virtual ~GuardEncoding();

    GuardField* parent() const;
    bool isGuardInverted() const;
    unsigned int encoding() const;

    virtual ObjectState* saveState() const;

    /// ObjectState name for guard encoding.
    static const std::string OSNAME_GUARD_ENCODING;
    /// ObjectState attribute key for invert flag.
    static const std::string OSKEY_INVERTED;
    /// ObjectState attribute key for encoding.
    static const std::string OSKEY_ENCODING;

protected:
    GuardEncoding(bool inverted, unsigned int encoding);
    GuardEncoding(const ObjectState* state)
	throw (ObjectStateLoadingException);
    void setParent(GuardField* parent);

private:
    /// The "invert" flag.
    bool inverted_;
    /// The encoding.
    unsigned int encoding_;
    /// The parent guard field.
    GuardField* parent_;
};

#endif
