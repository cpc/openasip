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
 * @file DialogPosition.hh
 *
 * Declaration of DialogPosition class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DIALOG_POSITION_HH
#define TTA_DIALOG_POSITION_HH

#include <wx/wx.h>
#include <map>

/**
 * Class that holds the positions of dialogs.
 */
class DialogPosition {
public:
    /**
     * Enumeration of all dialogs in application.
     */
    enum Dialogs {
        DIALOG_PROPERTIES,        ///< Operation properties dialog.
        DIALOG_SIMULATE,          ///< Simulation dialog.
        DIALOG_INPUT_OPERAND,     ///< Input operand dialog.
        DIALOG_OUTPUT_OPERAND,    ///< Output operand dialog.
        DIALOG_RESULT,            ///< Result dialog.
        DIALOG_OPTIONS,           ///< Options dialog.
        DIALOG_ADD_MODULE,        ///< Add module dialog.
        DIALOG_SIMULATION_INFO,   ///< Simulation info dialog.
        DIALOG_MEMORY,            ///< Memory dialog.
        DIALOG_ABOUT              ///< About dialog.
    };

    static wxPoint getPosition(Dialogs dialog);
    static void setPosition(Dialogs dialog, wxPoint point);

private:
    /// Container for dialog positions.
    typedef std::map<Dialogs, wxPoint> PositionMap;

    /// Copying not allowed.
    DialogPosition(const DialogPosition&);
    /// Assignment not allowed.
    DialogPosition& operator=(const DialogPosition&);

    DialogPosition();
    ~DialogPosition();

    /// Contains all dialog positions.
    static PositionMap positions_;
};

#endif
