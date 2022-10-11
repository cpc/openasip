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
    wxTreeItemId selectedOperationId();
    std::string moduleOfOperation(wxTreeItemId id);
    wxTreeItemId moduleIdOfOperation(wxTreeItemId id);
    std::string pathOfModule(wxTreeItemId id);
    wxTreeItemId pathIdOfModule(wxTreeItemId id);
    std::string selectedPath();
    wxTreeItemId selectedPathId();
    wxTreeItemId selectedModuleId();
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
