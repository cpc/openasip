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
 * @file Label.hh
 *
 * Declaration of Label class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_LABEL_HH
#define TTA_LABEL_HH

#include <string>
#include "Address.hh"

namespace TTAProgram {

class Scope;

/**
 * Marks locations of code or data area of the program.
 *
 * Labels are owned by scopes. Scopes do not provide methods to add
 * new labels, because the labels are automatically registered to a
 * scope by their constructor: a label cannot exist without its scope.
 *
 * A label must have a unique name within the owning scope. It does
 * not matter if there are other labels with the same name in outer
 * scopes, because the inner scope has precedence. As a result, all
 * labels in global scope must have a unique name.
 */
class Label {
public:
    virtual ~Label();

    std::string name() const;
    virtual Address address() const;
    const Scope& scope() const;

    void setAddress(Address address);

protected:
    Label();
    Label(const std::string& name, Address address, const Scope& scope);
    void setName(const std::string& name);
    void setScope(const Scope& scope);

private:
    /// Name of the label.
    std::string name_;
    /// Address of the location corresponding to this label.
    Address address_;
    /// Owning scope of the label.
    const Scope* scope_;
};

}

#endif
