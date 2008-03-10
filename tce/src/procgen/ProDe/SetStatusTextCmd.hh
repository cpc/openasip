/**
 * @file SetStatusTextCmd.hh
 *
 * Declaration of SetStatusTextCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_SET_STATUS_TEXT_CMD_HH
#define TTA_SET_STATUS_TEXT_CMD_HH

#include <string>
#include "ComponentCommand.hh"

class EditPart;

/**
 * Command for setting the status bar text.
 */
class SetStatusTextCmd: public ComponentCommand {
public:
    SetStatusTextCmd(std::string text);
    virtual ~SetStatusTextCmd();
    virtual bool Do();

private:
    /// Status text to set.
    std::string text_;
};
#endif
