/**
 * @file OSEdTreeView.hh
 *
 * Declaration of OSEdTreeView class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
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
