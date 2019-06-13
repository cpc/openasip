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
 * @file ToolbarButton.hh
 *
 * Declaration of ToolbarButton class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_TOOLBAR_BUTTON_HH
#define TTA_TOOLBAR_BUTTON_HH

#include <string>

#include "Serializable.hh"
#include "Exception.hh"

/**
 * This class repsesents a toolbar button. It contains information
 * which action is performed by the button and where the button is
 * placed in the toolbar. This class implements the Serializable
 * interface because toolbar button configurations are serialized in the
 * configuration file.
 */  
class ToolbarButton : public Serializable {
public:
    /// ObjectState name for ToolbarButton.
    static const std::string OSNAME_TOOLBAR_BUTTON;
    /// ObjectState attribute key for slot position.
    static const std::string OSKEY_SLOT;
    /// ObjectState attribute key for action name.
    static const std::string OSKEY_ACTION;

    ToolbarButton(int slot, const std::string& action);
    ToolbarButton(const ObjectState* state);
    ToolbarButton(const ToolbarButton& old);
    virtual ~ToolbarButton();

    std::string action() const;
    int slot() const;

    void loadState(const ObjectState* state);
    ObjectState* saveState() const;

private:
    /// Number of the slot in which this toolbar button is.
    int slot_;
    /// Name of the action performed by this toolbar button.
    std::string action_;
};

#endif
