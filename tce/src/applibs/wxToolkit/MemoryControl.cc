/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file MemoryControl.cc
 *
 * Definition of MemoryControl class.
 *
 * @author Jussi Nyk�nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Veli-Pekka J��skel�inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include <wx/clipbrd.h>
#include "MemoryControl.hh"
#include "tce_config.h"
#include "WxConversion.hh"
#include "Conversion.hh"
#include "ErrorDialog.hh"
#include "MemoryValueDialog.hh"
#include "NumberControl.hh"
#include "MemoryGridTable.hh"
#include "Memory.hh"

using std::string;

const string MemoryControl::SIZE_MAU = "MAU";
const string MemoryControl::SIZE_TWO_MAUS = "2 MAUs";
const string MemoryControl::SIZE_FOUR_MAUS = "4 MAUs";
const string MemoryControl::SIZE_EIGHT_MAUS = "8 MAUs";


const string MemoryControl::DATA_BIN = "Binary";
const string MemoryControl::DATA_HEX = "Hex";
const string MemoryControl::DATA_SIGNED_INT = "Signed int";
const string MemoryControl::DATA_UNSIGNED_INT = "Unsigned int";
const string MemoryControl::DATA_FLOAT = "Float";
const string MemoryControl::DATA_DOUBLE = "Double";

const wxString MemoryControl::WIDTH_8 = _T("8");
const wxString MemoryControl::WIDTH_16 = _T("16");
const wxString MemoryControl::WIDTH_32 = _T("32");

BEGIN_EVENT_TABLE(MemoryControl, wxPanel)

    EVT_BUTTON(ID_BUTTON_GO_TO, MemoryControl::onGoTo)

    EVT_CHOICE(ID_CHOICE_MODE, MemoryControl::onSizeModeChanged)
    EVT_CHOICE(ID_CHOICE_DATA, MemoryControl::onDataModeChanged)
    EVT_CHOICE(ID_CHOICE_WIDTH, MemoryControl::onWidthChanged)
    EVT_GRID_CELL_LEFT_DCLICK(MemoryControl::onWriteMemory)
    EVT_TEXT_ENTER(ID_ADDRESS_GO_TO, MemoryControl::onGoTo)

    EVT_KEY_DOWN(MemoryControl::onChar)
    EVT_CHAR(MemoryControl::onChar)
END_EVENT_TABLE()


/**
 * Constructor.
 *
 * @param parent Parent window.
 * @param memory The memory.
 * @param start The start point of memory.
 * @param end The end point of memory.
 * @param id Id of the widget.
 * @param pos Position of the widget.
 * @param size Size of the widget.
 * @param name Name of the widget.
 */
MemoryControl::MemoryControl(
    wxWindow* parent,
    Memory* memory,
    wxWindowID id,
    const wxPoint& pos,
    const wxSize& size,
    const wxString& name) :
    wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL, name),
    memory_(memory), MAUSize_(memory->MAUSize()), 
    start_(memory->start()), end_(memory->end()),
    grid_(NULL),
    goToAddress_(_T("0")),
    table_(NULL),
    sizer_(NULL),
    mausPerCell_(1) {

    createContents();

    dataMode_->Append(WxConversion::toWxString(DATA_BIN));
    dataMode_->Append(WxConversion::toWxString(DATA_HEX));
    dataMode_->Append(WxConversion::toWxString(DATA_SIGNED_INT));
    dataMode_->Append(WxConversion::toWxString(DATA_UNSIGNED_INT));
    dataMode_->Append(WxConversion::toWxString(DATA_FLOAT));
    dataMode_->Append(WxConversion::toWxString(DATA_DOUBLE));

    dataMode_->SetSelection(1);

    sizeMode_->Append(WxConversion::toWxString(SIZE_MAU));
    if (MAUSize_ <= 32) {
        sizeMode_->Append(WxConversion::toWxString(SIZE_TWO_MAUS));
    }
    if (MAUSize_ <= 16) {
        sizeMode_->Append(WxConversion::toWxString(SIZE_FOUR_MAUS));
    }
    if (MAUSize_ <= 8) {
        sizeMode_->Append(WxConversion::toWxString(SIZE_EIGHT_MAUS));
    }

    sizeMode_->SetSelection(0);

    widthMode_->Append(WIDTH_8);
    widthMode_->Append(WIDTH_16);
    widthMode_->Append(WIDTH_32);

    widthMode_->SetSelection(0);

    wxTextValidator hexValidator(wxFILTER_INCLUDE_CHAR_LIST, &goToAddress_);

#if wxCHECK_VERSION(2, 5, 4)
    wxArrayString includes;
    includes.Add(_T("0"));
    includes.Add(_T("1"));
    includes.Add(_T("2"));
    includes.Add(_T("3"));
    includes.Add(_T("4"));
    includes.Add(_T("5"));
    includes.Add(_T("6"));
    includes.Add(_T("7"));
    includes.Add(_T("8"));
    includes.Add(_T("9"));
    includes.Add(_T("a"));
    includes.Add(_T("b"));
    includes.Add(_T("c"));
    includes.Add(_T("d"));
    includes.Add(_T("e"));
    includes.Add(_T("f"));
    includes.Add(_T("A"));
    includes.Add(_T("B"));
    includes.Add(_T("C"));
    includes.Add(_T("D"));
    includes.Add(_T("E"));
    includes.Add(_T("F"));
    hexValidator.SetIncludes(includes);
#else
    hexValidator.SetIncludeList(
	wxStringList(
	    _T("0"), _T("1"), _T("2"), _T("3"), _T("4"), _T("5"), _T("6"),
	    _T("7"), _T("8"), _T("9"), _T("a"), _T("b"), _T("c"), _T("d"),
	    _T("e"), _T("f"), _T("A"), _T("B"), _T("C"), _T("D"), _T("E"),
	    _T("F"), NULL));
#endif

    FindWindow(ID_ADDRESS_GO_TO)->SetValidator(hexValidator);
    updateView();
}


/**
 * Destructor.
 */
MemoryControl::~MemoryControl() {
}


/**
 * Creates the components of the widget.
 */
void
MemoryControl::createContents() {

    sizer_ = new wxBoxSizer(wxVERTICAL);

    table_ = new MemoryGridTable(*memory_);
    grid_ = new wxGrid(this, ID_GRID, wxDefaultPosition, wxDefaultSize);
    grid_->SetTable(table_);

    grid_->SetDefaultCellAlignment(wxALIGN_RIGHT, wxALIGN_BOTTOM);
    grid_->SetRowLabelSize(100);
    grid_->SetDefaultCellFont(
        wxFont(12, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL,
               wxFONTWEIGHT_NORMAL));

    grid_->EnableEditing(false);
    grid_->DisableDragGridSize();
    grid_->DisableDragRowSize();

    sizer_->Add(grid_, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    dataMode_ = new wxChoice(
        this, ID_CHOICE_DATA, wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
    sizeMode_ = new wxChoice(
        this, ID_CHOICE_MODE, wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
    widthMode_ = new wxChoice(
	this, ID_CHOICE_WIDTH, wxDefaultPosition, wxDefaultSize, 0, NULL, 0);

    wxTextCtrl* addressCtrl = new wxTextCtrl(
        this, ID_ADDRESS_GO_TO, wxT(""), wxDefaultPosition, wxDefaultSize,
        wxTE_PROCESS_ENTER);

    wxButton* goTo = new wxButton(this, ID_BUTTON_GO_TO, wxT("Go to"));
    wxSizer* sizer1 = new wxBoxSizer(wxHORIZONTAL);

    wxStaticText* columnsLabel = new wxStaticText(this, -1, _T("Columns:"));
    wxStaticText* asLabel = new wxStaticText(this, -1, _T("as"));

    sizer1->Add(sizeMode_, 0, wxALIGN_CENTER|wxALL, 5);
    sizer1->Add(asLabel, 0, wxALIGN_CENTER|wxALL, 5);
    sizer1->Add(dataMode_, 0, wxALIGN_CENTER|wxALL, 5);
    sizer1->Add(addressCtrl, 0, wxALIGN_CENTER|wxALL, 5);
    sizer1->Add(goTo, 0, wxALIGN_CENTER|wxALL, 5);
    sizer1->Add(columnsLabel, 0, wxALIGN_CENTER|wxALL, 5);
    sizer1->Add(widthMode_, 0, wxALIGN_CENTER|wxALL, 5);

    sizer_->Add(sizer1, 0, wxALIGN_CENTER|wxALL, 5);

    SetSizer(sizer_);
    Fit();
}


/**
 * Handles the event when Go to button is pushed.
 *
 * The row which represents the asked address is selected.
 */
void
MemoryControl::onGoTo(wxCommandEvent&) {

    TransferDataFromWindow();
    if (goToAddress_ != _T("")) {

        Word addr = 0;
        try {
            // add 0x to the beginning of the address string
            goToAddress_ = _T("0x") + goToAddress_;
            addr = Conversion::toInt(WxConversion::toString(goToAddress_));
        } catch (const NumberFormatException& n) {
            string msg = "Invalid number: " +
                WxConversion::toString(goToAddress_);
            ErrorDialog dialog(this, WxConversion::toWxString(msg));
            dialog.ShowModal();
            return;
        }

        if (addr >= end_) {
            string msg = "Address " + WxConversion::toString(goToAddress_) +
                " out of memory bounds.";
            ErrorDialog dialog(this, WxConversion::toWxString(msg));
            dialog.ShowModal();
            return;
        }

        int row = 0;
	int col = 0;
	table_->findAddress(addr, row, col);
        //grid_->SetCellBackgroundColour(row, col, *wxGREEN);
        grid_->MakeCellVisible(row, col);
        grid_->SetGridCursor(row, col);
        updateView();
    }
}

/**
 * Updates the contents of the memory.
 *
 * Data is retrieved only for those area that is visible to user.
 */
void
MemoryControl::updateView() {

    unsigned digits = table_->sizeOfCell() * MAUSize_;

    string dataMode = WxConversion::toString(dataMode_->GetStringSelection());

    if (dataMode == DATA_HEX) {
        digits = (digits / 4) + 3;
    } else if (dataMode == DATA_SIGNED_INT ||
               dataMode == DATA_UNSIGNED_INT) {

        digits = (digits / 3) + 1;
    } else if (dataMode == DATA_FLOAT ||
               dataMode == DATA_DOUBLE) {

        digits = 16;
    }

    grid_->SetDefaultColSize(digits * 12, true);
    grid_->ForceRefresh();
}


/**
 * Handles the event size in which data is shown is changed.
 */
void
MemoryControl::onSizeModeChanged(wxCommandEvent&) {

    clearHighlights();
    string sizeMode = WxConversion::toString(sizeMode_->GetStringSelection());

    if (sizeMode == SIZE_MAU) {
        mausPerCell_ = 1;
	table_->setSizeMode(MemoryGridTable::SIZE_MAU);
    } else if (sizeMode == SIZE_TWO_MAUS) {
        mausPerCell_ = 2;
	table_->setSizeMode(MemoryGridTable::SIZE_TWO_MAUS);
    } else if (sizeMode == SIZE_FOUR_MAUS) {
        mausPerCell_ = 4;
	table_->setSizeMode(MemoryGridTable::SIZE_FOUR_MAUS);
    } else if (sizeMode == SIZE_EIGHT_MAUS) {
        mausPerCell_ = 8;
	table_->setSizeMode(MemoryGridTable::SIZE_EIGHT_MAUS);
    }

    updateView();
}


/**
 * Handles the event when the data mode is changed.
 */
void
MemoryControl::onDataModeChanged(wxCommandEvent&) {

    string dataMode = WxConversion::toString(dataMode_->GetStringSelection());

    if (dataMode == DATA_BIN) {
	table_->setDataMode(MemoryGridTable::DATA_BIN);
    } else if (dataMode == DATA_HEX) {
	table_->setDataMode(MemoryGridTable::DATA_HEX);
    } else if (dataMode == DATA_SIGNED_INT) {
	table_->setDataMode(MemoryGridTable::DATA_SIGNED_INT);
    } else if (dataMode == DATA_UNSIGNED_INT) {
	table_->setDataMode(MemoryGridTable::DATA_UNSIGNED_INT);
    } else if (dataMode == DATA_FLOAT) {
	table_->setDataMode(MemoryGridTable::DATA_FLOAT);
    } else if (dataMode == DATA_DOUBLE) {
	table_->setDataMode(MemoryGridTable::DATA_DOUBLE);
    }
    updateView();
}

/**
 * Handles the event when the size of the window is changed.
 */
void
MemoryControl::onSize(wxSizeEvent& event) {
    updateView();
    event.Skip();
}

/**
 * Handles the event when user double clicks a memory cell.
 *
 * A new dialog is opened in which user can type a new value for the cell.
 */
void
MemoryControl::onWriteMemory(wxGridEvent& event) {

    int column = event.GetCol();
    int row = event.GetRow();

    string stringValue =
        WxConversion::toString(grid_->GetCellValue(row, column));

    int value = 0;
    MemoryValueDialog dialog(this, table_->sizeOfCell() * MAUSize_);
    dialog.CenterOnParent();

    try {
        value = Conversion::toInt(stringValue);
        dialog.setValue(value);
    } catch (const NumberFormatException& n) {
        // can not convert for some reason (do something?)
        dialog.setValue(0);
    }

    if(dialog.ShowModal() == wxID_OK) {

	if (dialog.mode() != NumberControl::MODE_DOUBLE) {
	    UIntWord memoryValue = dialog.intValue();
	    table_->writeValue(row, column, memoryValue);
	} else {
	    DoubleWord memoryValue = dialog.doubleValue();
	    table_->writeValue(row, column, memoryValue);
	}
        updateView();
    }

}

/**
 * Handles the event when key is pressed.
 *
 * Two hardcoded keyboard commands currently exist: delete and copy.
 *
 * @param event Key event ot be handled.
 */
void
MemoryControl::onChar(wxKeyEvent& event) {

    if (event.GetKeyCode() == WXK_DELETE) {
        if (grid_->IsSelection()) {
            clearMemory();
        }
    } else if (event.m_controlDown &&
               (event.GetKeyCode() == 'c' ||
                event.GetKeyCode() == 'C' ||
                event.GetKeyCode() == WXK_INSERT)) {

        // Copy contents of the selected cells to the clipboard.
        if (grid_->IsSelection()) {
            copySelection();
        }
    }
}


/**
 * Clears the contents of the selected cells, that is, the contents
 * is set to zero.
 */
void
MemoryControl::clearMemory() {

    wxGridCellCoordsArray topLeft = grid_->GetSelectionBlockTopLeft();
    wxGridCellCoordsArray botRight = grid_->GetSelectionBlockBottomRight();

    int currentRow = topLeft[0].GetRow();
    int currentCol = topLeft[0].GetCol();

    int endRow = botRight[0].GetRow();
    int endCol = botRight[0].GetCol();

    while (currentRow <= endRow) {
        currentCol = topLeft[0].GetCol();
        while (currentCol <= endCol) {

            UIntWord data = 0;
            table_->writeValue(currentRow, currentCol, data);
            currentCol++;
        }
        currentRow++;
    }
    updateView();

}


/**
 * Sets the memory to display in the window.
 *
 * @param memory Memory to display.
 */
void
MemoryControl::setMemory(Memory* memory) {

    clearHighlights();
    MAUSize_ = memory->MAUSize();
    memory_ = memory;
    start_ = memory->start();
    end_ = memory->end();

    table_ = new MemoryGridTable(*memory_);
    grid_->SetTable(table_);

    updateView();
}


/**
 * Event handler for the grid width choicer.
 */
void
MemoryControl::onWidthChanged(wxCommandEvent&) {
    clearHighlights();
    wxString width = widthMode_->GetStringSelection();
    if (width == WIDTH_8) {
	table_->setNumberOfColumns(8);
    } else if (width == WIDTH_16) {
	table_->setNumberOfColumns(16);
    } else  if (width == WIDTH_32) {
	table_->setNumberOfColumns(32);
    }
    grid_->SetTable(table_);
    updateView();
}


/**
 * Copies contents of the selected cells to the clipboard.
 */
void
MemoryControl::copySelection() {

    if (grid_ == NULL || !(grid_->IsSelection())) {
        return;
    }

    if (wxTheClipboard->Open()) {

        wxTextDataObject* data = new wxTextDataObject();
        wxString contents;

        wxGridCellCoordsArray topLs = grid_->GetSelectionBlockTopLeft();
        wxGridCellCoordsArray bottomRs = grid_->GetSelectionBlockBottomRight();

        for (unsigned i = 0; i < topLs.Count(); i++) {
            if (i > 0) {
                contents.Append(_T("\n"));
            }
            int row = topLs.Item(i).GetRow();
            for (; row <= bottomRs.Item(i).GetRow(); row++) {
                int col = topLs.Item(i).GetCol();
                for (; col <= bottomRs.Item(i).GetCol(); col++) {
                    contents.Append(grid_->GetCellValue(row, col));
                    contents.Append(_T(" "));
                }
                contents.Append(_T("\n"));
            }
        }

        data->SetText(contents);

        wxTheClipboard->SetData(data);
        wxTheClipboard->Close();
    }
}


void
MemoryControl::clearHighlights() {
    int cols = table_->GetNumberCols();
    while (!highlights_.empty()) {
        int row = *highlights_.begin() / cols;
        int col = *highlights_.begin() % cols;
        grid_->SetCellBackgroundColour(row, col, *wxWHITE);
        highlights_.erase(highlights_.begin());
    }
}

void
MemoryControl::highlight(Word address, unsigned count, const wxColour& colour) {
    if (count < 1 || address < start_ || address + count > end_) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }

    int cols = table_->GetNumberCols();
    for (unsigned i = 0; i < count; i++) {
        unsigned cell = (address - start_ + i) / mausPerCell_;
        int row = cell / cols;
        int col = cell % cols;
        highlights_.push_back(cell);
        grid_->SetCellBackgroundColour(row, col, colour);
    }
}
