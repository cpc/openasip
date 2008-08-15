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
 * @file OSEdInformer.cc
 *
 * Definition of OSEdInformer class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#include "OSEdInformer.hh"
#include "OSEdListener.hh"

using std::vector;

/**
 * Constructor.
 */
OSEdInformer::OSEdInformer() {
}

/**
 * Destructor.
 */
OSEdInformer::~OSEdInformer() {
}

/**
 * Handles event of given id.
 *
 * If no such id exists, nothing is done.
 *
 * @param event Event to be handled.
 */
void
OSEdInformer::handleEvent(EventId event) {
    ListenerContainer::iterator iter = listeners_.find(event);
    if (iter != listeners_.end()) {
        for (size_t i = 0; i < (*iter).second.size(); i++) {
            (*iter).second[i]->handleEvent(event);
        }
    }
}

/**
 * Registers listener to informers data base.
 *
 * @param event Event of which given listener is interested in.
 * @param listener Listener to be registered.
 */
void
OSEdInformer::registerListener(EventId event, OSEdListener* listener) {
    ListenerContainer::iterator iter = listeners_.find(event);
    if (iter != listeners_.end()) {
        (*iter).second.push_back(listener);
    } else {
        vector<OSEdListener*> newListenerContainer;
        newListenerContainer.push_back(listener);
        listeners_[event] = newListenerContainer;
    }
}

/**
 * Unregisters listener from the informer.
 *
 * @param event Event for which given listener is registered to listen.
 * @param listener Listener to be unregistered.
 */
void
OSEdInformer::unregisterListener(EventId event, OSEdListener* listener) {
    ListenerContainer::iterator iter = listeners_.find(event);
    if (iter != listeners_.end()) {
        vector<OSEdListener*>::iterator vecIter = (*iter).second.begin();
        while (vecIter != (*iter).second.end()) {
            if (*vecIter == listener) {
                (*iter).second.erase(vecIter);
                return;
            }
            vecIter++;
        }
    }
}
