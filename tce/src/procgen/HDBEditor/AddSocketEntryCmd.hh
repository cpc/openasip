/**
 * @file AddSocketEntryCmd.hh
 *
 * Declaration of AddSocketEntryCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_ADD_SOCKET_ENTRY_CMD_HH
#define TTA_ADD_SOCKET_ENTRY_CMD_CC

#include "GUICommand.hh"

/**
 * Command for adding new socket entries to a HDB.
 */
class AddSocketEntryCmd : public GUICommand {
public:
    AddSocketEntryCmd();
    virtual ~AddSocketEntryCmd();
    virtual bool Do();
    virtual int id() const;
    virtual AddSocketEntryCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();

};

#endif
