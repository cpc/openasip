/**
 * @file SaveOptionsCmd.hh
 *
 * Declaration of SaveOptionsCmd class.
 *
 * @author Tommi Rantanen 2004 (tommi.rantanen@tut.fi)
 */

#ifndef TTA_SAVE_OPTIONS_CMD_HH
#define TTA_SAVE_OPTIONS_CMD_HH

#include "EditorCommand.hh"

/**
 * wxCommand for saving editor options.
 *
 * Creates and shows an OptionsDialog.
 */
class SaveOptionsCmd : public EditorCommand {
public:
    SaveOptionsCmd();
    virtual ~SaveOptionsCmd();
    virtual bool Do();
    virtual int id() const;
    virtual SaveOptionsCmd* create() const;
    virtual std::string icon() const;
    virtual std::string shortName() const;
    virtual bool isEnabled();
};

#endif
