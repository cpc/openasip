/**
 * @file AddFUImplementationCmd.hh
 *
 * Declaration of AddFUImplementationCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_ADD_FU_IMPLEMENTATION_CMD_HH
#define TTA_ADD_FU_IMPLEMENTATION_CMD_HH

#include "GUICommand.hh"

/**
 * Command for adding implementation of a FU architecture to HDB.
 */
class AddFUImplementationCmd : public GUICommand {
public:
    AddFUImplementationCmd();
    virtual ~AddFUImplementationCmd();
    virtual bool Do();
    virtual int id() const;
    virtual AddFUImplementationCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();

};

#endif
