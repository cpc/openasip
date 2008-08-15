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
