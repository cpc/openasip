/**
 * @file HDBBrowserWindow.hh
 *
 * Declaration of HDBBrowserWindow class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
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
