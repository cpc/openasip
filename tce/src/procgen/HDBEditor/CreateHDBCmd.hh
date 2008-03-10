/**
 * @file CreateHDBCmd.hh
 *
 * Declaration of CreateHDBCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_CREATE_HDB_CMD_HH
#define TTA_CREATE_HDB_CMD_HH

#include "GUICommand.hh"

/**
 * Command for creating a new HDB in HDBEditor.
 *
 * Displays a file dialog for choosing filename for the new HDB.
 */
class CreateHDBCmd : public GUICommand {
public:
    CreateHDBCmd();
    virtual ~CreateHDBCmd();
    virtual bool Do();
    virtual int id() const;
    virtual CreateHDBCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();

};
#endif
