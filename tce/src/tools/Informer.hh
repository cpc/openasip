/**
 * @file Informer.hh
 *
 * Declaration of Informer class.
 *
 * @author Atte Oksman 2005 (oksman@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_INFORMER_HH
#define TTA_INFORMER_HH

#include <vector>
#include <utility>

#include "Listener.hh"


/**
 * The Informer class delivers notifications of events (represented by
 * integers) to Listeners.
 *
 * A Listener can register to be notified of certain event(s) and will
 * thereafter receive a notification every time the event takes place.
 * Naturally, a Listener can also be unregistered to not receive any more
 * notifications of the event.
 */
class Informer {

public:
    Informer();
    virtual ~Informer();

    void handleEvent(int event);
    virtual bool registerListener(int event, Listener* listener);
    virtual bool unregisterListener(int event, Listener* listener);

private:
    std::size_t findListenerSlot(int event, Listener* listener);
    typedef std::vector<std::pair<int, Listener*> > ListenerList;
    ListenerList eventListeners_;

};

#include "Informer.icc"

#endif
