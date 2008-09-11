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
 * @file DropDownMenu.hh
 *
 * Declaration of DropDownMenu class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DROP_DOWN_MENU_HH
#define TTA_DROP_DOWN_MENU_HH

#include <wx/wx.h>

/**
 * Class that models drop down menu.
 *
 * Drop down menu is shown, when user clicks right mouse button over
 * tree item or list item in main window.
 */
class DropDownMenu : public wxMenu {
public:

    /**
     * Type of the drop down menu.
     */
    enum MenuType {
        MENU_PATH,      ///< Path menu.
        MENU_MODULE,    ///< Module menu.
        MENU_OPERATION  ///< Operation menu.
    };

    explicit DropDownMenu(MenuType type);
    virtual ~DropDownMenu();

private:
    /// Copying not allowed.
    DropDownMenu(const DropDownMenu&);
    /// Assignment not allowed.
    DropDownMenu& operator=(const DropDownMenu&);
    
    void onMenuEvent(wxCommandEvent& event);
    void updateMenu();

    DECLARE_EVENT_TABLE()
};

#endif
