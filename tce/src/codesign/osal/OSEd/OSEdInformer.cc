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
