/**
 * @file CopyComponentCmd.hh
 *
 * Declaration of CopyComponentCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_COPY_COMPONENT_CMD_HH
#define TTA_COPY_COMPONENT_CMD_HH

#include <wx/wx.h>
#include <wx/cmdproc.h>
#include "EditorCommand.hh"


/**
 * Command for copying machine components to the clipboard.
 */
class CopyComponentCmd: public EditorCommand {
public:
    CopyComponentCmd();
    virtual bool Do();
    virtual int id() const;
    virtual CopyComponentCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();
};

#endif
