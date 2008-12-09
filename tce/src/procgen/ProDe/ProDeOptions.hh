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
 * @file ProDeOptions.hh
 *
 * Declaration of class ProDeOptions.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PRODE_OPTIONS_HH
#define TTA_PRODE_OPTIONS_HH

#include <string>
#include <vector>

#include "GUIOptions.hh"

/**
 * Represents the options of the editor.
 */
class ProDeOptions : public GUIOptions {
public:
    ProDeOptions();
    ProDeOptions(const ObjectState* state)
        throw (ObjectStateLoadingException);
    ProDeOptions(const ProDeOptions& old);
    virtual ~ProDeOptions();

    int undoStackSize() const;
    void setUndoStackSize(int size);

    void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    ObjectState* saveState() const;

    /// ObjectState name for ProDeOptions.
    static const std::string CONFIGURATION_NAME;
    /// ObjectState attribute key for the size of the undo stack.
    static const std::string OSKEY_UNDO_STACK_SIZE;

private:
    /// Undo stack size.
    int undoStackSize_;
};

#endif
