/**
 * @file AddIUFromHDBCmd.hh
 *
 * Declaration of AddIUFromHDBCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_ADD_IU_FROM_HDB_CMD_HH
#define TTA_ADD_IU_FROM_HDB_CMD_HH

#include "EditorCommand.hh"

/**
 * wxCommand for opening a dialog for adding immediate units from a HDB.
 */
class AddIUFromHDBCmd : public EditorCommand {
public:
    AddIUFromHDBCmd();
    virtual ~AddIUFromHDBCmd();
    virtual bool Do();
    virtual int id() const;
    virtual AddIUFromHDBCmd* create() const;
    virtual std::string icon() const;
    virtual std::string shortName() const;
    virtual bool isEnabled();
};

#endif
