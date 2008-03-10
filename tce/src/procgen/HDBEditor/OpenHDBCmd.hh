/**
 * @file OpenHDBCmd.hh
 *
 * Declaration of OpenHDBCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OPEN_HDB_CMD_HH
#define TTA_OPEN_HDB_CMD_HH

#include "GUICommand.hh"

/**
 * Command for opening HDB in HDBEditor.
 *
 * Displays a file dialog for choosing the HDB file to open.
 */
class OpenHDBCmd : public GUICommand {
public:
    OpenHDBCmd();
    virtual ~OpenHDBCmd();
    virtual bool Do();
    virtual int id() const;
    virtual OpenHDBCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();

};
#endif
