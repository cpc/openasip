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
 * @file Informer.cc
 *
 * Implementation of Informer class.
 *
 * @author Atte Oksman 2005 (oksman@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <utility>

#include "Informer.hh"
#include "Listener.hh"
#include "Application.hh"


/**
 * Constructor.
 */
Informer::Informer() {
}


/**
 * Destructor.
 */
Informer::~Informer() {
}

/**
 * Finds the event slot for the given listener listening to the given event.
 *
 * In case the listener is not found, it's added to the list and index of
 * it is returned.
 *
 * @param event Event the listener is listening to.
 * @param listener The listener.
 * @return The index of the event slot.
 */
inline std::size_t
Informer::findListenerSlot(int event, Listener* listener) {
    for (std::size_t i = 0; i < eventListeners_.size(); ++i) {
        if (eventListeners_.at(i).first == event &&
            eventListeners_.at(i).second == listener) {
            return i;
        }
    }
    eventListeners_.push_back(std::make_pair(event, listener));
    return eventListeners_.size() - 1;
}

/**
 * Adds the given Listener to the list of listeners of the given event.
 *
 * After this function has been called, the listener will be notified every
 * time the event occurs.
 *
 * @param event The code identifying the event.
 * @param listener The Listener that should be added.
 * @return 'True' if adding of the listener succeeds, 'false' otherwise.
 */
bool
Informer::registerListener(int event, Listener* listener) {
    findListenerSlot(event, listener);
    return true;
}


/**
 * Removes the given Listener from the list of listeners of the given event.
 *
 * After this function has been called, the listener will not be notified
 * that the event has occurred.
 *
 * @param event The code identifying the event.
 * @param listener The Listener that should be removed.
 * @return 'True' if removing the listener succeeds, 'false' otherwise.
 */
bool
Informer::unregisterListener(int event, Listener* listener) {
    std::size_t index = findListenerSlot(event, listener);
    ListenerList::iterator i = eventListeners_.begin() + index;
    eventListeners_.erase(i);
    return true;
}
