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
 * @file AddIUFromHDBDialog.cc
 *
 * Implementation of AddIUFromHDBDialog class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>
#include <wx/dir.h>
#include <wx/imaglist.h>
#include <wx/listctrl.h>
#include <wx/statline.h>

#include "AddIUFromHDBDialog.hh"
#include "Conversion.hh"
#include "Environment.hh"
#include "FileSystem.hh"
#include "HDBManager.hh"
#include "HDBRegistry.hh"
#include "Machine.hh"
#include "MapTools.hh"
#include "Model.hh"
#include "ProDeConstants.hh"
#include "RFArchitecture.hh"
#include "WarningDialog.hh"
#include "WidgetTools.hh"
#include "WxConversion.hh"

#if !wxCHECK_VERSION(3, 0, 0)
typedef long int wxIntPtr;
#endif

using boost::format;
using std::string;
using namespace TTAMachine;
using namespace HDB;

BEGIN_EVENT_TABLE(AddIUFromHDBDialog, wxDialog)
EVT_LIST_ITEM_SELECTED(ID_LIST, AddIUFromHDBDialog::onListSelectionChange)
EVT_LIST_ITEM_DESELECTED(ID_LIST, AddIUFromHDBDialog::onListSelectionChange)
EVT_BUTTON(ID_ADD, AddIUFromHDBDialog::onAdd)
EVT_BUTTON(ID_CLOSE, AddIUFromHDBDialog::onClose)
EVT_LIST_COL_CLICK(ID_LIST, AddIUFromHDBDialog::onColumnClick)
END_EVENT_TABLE()

const int AddIUFromHDBDialog::DEFAULT_SIZE = 1;
const int AddIUFromHDBDialog::DEFAULT_WIDTH = 32;
const Machine::Extension AddIUFromHDBDialog::DEFAULT_EXTENSION_MODE =
    Machine::ZERO;

const wxString AddIUFromHDBDialog::HDB_FILE_FILTER = _T("*.hdb");

int wxCALLBACK IUListCompareASC(wxIntPtr item1, wxIntPtr item2,
                                wxIntPtr sortData) {

  ListItemData *lid1 = (ListItemData *)item1;
  ListItemData *lid2 = (ListItemData *)item2;
  int sortColumn = (int)sortData;

  if (sortColumn == 0) {
    return lid1->width - lid2->width;
  } else if (sortColumn == 1) {
    return lid1->size - lid2->size;
  } else if (sortColumn == 2) {
    return lid1->readPorts - lid2->readPorts;
  } else if (sortColumn == 3) {
    return lid1->latency - lid2->latency;
  } else if (sortColumn == 4) {
    return lid1->hdbId - lid2->hdbId;
  } else if (sortColumn == 5) {
    return lid1->path.Cmp(lid2->path);
  }

  return 0;
}

int wxCALLBACK IUListCompareDESC(wxIntPtr item1, wxIntPtr item2,
                                 wxIntPtr sortData) {

  ListItemData *lid1 = (ListItemData *)item1;
  ListItemData *lid2 = (ListItemData *)item2;
  int sortColumn = (int)sortData;

  if (sortColumn == 0) {
    return lid2->width - lid1->width;
  } else if (sortColumn == 1) {
    return lid2->size - lid1->size;
  } else if (sortColumn == 2) {
    return lid2->readPorts - lid1->readPorts;
  } else if (sortColumn == 3) {
    return lid2->latency - lid1->latency;
  } else if (sortColumn == 4) {
    return lid2->hdbId - lid1->hdbId;
  } else if (sortColumn == 5) {
    return lid2->path.Cmp(lid1->path);
  }

  return 0;
}

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param machine Parent Machine of the immediate slots.
 */
AddIUFromHDBDialog::AddIUFromHDBDialog(wxWindow *parent, Model *model)
    : wxDialog(parent, -1, _T("HDB Immediate Units"), wxDefaultPosition,
               wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
      model_(model), sortColumn_(0), sortASC_(true) {

  createContents(this, true, true);
  SetSize(400, 300);

  list_ = dynamic_cast<wxListCtrl *>(FindWindow(ID_LIST));

  list_->InsertColumn(0, _T("Width"), wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE);
  list_->InsertColumn(1, _T("Size"), wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE);
  list_->InsertColumn(2, _T("Read ports"), wxLIST_FORMAT_LEFT, 100);
  // list_->InsertColumn(5, _T("Max Reads"));
  // list_->InsertColumn(6, _T("Max RW"));
  list_->InsertColumn(3, _T("Latency"), wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE);
  list_->InsertColumn(4, _T("ID"), wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE);
  list_->InsertColumn(5, _T("HDB"), wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE);

  string iconPath =
      Environment::iconDirPath() + FileSystem::DIRECTORY_SEPARATOR;

  wxImageList *imageList = new wxImageList(13, 17);
  imageList->Add(wxIcon(
      WxConversion::toWxString(iconPath + ProDeConstants::ICON_SORT_DESC)));
  imageList->Add(wxIcon(
      WxConversion::toWxString(iconPath + ProDeConstants::ICON_SORT_ASC)));
  list_->SetImageList(imageList, wxIMAGE_LIST_SMALL);

  // Disable conditional buttons.
  FindWindow(ID_ADD)->Disable();
}

/**
 * The Destructor.
 */
AddIUFromHDBDialog::~AddIUFromHDBDialog() {
  MapTools::deleteAllValues(iuArchitectures_);
}

/**
 * Transfers data from the HDBs to the dialog list.
 *
 * @return True, if the the data transfer was succesful.
 */
bool AddIUFromHDBDialog::TransferDataToWindow() {

  MapTools::deleteAllValues(iuArchitectures_);
  list_->DeleteAllItems();

  const std::vector<string> hdbPaths = Environment::hdbPaths();
  std::vector<string>::const_iterator hdbIter = hdbPaths.begin();

  wxString paths;
  bool hdbsFound = false;

  // Read all hdb files found in the search directories.
  for (; hdbIter != hdbPaths.end(); hdbIter++) {

    wxString file;
    wxString hdbPath = WxConversion::toWxString(*hdbIter);
    paths.Append(hdbPath);
    paths.Append(_T("\n"));
    wxDir dir;

    if (dir.Exists(hdbPath) && dir.Open(hdbPath) &&
        dir.GetFirst(&file, HDB_FILE_FILTER)) {

      do {
        string path = *hdbIter + FileSystem::DIRECTORY_SEPARATOR +
                      WxConversion::toString(file);

        if (loadHDB(path)) {
          hdbsFound = true;
        }
      } while (dir.GetNext(&file));
    }
  }

  if (!hdbsFound) {
    wxString message = _T("No HDBs found in HDB search paths:\n");
    message.Append(paths);
    WarningDialog dialog(this, message);
    dialog.ShowModal();
  }

  list_->SetColumnWidth(5, wxLIST_AUTOSIZE);
  return wxDialog::TransferDataToWindow();
}

/**
 * Loads immediate units from a HDB to the dialog list.
 *
 * @param path Full path to the HDB file to load.
 * @return True, if the HDB was succesfully loaded.
 */
bool AddIUFromHDBDialog::loadHDB(const std::string &path) {

  HDBManager *manager = NULL;

  try {
    manager = &HDBRegistry::instance().hdb(path);
  } catch (Exception &e) {
    wxString message = _T("Unable to open HDB '");
    message.Append(WxConversion::toWxString(path));
    message.Append(_T("':'"));
    message.Append(WxConversion::toWxString(e.errorMessage()));
    WarningDialog dialog(this, message);
    dialog.ShowModal();
    return false;
  }

  const std::set<RowID> rfArchIDs = manager->rfArchitectureIDs();
  std::set<RowID>::iterator iter = rfArchIDs.begin();

  // Read properties of all immediate units in the HDB and append
  // data in the list widget.
  for (; iter != rfArchIDs.end(); iter++) {

    RFArchitecture *arch = manager->rfArchitectureByID(*iter);

    if (arch->writePortCount() != 1 || arch->bidirPortCount() != 0) {

      // Register file architecture must contain one read port and
      // no bidirectional ports or it's not listed as an immediate
      // unit architecture.
      continue;
    }

    iuArchitectures_.insert(
        std::pair<int, RFArchitecture *>(list_->GetItemCount(), arch));

    ListItemData *lid = new ListItemData;

    if (arch->hasParameterizedWidth()) {
      list_->InsertItem(0, _T("param"));
      lid->width = 0;
    } else {
      list_->InsertItem(0, WxConversion::toWxString(arch->width()));
      lid->width = arch->width();
    }
    if (arch->hasParameterizedSize()) {
      list_->SetItem(0, 1, _T("param"));
      lid->size = 0;
    } else {
      list_->SetItem(0, 1, WxConversion::toWxString(arch->size()));
      lid->size = arch->size();
    }
    list_->SetItem(0, 2, WxConversion::toWxString(arch->readPortCount()));
    lid->readPorts = arch->readPortCount();
    list_->SetItem(0, 3, WxConversion::toWxString(arch->latency()));
    lid->latency = arch->latency();
    list_->SetItem(0, 4, WxConversion::toWxString(*iter));
    lid->hdbId = *iter;
    list_->SetItem(0, 5, WxConversion::toWxString(path));
    lid->path = WxConversion::toWxString(path);
    lid->id = list_->GetItemCount() - 1;
    list_->SetItemData(0, (long)lid);
  }
  // default sorting column is "Width"
  list_->SortItems(IUListCompareASC, 0);
  setColumnImage(0, sortASC_);

  return true;
}

/**
 * Enables and disables the delete button according to slot list selection.
 */
void AddIUFromHDBDialog::onListSelectionChange(wxListEvent &) {
  if (list_->GetSelectedItemCount() == 1) {
    FindWindow(ID_ADD)->Enable();
  } else {
    FindWindow(ID_ADD)->Disable();
  }
}

/**
 * Adds a new register file to the machine when "Add" button is pressed.
 */
void AddIUFromHDBDialog::onAdd(wxCommandEvent &) {

  long item = -1;
  item = list_->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
  if (item == -1)
    return;

  ListItemData *lid = (ListItemData *)list_->GetItemData(item);
  int id = lid->id;
  const RFArchitecture *arch =
      MapTools::valueForKey<RFArchitecture *>(iuArchitectures_, id);

  Machine *machine = model_->getMachine();

  // Set size and width.
  int size = DEFAULT_SIZE;
  int width = DEFAULT_WIDTH;

  if (!arch->hasParameterizedWidth()) {
    size = arch->width();
  }

  if (!arch->hasParameterizedSize()) {
    size = arch->size();
  }

  // Generate name for the new immediate unit.
  string name =
      "IU_" + Conversion::toString(size) + "x" + Conversion::toString(width);

  string iuName = name;
  int idx = 1;
  while (machine->immediateUnitNavigator().hasItem(iuName)) {
    iuName = name + "_" + Conversion::toString(idx);
    idx++;
  }

  int maxReads = arch->maxReads();
  int guardLatency = arch->guardLatency();

  // Create an immediate unit object and attach it to the machine.
  ImmediateUnit *iu = new ImmediateUnit(iuName, size, width, maxReads,
                                        guardLatency, DEFAULT_EXTENSION_MODE);

  for (int i = 0; i < arch->readPortCount(); i++) {
    string name = "r" + Conversion::toString(i);
    new RFPort(name, *iu);
  }

  model_->pushToStack();
  machine->addImmediateUnit(*iu);
  model_->notifyObservers();
}

/**
 * Closes the dialog when the close button is pressed.
 */
void AddIUFromHDBDialog::onClose(wxCommandEvent &) { Close(); }

/**
 * Creates the dialog contents.
 *
 * @param parent Parent dialog of the contents.
 * @param call_fit If true, fits the contents inside the dialog.
 * @param set_sizer If true, sets the main sizer as dialog contents.
 * @return Top level sizer of the dialog contents.
 */
wxSizer *AddIUFromHDBDialog::createContents(wxWindow *parent, bool call_fit,
                                            bool set_sizer) {

  wxFlexGridSizer *item0 = new wxFlexGridSizer(1, 0, 0);
  item0->AddGrowableCol(0);
  item0->AddGrowableRow(0);

  wxListCtrl *item1 =
      new wxListCtrl(parent, ID_LIST, wxDefaultPosition, wxSize(160, 120),
                     wxLC_REPORT | wxSUNKEN_BORDER);
  item0->Add(item1, 0, wxGROW | wxALL, 5);

  wxButton *item2 = new wxButton(parent, ID_ADD, wxT("&Add"), wxDefaultPosition,
                                 wxDefaultSize, 0);
  item0->Add(item2, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL, 5);

  wxStaticLine *item3 = new wxStaticLine(parent, ID_LINE, wxDefaultPosition,
                                         wxSize(20, -1), wxLI_HORIZONTAL);
  item0->Add(item3, 0, wxGROW | wxALL, 5);

  wxButton *item4 = new wxButton(parent, ID_CLOSE, wxT("&Close"),
                                 wxDefaultPosition, wxDefaultSize, 0);
  item0->Add(item4, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL, 5);

  if (set_sizer) {
    parent->SetSizer(item0);
    if (call_fit) {
      item0->SetSizeHints(parent);
    }
  }

  return item0;
}

/**
 * Sorts HDB IU list according to clicked column.
 */
void AddIUFromHDBDialog::onColumnClick(wxListEvent &event) {

  int clickedColumn = event.GetColumn();

  if (clickedColumn == sortColumn_) {
    sortASC_ = !sortASC_;
  } else {
    sortASC_ = true;
    setColumnImage(sortColumn_, -1); // removes arrow from old column
    sortColumn_ = clickedColumn;
  }

  setColumnImage(clickedColumn, sortASC_);

  if (sortASC_) {
    list_->SortItems(IUListCompareASC, clickedColumn);
  } else {
    list_->SortItems(IUListCompareDESC, clickedColumn);
  }
}

/**
 * Sets sorting arrow image on selected column
 *
 * @param col Column index to set the image
 * @param image Image index in wxImageList
 */
void AddIUFromHDBDialog::setColumnImage(int col, int image) {
  wxListItem item;
  item.SetMask(wxLIST_MASK_IMAGE);
  item.SetImage(image);
  list_->SetColumn(col, item);
}
