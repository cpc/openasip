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
 * @file ProximUnitWindow.cc
 *
 * Declaration of ProximUnitWindow class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */


#ifndef TTA_PROXIM_UNIT_WINDOW_HH
#define TTA_PROXIM_UNIT_WINDOW_HH

#include "ProximSimulatorWindow.hh"
#include "SimulatorEvent.hh"

class SimulatorFrontend;
class ProximMainFrame;
class wxListCtrl;

/**
 * Parent class for windows displaying list information of the Proxim machine
 * state window units.
 *
 * This window listens to SimulatorEvents and updates the window
 * contents automatically.
 */
class ProximUnitWindow : public ProximSimulatorWindow {
public:
    ProximUnitWindow(ProximMainFrame* parent, int id);
    virtual ~ProximUnitWindow();

protected:
    /// Simulator instance .
    SimulatorFrontend* simulator_;
    /// Unit choicer widget.
    wxChoice* unitChoice_;
    /// Value display mode choicer widget.
    wxChoice* modeChoice_;
    /// List widget for the values.
    wxListCtrl* valueList_;

    /// String for the mode choicer integer mode.
    static const wxString MODE_INT;
    /// String for the mode choicer unsigned integer mode.
    static const wxString MODE_UNSIGNED;
    /// String for the mode choicer hexadecimal mode.
    static const wxString MODE_HEX;
    /// String for the mode choicer binary mode.
    static const wxString MODE_BIN;

private:
    void onProgramLoaded(const SimulatorEvent& event);
    void onSimulationStop(const SimulatorEvent& event);
    void onChoice(wxCommandEvent& event);
    void onClose(wxCommandEvent& event);
    wxSizer* createContents(
        wxWindow* parent, bool call_fit, bool set_sizer);

    virtual void reinitialize();
    virtual void update();

    // Widget IDs.
    enum {
	ID_UNIT_CHOICE = 10000,
        ID_MODE_CHOICE,
	ID_HELP,
	ID_CLOSE,
	ID_LINE,
	ID_VALUE_LIST
    };

    DECLARE_EVENT_TABLE()
};

#endif
