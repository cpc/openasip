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
 * @file SimulateDialog.hh
 *
 * Declaration of SimulateDialog class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIMULATE_DIALOG_HH
#define TTA_SIMULATE_DIALOG_HH

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <vector>
#include <string>

#include "OperationContext.hh"
#include "SimValue.hh"

class SimulationInfoDialog;
class Operation;
class DataObject;
class NumberControl;

/**
 * Class for simulating operation behavior.
 */
class SimulateDialog : public wxDialog {
public:

    SimulateDialog(
        wxWindow* parent, 
        Operation* operation, 
        const std::string& pathName,
        const std::string& modName);
    virtual ~SimulateDialog();

    std::string formattedValue(SimValue* value);

    /// Name of the dialog so it can be found with wxWindow::FindWindowByName
    static const wxString DIALOG_NAME;

private:
    /// Copying not allowed.
    SimulateDialog(const SimulateDialog&);
    /// Assignment not allowed.
    SimulateDialog& operator=(const SimulateDialog&);

    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    wxBitmap createBitmap(unsigned int index);
    virtual bool TransferDataToWindow();
    void updateLists();
    void setTexts();
    void onSelection(wxListEvent& event);
    void onUpdateValue(wxCommandEvent& event);
    void onReset(wxCommandEvent& event);
    void onTrigger(wxCommandEvent& event);
    void onLateResult(wxCommandEvent& event);
    void onAdvanceClock(wxCommandEvent& event);
    void onFormatChanged(wxCommandEvent& event);
    void showOrHideRegisters(wxCommandEvent& event);
    void onOK(wxCommandEvent& event);
    void onClose(wxCloseEvent&);
    void setInputValues(std::vector<SimValue*> inputs);
    void setOutputValues(std::vector<SimValue*> outputs);
    void createState();

    /// Signed int format.
    static const std::string FORMAT_SIGNED_INT;
    /// Unsigned int format.
    static const std::string FORMAT_UNSIGNED_INT;
    /// Double format.
    static const std::string FORMAT_DOUBLE;
    /// Float format.
    static const std::string FORMAT_FLOAT;
    /// Binary format.
    static const std::string FORMAT_BINARY;
    /// Hexadecimal format.
    static const std::string FORMAT_HEXA_DECIMAL;

    /**
     * Component ids.
     */
    enum {
        ID_INPUT_LIST = 100000,
        ID_VALUE,
        ID_BUTTON_UPDATE,
        ID_OUTPUT_LIST,
        ID_TEXT_READY,
        ID_BITMAP_READY,
        ID_TEXT_UPDATE,
        ID_BITMAP_UPDATE,
        ID_TEXT_CLOCK,
        ID_TEXT_CLOCK_VALUE,
        ID_BUTTON_RESET,
        ID_BUTTON_TRIGGER,
        ID_BUTTON_LATE_RESULT,
        ID_BUTTON_ADVANCE_LOCK,
        ID_BUTTON_SHOW_HIDE_REGISTERS,
        ID_FORMAT_TEXT,
        ID_FORMAT
    };

    /// Input operand sizer.
    wxStaticBoxSizer* inputSizer_;
    /// Output operand sizer.
    wxStaticBoxSizer* outputSizer_;
    /// Input list.
    wxListCtrl* inputList_;
    /// Output list.
    wxListCtrl* outputList_;
    /// Text control for updating input values.
    NumberControl* updateValue_;
    /// Choice list of possible output formats.
    wxChoice* format_;
    /// Bitmap for Trigger result.
    //wxStaticBitmap* triggerBM_;
    /// Bitmap for Late Result result.
    //wxStaticBitmap* lateResultBM_;
    /// Operation to be simulated.
    Operation* operation_;
    /// Path in which operation module belongs to.
    std::string pathName_;
    /// Module in which operation belongs to.
    std::string modName_;
    /// List of input values.
    std::vector<DataObject*> inputs_;
    /// List of output values.
    std::vector<DataObject*> outputs_;
    /// Variable where the value of clock is kept.
    unsigned int clock_;
    /// Dialog for showing additional information.
    SimulationInfoDialog* infoDialog_;
    
    DECLARE_EVENT_TABLE()
};

#endif
