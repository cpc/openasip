/**
 * @file AddFUFromHDBCmd.hh
 *
 * Declaration of AddFUFromHDBCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_ADD_FU_FROM_HDB_CMD_HH
#define TTA_ADD_FU_FROM_HDB_CMD_HH

#include "EditorCommand.hh"

/**
 * wxCommand for opening a dialog for adding function units from a HDB.
 */
class AddFUFromHDBCmd : public EditorCommand {
public:
    AddFUFromHDBCmd();
    virtual ~AddFUFromHDBCmd();
    virtual bool Do();
    virtual int id() const;
    virtual AddFUFromHDBCmd* create() const;
    virtual std::string icon() const;
    virtual std::string shortName() const;
    virtual bool isEnabled();
};

#endif
