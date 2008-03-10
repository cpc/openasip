/**
 * @file SetRFArchitectureCmd.hh
 *
 * Declaration of SetRFArchitectureCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SET_RF_ARCHITECTURE_CMD_HH
#define TTA_SET_RF_ARCHITECTURE_CMD_HH

#include "GUICommand.hh"

/**
 * Command for setting architecture of a RFEntry.
 */
class SetRFArchitectureCmd : public GUICommand {
public:
    SetRFArchitectureCmd();
    virtual ~SetRFArchitectureCmd();
    virtual bool Do();
    virtual int id() const;
    virtual SetRFArchitectureCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();

};

#endif
