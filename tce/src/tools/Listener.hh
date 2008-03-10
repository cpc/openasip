/**
 * @file Listener.hh
 *
 * Declaration of Listener class.
 *
 * @author Atte Oksman 2005 (oksman@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_LISTENER_HH
#define TTA_LISTENER_HH


/**
 *
 */
class Listener {
public:
    Listener();
    virtual ~Listener();
    virtual void handleEvent(int event);
    virtual void handleEvent();

};

#endif
