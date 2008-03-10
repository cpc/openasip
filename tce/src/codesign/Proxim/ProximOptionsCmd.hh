/**
 * @file ProximOptionsCmd.hh
 *
 * Declaration of ProximOptionsCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_OPTIONS_CMD_HH
#define TTA_PROXIM_OPTIONS_CMD_HH

#include "GUICommand.hh"

/**
 * Command for editing Proxim options.
 *
 * Creates and shows ProximOptionsDialog.
 */
class ProximOptionsCmd : public GUICommand {
public:
    ProximOptionsCmd();
    virtual ~ProximOptionsCmd();
    virtual bool Do();
    virtual int id() const;
    virtual ProximOptionsCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();
};

#endif
