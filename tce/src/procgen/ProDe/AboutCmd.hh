/**
 * @file AboutCmd.hh
 *
 * Declaration of AboutCmd class.
 *
 * @author Tommi Rantanen 2004 (tommi.rantanen@tut.fi)
 */

#ifndef TTA_ABOUT_CMD_HH
#define TTA_ABOUT_CMD_HH

#include "EditorCommand.hh"

/**
 * wxCommand for editing connections.
 */
class AboutCmd : public EditorCommand {
public:
    AboutCmd();
    virtual ~AboutCmd();
    virtual bool Do();
    virtual int id() const;
    virtual AboutCmd* create() const;
    virtual bool isEnabled();
    virtual std::string icon() const;
};

#endif
