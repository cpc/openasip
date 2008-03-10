/**
 * @file ProximNewRegisterWindowCmd.hh
 *
 * Declaration of ProximNewRegisterWindowCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_NEW_REGISTER_WINDOW_CMD_HH
#define TTA_PROXIM_NEW_REGISTER_WINDOW_CMD_HH

#include "GUICommand.hh"

/**
 * Command for opening a new register  window in Proxim.
 */
class ProximNewRegisterWindowCmd : public GUICommand {
public:
    ProximNewRegisterWindowCmd();
    virtual ~ProximNewRegisterWindowCmd();
    virtual bool Do();
    virtual int id() const;
    virtual ProximNewRegisterWindowCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();

};
#endif
