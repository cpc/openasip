/**
 * @file DialogPosition.cc
 *
 * Definition of DialogPosition class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
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
