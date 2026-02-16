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

    OSEdInformer(const OSEdInformer&) = delete;
    OSEdInformer& operator=(const OSEdInformer&) = delete;

private:
    /// Container for mapping event ids to Listener objects.
    typedef std::map<EventId, std::vector<OSEdListener*> > ListenerContainer;

    /// All listeners.
    ListenerContainer listeners_;
};

#endif
