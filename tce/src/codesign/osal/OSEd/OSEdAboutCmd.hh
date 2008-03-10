/**
 * @file OSEdAboutCmd.hh
 *
 * Declaration of OSEdAboutCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OSED_ABOUT_CMD_HH
#define TTA_OSED_ABOUT_CMD_HH

#include <string>

#include "GUICommand.hh"

/**
 * Command that shows the about dialog of OSEd.
 */
class OSEdAboutCmd : public GUICommand {
public:
    OSEdAboutCmd();
    virtual ~OSEdAboutCmd();

    virtual int id() const;
    virtual GUICommand* create() const;
    virtual bool Do();
    virtual bool isEnabled();
    virtual std::string icon() const;

private:
    /// Copying not allowed.
    OSEdAboutCmd(const OSEdAboutCmd&);
    /// Assignment not allowed.
    OSEdAboutCmd& operator=(const OSEdAboutCmd&);
};

#endif
