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
 * @file Informer.cc
 *
 * Implementation of Informer class.
 *
 * @author Atte Oksman 2005 (oksman-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
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
