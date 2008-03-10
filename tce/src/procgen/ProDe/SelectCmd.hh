/**
 * @file SelectCmd.hh
 *
 * Declaration of SelectCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_SELECT_CMD_HH
#define TTA_SELECT_CMD_HH

#include <string>
#include "EditorCommand.hh"

/**
 * EditorCommand for changing the canvas tool to the SelectTool.
 */
class SelectCmd : public EditorCommand {
public:
    SelectCmd();
    virtual ~SelectCmd();
    virtual bool Do();
    virtual int id() const;
    virtual SelectCmd* create() const;
    virtual std::string icon() const;
    bool isEnabled();
};

#endif
