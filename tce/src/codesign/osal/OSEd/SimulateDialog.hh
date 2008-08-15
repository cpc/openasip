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
 * @file SimulateDialog.hh
 *
 * Declaration of SimulateDialog class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
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
