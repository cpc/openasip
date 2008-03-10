/**
 * @file AddSocketCmd.hh
 *
 * Declaration of AddSocketCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_ADD_SOCKET_CMD_HH
#define TTA_ADD_SOCKET_CMD_HH

#include "EditorCommand.hh"

/**
 * Command for adding new sockets to the Machine.
 *
 * Displays a socket dialog and creates a new socket according to the
 * dialog output.
 */
class AddSocketCmd : public EditorCommand {
public:
    AddSocketCmd();
    virtual bool Do();
    virtual int id() const;
    virtual AddSocketCmd* create() const;
    virtual std::string shortName() const;
    virtual bool isEnabled();
};

#endif
