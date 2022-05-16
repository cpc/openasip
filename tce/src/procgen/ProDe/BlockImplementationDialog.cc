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
 * @file BlockImplementationDialog.cc
 *
 * Implementation of BlockImplementationDialog class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @author Esa Määttä 2007 (esa.maatta-no.spam-tut.fi)
 * @note rating: red
 */

#include "BlockImplementationDialog.hh"
#include "Conversion.hh"
#include "ErrorDialog.hh"
#include "FUArchitecture.hh"
#include "FUEntry.hh"
#include "FUImplementation.hh"
#include "FunctionUnit.hh"
#include "HDBManager.hh"
#include "HDBRegistry.hh"
#include "ImmediateUnit.hh"
#include "InformationDialog.hh"
#include "RFArchitecture.hh"
#include "RFEntry.hh"
#include "RFImplementation.hh"
#include "RegisterFile.hh"
#include "StringTools.hh"
#include "UnitImplementationLocation.hh"
#include "WidgetTools.hh"
#include "WxConversion.hh"
#include <wx/statline.h>

#if wxCHECK_VERSION(3, 0, 0)
#define wxOPEN wxFD_OPEN
#define wxFILE_MUST_EXIST wxFD_FILE_MUST_EXIST
#endif

BEGIN_EVENT_TABLE(BlockImplementationDialog, wxDialog)
EVT_BUTTON(ID_BROWSE, BlockImplementationDialog::onBrowse)
EVT_BUTTON(wxID_OK, BlockImplementationDialog::onOK)
EVT_CHOICE(ID_HDB_CHOICE, BlockImplementationDialog::onHDBSelection)

EVT_LIST_ITEM_FOCUSED(ID_LIST, BlockImplementationDialog::onImplSelection)
EVT_LIST_DELETE_ITEM(ID_LIST, BlockImplementationDialog::onImplSelection)
EVT_LIST_ITEM_SELECTED(ID_LIST, BlockImplementationDialog::onImplSelection)
EVT_LIST_ITEM_DESELECTED(ID_LIST, BlockImplementationDialog::onImplSelection)
EVT_LIST_ITEM_ACTIVATED(ID_LIST, BlockImplementationDialog::onImplActivation)
END_EVENT_TABLE()
using namespace IDF;
using namespace TTAMachine;
using namespace HDB;

std::set<std::string> BlockImplementationDialog::hdbs_;
int BlockImplementationDialog::selection_ = 0;
const std::string BlockImplementationDialog::defaultHDB_("asic_130nm_1.5V.hdb");
/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param block Block to select the implementation for.
 * @param impl IDF UnitImplementationLocation object for the block.
 */
BlockImplementationDialog::BlockImplementationDialog(
    wxWindow *parent, const TTAMachine::Component &block,
    UnitImplementationLocation &impl)
    : wxDialog(parent, -1, _T("Select implementation from a HDB file"),
               wxDefaultPosition, wxDefaultSize,
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
      block_(block), impl_(impl) {

  createContents(this, true, true);
  SetSize(wxSize(580, 300));

  list_ = dynamic_cast<wxListCtrl *>(FindWindow(ID_LIST));
  hdbChoice_ = dynamic_cast<wxChoice *>(FindWindow(ID_HDB_CHOICE));

  list_->InsertColumn(0, _T("module name"), wxLIST_FORMAT_LEFT, 150);
  list_->InsertColumn(1, _T("id"), wxLIST_FORMAT_LEFT, 50);
  list_->InsertColumn(2, _T("param size"), wxLIST_FORMAT_LEFT, 100);
  list_->InsertColumn(3, _T("param width"), wxLIST_FORMAT_LEFT, 100);

  if (dynamic_cast<const RegisterFile *>(&block) != 0) {
    list_->InsertColumn(4, _T("guard latency"), wxLIST_FORMAT_LEFT, 170);
  }

  HDBRegistry &registry = HDBRegistry::instance();
  registry.loadFromSearchPaths();
  for (int i = 0; i < registry.hdbCount(); i++) {
    hdbs_.insert(registry.hdbPath(i));
  }
  if (!hdbs_.empty()) {
    std::set<std::string>::iterator iter = hdbs_.begin();
    bool defaultHDBFound = false;
    for (; iter != hdbs_.end(); iter++) {
      std::string shortPath = Environment::shortHDBPath(*iter);
      hdbChoice_->Append(WxConversion::toWxString(shortPath));
      if (!defaultHDBFound && StringTools::endsWith(*iter, defaultHDB_)) {
        selection_ = hdbChoice_->GetCount() - 1;
        defaultHDBFound = true;
      }
    }
    hdbChoice_->SetSelection(selection_);
    wxCommandEvent dummy;
    onHDBSelection(dummy);
  }
  FindWindow(wxID_OK)->Disable();
}

/**
 * The Destructor.
 */
BlockImplementationDialog::~BlockImplementationDialog() {}

/**
 * Event handler for the 'Browse...' button.
 *
 * Opens a file dialog.
 */
void BlockImplementationDialog::onBrowse(wxCommandEvent &) {
  wxFileDialog dialog(
      this, _T("Choose a HDB file containing the implementation"), _T(""),
      _T(""), _T("HDBs|*.hdb|All files|*.*"), (wxOPEN | wxFILE_MUST_EXIST));

  if (dialog.ShowModal() == wxID_OK) {
    std::string hdb = std::string(dialog.GetPath().mb_str());
    hdb = Environment::shortHDBPath(hdb);
    auto wxHDB = WxConversion::toWxString(hdb);
    int item = hdbChoice_->FindString(wxHDB);
    if (item == wxNOT_FOUND) {
      item = hdbChoice_->Append(wxHDB);
    }
    hdbChoice_->Select(item);
    wxListEvent dummy;
    onHDBSelection(dummy);
  }
}

void BlockImplementationDialog::onImplSelection(wxListEvent &) {
  if (list_->GetSelectedItemCount() == 1) {
    FindWindow(wxID_OK)->Enable();
  } else {
    FindWindow(wxID_OK)->Disable();
  }
}

/**
 * Event handler for the HDB choicer widget.
 */
void BlockImplementationDialog::onHDBSelection(wxCommandEvent &) {

  std::string path = WxConversion::toString(hdbChoice_->GetStringSelection());
  path = Environment::longHDBPath(path);

  list_->DeleteAllItems();

  try {
    HDBManager &hdb = HDBRegistry::instance().hdb(path);

    const FunctionUnit *fu = dynamic_cast<const FunctionUnit *>(&block_);
    if (fu != NULL) {
      int i = 0;
      std::set<RowID> fuEntryIDs = hdb.fuEntriesByArchitecture(*fu);
      if (fuEntryIDs.empty()) {
        wxString message = _T("No implementations for '");
        message.Append(WxConversion::toWxString(fu->name()));
        message.Append(_T("' found in '"));
        message.Append(hdbChoice_->GetStringSelection());
        message.Append(_T("'."));
        InformationDialog dialog(this, message);
        dialog.ShowModal();
      }
      std::set<RowID>::iterator iter = fuEntryIDs.begin();
      for (; iter != fuEntryIDs.end(); iter++) {

        if (hdb.fuByEntryID(*iter)->hasImplementation()) {
          const FUImplementation &impl =
              hdb.fuByEntryID(*iter)->implementation();
          const FUArchitecture &arch = hdb.fuByEntryID(*iter)->architecture();
          list_->InsertItem(i, WxConversion::toWxString(impl.moduleName()));
          // we want to select the FU *entry ID* which points to the
          // implementation
          list_->SetItem(i, 1, WxConversion::toWxString(*iter));
          if (arch.hasParameterizedWidth("p1")) {
            list_->SetItem(i, 3, WxConversion::toWxString("*"));
          }
        }
      }
    }

    // Register files.
    const BaseRegisterFile *rf =
        dynamic_cast<const BaseRegisterFile *>(&block_);

    if (rf != NULL) {
      int width = rf->width();
      int size = rf->size();
      int i = 0;
      int readPorts = 0;
      int writePorts = 0;
      int bidirPorts = 0;
      for (int p = 0; p < rf->portCount(); p++) {
        const RFPort *port = rf->port(p);
        if (port->inputSocket() != NULL && port->outputSocket() != NULL) {

          bidirPorts++;
        } else if (port->inputSocket() != NULL) {
          writePorts++;
        } else if (port->outputSocket() != NULL) {
          readPorts++;
        }
      }

      int maxReads = 0;
      int maxWrites = 0;
      int guardLatency = 0;
      int latency = 1;
      const RegisterFile *r = dynamic_cast<const RegisterFile *>(rf);
      if (r != NULL) {
        maxReads = r->maxReads();
        maxWrites = r->maxWrites();
        guardLatency = r->guardLatency();
      }
      const ImmediateUnit *iu = dynamic_cast<const ImmediateUnit *>(rf);
      if (iu != NULL) {
        // TODO: XXX
        latency = iu->latency();
        // Immediate units have always one write port (not seen in adf)
        writePorts = 1;
      }
      std::set<RowID> rfEntryIDs = hdb.rfEntriesByArchitecture(
          readPorts, writePorts, bidirPorts, maxReads, maxWrites, latency,
          r->isUsedAsGuard(), guardLatency, width, size);

      if (rfEntryIDs.empty()) {
        wxString message = _T("No implementations for '");
        message.Append(WxConversion::toWxString(rf->name()));
        message.Append(_T("' found in '"));
        message.Append(hdbChoice_->GetStringSelection());
        message.Append(_T("'."));
        InformationDialog dialog(this, message);
        dialog.ShowModal();
      }
      std::set<RowID>::iterator iter = rfEntryIDs.begin();
      for (; iter != rfEntryIDs.end(); iter++) {
        if (hdb.rfByEntryID(*iter)->hasImplementation()) {

          const RFImplementation &impl =
              hdb.rfByEntryID(*iter)->implementation();

          const RFArchitecture &arch = hdb.rfByEntryID(*iter)->architecture();

          list_->InsertItem(i, WxConversion::toWxString(impl.moduleName()));

          // we want to select the RF *entry ID* which points to the
          // implementation
          list_->SetItem(i, 1, WxConversion::toWxString(*iter));

          if (arch.hasParameterizedSize()) {
            list_->SetItem(i, 2, WxConversion::toWxString("*"));
          }
          if (arch.hasParameterizedWidth()) {
            list_->SetItem(i, 3, WxConversion::toWxString("*"));
          }

          list_->SetItem(i, 4,
                         r->isUsedAsGuard()
                             ? WxConversion::toWxString(guardLatency)
                             : WxConversion::toWxString("not used as guard"));
        }
      }
    }
  } catch (Exception &e) {
    wxString message = _T("Error opening '");
    message.Append(WxConversion::toWxString(path));
    message.Append(_T("':\n\n"));
    message.Append(WxConversion::toWxString(e.errorMessage()));
    ErrorDialog dialog(this, message);
    dialog.ShowModal();
    hdbChoice_->Delete(hdbChoice_->GetSelection());
    return;
  }

  hdbs_.insert(path);
  selection_ = hdbChoice_->GetSelection();
}

/**
 * Do the things that are done to close the dialog on ok
 */
void BlockImplementationDialog::doOK() {
  std::string HDBpath =
      WxConversion::toString(hdbChoice_->GetStringSelection());
  HDBpath = Environment::longHDBPath(HDBpath);
  impl_.setHDBFile(HDBpath);
  int id = Conversion::toInt(WidgetTools::lcStringSelection(list_, 1));
  impl_.setID(id);
  EndModal(wxID_OK);
}

/**
 * Event handler for the OK button.
 *
 * Updates the UnitImplementationLocation object.
 */
void BlockImplementationDialog::onOK(wxCommandEvent &) { doOK(); }

/**
 * Event handler for list activation OK button.
 *
 * Updates the UnitImplementationLocation object.
 */
void BlockImplementationDialog::onImplActivation(wxListEvent &) { doOK(); }

/**
 * Creates the dialog widgets.
 */
wxSizer *BlockImplementationDialog::createContents(wxWindow *parent,
                                                   bool call_fit,
                                                   bool set_sizer) {
  wxFlexGridSizer *item0 = new wxFlexGridSizer(1, 0, 0);
  item0->AddGrowableCol(0);
  item0->AddGrowableRow(1);

  wxBoxSizer *item1 = new wxBoxSizer(wxHORIZONTAL);

  wxString *strs2 = (wxString *)NULL;
  wxChoice *item2 = new wxChoice(parent, ID_HDB_CHOICE, wxDefaultPosition,
                                 wxSize(250, -1), 0, strs2, 0);
  item1->Add(item2, 0, wxALIGN_CENTER | wxALL, 5);

  wxButton *item3 = new wxButton(parent, ID_BROWSE, wxT("Browse..."),
                                 wxDefaultPosition, wxDefaultSize, 0);
  item1->Add(item3, 0, wxALIGN_CENTER | wxALL, 5);

  item0->Add(item1, 0, wxALIGN_CENTER | wxALL, 5);

  wxListCtrl *item4 =
      new wxListCtrl(parent, ID_LIST, wxDefaultPosition, wxSize(300, 120),
                     wxLC_REPORT | wxSUNKEN_BORDER);
  item0->Add(item4, 0, wxGROW | wxALL, 5);

  wxStaticLine *item5 = new wxStaticLine(parent, ID_LINE, wxDefaultPosition,
                                         wxSize(20, -1), wxLI_HORIZONTAL);
  item0->Add(item5, 0, wxGROW | wxALL, 5);

  wxBoxSizer *item6 = new wxBoxSizer(wxHORIZONTAL);

  wxButton *item7 = new wxButton(parent, wxID_CANCEL, wxT("&Cancel"),
                                 wxDefaultPosition, wxDefaultSize, 0);
  item6->Add(item7, 0, wxALIGN_CENTER | wxALL, 5);

  wxButton *item8 = new wxButton(parent, wxID_OK, wxT("&OK"), wxDefaultPosition,
                                 wxDefaultSize, 0);
  item6->Add(item8, 0, wxALIGN_CENTER | wxALL, 5);

  item0->Add(item6, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL, 5);

  if (set_sizer) {
    parent->SetSizer(item0);
    if (call_fit)
      item0->SetSizeHints(parent);
  }

  return item0;
}
