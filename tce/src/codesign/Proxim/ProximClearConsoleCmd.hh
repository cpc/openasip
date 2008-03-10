/**
 * @file ProximClearConsoleCmd.hh
 *
 * Declaration of ProximClearConsoleCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_CLEAR_CONSOLE_CMD_HH
#define TTA_PROXIM_CLEAR_CONSOLE_CMD_HH

#include "GUICommand.hh"

/**
 * Command for clearing the console window in Proxim.
 */
class ProximClearConsoleCmd : public GUICommand {
public:
    ProximClearConsoleCmd();
    virtual ~ProximClearConsoleCmd();
    virtual bool Do();
    virtual int id() const;
    virtual ProximClearConsoleCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();

};
#endif
