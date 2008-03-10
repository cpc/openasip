/**
 * @file AddBusEntryCmd.hh
 *
 * Declaration of AddBusEntryCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_ADD_BUS_ENTRY_CMD_HH
#define TTA_ADD_BUS_ENTRY_CMD_CC

#include "GUICommand.hh"

/**
 * Command for adding new bus entries to a HDB.
 */
class AddBusEntryCmd : public GUICommand {
public:
    AddBusEntryCmd();
    virtual ~AddBusEntryCmd();
    virtual bool Do();
    virtual int id() const;
    virtual AddBusEntryCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();

};

#endif
