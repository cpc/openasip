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
 * @file DialogPosition.cc
 *
 * Definition of DialogPosition class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "DialogPosition.hh"

using std::map;

// Initialize static data members.
map<DialogPosition::Dialogs, wxPoint> DialogPosition::positions_;

/**
 * Gets the position of the dialog.
 *
 * If position is not stored, returns wxDefaultPosition.
 *
 * @param dialog Dialog which position is asked.
 * @return The position of the dialog.
 */
wxPoint
DialogPosition::getPosition(Dialogs dialog) {
    PositionMap::iterator it = positions_.find(dialog);
    if (it == positions_.end()) {
        return wxDefaultPosition;
    }
    return (*it).second;
}

/**
 * Sets the position of the dialog.
 * 
 * @param dialog Dialog which position is set.
 */
void
DialogPosition::setPosition(Dialogs dialog, wxPoint point) {
    PositionMap::iterator it = positions_.find(dialog);
    if (it == positions_.end()) {
        positions_.insert(PositionMap::value_type(dialog, point));
    } else {
        (*it).second = point;
    }
}
