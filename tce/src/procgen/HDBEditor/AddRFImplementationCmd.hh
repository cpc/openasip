/**
 * @file AddRFImplementationCmd.hh
 *
 * Declaration of AddRFImplementationCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_ADD_RF_IMPLEMENTATION_CMD_HH
#define TTA_ADD_RF_IMPLEMENTATION_CMD_HH

#include "GUICommand.hh"

/**
 * Command for adding implementation of a RF architecture to HDB.
 */
class AddRFImplementationCmd : public GUICommand {
public:
    AddRFImplementationCmd();
    virtual ~AddRFImplementationCmd();
    virtual bool Do();
    virtual int id() const;
    virtual AddRFImplementationCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();

};

#endif
