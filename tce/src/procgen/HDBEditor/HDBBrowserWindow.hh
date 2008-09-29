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
 * @file HDBBrowserWindow.hh
 *
 * Declaration of HDBBrowserWindow class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_HDB_BROWSER_WINDOW_HH
#define TTA_HDB_BROWSER_WINDOW_HH

#include <map>
#include <wx/wx.h>
#include <wx/treectrl.h>
#include <wx/panel.h>
#include "Exception.hh"
#include "DBTypes.hh"

class HDBBrowserInfoPanel;

namespace HDB {
    class CachedHDBManager;
}

namespace TTAMachine {
    class FunctionUnit;
}

/**
 * Window for browsing an HDB.
 *
 * Displays a tree-view of the HDB, and an info panel which shows details
 * of the element selected in the tree-view.
 */
class HDBBrowserWindow : public wxPanel {
public:
    HDBBrowserWindow(wxWindow* parent, wxWindowID id);
    virtual ~HDBBrowserWindow();
    void setHDBManager(HDB::CachedHDBManager& manager);
    void update();

    bool isFUArchitectureSelected();
    bool isFUImplementationSelected();
    bool isRFArchitectureSelected();
    bool isRFImplementationSelected();
    bool isFUEntrySelected();
    bool isRFEntrySelected();
    bool isBusEntrySelected();
    bool isSocketEntrySelected();
    bool isCostFunctionPluginSelected();

    RowID selectedFUArchitecture();
    RowID selectedFUImplementation();
    RowID selectedRFArchitecture();
    RowID selectedRFImplementation();
    RowID selectedFUEntry();
    RowID selectedRFEntry();
    RowID selectedBusEntry();
    RowID selectedSocketEntry();
    RowID selectedCostFunctionPlugin();

    void selectRFArchitecture(int id);
    void selectFUArchitecture(int id);
    void selectRFImplementation(int id);
    void selectFUImplementation(int id);
    void selectFUEntry(int id);
    void selectRFEntry(int id);
    void selectBusEntry(int id);
    void selectSocketEntry(int id);
    void selectCostFunctionPlugin(int id);
    void openLink(const wxString& link);

private:
    /// HDB to display in the browser.
    HDB::CachedHDBManager* manager_;
    /// Tree widget displayign the HDB hierarchy.
    wxTreeCtrl* tree_;

    void onItemSelected(wxTreeEvent& event);
    void onRMBClick(wxTreeEvent& event);
    wxString fuArchLabel(const TTAMachine::FunctionUnit& fu) const;

    /// Widget window IDs.
    enum {
        ID_TREE_VIEW = 20000
    };

    /// Map of rf architecture RowIDs to tree item IDs.
    std::map<RowID, wxTreeItemId> rfArchitectures_;
    /// Map of rf implementation RowIDs to tree item IDs.
    std::map<RowID, wxTreeItemId> rfImplementations_;
    /// Map of fu architecture RowIDs to tree item IDs.
    std::map<RowID, wxTreeItemId> fuArchitectures_;
    /// Map of fu implementation RowIDs to tree item IDs.
    std::map<RowID, wxTreeItemId> fuImplementations_;
    /// Map of fu entry RowIDs to tree item IDs.
    std::map<RowID, wxTreeItemId> fuEntries_;
    /// Map of rf entry RowIDs to tree item IDs.
    std::map<RowID, wxTreeItemId> rfEntries_;
    /// Map of bus entry RowIDs to tree item IDs.
    std::map<RowID, wxTreeItemId> busEntries_;
    /// Map of socket entry RowIDs to tree item IDs.
    std::map<RowID, wxTreeItemId> socketEntries_;
    /// Map of cost function plugin  RowIDs to tree item IDs.
    std::map<RowID, wxTreeItemId> costPlugins_;

    /// Info panel widget for displaying the selected element information.
    HDBBrowserInfoPanel* infoPanel_;

    DECLARE_EVENT_TABLE()
};

#endif
