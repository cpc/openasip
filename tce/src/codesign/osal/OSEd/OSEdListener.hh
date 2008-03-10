/**
 * @file OSEdListener.hh
 *
 * Declaration of OSEdListener class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OSED_LISTENER_HH
#define TTA_OSED_LISTENER_HH

#include "OSEdInformer.hh"

/**
 * Interface class that is the base class for all listener classes
 * in OSEd.
 */
class OSEdListener {
public:
    OSEdListener();
    virtual ~OSEdListener();
    virtual void handleEvent(OSEdInformer::EventId id) = 0;

private:
    /// Copying not allowed.
    OSEdListener(const OSEdListener&);
    /// Assignment not allowed.
    OSEdListener& operator=(const OSEdListener&);
};

#endif
