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
 * @file OSEdInfoView.hh
 *
 * Declaration of OSEdInfoView class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OSED_INFO_VIEW_HH
#define TTA_OSED_INFO_VIEW_HH

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <string>

class Operation;

/**
 * Models an info screen.
 *
 * Info screen shows info about search paths, modules, and operations.
 */
class OSEdInfoView : public wxListCtrl {
public:
    OSEdInfoView(wxWindow* parent);
    virtual ~OSEdInfoView();
    
    void clear();
    void pathView();
    void moduleView(const std::string& name);
    void operationView(const std::string& path, const std::string& mod);
    void operationPropertyView(
        const std::string& opName,
        const std::string& modName,
        const std::string& pathName);

    std::string selectedPath();
    std::string selectedModule();
    std::string selectedOperation();

private:

    void onSelection(wxListEvent& event);
    void onDropDownMenu(wxMouseEvent& event);
    void insertOperationPropertyColumns();
    int  writeStaticPropertiesOfOperation(Operation* op);

    /**
     * Possible modes of info view.
     */
    enum InfoMode {
        MODE_NOMODE,    ///< Initial state.
        MODE_PATH,      ///< Path view.
        MODE_MODULE,    ///< Module view.
        MODE_OPERATION, ///< Operation View.
        MODE_PROPERTY   ///< Operation property view.
    };
    
    /// Mode of the info view.
    InfoMode mode_;

    DECLARE_EVENT_TABLE()
};

#endif
