/**
 * @file CutComponentCmd.hh
 *
 * Declaration of CutComponentCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_CUT_COMPONENT_CMD_HH
#define TTA_CUT_COMPONENT_CMD_HH

#include <wx/wx.h>
#include <wx/cmdproc.h>
#include "EditorCommand.hh"


/**
 * Command for cutting machine components to the clipboard.
 */
class CutComponentCmd: public EditorCommand {
public:
    CutComponentCmd();
    virtual bool Do();
    virtual int id() const;
    virtual CutComponentCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();
};

#endif
