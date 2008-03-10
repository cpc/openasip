/**
 * @file AddRFArchitectureCmd.hh
 *
 * Declaration of AddRFArchitectureCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_ADD_RF_ARCHITECTURE_CMD_HH
#define TTA_ADD_RF_ARCHITECTURE_CMD_HH

#include "GUICommand.hh"

/**
 * Command for adding a new register file archtiecture to a HDB.
 */
class AddRFArchitectureCmd : public GUICommand {
public:
    AddRFArchitectureCmd();
    virtual ~AddRFArchitectureCmd();
    virtual bool Do();
    virtual int id() const;
    virtual AddRFArchitectureCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();

};

#endif
