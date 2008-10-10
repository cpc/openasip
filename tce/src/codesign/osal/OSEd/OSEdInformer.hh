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
 * @file OSEdInformer.hh
 *
 * Declaration of OSEdInformer class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OSED_INFORMER_HH
#define TTA_OSED_INFORMER_HH

#include <wx/wx.h>
#include <map>
#include <vector>

class OSEdListener;

/**
 * Notifies listeners that a certain event has occurred.
 *
 * Listeners are OSEdListener objects that are interested on events.
 */
class OSEdInformer {
public:
    /**
     * All possible events.
     */
    enum EventId {
        EVENT_REGISTER,    ///< Event when register value might change.
        EVENT_RESET,       ///< Event when operation is reseted.
        EVENT_MEMORY       ///< Event when memory may be changed.
    };
    
    OSEdInformer();
    virtual ~OSEdInformer();

    void handleEvent(EventId event);
    void registerListener(EventId event, OSEdListener* listener);
    void unregisterListener(EventId event, OSEdListener* listener);

private:
    /// Container for mapping event ids to Listener objects.
    typedef std::map<EventId, std::vector<OSEdListener*> > ListenerContainer;

    /// Copying not allowed.
    OSEdInformer(const OSEdInformer&);
    /// Assignment not allowed.
    OSEdInformer& operator=(const OSEdInformer&);

    /// All listeners.
    ListenerContainer listeners_;
};

#endif
