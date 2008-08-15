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
 * @file ToolbarButton.hh
 *
 * Declaration of ToolbarButton class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen@tut.fi)
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

    void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);
    ObjectState* saveState() const;

private:
    /// Number of the slot in which this toolbar button is.
    int slot_;
    /// Name of the action performed by this toolbar button.
    std::string action_;
};

#endif
