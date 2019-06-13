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
 * @file Informer.hh
 *
 * Declaration of Informer class.
 *
 * @author Atte Oksman 2005 (oksman-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
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
