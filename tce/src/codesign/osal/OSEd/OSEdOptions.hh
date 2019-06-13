/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file OSEdOptions.hh
 *
 * Declaration of OSEdOptions class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OSED_OPTIONS_HH
#define TTA_OSED_OPTIONS_HH

#include <string>

#include "Serializable.hh"
#include "Exception.hh"

/**
 * Class that represents the OSEd options.
 *
 * Options are stored in XML file.
 */
class OSEdOptions : public Serializable {
public:
    OSEdOptions();
    virtual ~OSEdOptions();

    std::string editor() const;
    void setEditor(const std::string& editor);

    virtual void loadState(const ObjectState* state);
    ObjectState* saveState() const;
private:
    /// Copying not allowed.
    OSEdOptions(const OSEdOptions&);
    /// Assignment not allowed.
    OSEdOptions& operator=(const OSEdOptions&);

    /// Editor used to edit operation behavior files.
    std::string editor_;
};

#endif
