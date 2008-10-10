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
 * @file ProximUnitWindow.cc
 *
 * Declaration of ProximUnitWindow class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
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

    // Toplevel sizer for the window widgets.
    wxBoxSizer* sizer_;

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
