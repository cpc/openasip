/**
 * @file OSEdInformer.hh
 *
 * Declaration of OSEdInformer class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
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
