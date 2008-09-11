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
 * @file OSEdTreeView.hh
 *
 * Declaration of OSEdTreeView class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OSED_TREE_VIEW_HH
#define TTA_OSED_TREE_VIEW_HH

#include <wx/wx.h>
#include <wx/treectrl.h>

#include <map>
#include <string>
#include <vector>

#include "Exception.hh"

class OSEdInfoView;
class Operation;

/**
 * Models a tree view of the operation data base.
 *
 * Shows a tree-like structure of search paths, operation modules, and
 * operations.
 */
class OSEdTreeView : public wxTreeCtrl {
public:
    OSEdTreeView(wxWindow* parent, OSEdInfoView* infoView);
    virtual ~OSEdTreeView();
    
    Operation* selectedOperation();
    wxTreeItemId selectedOperationId() throw (NotAvailable);
    std::string moduleOfOperation(wxTreeItemId id);
    wxTreeItemId moduleIdOfOperation(wxTreeItemId id);
    std::string pathOfModule(wxTreeItemId id);
    wxTreeItemId pathIdOfModule(wxTreeItemId id);
    std::string selectedPath();
    wxTreeItemId selectedPathId() throw (NotAvailable);
    wxTreeItemId selectedModuleId() throw (NotAvailable);
    std::string selectedModule();
    OSEdInfoView* infoView() const;
    std::vector<std::string> constructTree();
    void addItem(wxTreeItemId parent, std::string item);
    void changeText(wxTreeItemId id, const std::string& text);

    void removeItem(wxTreeItemId id);
    void update();

    bool isPathSelected() const;
    bool isModuleSelected() const;
    bool isOperationSelected() const;

private:
    /// Value type for the map.
    typedef std::map<std::string, wxTreeItemId>::value_type ValType;
    /// Iterators for the maps.
    typedef std::map<std::string, wxTreeItemId>::iterator Iter;
    typedef std::multimap<std::string, wxTreeItemId>::iterator IterM;
    
    /// Copying not allowed.
    OSEdTreeView(const OSEdTreeView&);
    /// Assignment not allowed.
    OSEdTreeView& operator=(const OSEdTreeView&);
    
    bool isPath(wxTreeItemId id) const;
    bool isModule(wxTreeItemId id) const;
    bool isOperation(wxTreeItemId id) const;
    
    void onItemClicked(wxTreeEvent& event);
    void onDropDownMenu(wxMouseEvent& event);
    void onItemSelected(wxTreeEvent& event);

    /// An info window controlled by tree view.
    OSEdInfoView* infoView_;
    /// Paths of the operation data base.
    std::map<std::string, wxTreeItemId> paths_;
    /// Modules of the operation data base.
    std::multimap<std::string, wxTreeItemId> modules_;
    /// Operations of operation data base.
    std::multimap<std::string, wxTreeItemId> operations_; 
    
    DECLARE_EVENT_TABLE()
};

#endif
