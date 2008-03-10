/**
 * @file EditConnectionsCmd.hh
 *
 * Declaration of EditConnectionsCmd class.
 *
 * @author Tommi Rantanen 2004 (tommi.rantanen@tut.fi)
 */

#ifndef TTA_EDIT_CONNECTIONS_CMD_HH
#define TTA_EDIT_CONNECTIONS_CMD_HH

#include <string>

#include "EditorCommand.hh"

/**
 * wxCommand for editing connections.
 */
class EditConnectionsCmd : public EditorCommand {
public:
    EditConnectionsCmd();
    virtual ~EditConnectionsCmd();
    virtual bool Do();
    virtual int id() const;
    virtual EditConnectionsCmd* create() const;
    virtual std::string icon() const;
    virtual std::string shortName() const;
    virtual bool isEnabled();
};

#endif
