/**
 * @file AddRFFromHDBCmd.hh
 *
 * Declaration of AddRFFromHDBCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_ADD_RF_FROM_HDB_CMD_HH
#define TTA_ADD_RF_FROM_HDB_CMD_HH

#include "EditorCommand.hh"

/**
 * wxCommand for opening a dialog for adding register files from a HDB.
 */
class AddRFFromHDBCmd : public EditorCommand {
public:
    AddRFFromHDBCmd();
    virtual ~AddRFFromHDBCmd();
    virtual bool Do();
    virtual int id() const;
    virtual AddRFFromHDBCmd* create() const;
    virtual std::string icon() const;
    virtual std::string shortName() const;
    virtual bool isEnabled();
};

#endif
