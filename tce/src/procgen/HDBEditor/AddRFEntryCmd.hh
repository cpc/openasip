/**
 * @file AddRFEntryCmd.hh
 *
 * Declaration of AddRFEntryCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_ADD_RF_ENTRY_CMD_HH
#define TTA_ADD_RF_ENTRY_CMD_CC

#include "GUICommand.hh"

/**
 * Command for adding new RF entries to a HDB.
 */
class AddRFEntryCmd : public GUICommand {
public:
    AddRFEntryCmd();
    virtual ~AddRFEntryCmd();
    virtual bool Do();
    virtual int id() const;
    virtual AddRFEntryCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();

};

#endif
