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
 * @file Label.hh
 *
 * Declaration of Label class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
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
