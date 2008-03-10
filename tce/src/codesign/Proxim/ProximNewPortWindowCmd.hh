/**
 * @file ProximNewPortWindowCmd.hh
 *
 * Declaration of ProximNewPortWindowCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_NEW_PORT_WINDOW_CMD_HH
#define TTA_PROXIM_NEW_PORT_WINDOW_CMD_HH

#include "GUICommand.hh"

/**
 * Command for opening a new port  window in Proxim.
 */
class ProximNewPortWindowCmd : public GUICommand {
public:
    ProximNewPortWindowCmd();
    virtual ~ProximNewPortWindowCmd();
    virtual bool Do();
    virtual int id() const;
    virtual ProximNewPortWindowCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();

};
#endif
