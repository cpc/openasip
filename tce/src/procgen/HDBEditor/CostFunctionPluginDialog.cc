/**
 * @file CostFunctionPluginDialog.cc
 *
 * Implementation of CostFunctionPluginDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <wx/listctrl.h>
#include <wx/statline.h>
#include <wx/valgen.h>
#include "CostFunctionPluginDialog.hh"
#include "HDBManager.hh"
#include "WxConversion.hh"
#include "ContainerTools.hh"
#include "CostEstimationDataDialog.hh"
#include "CostFunctionPlugin.hh"
#include "ErrorDialog.hh"

using namespace HDB;

BEGIN_EVENT_TABLE(CostFunctionPluginDialog, wxDialog)
    EVT_BUTTON(ID_ADD, CostFunctionPluginDialog::onAdd)
    EVT_BUTTON(ID_MODIFY, CostFunctionPluginDialog::onModify)
    EVT_BUTTON(ID_DELETE, CostFunctionPluginDialog::onDelete)
    EVT_BUTTON(ID_BROWSE, CostFunctionPluginDialog::onBrowse)
    EVT_BUTTON(wxID_OK, CostFunctionPluginDialog::onOK)

    EVT_LIST_ITEM_SELECTED(ID_LIST, CostFunctionPluginDialog::onDataSelection)
    EVT_LIST_ITEM_DESELECTED(ID_LIST, CostFunctionPluginDialog::onDataSelection)
END_EVENT_TABLE()

const wxString CostFunctionPluginDialog::TYPE_COST_FU =
    _T("FU Cost Estimator");
const wxString CostFunctionPluginDialog::TYPE_COST_RF =
    _T("RF Cost Estimator");
const wxString CostFunctionPluginDialog::TYPE_COST_DECOMP =
    _T("Decompressor Cost Estimator");
const wxString CostFunctionPluginDialog::TYPE_COST_ICDEC =
    _T("IC & Decoder Cost Estimator");

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param id Dialog window ID.
 * @param hdb HDB containing the cost function plugin.
 * @param pluginID Row ID of the plugin in the HDB.
 */
CostFunctionPluginDialog::CostFunctionPluginDialog(
    wxWindow* parent, wxWindowID id, HDB::HDBManager& hdb, int pluginID) :
    wxDialog(parent, id, _T("Cost Function Plugin")),
    hdb_(hdb), id_(pluginID) {

    createContents(this, true, true);

    list_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_LIST));
    typeChoice_ = dynamic_cast<wxChoice*>(FindWindow(ID_TYPE));

    list_->InsertColumn(0, _T("Type"), wxLIST_FORMAT_LEFT, 50);
    list_->InsertColumn(1, _T("ID"), wxLIST_FORMAT_LEFT, 50);
    list_->InsertColumn(2, _T("Name"), wxLIST_FORMAT_LEFT, 200);
    list_->InsertColumn(3, _T("Data"), wxLIST_FORMAT_RIGHT, 200);

    typeChoice_->Append(TYPE_COST_RF);
    typeChoice_->Append(TYPE_COST_FU);
    typeChoice_->Append(TYPE_COST_DECOMP);
    typeChoice_->Append(TYPE_COST_ICDEC);

    FindWindow(ID_NAME)->SetValidator(wxGenericValidator(&name_));
    FindWindow(ID_PATH)->SetValidator(wxGenericValidator(&path_));
    FindWindow(
        ID_DESCRIPTION)->SetValidator(wxGenericValidator(&description_));

    const CostFunctionPlugin* plugin = hdb_.costFunctionPluginByID(id_);

    name_ = WxConversion::toWxString(plugin->name());
    path_ = WxConversion::toWxString(plugin->pluginFilePath());
    description_ = WxConversion::toWxString(plugin->description());

    if (plugin->type() == CostFunctionPlugin::COST_FU) {
        typeChoice_->SetStringSelection(TYPE_COST_FU);
    } else if (plugin->type() == CostFunctionPlugin::COST_RF) {
        typeChoice_->SetStringSelection(TYPE_COST_RF);
    } else if (plugin->type() == CostFunctionPlugin::COST_DECOMP) {
        typeChoice_->SetStringSelection(TYPE_COST_DECOMP);
    } else if (plugin->type() == CostFunctionPlugin::COST_ICDEC) {
        typeChoice_->SetStringSelection(TYPE_COST_ICDEC);
    }
    delete plugin;

    FindWindow(ID_DELETE)->Disable();
    FindWindow(ID_MODIFY)->Disable();
}

/**
y * The Destructor.
 */
CostFunctionPluginDialog::~CostFunctionPluginDialog() {
}

/**
 * Transfers data to the dilaog widgets.
 */
bool
CostFunctionPluginDialog::TransferDataToWindow() {

    list_->DeleteAllItems();

    // Plugin cost estimation data.
    const std::set<RowID> pluginDataIDs = hdb_.costFunctionPluginDataIDs(id_);
    std::set<RowID>::const_iterator iter = pluginDataIDs.begin();
    for (; iter != pluginDataIDs.end(); iter++) {
        const CostEstimationData data = hdb_.costEstimationData(*iter);
        if (data.hasFUReference()) {
            list_->InsertItem(0, _T("FU"));
            list_->SetItem(0, 1, WxConversion::toWxString(data.fuReference()));
        } else if (data.hasRFReference()) {
            list_->InsertItem(0, _T("RF"));
            list_->SetItem(0, 1, WxConversion::toWxString(data.rfReference()));
        } else if (data.hasBusReference()) {
            list_->InsertItem(0, _T("Bus"));
            list_->SetItem(
                0, 1, WxConversion::toWxString(data.busReference()));
        } else if (data.hasSocketReference()) {
            list_->InsertItem(0, _T("Socket"));
            list_->SetItem(
                0, 1, WxConversion::toWxString(data.socketReference()));
        } else {
            list_->InsertItem(0, _T("-"));
            list_->SetItem(0, 1, _T("-"));
        }

        list_->SetItem(0, 2, WxConversion::toWxString(data.name()));
        list_->SetItem(
            0, 3, WxConversion::toWxString(data.value().stringValue()));
        list_->SetItemData(0, *iter);
    }

    return wxDialog::TransferDataToWindow();
}

/**
 * Returns RowID of the cost estimation data selected in the list.
 *
 * Returns -1 if no data is selected.
 *
 * @return RowID of the selected cost estiamtion data.
 */
int
CostFunctionPluginDialog::selectedData() {
    int item = list_->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    
    if (item < 0) {
        return -1;
    }

    return list_->GetItemData(item);
}

/**
 * Event handler for the plugin data list selection changes.
 *
 * Updates the delete and modify button enabled/disabled states.
 */
void
CostFunctionPluginDialog::onDataSelection(wxListEvent&) {
    if (selectedData() == -1) {
        FindWindow(ID_DELETE)->Disable();
        FindWindow(ID_MODIFY)->Disable();
    } else {
        FindWindow(ID_DELETE)->Enable();
        FindWindow(ID_MODIFY)->Enable();
    }
}

/**
 * Event handler for the browse plugin file path button.
 */
void
CostFunctionPluginDialog::onBrowse(wxCommandEvent&) {
    wxFileDialog dialog(
        this, _T("Choose a plugin file."), _T(""), _T(""),
        _T("Plugin files (*.so)|*.so|All Files|*.*"),
        wxOPEN | wxFILE_MUST_EXIST);
    if (dialog.ShowModal() == wxID_OK) {
        dynamic_cast<wxTextCtrl*>(
            FindWindow(ID_PATH))->SetValue(dialog.GetPath());
    }    
}
/**
 * Event handler for the add button.
 */
void
CostFunctionPluginDialog::onAdd(wxCommandEvent&) {
    TransferDataFromWindow();
    CostEstimationDataDialog dialog(this, -1, hdb_, id_, -1);
    dialog.ShowModal();
    TransferDataFromWindow();
    TransferDataToWindow();    
    
}

/**
 * Event handler for the modify button.
 */
void
CostFunctionPluginDialog::onModify(wxCommandEvent&) {
    TransferDataFromWindow();
    int selected = selectedData();
    if (selected >= 0) {
        CostEstimationDataDialog dialog(this, -1, hdb_, id_, selected);
        dialog.ShowModal();
        TransferDataToWindow();
    }
}

/**
 * Event handler for the Delete button.
 */
void
CostFunctionPluginDialog::onDelete(wxCommandEvent&) {
    TransferDataFromWindow();
    int selected = selectedData();
    if (selected >= 0) {
        hdb_.removeCostEstimationData(selected);
        TransferDataToWindow();
    }
}

/**
 * Event handler for the Close button.
 */
void
CostFunctionPluginDialog::onOK(wxCommandEvent&) {

    TransferDataFromWindow();

     if (name_.IsEmpty()) {
             wxString message(_T("Plugin name not set."));
             ErrorDialog dialog(this, message);
             dialog.ShowModal();
             return;
     }

    CostFunctionPlugin::CostFunctionPluginType type =
        CostFunctionPlugin::COST_FU;

    if (typeChoice_->GetStringSelection() == TYPE_COST_RF) {
        type = CostFunctionPlugin::COST_RF;        
    } else if (typeChoice_->GetStringSelection() == TYPE_COST_DECOMP) {
        type = CostFunctionPlugin::COST_DECOMP;
    } else if (typeChoice_->GetStringSelection() == TYPE_COST_ICDEC) {
        type = CostFunctionPlugin::COST_ICDEC;       
    } else {
        assert(typeChoice_->GetStringSelection() == TYPE_COST_FU);
    }

    std::string description = WxConversion::toString(description_);
    std::string name = WxConversion::toString(name_);
    std::string path = WxConversion::toString(path_);
    CostFunctionPlugin modified(-1, description, name, path, type);
    hdb_.modifyCostFunctionPlugin(id_, modified);
    EndModal(wxID_OK);
}

/**
 * Creates the dialog widgets.
 *
 * @param parent Parent window of the dialog widgets.
 */
wxSizer*
CostFunctionPluginDialog::createContents(
    wxWindow* parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxFlexGridSizer *item1 = new wxFlexGridSizer( 3, 0, 0 );

    wxStaticText *item2 = new wxStaticText( parent, ID_TEXT, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item2, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item3 = new wxTextCtrl( parent, ID_NAME, wxT(""), wxDefaultPosition, wxSize(300,-1), 0 );
    item1->Add( item3, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item4 = new wxStaticText( parent, ID_TEXT, wxT("Plugin file path:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item4, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item5 = new wxTextCtrl( parent, ID_PATH, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item1->Add( item5, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxButton *item6 = new wxButton( parent, ID_BROWSE, wxT("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item6, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item7 = new wxStaticText( parent, ID_TEXT, wxT("Plugin type:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item7, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxString *strs8 = (wxString*) NULL;
    wxChoice *item8 = new wxChoice( parent, ID_TYPE, wxDefaultPosition, wxSize(100,-1), 0, strs8, 0 );
    item1->Add( item8, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item1->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item9 = new wxStaticText( parent, ID_TEXT, wxT("Description:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item9, 0, wxALIGN_RIGHT|wxALL, 5 );

    wxTextCtrl *item10 = new wxTextCtrl( parent, ID_DESCRIPTION, wxT(""), wxDefaultPosition, wxSize(200,60), wxTE_MULTILINE );
    item1->Add( item10, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticBox *item12 = new wxStaticBox( parent, -1, wxT("Plugin data:") );
    wxStaticBoxSizer *item11 = new wxStaticBoxSizer( item12, wxVERTICAL );

    wxListCtrl *item13 = new wxListCtrl( parent, ID_LIST, wxDefaultPosition, wxSize(500,300), wxLC_REPORT|wxSUNKEN_BORDER );
    item11->Add( item13, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item11, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item14 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item15 = new wxButton( parent, ID_ADD, wxT("Add..."), wxDefaultPosition, wxDefaultSize, 0 );
    item14->Add( item15, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item16 = new wxButton( parent, ID_MODIFY, wxT("Modify..."), wxDefaultPosition, wxDefaultSize, 0 );
    item14->Add( item16, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item17 = new wxButton( parent, ID_DELETE, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    item14->Add( item17, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item14, 0, wxALIGN_CENTER, 5 );

    wxStaticLine *item18 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add( item18, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item19 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item20 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item19->Add( item20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item21 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item19->Add( item21, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item19, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}
