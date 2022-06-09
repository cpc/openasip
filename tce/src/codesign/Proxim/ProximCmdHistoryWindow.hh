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
 * @file ProximCmdHistoryWindow.hh
 *
 * Declaration of ProximCmdHistoryWindow class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_CMD_HISTORY_WINDOW_HH
#define TTA_PROXIM_CMD_HISTORY_WINDOW_HH

#include <wx/wx.h>

#include "ProximSimulatorWindow.hh"

class SimulatorEvent;
class ProximLineReader;
class ProximMainFrame;

/**
 * Window for browsing Proxim command history.
 */
class ProximCmdHistoryWindow : public ProximSimulatorWindow {
public:
    ProximCmdHistoryWindow(
        ProximMainFrame* parent,
        wxWindowID id,
        ProximLineReader& lineReader);

    virtual ~ProximCmdHistoryWindow();
private:
    void onSimulatorCommand(SimulatorEvent& event);
    void updateCommandList();
    wxSizer* createContents(wxWindow* parent, bool set_sizer, bool call_fit);

    void onClose(wxCommandEvent& event);
    void onSave(wxCommandEvent& event);
    void onCommandDClick(wxCommandEvent& event);

    // Widget IDs.
    enum {
        ID_LIST = 10000,
        ID_SAVE,
        ID_CLOSE,
        ID_LINE
    };

    /// Line reader storing the command history.
    ProximLineReader& lineReader_;
    /// ListBox widget displaying the command history.
    wxListBox* cmdList_;

    DECLARE_EVENT_TABLE()
};
#endif
