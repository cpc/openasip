/**
 * @file AddFUEntryCmd.hh
 *
 * Declaration of AddFUEntryCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_ADD_FU_ENTRY_CMD_HH
#define TTA_ADD_FU_ENTRY_CMD_HH

#include "GUICommand.hh"

/**
 * Command for adding new FU entries to a HDB.
 */
class AddFUEntryCmd : public GUICommand {
public:
    AddFUEntryCmd();
    virtual ~AddFUEntryCmd();
    virtual bool Do();
    virtual int id() const;
    virtual AddFUEntryCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();

};

#endif
