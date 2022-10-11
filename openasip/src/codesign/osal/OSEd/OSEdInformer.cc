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
 * @file OSEdInformer.cc
 *
 * Definition of OSEdInformer class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
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
