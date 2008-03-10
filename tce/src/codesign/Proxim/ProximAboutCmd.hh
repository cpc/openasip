/**
 * @file ProximAboutCmd.hh
 *
 * Declaration of ProximAboutCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_ABOUT_CMD_HH
#define TTA_PROXIM_ABOUT_CMD_HH

#include "GUICommand.hh"


/**
 * Command for displaying Proxim about dialog.
 */
class ProximAboutCmd : public GUICommand {
public:
    ProximAboutCmd();
    virtual ~ProximAboutCmd();
    virtual bool Do();
    virtual int id() const;
    virtual ProximAboutCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();
};

#endif
