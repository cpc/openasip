/**
 * @file SetFUArchitectureCmd.hh
 *
 * Declaration of SetFUArchitectureCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SET_FU_ARCHITECTURE_CMD_HH
#define TTA_SET_FU_ARCHITECTURE_CMD_HH

#include "GUICommand.hh"

/**
 * Command for setting architecture of a FUEntry.
 */
class SetFUArchitectureCmd : public GUICommand {
public:
    SetFUArchitectureCmd();
    virtual ~SetFUArchitectureCmd();
    virtual bool Do();
    virtual int id() const;
    virtual SetFUArchitectureCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();

};

#endif
