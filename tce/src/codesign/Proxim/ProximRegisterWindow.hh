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
 * @file ProximRegisterWindow.hh
 *
 * Declaration of ProximRegisterWindow class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */


#ifndef TTA_PROXIM_REGISTER_WINDOW_HH
#define TTA_PROXIM_REGISTER_WINDOW_HH

#include "SimulatorEvent.hh"
#include "ProximUnitWindow.hh"

namespace TTAMachine {
    class RegisterFile;
    class ImmediateUnit;
}

class ProximMainFrame;

/**
 * Proxim subwindow which displays register values.
 *
 * This window listens to SimulatorEvents and updates the window
 * contents automatically.
 */
class ProximRegisterWindow : public ProximUnitWindow {
public:
    ProximRegisterWindow(ProximMainFrame* parent, int id);
    virtual ~ProximRegisterWindow();
    void showRegisterFile(const std::string& name);
    void showImmediateUnit(const std::string& name);

private:
    void loadRegisterFile(const TTAMachine::RegisterFile& rf);
    void loadImmediateUnit(const TTAMachine::ImmediateUnit& iu);
    void loadUniversalIntegerRF();
    void loadUniversalFloatRF();
    void loadUniversalBoolRF();
    virtual void reinitialize();
    virtual void update();

    /// Unit choicer string for universal machine integer register file.
    static const wxString UNIVERSAL_INT_RF;
    /// Unit choicer string for universal machine double register file.
    static const wxString UNIVERSAL_DOUBLE_RF;
    /// Unit choicer string for universal machine boolean register file.
    static const wxString UNIVERSAL_BOOL_RF;

    /// Prefix for register files in the unit choicer.
    static const std::string RF_PREFIX;
    /// Prefix for immediate units in the unit choicer.
    static const std::string IMM_PREFIX;
};

#endif
